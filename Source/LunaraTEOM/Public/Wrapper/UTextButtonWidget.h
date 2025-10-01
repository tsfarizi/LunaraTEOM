#pragma once
#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "UTextButtonWidget.generated.h"

class STextButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClickedBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonPressedBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonReleasedBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonHoveredBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonUnhoveredBP);

UCLASS(meta=(DisplayName="Text Button (Slate)"))
class LUNARATEOM_API UTextButtonWidget : public UWidget
{
    GENERATED_BODY()

public:
    /** Text displayed as the button label. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Content")
    FText Label = FText::FromString(TEXT("Button"));

    /** Font asset used when rendering the button label. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance", meta=(AllowedClasses="Font,FontFace"))
    TSoftObjectPtr<UObject> FontObject;

    /** Point size applied to the label font. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance", meta=(ClampMin=1))
    int32 FontSize = 18;

    /** Event fired when the button is clicked with a full press and release. */
    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnButtonClickedBP OnClicked;

    /** Event fired immediately when the button receives a press input. */
    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnButtonPressedBP OnPressed;

    /** Event fired when the press input is released while over the button. */
    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnButtonReleasedBP OnReleased;

    /** Event fired when a pointer begins hovering the button. */
    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnButtonHoveredBP OnHovered;

    /** Event fired when a pointer stops hovering the button. */
    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnButtonUnhoveredBP OnUnhovered;

    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    virtual void SynchronizeProperties() override;

private:
    FReply HandleSlateClicked();
    void HandleSlatePressed();
    void HandleSlateReleased();
    void HandleSlateHovered();
    void HandleSlateUnhovered();
    FText GetLabel() const;

    TSharedPtr<STextButtonWidget> MySlate;
};
