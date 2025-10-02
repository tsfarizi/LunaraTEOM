#include "UI/Player/LunaraTeomPlayerController.h"

ALunaraTeomPlayerController::ALunaraTeomPlayerController()
{
	// Apply defaults
	bShowMouseCursor = true;
	bEnableClickEvents = bEnableMouseClickEvents;
	bEnableMouseOverEvents = bEnableMouseHoverEvents;
	DefaultMouseCursor = DefaultCursor;
}

void ALunaraTeomPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Ensure cursor + input mode are set at start
	SetShowMouseCursor(true);
	SetInputModeGameAndUI();
}

/** Toggle cursor visibility */
void ALunaraTeomPlayerController::SetMouseCursorVisible(bool bVisible)
{
	bShowMouseCursor = bVisible;
	SetShowMouseCursor(bVisible);
}

/** Force Game Only input mode */
void ALunaraTeomPlayerController::SetInputModeGameOnly()
{
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
}

/** Force Game + UI input mode */
void ALunaraTeomPlayerController::SetInputModeGameAndUI(bool bHideCursorDuringCapture, bool bLockToViewport)
{
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(bHideCursorDuringCapture);

	InputMode.SetLockMouseToViewportBehavior(
		bLockToViewport ? EMouseLockMode::LockOnCapture : EMouseLockMode::DoNotLock);

	SetInputMode(InputMode);

	SetShowMouseCursor(true);
	bEnableClickEvents = bEnableMouseClickEvents;
	bEnableMouseOverEvents = bEnableMouseHoverEvents;
	DefaultMouseCursor = DefaultCursor;
}

/** Force UI Only input mode */
void ALunaraTeomPlayerController::SetInputModeUIOnly()
{
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
}
