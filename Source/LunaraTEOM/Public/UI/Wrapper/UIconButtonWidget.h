#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "UIconButtonWidget.generated.h"

class SIconButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIconButtonClickedBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIconButtonPressedBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIconButtonReleasedBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIconButtonHoveredBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIconButtonUnhoveredBP);

UCLASS(meta = (DisplayName = "Icon Button (Slate)", Category = "Lunara|Slate"))
class LUNARATEOM_API UIconButtonWidget : public UWidget
{
    GENERATED_BODY()

public:
    /** Asset that will be rendered inside the button as the icon texture, material, vector art, or SVG. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (AllowedClasses = "Texture2D,MaterialInterface,SlateVectorArtData,SVGData"))
    TSoftObjectPtr<UObject> IconAsset;

    /** Diameter in pixels used to size the circular icon button. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (ClampMin = 16))
    float Diameter = 64.f;

    /** Event fired when the button is clicked with a confirmed release. */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnIconButtonClickedBP OnClicked;

    /** Event fired as soon as the button receives a press input. */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnIconButtonPressedBP OnPressed;

    /** Event fired when the press input is released over the button. */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnIconButtonReleasedBP OnReleased;

    /** Event fired when the pointer begins hovering the button. */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnIconButtonHoveredBP OnHovered;

    /** Event fired when the pointer stops hovering the button. */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnIconButtonUnhoveredBP OnUnhovered;

    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    virtual void SynchronizeProperties() override;

private:
    FReply HandleSlateClicked();
    void HandleSlatePressed();
    void HandleSlateReleased();
    void HandleSlateHovered();
    void HandleSlateUnhovered();

    TSharedPtr<SIconButtonWidget> MySlate;
};
