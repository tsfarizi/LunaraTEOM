#include "Widget/STextButtonWidget.h"
#include "Widget/SBeveledBorder.h"

#include "UI/LunaraTeomSlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleAsset.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"

#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"

#include "SlateOptMacros.h"

#include "Engine/Font.h"
#include "Engine/FontFace.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STextButtonWidget::Construct(const FArguments& InArgs)
{
    OnClicked = InArgs._OnClicked;
    ResolveStyle(InArgs._Style);

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

    const FLinearColor DeepBlue = StyleRef ? StyleRef->DeepBlue : FLinearColor(0.08f, 0.12f, 0.20f);
    const FLinearColor MoonGold = StyleRef ? StyleRef->MoonGold : FLinearColor(0.72f, 0.54f, 0.23f);
    const FLinearColor BaseGold = StyleRef ? StyleRef->BaseGold : FLinearColor(0.66f, 0.48f, 0.18f);

    constexpr float OuterBevelValue     = 8.f;
    constexpr float OuterNotch         = 10.f;
    constexpr float OuterNotchSize     = 20.f;

    constexpr float MidBevelValue      = 6.f;
    constexpr float MidNotch          = 8.f;
    constexpr float MidNotchSize      = 20.f;

    constexpr float InnerBevelValue    = 4.f;
    constexpr float InnerNotch        = 8.f;
    constexpr float InnerNotchSize    = 16.f;

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
                .NotchDepth(OuterNotch)
                .NotchHeight(OuterNotchSize)
                .Color(DeepBlue)
                .Padding(FMargin(4.f))
                [
                    SNew(SBeveledBorder)
                    .Bevel(MidBevelValue)
                    .NotchDepth(MidNotch)
                    .NotchHeight(MidNotchSize)
                    .Color(MoonGold * 0.9f)
                    .Padding(FMargin(2.f))
                    [
                        SNew(SBeveledBorder)
                        .Bevel(InnerBevelValue)
                        .NotchDepth(InnerNotch)
                        .NotchHeight(InnerNotchSize)
                        .Color(BaseGold)
                        .Padding(FMargin(4.f))
                        [
                            SAssignNew(InnerPanel, SBeveledBorder)
                            .Bevel(InnerBevelValue * 0.75f)
                            .NotchDepth(InnerNotch * 0.75f)
                            .NotchHeight(InnerNotchSize * 0.85f)
                            .Color(TAttribute<FLinearColor>::Create(
                                TAttribute<FLinearColor>::FGetter::CreateSP(this, &STextButtonWidget::GetInnerPanelColor)))
                            .Padding(FMargin(14.f, 8.f))
                            [
                                SAssignNew(LabelText, STextBlock)
                                .Text(InArgs._Label)
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

void STextButtonWidget::ResolveStyle(const FLunaraTeomSlateStyle* InStyle)
{
    StyleRef = InStyle;

    if (!StyleRef)
    {
        if (USlateWidgetStyleAsset* StyleAsset =
            LoadObject<USlateWidgetStyleAsset>(nullptr, TEXT("/Game/UI/Styles/LunaraStyle.LunaraStyle")))
        {
            if (const FLunaraTeomSlateStyle* Resolved = StyleAsset->GetStyle<FLunaraTeomSlateStyle>())
            {
                StyleRef = Resolved;
            }
        }
    }

    if (!StyleRef)
    {
        StyleRef = &FLunaraTeomSlateStyle::GetDefault();
    }

    TextColor = StyleRef ? FSlateColor(StyleRef->DeepBlue) : FSlateColor(FLinearColor::White);
}

void STextButtonWidget::RebuildLabelFont()
{
    LabelFont = FSlateFontInfo();

    UObject* FontObj = ExplicitFontObject.IsNull() ? nullptr : ExplicitFontObject.LoadSynchronous();
    if (UFontFace* Face = Cast<UFontFace>(FontObj))
    {
        LabelFont = FSlateFontInfo(Face, ExplicitFontSize);
    }
    else if (UFont* Font = Cast<UFont>(FontObj))
    {
        LabelFont = FSlateFontInfo(Font, ExplicitFontSize);
    }
    else if (StyleRef)
    {
        LabelFont = StyleRef->CinzelBold;
        LabelFont.Size = ExplicitFontSize > 0 ? ExplicitFontSize : StyleRef->CinzelBold.Size;
    }

    if (!LabelFont.HasValidFont())
    {
        LabelFont = FCoreStyle::GetDefaultFontStyle("Bold", ExplicitFontSize);
    }

    if (LabelText.IsValid())
    {
        LabelText->SetFont(LabelFont);
    }
}

void STextButtonWidget::HandleHovered()
{
    HoverSeq.Play(AsShared());
}

void STextButtonWidget::HandleUnhovered()
{
    HoverSeq.Reverse();
}

void STextButtonWidget::HandlePressed()
{
    PressSeq.Play(AsShared());
}

void STextButtonWidget::HandleReleased()
{
    PressSeq.Reverse();
}

FLinearColor STextButtonWidget::GetInnerPanelColor() const
{
    const FLinearColor BaseColor = StyleRef ? StyleRef->HighlightGold : FLinearColor(0.85f, 0.65f, 0.30f);
    const FLinearColor HoverColor = BaseColor * 1.12f;
    const FLinearColor PressColor = BaseColor * 0.8f;

    FLinearColor Mixed = FMath::Lerp(BaseColor, HoverColor, GetHoverAlpha());
    Mixed = FMath::Lerp(Mixed, PressColor, GetPressAlpha());
    Mixed.A = BaseColor.A;
    return Mixed;
}

FSlateColor STextButtonWidget::GetLabelColor() const
{
    const FLinearColor Base = TextColor.GetSpecifiedColor();
    const FLinearColor Hover = Base + FLinearColor(0.08f, 0.08f, 0.08f, 0.f);
    const FLinearColor Press = Base * 0.85f;

    FLinearColor Mixed = FMath::Lerp(Base, Hover, GetHoverAlpha());
    Mixed = FMath::Lerp(Mixed, Press, GetPressAlpha());
    Mixed.A = Base.A;

    return FSlateColor(Mixed);
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
