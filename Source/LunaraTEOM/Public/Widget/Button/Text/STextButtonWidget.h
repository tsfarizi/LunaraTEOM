#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateTypes.h"
#include "Animation/CurveSequence.h"
#include "Rendering/SlateRenderTransform.h"

struct FLunaraTeomSlateStyle;

DECLARE_DELEGATE_RetVal(FReply, FOnSlateClicked)

class SBeveledBorder;
class STextBlock;

class LUNARATEOM_API STextButtonWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STextButtonWidget)
        : _Label()
        , _Style(nullptr)
        , _FontObject(nullptr)
        , _FontSize(18)
    {}
        SLATE_ATTRIBUTE(FText, Label)
        SLATE_EVENT(FOnSlateClicked, OnClicked)
        SLATE_EVENT(FSimpleDelegate, OnPressed)
        SLATE_EVENT(FSimpleDelegate, OnReleased)
        SLATE_EVENT(FSimpleDelegate, OnHovered)
        SLATE_EVENT(FSimpleDelegate, OnUnhovered)
        SLATE_ARGUMENT(const FLunaraTeomSlateStyle*, Style)
        SLATE_ARGUMENT(TSoftObjectPtr<UObject>, FontObject)
        SLATE_ARGUMENT(int32, FontSize)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    void SetExplicitFont(TSoftObjectPtr<UObject> InFontObject, int32 InFontSize);

private:
    void BuildLayout();

    FReply HandleClick();

    void ResolveStyle(const FLunaraTeomSlateStyle* InStyle);
    void RebuildLabelFont();
    void HandleHovered();
    void HandleUnhovered();
    void HandlePressed();
    void HandleReleased();
    FLinearColor GetInnerPanelColor() const;
    FSlateColor  GetLabelColor() const;
    float GetHoverAlpha() const;
    float GetPressAlpha() const;
    float GetClickCycleScale() const;
    TOptional<FSlateRenderTransform> GetButtonRenderTransform() const;

private:
    FOnSlateClicked OnClicked;
    FSimpleDelegate OnPressed;
    FSimpleDelegate OnReleased;
    FSimpleDelegate OnHovered;
    FSimpleDelegate OnUnhovered;

    FButtonStyle ButtonStyle;

    FCurveSequence HoverSeq;
    FCurveHandle   HoverAlpha;
    FCurveSequence PressSeq;
    FCurveHandle   PressAlpha;
    FCurveSequence ClickCycleSeq;
    FCurveHandle   ClickShrinkAlpha;
    FCurveHandle   ClickReturnAlpha;

    TAttribute<FText> LabelAttr;
    TSharedPtr<SBeveledBorder> InnerPanel;
    TSharedPtr<STextBlock>     LabelText;

    const FLunaraTeomSlateStyle* StyleRef = nullptr;
    FSlateFontInfo LabelFont;

    TSoftObjectPtr<UObject> ExplicitFontObject;
    int32 ExplicitFontSize = 18;

    FSlateColor TextColor = FSlateColor(FLinearColor::White);
};
