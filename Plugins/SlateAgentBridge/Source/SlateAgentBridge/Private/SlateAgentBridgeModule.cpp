// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlateAgentBridgeModule.h"

#include "SlateAgentBridgeEditorModeCommands.h"
#include "SlateAgentBridgeLiveCodingManager.h"
#include "SlateAgentBridgeLiveCodingTypes.h"
#include "SlateAgentBridgeLog.h"

#include "Async/Async.h"
#include "ILiveCodingModule.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "HAL/Event.h"
#include "HAL/PlatformProcess.h"
#include "HttpPath.h"
#include "HttpServerConstants.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "Modules/ModuleManager.h"
#include "Math/NumericLimits.h"
#include "Misc/ConfigCacheIni.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

#define LOCTEXT_NAMESPACE "SlateAgentBridgeModule"

DEFINE_LOG_CATEGORY(LogSlateAgentBridge);

namespace SlateAgentBridge
{
	static constexpr uint32 DefaultLiveCodingPort = 8133;
	static constexpr const TCHAR* DefaultRoutePath = TEXT("/api/livecoding/compile");
	static constexpr const TCHAR* ConfigSection = TEXT("/Script/SlateAgentBridge.SlateAgentBridgeSettings");
	static constexpr const TCHAR* ConfigPortKey = TEXT("LiveCodingHttpPort");
	static constexpr const TCHAR* ConfigRouteKey = TEXT("LiveCodingRoutePath");
}

void FSlateAgentBridgeModule::StartupModule()
{
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

	LiveCodingManager = MakeUnique<FSlateAgentBridgeLiveCodingManager>();
	LiveCodingManager->Initialize();

	RegisterHttpEndpoint();

	UE_LOG(LogSlateAgentBridge, Display, TEXT("SlateAgentBridge module started. Live coding endpoint: http://127.0.0.1:%u%s"), HttpServerPort, *LiveCodingRoutePath);

	FSlateAgentBridgeEditorModeCommands::Register();
}

void FSlateAgentBridgeModule::ShutdownModule()
{
	UnregisterHttpEndpoint();

	if (LiveCodingManager)
	{
		LiveCodingManager->Shutdown();
		LiveCodingManager.Reset();
	}

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

bool FSlateAgentBridgeModule::HandleLiveCodingCompileRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	if (!LiveCodingManager)
	{
		TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(TEXT("{\"status\":\"error\",\"message\":\"Live coding manager is not initialized\"}"), TEXT("application/json"));
		Response->Code = EHttpServerResponseCodes::ServerError;
		OnComplete(MoveTemp(Response));
		return true;
	}

	struct FCompileExecutionContext
	{
		bool bSuccess = false;
		FString ErrorMessage;
		ELiveCodingCompileResult CompileResult = ELiveCodingCompileResult::NotStarted;
	};

	FCompileExecutionContext ExecutionContext;
	FEvent* CompletionEvent = FPlatformProcess::GetSynchEventFromPool(true);
	FSlateAgentBridgeLiveCodingManager* ManagerPtr = LiveCodingManager.Get();

	AsyncTask(ENamedThreads::GameThread, [ManagerPtr, &ExecutionContext, CompletionEvent]()
	{
		ExecutionContext.bSuccess = ManagerPtr->ExecuteCompile(ExecutionContext.ErrorMessage, ExecutionContext.CompileResult);
		CompletionEvent->Trigger();
	});

	CompletionEvent->Wait();
	FPlatformProcess::ReturnSynchEventToPool(CompletionEvent);

	TArray<FSlateAgentBridgeLogEntry> LogSnapshot;
	FDateTime SnapshotTimestamp;
	ELiveCodingCompileResult SnapshotResult = ELiveCodingCompileResult::NotStarted;
	bool bHasSnapshotResult = false;

	ManagerPtr->GetLastCompileSnapshot(LogSnapshot, SnapshotTimestamp, SnapshotResult, bHasSnapshotResult);

	const ELiveCodingCompileResult ResultToReport = ExecutionContext.bSuccess ? ExecutionContext.CompileResult : SnapshotResult;

	TSharedRef<FJsonObject> JsonPayload = MakeShared<FJsonObject>();
	JsonPayload->SetStringField(TEXT("status"), ExecutionContext.bSuccess ? TEXT("ok") : TEXT("error"));
	JsonPayload->SetStringField(TEXT("compileResult"), FSlateAgentBridgeLiveCodingManager::CompileResultToString(ResultToReport));

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

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSlateAgentBridgeModule, SlateAgentBridge)
