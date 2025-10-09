#pragma once

#include "CoreMinimal.h"
#include "SlateAgentBridgeLiveCodingTypes.h"

#include "HAL/CriticalSection.h"

enum class ELiveCodingCompileResult : uint8;

class FSlateAgentBridgeLiveCodingLogCapture;

/**
 * Owns the Live Coding compile flow and maintains the latest log snapshot.
 */
class FSlateAgentBridgeLiveCodingManager
{
public:
	FSlateAgentBridgeLiveCodingManager();
	~FSlateAgentBridgeLiveCodingManager();

	void Initialize();
	void Shutdown();

	bool ExecuteCompile(FString& OutErrorMessage, ELiveCodingCompileResult& OutResult);
	void GetLastCompileSnapshot(TArray<FSlateAgentBridgeLogEntry>& OutEntries, FDateTime& OutTimestamp, ELiveCodingCompileResult& OutResult, bool& bOutHasResult) const;

	static FString CompileResultToString(ELiveCodingCompileResult CompileResult);

private:
	bool EnsureCaptureAvailable(FString& OutErrorMessage) const;
	bool EnsureLiveCodingAvailable(FString& OutErrorMessage, class ILiveCodingModule*& OutModule) const;

private:
	TUniquePtr<FSlateAgentBridgeLiveCodingLogCapture> LogCapture;
	mutable FCriticalSection LiveCodingMutex;
	mutable FCriticalSection LogMutex;
	TArray<FSlateAgentBridgeLogEntry> LastCompileLogEntries;
	FDateTime LastCompileTimestamp;
	ELiveCodingCompileResult LastCompileResult;
	bool bHasCompileResult;
};
