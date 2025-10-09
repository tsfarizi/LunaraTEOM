#include "SlateAgentBridgeLiveCodingManager.h"

#include "SlateAgentBridgeLiveCodingLogCapture.h"
#include "SlateAgentBridgeLog.h"

#include "ILiveCodingModule.h"
#include "Logging/LogMacros.h"
#include "Modules/ModuleManager.h"
#include "Misc/OutputDeviceRedirector.h"
#include "Misc/ScopeLock.h"

FSlateAgentBridgeLiveCodingManager::FSlateAgentBridgeLiveCodingManager()
	: LastCompileTimestamp(FDateTime(0))
	, LastCompileResult(ELiveCodingCompileResult::NotStarted)
	, bHasCompileResult(false)
{
}

FSlateAgentBridgeLiveCodingManager::~FSlateAgentBridgeLiveCodingManager()
{
	Shutdown();
}

void FSlateAgentBridgeLiveCodingManager::Initialize()
{
	if (!LogCapture.IsValid())
	{
		LogCapture = MakeUnique<FSlateAgentBridgeLiveCodingLogCapture>();
		if (GLog)
		{
			GLog->AddOutputDevice(LogCapture.Get());
		}
	}

	{
		FScopeLock LogLock(&LogMutex);
		LastCompileLogEntries.Reset();
		LastCompileTimestamp = FDateTime(0);
		LastCompileResult = ELiveCodingCompileResult::NotStarted;
		bHasCompileResult = false;
	}
}

void FSlateAgentBridgeLiveCodingManager::Shutdown()
{
	if (LogCapture.IsValid())
	{
		if (GLog)
		{
			GLog->RemoveOutputDevice(LogCapture.Get());
		}
		LogCapture.Reset();
	}
}

bool FSlateAgentBridgeLiveCodingManager::ExecuteCompile(FString& OutErrorMessage, ELiveCodingCompileResult& OutResult)
{
	check(IsInGameThread());

	FScopeLock CompileLock(&LiveCodingMutex);

	if (!EnsureCaptureAvailable(OutErrorMessage))
	{
		return false;
	}

	ILiveCodingModule* LiveCodingModule = nullptr;
	if (!EnsureLiveCodingAvailable(OutErrorMessage, LiveCodingModule))
	{
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

	LogCapture->StartCapture();
	const bool bCompileRequestAccepted = LiveCodingModule->Compile(ELiveCodingCompileFlags::WaitForCompletion, &OutResult);
	TArray<FSlateAgentBridgeLogEntry> CapturedEntries = LogCapture->StopCapture();

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

void FSlateAgentBridgeLiveCodingManager::GetLastCompileSnapshot(TArray<FSlateAgentBridgeLogEntry>& OutEntries, FDateTime& OutTimestamp, ELiveCodingCompileResult& OutResult, bool& bOutHasResult) const
{
	FScopeLock LogLock(&LogMutex);
	OutEntries = LastCompileLogEntries;
	OutTimestamp = LastCompileTimestamp;
	OutResult = LastCompileResult;
	bOutHasResult = bHasCompileResult;
}

FString FSlateAgentBridgeLiveCodingManager::CompileResultToString(ELiveCodingCompileResult CompileResult)
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

bool FSlateAgentBridgeLiveCodingManager::EnsureCaptureAvailable(FString& OutErrorMessage) const
{
	if (!LogCapture.IsValid())
	{
		OutErrorMessage = TEXT("Live coding log capture is not available.");
		UE_LOG(LogSlateAgentBridge, Error, TEXT("%s"), *OutErrorMessage);
		return false;
	}

	return true;
}

bool FSlateAgentBridgeLiveCodingManager::EnsureLiveCodingAvailable(FString& OutErrorMessage, ILiveCodingModule*& OutModule) const
{
	OutModule = FModuleManager::LoadModulePtr<ILiveCodingModule>(LIVE_CODING_MODULE_NAME);
	if (!OutModule)
	{
		OutErrorMessage = TEXT("Live Coding module is unavailable. Enable Live Coding in the editor first.");
		UE_LOG(LogSlateAgentBridge, Error, TEXT("%s"), *OutErrorMessage);
		return false;
	}

	if (!OutModule->CanEnableForSession())
	{
		OutErrorMessage = OutModule->GetEnableErrorText().ToString();
		UE_LOG(LogSlateAgentBridge, Error, TEXT("Live Coding cannot be enabled: %s"), *OutErrorMessage);
		return false;
	}

	return true;
}
