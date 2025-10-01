#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LunaraTeomPlayerController.generated.h"

/**
 * Custom PlayerController for LunaraTEOM.
 * Handles input mode switching, mouse cursor visibility, and future scalable UI/game interactions.
 * Designed to be extended via Blueprint for quick iteration.
 */
UCLASS()
class LUNARATEOM_API ALunaraTeomPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALunaraTeomPlayerController();

protected:
	//~ Begin APlayerController interface
	virtual void BeginPlay() override;
	//~ End APlayerController interface

public:
	/**
	 * Enables or disables the mouse cursor at runtime.
	 * @param bVisible - If true, show the mouse cursor; otherwise hide it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Input|Cursor")
	void SetMouseCursorVisible(bool bVisible);

	/**
	 * Switches input mode between:
	 * - Game Only (exclusive control for gameplay)
	 * - Game and UI (cursor visible, game + UI input both active)
	 * - UI Only (useful for menus, blocks gameplay input)
	 */
	UFUNCTION(BlueprintCallable, Category = "Input|Mode")
	void SetInputModeGameOnly();

	UFUNCTION(BlueprintCallable, Category = "Input|Mode")
	void SetInputModeGameAndUI(bool bHideCursorDuringCapture = false, bool bLockToViewport = false);

	UFUNCTION(BlueprintCallable, Category = "Input|Mode")
	void SetInputModeUIOnly();

        /** Enables mouse click events so Slate widgets and actors can respond to presses. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Cursor")
        bool bEnableMouseClickEvents = true;

        /** Enables mouse hover events for tooltips and hover-driven feedback. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Cursor")
        bool bEnableMouseHoverEvents = true;

        /** Default hardware cursor icon (set in editor or Blueprint). */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Cursor")
        TEnumAsByte<EMouseCursor::Type> DefaultCursor = EMouseCursor::Default;
};
