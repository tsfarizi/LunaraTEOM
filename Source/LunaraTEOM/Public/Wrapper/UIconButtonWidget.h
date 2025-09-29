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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (AllowedClasses = "Texture2D,MaterialInterface"))
    TSoftObjectPtr<UObject> IconAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (ClampMin = 16))
    float Diameter = 64.f;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnIconButtonClickedBP OnClicked;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnIconButtonPressedBP OnPressed;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnIconButtonReleasedBP OnReleased;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnIconButtonHoveredBP OnHovered;

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
