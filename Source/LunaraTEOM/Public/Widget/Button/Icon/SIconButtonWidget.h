#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateTypes.h"
#include "Animation/CurveSequence.h"
#include "Rendering/SlateRenderTransform.h"

struct FLunaraTeomSlateStyle;

DECLARE_DELEGATE_RetVal(FReply, FOnSlateIconButtonClicked)

class SButton;
class SBox;
class SImage;

class LUNARATEOM_API SIconButtonWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SIconButtonWidget)
        : _Style(nullptr)
        , _IconAsset(nullptr)
        , _Diameter(64.f)
    {}
        SLATE_ARGUMENT(const FLunaraTeomSlateStyle*, Style)
        SLATE_ARGUMENT(TSoftObjectPtr<UObject>, IconAsset)
        SLATE_ARGUMENT(float, Diameter)
        SLATE_EVENT(FOnSlateIconButtonClicked, OnClicked)
        SLATE_EVENT(FSimpleDelegate, OnPressed)
        SLATE_EVENT(FSimpleDelegate, OnReleased)
        SLATE_EVENT(FSimpleDelegate, OnHovered)
        SLATE_EVENT(FSimpleDelegate, OnUnhovered)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetIconAsset(TSoftObjectPtr<UObject> InIconAsset);
    void SetDiameter(float InDiameter);

private:
    void BuildLayout();

    FReply HandleClick();
    void HandleHovered();
    void HandleUnhovered();
    void HandlePressed();
    void HandleReleased();

    void ResolveStyle(const FLunaraTeomSlateStyle* InStyle);
    void RefreshIconBrush();

    float GetHoverAlpha() const;
    float GetPressAlpha() const;
    float GetClickCycleScale() const;
    TOptional<FSlateRenderTransform> GetButtonRenderTransform() const;

    FLinearColor GetOuterStrokeColor() const;
    FLinearColor GetInnerStrokeColor() const;
    FLinearColor GetFillColor() const;

    virtual int32 OnPaint(const FPaintArgs& Args,
        const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect,
        FSlateWindowElementList& OutDrawElements,
        int32 LayerId,
        const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled) const override;

private:
    FOnSlateIconButtonClicked OnClicked;
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

    const FLunaraTeomSlateStyle* StyleRef = nullptr;

    FLinearColor OuterStrokeBase = FLinearColor::White;
    FLinearColor InnerStrokeBase = FLinearColor::White;
    FLinearColor FillBase        = FLinearColor::Black;

    TSoftObjectPtr<UObject> IconAsset;
    FSlateBrush IconBrush;

    float ButtonDiameter = 64.f;
    float IconScale = 0.55f;

    TSharedPtr<SBox> ButtonBox;
    TSharedPtr<SBox> IconBox;
    TSharedPtr<SImage> IconImage;
};
