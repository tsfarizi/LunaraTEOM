#include "UI/Widget/Button/Text/STextButtonWidget.h"

#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STextButtonWidget::Construct(const FArguments& InArgs)
{
    OnClicked   = InArgs._OnClicked;
    OnPressed   = InArgs._OnPressed;
    OnReleased  = InArgs._OnReleased;
    OnHovered   = InArgs._OnHovered;
    OnUnhovered = InArgs._OnUnhovered;
    ResolveStyle(InArgs._Style);

    LabelAttr         = InArgs._Label;
    ExplicitFontObject = InArgs._FontObject;
    ExplicitFontSize   = FMath::Max(1, InArgs._FontSize);

    HoverSeq = FCurveSequence();
    HoverAlpha = HoverSeq.AddCurve(0.f, 0.15f, ECurveEaseFunction::QuadInOut);
    HoverSeq.JumpToStart();

    PressSeq = FCurveSequence();
    PressAlpha = PressSeq.AddCurve(0.f, 0.08f, ECurveEaseFunction::QuadInOut);
    PressSeq.JumpToStart();

    ClickCycleSeq = FCurveSequence();
    constexpr float ShrinkDuration = 0.2f;
    constexpr float PauseDuration = 0.05f;
    constexpr float ReappearDuration = 0.25f;
    ClickShrinkAlpha = ClickCycleSeq.AddCurve(0.f, ShrinkDuration, ECurveEaseFunction::CubicIn);
    ClickReturnAlpha = ClickCycleSeq.AddCurve(ShrinkDuration + PauseDuration, ReappearDuration, ECurveEaseFunction::QuadOut);

    ButtonStyle = FButtonStyle()
        .SetNormal(FSlateNoResource())
        .SetHovered(FSlateNoResource())
        .SetPressed(FSlateNoResource())
        .SetDisabled(FSlateNoResource());
    ButtonStyle.NormalPadding  = FMargin(0.f);
    ButtonStyle.PressedPadding = FMargin(0.f);

    BuildLayout();
    RebuildLabelFont();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STextButtonWidget::SetExplicitFont(TSoftObjectPtr<UObject> InFontObject, int32 InFontSize)
{
    ExplicitFontObject = InFontObject;
    ExplicitFontSize   = FMath::Max(1, InFontSize);
    RebuildLabelFont();
}

FReply STextButtonWidget::HandleClick()
{
    ClickCycleSeq.JumpToStart();
    ClickCycleSeq.Play(AsShared());

    if (OnClicked.IsBound())
    {
        return OnClicked.Execute();
    }
    return FReply::Handled();
}

void STextButtonWidget::HandleHovered()
{
    HoverSeq.Play(AsShared());
    OnHovered.ExecuteIfBound();
}

void STextButtonWidget::HandleUnhovered()
{
    HoverSeq.Reverse();
    OnUnhovered.ExecuteIfBound();
}

void STextButtonWidget::HandlePressed()
{
    PressSeq.Play(AsShared());
    OnPressed.ExecuteIfBound();
}

void STextButtonWidget::HandleReleased()
{
    PressSeq.Reverse();
    OnReleased.ExecuteIfBound();
}

float STextButtonWidget::GetHoverAlpha() const
{
    return HoverAlpha.GetLerp();
}

float STextButtonWidget::GetPressAlpha() const
{
    return PressAlpha.GetLerp();
}

float STextButtonWidget::GetClickCycleScale() const
{
    const float ShrinkLerp = ClickShrinkAlpha.GetLerp();
    const float ReturnLerp = ClickReturnAlpha.GetLerp();

    if (ReturnLerp > KINDA_SMALL_NUMBER)
    {
        return FMath::Clamp(FMath::Lerp(0.1f, 1.f, ReturnLerp), 0.f, 1.f);
    }

    return FMath::Clamp(FMath::Lerp(1.f, 0.f, ShrinkLerp), 0.f, 1.f);
}

TOptional<FSlateRenderTransform> STextButtonWidget::GetButtonRenderTransform() const
{
    constexpr float PressScaleAmount = 0.10f;

    const float PressScale = 1.f + (GetPressAlpha() * PressScaleAmount);
    const float CycleScale = GetClickCycleScale();

    const float FinalScale = FMath::Max(0.f, PressScale * CycleScale);

    return FSlateRenderTransform(FScale2D(FinalScale, FinalScale));
}
