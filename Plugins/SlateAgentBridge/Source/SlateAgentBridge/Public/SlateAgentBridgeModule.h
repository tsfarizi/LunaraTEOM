// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "HttpRouteHandle.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "HttpRequestHandler.h"
#include "Templates/UniquePtr.h"

class IHttpRouter;
class FSlateAgentBridgeLiveCodingManager;

/**
 * This is the module definition for the editor mode. You can implement custom functionality
 * as your plugin module starts up and shuts down. See IModuleInterface for more extensibility options.
 */
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

private:
	TSharedPtr<IHttpRouter> HttpRouter;
	FHttpRouteHandle LiveCodingRouteHandle;
	TUniquePtr<FSlateAgentBridgeLiveCodingManager> LiveCodingManager;
	uint32 HttpServerPort = 8133;
	FString LiveCodingRoutePath = TEXT("/api/livecoding/compile");
};
