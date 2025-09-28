#include "Widget/Button/Icon/SIconButtonWidget.h"

#include "Widget/SBeveledBorder.h"

#include "UI/LunaraTeomSlateWidgetStyle.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"

#include "Styling/CoreStyle.h"
#include "Brushes/SlateRoundedBoxBrush.h"

#include "Rendering/DrawElements.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SIconButtonWidget::BuildLayout()
{
    const FLinearColor DeepBlue = StyleRef ? StyleRef->DeepBlue : FLinearColor(0.08f, 0.12f, 0.20f);
    const FLinearColor MoonGold = StyleRef ? StyleRef->MoonGold : FLinearColor(0.72f, 0.54f, 0.23f);
    const FLinearColor BaseGold = StyleRef ? StyleRef->BaseGold : FLinearColor(0.66f, 0.48f, 0.18f);

    constexpr float OuterBevelValue  = 8.f;
    constexpr float OuterNotchDepth  = 10.f;
    constexpr float OuterNotchHeight = 20.f;

    constexpr float MidBevelValue    = 6.f;
    constexpr float MidNotchDepth    = 8.f;
    constexpr float MidNotchHeight   = 20.f;

    constexpr float InnerBevelValue  = 4.f;
    constexpr float InnerNotchDepth  = 8.f;
    constexpr float InnerNotchHeight = 16.f;

    ChildSlot
    [
        SAssignNew(ButtonBox, SBox)
        .WidthOverride(ButtonDiameter)
        .HeightOverride(ButtonDiameter)
        .RenderTransformPivot(FVector2D(0.5f, 0.5f))
        .RenderTransform(this, &SIconButtonWidget::GetButtonRenderTransform)
        [
            SNew(SButton)
            .ButtonStyle(&ButtonStyle)
            .ContentPadding(0.f)
            .OnClicked(this, &SIconButtonWidget::HandleClick)
            .OnHovered(this, &SIconButtonWidget::HandleHovered)
            .OnUnhovered(this, &SIconButtonWidget::HandleUnhovered)
            .OnPressed(this, &SIconButtonWidget::HandlePressed)
            .OnReleased(this, &SIconButtonWidget::HandleReleased)
            [
                SNew(SBeveledBorder)
                .Bevel(OuterBevelValue)
                .NotchDepth(OuterNotchDepth)
                .NotchHeight(OuterNotchHeight)
                .Color(DeepBlue)
                .Padding(FMargin(4.f))
                [
                    SNew(SBeveledBorder)
                    .Bevel(MidBevelValue)
                    .NotchDepth(MidNotchDepth)
                    .NotchHeight(MidNotchHeight)
                    .Color(MoonGold * 0.9f)
                    .Padding(FMargin(2.f))
                    [
                        SNew(SBeveledBorder)
                        .Bevel(InnerBevelValue)
                        .NotchDepth(InnerNotchDepth)
                        .NotchHeight(InnerNotchHeight)
                        .Color(BaseGold)
                        .Padding(FMargin(4.f))
                        [
                            SNew(SOverlay)
                            + SOverlay::Slot()
                            .HAlign(HAlign_Fill)
                            .VAlign(VAlign_Fill)
                            [
                                SAssignNew(InnerPanel, SBeveledBorder)
                                .Bevel(InnerBevelValue * 0.75f)
                                .NotchDepth(InnerNotchDepth * 0.75f)
                                .NotchHeight(InnerNotchHeight * 0.85f)
                                .Color(TAttribute<FLinearColor>::Create(
                                    TAttribute<FLinearColor>::FGetter::CreateSP(this, &SIconButtonWidget::GetFillColor)))
                                .Padding(FMargin(0.f))
                            ]
                            + SOverlay::Slot()
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Center)
                            [
                                SNew(SBox)
                                .WidthOverride(ButtonDiameter * IconScale)
                                .HeightOverride(ButtonDiameter * IconScale)
                                [
                                    SAssignNew(IconImage, SImage)
                                    .Image(nullptr)
                                    .ColorAndOpacity(FLinearColor::White)
                                    .Visibility(EVisibility::Collapsed)
                                ]
                            ]
                        ]
                    ]
                ]
            ]
        ]
    ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

int32 SIconButtonWidget::OnPaint(const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled) const
{
    const FVector2D Size = AllottedGeometry.GetLocalSize();
    const float Diameter = FMath::Min(Size.X, Size.Y);

    const float OuterStroke = FMath::Clamp(Diameter * 0.08f, 2.f, 8.f);
    const float InnerStroke = FMath::Clamp(Diameter * 0.05f, 1.f, 6.f);

    const FVector2D OuterSize(Diameter, Diameter);
    const FVector2D OuterOffset((Size - OuterSize) * 0.5f);

    const FVector2D MiddleSize = OuterSize - FVector2D(OuterStroke * 2.f, OuterStroke * 2.f);
    const FVector2D MiddleOffset = OuterOffset + FVector2D(OuterStroke, OuterStroke);

    const FVector2D InnerSize = MiddleSize - FVector2D(InnerStroke * 2.f, InnerStroke * 2.f);
    const FVector2D InnerOffset = MiddleOffset + FVector2D(InnerStroke, InnerStroke);

    const FLinearColor Tint = InWidgetStyle.GetColorAndOpacityTint();

    const FLinearColor OuterColor = GetOuterStrokeColor() * Tint;
    const FLinearColor InnerColor = GetInnerStrokeColor() * Tint;
    const FLinearColor FillColor  = GetFillColor() * Tint;

    const FVector4 OuterRadius(Diameter * 0.5f);
    const FVector4 MiddleRadius(FMath::Max(1.f, MiddleSize.X * 0.5f));
    const FVector4 InnerRadius(FMath::Max(1.f, InnerSize.X * 0.5f));

    const FLinearColor ShadowColor(0.f, 0.f, 0.f, 0.28f * Tint.A);
    const FVector2D ShadowOffset(2.f, 2.f);

    FSlateRoundedBoxBrush OuterShadowBrush(ShadowColor, OuterRadius);
    FSlateRoundedBoxBrush MiddleShadowBrush(ShadowColor * 0.9f, MiddleRadius);
    FSlateRoundedBoxBrush InnerShadowBrush(ShadowColor * 0.8f, InnerRadius);

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(OuterOffset + ShadowOffset, OuterSize),
        &OuterShadowBrush,
        ESlateDrawEffect::None,
        ShadowColor);

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId + 1,
        AllottedGeometry.ToPaintGeometry(MiddleOffset + ShadowOffset * 0.8f, MiddleSize),
        &MiddleShadowBrush,
        ESlateDrawEffect::None,
        ShadowColor * 0.9f);

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId + 2,
        AllottedGeometry.ToPaintGeometry(InnerOffset + ShadowOffset * 0.6f, InnerSize),
        &InnerShadowBrush,
        ESlateDrawEffect::None,
        ShadowColor * 0.8f);

    FSlateRoundedBoxBrush OuterBrush(OuterColor, OuterRadius);
    FSlateRoundedBoxBrush MiddleBrush(InnerColor, MiddleRadius);
    FSlateRoundedBoxBrush InnerBrush(FillColor, InnerRadius);

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId + 3,
        AllottedGeometry.ToPaintGeometry(OuterOffset, OuterSize),
        &OuterBrush,
        ESlateDrawEffect::None,
        OuterColor);

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId + 4,
        AllottedGeometry.ToPaintGeometry(MiddleOffset, MiddleSize),
        &MiddleBrush,
        ESlateDrawEffect::None,
        InnerColor);

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId + 5,
        AllottedGeometry.ToPaintGeometry(InnerOffset, InnerSize),
        &InnerBrush,
        ESlateDrawEffect::None,
        FillColor);

    const FVector2D HighlightSize(InnerSize.X, InnerSize.Y * 0.55f);
    const FVector2D HighlightOffset = InnerOffset;
    const FVector2D LowlightSize(InnerSize.X, InnerSize.Y * 0.45f);
    const FVector2D LowlightOffset = InnerOffset + FVector2D(0.f, InnerSize.Y * 0.55f);

    const FLinearColor HighlightColor = (FillColor * 1.18f).CopyWithNewOpacity(0.35f * FillColor.A);
    const FLinearColor LowlightColor  = (FillColor * 0.72f).CopyWithNewOpacity(0.4f * FillColor.A);

    FSlateRoundedBoxBrush HighlightBrush(HighlightColor, InnerRadius);
    FSlateRoundedBoxBrush LowlightBrush(LowlightColor, InnerRadius);

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId + 6,
        AllottedGeometry.ToPaintGeometry(HighlightOffset, HighlightSize),
        &HighlightBrush,
        ESlateDrawEffect::None,
        HighlightColor);

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId + 7,
        AllottedGeometry.ToPaintGeometry(LowlightOffset, LowlightSize),
        &LowlightBrush,
        ESlateDrawEffect::None,
        LowlightColor);

    return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId + 8, InWidgetStyle, bParentEnabled);
}
