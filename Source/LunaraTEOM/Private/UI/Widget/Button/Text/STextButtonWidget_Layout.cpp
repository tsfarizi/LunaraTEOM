#include "UI/Widget/Button/Text/STextButtonWidget.h"

#include "UI/Widget/SBeveledBorder.h"

#include "UI/Style/LunaraTeomSlateWidgetStyle.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"

#include "Styling/CoreStyle.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STextButtonWidget::BuildLayout()
{
    const FLinearColor PrimaryColor = StyleRef ? StyleRef->PrimaryColor : FLinearColor(0.08f, 0.12f, 0.20f);
    const FLinearColor SecondaryColor = StyleRef ? StyleRef->SecondaryColor : FLinearColor(0.72f, 0.54f, 0.23f);
    const FLinearColor SurfaceColor = StyleRef ? StyleRef->SurfaceColor : FLinearColor(0.66f, 0.48f, 0.18f);

    constexpr float OuterBevelValue     = 8.f;
    constexpr float OuterNotchDepth     = 10.f;
    constexpr float OuterNotchHeight    = 20.f;

    constexpr float MidBevelValue       = 6.f;
    constexpr float MidNotchDepth       = 8.f;
    constexpr float MidNotchHeight      = 20.f;

    constexpr float InnerBevelValue     = 4.f;
    constexpr float InnerNotchDepth     = 8.f;
    constexpr float InnerNotchHeight    = 16.f;

    ChildSlot
    [
        SNew(SBox)
        .RenderTransformPivot(FVector2D(0.5f, 0.5f))
        .RenderTransform(this, &STextButtonWidget::GetButtonRenderTransform)
        [
            SNew(SButton)
            .ButtonStyle(&ButtonStyle)
            .ContentPadding(0.f)
            .OnClicked(this, &STextButtonWidget::HandleClick)
            .OnHovered(this, &STextButtonWidget::HandleHovered)
            .OnUnhovered(this, &STextButtonWidget::HandleUnhovered)
            .OnPressed(this, &STextButtonWidget::HandlePressed)
            .OnReleased(this, &STextButtonWidget::HandleReleased)
            [
                SNew(SBeveledBorder)
                .Bevel(OuterBevelValue)
                .NotchDepth(OuterNotchDepth)
                .NotchHeight(OuterNotchHeight)
                .Color(PrimaryColor)
                .Padding(FMargin(4.f))
                [
                    SNew(SBeveledBorder)
                    .Bevel(MidBevelValue)
                    .NotchDepth(MidNotchDepth)
                    .NotchHeight(MidNotchHeight)
                    .Color(SecondaryColor * 0.9f)
                    .Padding(FMargin(2.f))
                    [
                        SNew(SBeveledBorder)
                        .Bevel(InnerBevelValue)
                        .NotchDepth(InnerNotchDepth)
                        .NotchHeight(InnerNotchHeight)
                        .Color(SurfaceColor)
                        .Padding(FMargin(4.f))
                        [
                            SAssignNew(InnerPanel, SBeveledBorder)
                            .Bevel(InnerBevelValue * 0.75f)
                            .NotchDepth(InnerNotchDepth * 0.75f)
                            .NotchHeight(InnerNotchHeight * 0.85f)
                            .Color(TAttribute<FLinearColor>::Create(
                                TAttribute<FLinearColor>::FGetter::CreateSP(this, &STextButtonWidget::GetInnerPanelColor)))
                            .Padding(FMargin(14.f, 8.f))
                            [
                                SAssignNew(LabelText, STextBlock)
                                .Text(LabelAttr)
                                .Justification(ETextJustify::Center)
                                .ColorAndOpacity(TAttribute<FSlateColor>::Create(
                                    TAttribute<FSlateColor>::FGetter::CreateSP(this, &STextButtonWidget::GetLabelColor)))
                                .Font(LabelFont)
                                .ShadowOffset(FVector2D(1.f, 1.f))
                                .ShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.5f))
                            ]
                        ]
                    ]
                ]
            ]
        ]
    ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
