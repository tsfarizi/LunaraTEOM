// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlateAgentBridgeModule.h"

#include "SlateAgentBridgeEditorModeCommands.h"

#include "Async/Async.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "HAL/Event.h"
#include "HAL/PlatformProcess.h"
#include "HttpPath.h"
#include "HttpServerConstants.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "ILiveCodingModule.h"
#include "Math/NumericLimits.h"
#include "Logging/LogVerbosity.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/OutputDevice.h"
#include "Misc/OutputDeviceRedirector.h"
#include "Misc/ScopeLock.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

#define LOCTEXT_NAMESPACE "SlateAgentBridgeModule"

DEFINE_LOG_CATEGORY_STATIC(LogSlateAgentBridge, Log, All);

namespace SlateAgentBridge
{
	static constexpr uint32 DefaultLiveCodingPort = 8133;
	static constexpr const TCHAR* DefaultRoutePath = TEXT("/api/livecoding/compile");
	static constexpr const TCHAR* ConfigSection = TEXT("/Script/SlateAgentBridge.SlateAgentBridgeSettings");
	static constexpr const TCHAR* ConfigPortKey = TEXT("LiveCodingHttpPort");
	static constexpr const TCHAR* ConfigRouteKey = TEXT("LiveCodingRoutePath");

	static FString GetVerbosityString(const ELogVerbosity::Type Verbosity)
	{
		return FString(::ToString(Verbosity));
	}
}

class FLiveCodingLogCapture : public FOutputDevice
{
public:
	FLiveCodingLogCapture() = default;

	void StartCapture()
	{
		FScopeLock CaptureLock(&CaptureMutex);
		CapturedEntries.Reset();
		bIsCapturing = true;
	}

	TArray<FSlateAgentBridgeLogEntry> StopCapture()
	{
		FScopeLock CaptureLock(&CaptureMutex);
		bIsCapturing = false;
		return MoveTemp(CapturedEntries);
	}

	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category) override
	{
		if (Category.IsNone())
		{
			return;
		}

		const FString CategoryString = Category.ToString();
		if (!CategoryString.Contains(TEXT("LiveCoding")))
		{
			return;
		}

		FScopeLock CaptureLock(&CaptureMutex);
		if (!bIsCapturing)
		{
			return;
		}

		FSlateAgentBridgeLogEntry& NewEntry = CapturedEntries.AddDefaulted_GetRef();
		NewEntry.Category = CategoryString;
		NewEntry.Message = FString(V);
		NewEntry.Verbosity = SlateAgentBridge::GetVerbosityString(Verbosity);
		NewEntry.Timestamp = FDateTime::UtcNow();
	}

private:
	FCriticalSection CaptureMutex;
	bool bIsCapturing = false;
	TArray<FSlateAgentBridgeLogEntry> CapturedEntries;
};

void FSlateAgentBridgeModule::StartupModule()
{
	LastCompileResult = ELiveCodingCompileResult::NotStarted;
	HttpServerPort = SlateAgentBridge::DefaultLiveCodingPort;
	LiveCodingRoutePath = SlateAgentBridge::DefaultRoutePath;

	if (GConfig)
	{
		int32 ConfiguredPort = 0;
		if (GConfig->GetInt(SlateAgentBridge::ConfigSection, SlateAgentBridge::ConfigPortKey, ConfiguredPort, GEditorPerProjectIni)
			&& ConfiguredPort > 0 && ConfiguredPort <= TNumericLimits<uint16>::Max())
		{
			HttpServerPort = static_cast<uint32>(ConfiguredPort);
		}

		FString ConfiguredRoute;
		if (GConfig->GetString(SlateAgentBridge::ConfigSection, SlateAgentBridge::ConfigRouteKey, ConfiguredRoute, GEditorPerProjectIni)
			&& !ConfiguredRoute.IsEmpty())
		{
			if (!ConfiguredRoute.StartsWith(TEXT("/")))
			{
				ConfiguredRoute.InsertAt(0, TEXT("/"));
			}
			LiveCodingRoutePath = ConfiguredRoute;
		}
	}

	InitializeLogCapture();
	RegisterHttpEndpoint();

	UE_LOG(LogSlateAgentBridge, Display, TEXT("SlateAgentBridge module started. Live coding endpoint: http://127.0.0.1:%u%s"), HttpServerPort, *LiveCodingRoutePath);

	FSlateAgentBridgeEditorModeCommands::Register();
}

void FSlateAgentBridgeModule::ShutdownModule()
{
	UnregisterHttpEndpoint();
	ReleaseLogCapture();

	FSlateAgentBridgeEditorModeCommands::Unregister();

	UE_LOG(LogSlateAgentBridge, Display, TEXT("SlateAgentBridge module shut down."));
}

void FSlateAgentBridgeModule::RegisterHttpEndpoint()
{
	if (LiveCodingRouteHandle.IsValid())
	{
		return;
	}

	if (!FModuleManager::Get().IsModuleLoaded("HTTPServer"))
	{
		FModuleManager::LoadModuleChecked<FHttpServerModule>("HTTPServer");
	}

	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();

	HttpRouter = HttpServerModule.GetHttpRouter(HttpServerPort, /* bFailOnBindFailure */ true);
	if (!HttpRouter.IsValid())
	{
		UE_LOG(LogSlateAgentBridge, Error, TEXT("Unable to start HTTP router on port %u for live coding endpoint."), HttpServerPort);
		return;
	}

	const FHttpPath RoutePath(LiveCodingRoutePath);
	const FHttpRequestHandler Handler = FHttpRequestHandler::CreateRaw(this, &FSlateAgentBridgeModule::HandleLiveCodingCompileRequest);

	LiveCodingRouteHandle = HttpRouter->BindRoute(RoutePath, EHttpServerRequestVerbs::VERB_POST, Handler);
	if (!LiveCodingRouteHandle.IsValid())
	{
		UE_LOG(LogSlateAgentBridge, Error, TEXT("Failed to bind live coding compile route at %s"), *LiveCodingRoutePath);
		HttpRouter.Reset();
		return;
	}

	HttpServerModule.StartAllListeners();

	UE_LOG(LogSlateAgentBridge, Display, TEXT("Live coding HTTP endpoint bound to %s on port %u."), *LiveCodingRoutePath, HttpServerPort);
}

void FSlateAgentBridgeModule::UnregisterHttpEndpoint()
{
	if (HttpRouter.IsValid() && LiveCodingRouteHandle.IsValid())
	{
		HttpRouter->UnbindRoute(LiveCodingRouteHandle);
		LiveCodingRouteHandle = FHttpRouteHandle();
	}

	HttpRouter.Reset();
}

void FSlateAgentBridgeModule::InitializeLogCapture()
{
	if (!LiveCodingLogCapture.IsValid())
	{
		LiveCodingLogCapture = MakeUnique<FLiveCodingLogCapture>();
		if (GLog)
		{
			GLog->AddOutputDevice(LiveCodingLogCapture.Get());
		}
	}
}

void FSlateAgentBridgeModule::ReleaseLogCapture()
{
	if (LiveCodingLogCapture.IsValid())
	{
		if (GLog)
		{
			GLog->RemoveOutputDevice(LiveCodingLogCapture.Get());
		}
		LiveCodingLogCapture.Reset();
	}
}

bool FSlateAgentBridgeModule::HandleLiveCodingCompileRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogSlateAgentBridge, Display, TEXT("Received live coding compile request for %s."), *Request.RelativePath.GetPath());

	struct FCompileExecutionContext
	{
		bool bSuccess = false;
		FString ErrorMessage;
		ELiveCodingCompileResult CompileResult = ELiveCodingCompileResult::NotStarted;
	};

	FCompileExecutionContext ExecutionContext;

	FEvent* CompletionEvent = FPlatformProcess::GetSynchEventFromPool(true);

	AsyncTask(ENamedThreads::GameThread, [this, &ExecutionContext, CompletionEvent]()
	{
		FScopeLock CompileLock(&LiveCodingMutex);
		ExecutionContext.bSuccess = ExecuteLiveCodingCompileOnGameThread(ExecutionContext.ErrorMessage, ExecutionContext.CompileResult);
		CompletionEvent->Trigger();
	});

	CompletionEvent->Wait();
	FPlatformProcess::ReturnSynchEventToPool(CompletionEvent);

	TArray<FSlateAgentBridgeLogEntry> LogSnapshot;
	FDateTime SnapshotTimestamp;
	bool bHasSnapshotResult = false;

	{
		FScopeLock LogLock(&LogMutex);
		LogSnapshot = LastCompileLogEntries;
		SnapshotTimestamp = LastCompileTimestamp;
		bHasSnapshotResult = bHasCompileResult;
	}

	TSharedRef<FJsonObject> JsonPayload = MakeShared<FJsonObject>();
	JsonPayload->SetStringField(TEXT("status"), ExecutionContext.bSuccess ? TEXT("ok") : TEXT("error"));
	JsonPayload->SetStringField(TEXT("compileResult"), CompileResultToString(ExecutionContext.CompileResult));

	if (!ExecutionContext.ErrorMessage.IsEmpty())
	{
		JsonPayload->SetStringField(TEXT("message"), ExecutionContext.ErrorMessage);
	}

	if (SnapshotTimestamp.GetTicks() > 0)
	{
		JsonPayload->SetStringField(TEXT("timestampUtc"), SnapshotTimestamp.ToIso8601());
	}

	TArray<TSharedPtr<FJsonValue>> LogArray;
	LogArray.Reserve(LogSnapshot.Num());
	for (const FSlateAgentBridgeLogEntry& Entry : LogSnapshot)
	{
		TSharedRef<FJsonObject> LogObject = MakeShared<FJsonObject>();
		LogObject->SetStringField(TEXT("timeUtc"), Entry.Timestamp.ToIso8601());
		LogObject->SetStringField(TEXT("category"), Entry.Category);
		LogObject->SetStringField(TEXT("verbosity"), Entry.Verbosity);
		LogObject->SetStringField(TEXT("message"), Entry.Message);
		LogArray.Add(MakeShared<FJsonValueObject>(LogObject));
	}
	JsonPayload->SetArrayField(TEXT("log"), LogArray);

	JsonPayload->SetBoolField(TEXT("hasPreviousResult"), bHasSnapshotResult);
	JsonPayload->SetStringField(TEXT("route"), LiveCodingRoutePath);

	FString ResponseBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseBody);
	FJsonSerializer::Serialize(JsonPayload, Writer);

	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(ResponseBody, TEXT("application/json"));
	Response->Code = ExecutionContext.bSuccess ? EHttpServerResponseCodes::Ok : EHttpServerResponseCodes::ServerError;
	OnComplete(MoveTemp(Response));

	return true;
}

bool FSlateAgentBridgeModule::ExecuteLiveCodingCompileOnGameThread(FString& OutErrorMessage, ELiveCodingCompileResult& OutResult)
{
	check(IsInGameThread());

	OutResult = ELiveCodingCompileResult::NotStarted;

	if (!LiveCodingLogCapture.IsValid())
	{
		OutErrorMessage = TEXT("Live coding log capture is not available.");
		UE_LOG(LogSlateAgentBridge, Error, TEXT("%s"), *OutErrorMessage);
		return false;
	}

	ILiveCodingModule* LiveCodingModule = FModuleManager::LoadModulePtr<ILiveCodingModule>(LIVE_CODING_MODULE_NAME);
	if (!LiveCodingModule)
	{
		OutErrorMessage = TEXT("Live Coding module is unavailable. Enable Live Coding in the editor first.");
		UE_LOG(LogSlateAgentBridge, Error, TEXT("%s"), *OutErrorMessage);
		return false;
	}

	if (!LiveCodingModule->CanEnableForSession())
	{
		OutErrorMessage = LiveCodingModule->GetEnableErrorText().ToString();
		UE_LOG(LogSlateAgentBridge, Error, TEXT("Live Coding cannot be enabled: %s"), *OutErrorMessage);
		return false;
	}

	if (!LiveCodingModule->IsEnabledForSession())
	{
		LiveCodingModule->EnableForSession(true);
	}

	if (!LiveCodingModule->HasStarted())
	{
		LiveCodingModule->EnableForSession(true);
	}

	if (LiveCodingModule->IsCompiling())
	{
		OutErrorMessage = TEXT("A Live Coding compile is already in progress.");
		UE_LOG(LogSlateAgentBridge, Warning, TEXT("%s"), *OutErrorMessage);
		OutResult = ELiveCodingCompileResult::CompileStillActive;
		return false;
	}

	UE_LOG(LogSlateAgentBridge, Display, TEXT("Live Coding compile started via HTTP endpoint."));

	LiveCodingLogCapture->StartCapture();
	const bool bCompileRequestAccepted = LiveCodingModule->Compile(ELiveCodingCompileFlags::WaitForCompletion, &OutResult);
	TArray<FSlateAgentBridgeLogEntry> CapturedEntries = LiveCodingLogCapture->StopCapture();

	{
		FScopeLock LogLock(&LogMutex);
		LastCompileLogEntries = MoveTemp(CapturedEntries);
		LastCompileTimestamp = FDateTime::UtcNow();
		LastCompileResult = OutResult;
		bHasCompileResult = true;
	}

	if (!bCompileRequestAccepted)
	{
		OutErrorMessage = TEXT("Live Coding compile request was rejected.");
		UE_LOG(LogSlateAgentBridge, Error, TEXT("%s"), *OutErrorMessage);
		return false;
	}

	switch (OutResult)
	{
	case ELiveCodingCompileResult::Success:
		UE_LOG(LogSlateAgentBridge, Display, TEXT("Live Coding compile completed with changes."));
		return true;

	case ELiveCodingCompileResult::NoChanges:
		UE_LOG(LogSlateAgentBridge, Display, TEXT("Live Coding compile completed with no changes."));
		return true;

	case ELiveCodingCompileResult::InProgress:
		OutErrorMessage = TEXT("Live Coding compile did not complete within the allotted time.");
		UE_LOG(LogSlateAgentBridge, Warning, TEXT("%s"), *OutErrorMessage);
		return false;

	case ELiveCodingCompileResult::CompileStillActive:
		OutErrorMessage = TEXT("Another Live Coding compile is still active.");
		UE_LOG(LogSlateAgentBridge, Warning, TEXT("%s"), *OutErrorMessage);
		return false;

	case ELiveCodingCompileResult::NotStarted:
		OutErrorMessage = TEXT("Live Coding compile could not be started.");
		UE_LOG(LogSlateAgentBridge, Error, TEXT("%s"), *OutErrorMessage);
		return false;

	case ELiveCodingCompileResult::Failure:
		OutErrorMessage = TEXT("Live Coding compile failed. See log for details.");
		UE_LOG(LogSlateAgentBridge, Error, TEXT("%s"), *OutErrorMessage);
		return false;

	case ELiveCodingCompileResult::Cancelled:
		OutErrorMessage = TEXT("Live Coding compile was cancelled.");
		UE_LOG(LogSlateAgentBridge, Warning, TEXT("%s"), *OutErrorMessage);
		return false;

	default:
		OutErrorMessage = TEXT("Live Coding compile result is unknown.");
		UE_LOG(LogSlateAgentBridge, Warning, TEXT("%s"), *OutErrorMessage);
		return false;
	}
}

FString FSlateAgentBridgeModule::CompileResultToString(ELiveCodingCompileResult CompileResult) const
{
	switch (CompileResult)
	{
	case ELiveCodingCompileResult::Success:
		return TEXT("Success");
	case ELiveCodingCompileResult::NoChanges:
		return TEXT("NoChanges");
	case ELiveCodingCompileResult::InProgress:
		return TEXT("InProgress");
	case ELiveCodingCompileResult::CompileStillActive:
		return TEXT("CompileStillActive");
	case ELiveCodingCompileResult::NotStarted:
		return TEXT("NotStarted");
	case ELiveCodingCompileResult::Failure:
		return TEXT("Failure");
	case ELiveCodingCompileResult::Cancelled:
		return TEXT("Cancelled");
	default:
		return TEXT("Unknown");
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSlateAgentBridgeModule, SlateAgentBridge)
