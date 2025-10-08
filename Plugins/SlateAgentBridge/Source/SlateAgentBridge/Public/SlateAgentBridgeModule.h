// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "HttpRouteHandle.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "HttpRequestHandler.h"
#include "Misc/DateTime.h"
#include "HAL/CriticalSection.h"
#include "Templates/UniquePtr.h"

class IHttpRouter;
enum class ELiveCodingCompileResult : uint8;
class FLiveCodingLogCapture;

/**
 * This is the module definition for the editor mode. You can implement custom functionality
 * as your plugin module starts up and shuts down. See IModuleInterface for more extensibility options.
 */
struct FSlateAgentBridgeLogEntry
{
	FString Message;
	FString Category;
	FString Verbosity;
	FDateTime Timestamp;
};

class FSlateAgentBridgeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterHttpEndpoint();
	void UnregisterHttpEndpoint();
	bool HandleLiveCodingCompileRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	void InitializeLogCapture();
	void ReleaseLogCapture();
	bool ExecuteLiveCodingCompileOnGameThread(FString& OutErrorMessage, ELiveCodingCompileResult& OutResult);
	FString CompileResultToString(ELiveCodingCompileResult CompileResult) const;

private:
	TSharedPtr<IHttpRouter> HttpRouter;
	FHttpRouteHandle LiveCodingRouteHandle;
	TUniquePtr<FLiveCodingLogCapture> LiveCodingLogCapture;
	FCriticalSection LiveCodingMutex;
	FCriticalSection LogMutex;
	TArray<FSlateAgentBridgeLogEntry> LastCompileLogEntries;
	FDateTime LastCompileTimestamp = FDateTime(0);
	ELiveCodingCompileResult LastCompileResult;
	bool bHasCompileResult = false;
	uint32 HttpServerPort = 8133;
	FString LiveCodingRoutePath = TEXT("/api/livecoding/compile");
};
