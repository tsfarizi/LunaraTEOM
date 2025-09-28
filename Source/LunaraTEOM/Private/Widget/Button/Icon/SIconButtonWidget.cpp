#include "Widget/Button/Icon/SIconButtonWidget.h"

#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"

#include "Engine/Texture2D.h"
#include "Materials/MaterialInterface.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SIconButtonWidget::Construct(const FArguments& InArgs)
{
    OnClicked = InArgs._OnClicked;

    ResolveStyle(InArgs._Style);

    IconAsset      = InArgs._IconAsset;
    ButtonDiameter = FMath::Max(12.f, InArgs._Diameter);

    HoverSeq = FCurveSequence();
    HoverAlpha = HoverSeq.AddCurve(0.f, 0.12f, ECurveEaseFunction::QuadInOut);
    HoverSeq.JumpToStart();

    PressSeq = FCurveSequence();
    PressAlpha = PressSeq.AddCurve(0.f, 0.08f, ECurveEaseFunction::QuadInOut);
    PressSeq.JumpToStart();

    ClickCycleSeq = FCurveSequence();
    constexpr float ShrinkDuration = 0.18f;
    constexpr float PauseDuration = 0.05f;
    constexpr float ReappearDuration = 0.22f;
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
    RefreshIconBrush();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SIconButtonWidget::SetIconAsset(TSoftObjectPtr<UObject> InIconAsset)
{
    IconAsset = InIconAsset;
    RefreshIconBrush();
}

void SIconButtonWidget::SetDiameter(float InDiameter)
{
    ButtonDiameter = FMath::Max(12.f, InDiameter);

    if (ButtonBox.IsValid())
    {
        ButtonBox->SetWidthOverride(ButtonDiameter);
        ButtonBox->SetHeightOverride(ButtonDiameter);
    }

    if (IconBox.IsValid())
    {
        IconBox->SetWidthOverride(ButtonDiameter * IconScale);
        IconBox->SetHeightOverride(ButtonDiameter * IconScale);
    }

    RefreshIconBrush();
}

FReply SIconButtonWidget::HandleClick()
{
    ClickCycleSeq.JumpToStart();
    ClickCycleSeq.Play(AsShared());

    if (OnClicked.IsBound())
    {
        return OnClicked.Execute();
    }

    return FReply::Handled();
}

void SIconButtonWidget::HandleHovered()
{
    HoverSeq.Play(AsShared());
}

void SIconButtonWidget::HandleUnhovered()
{
    HoverSeq.Reverse();
}

void SIconButtonWidget::HandlePressed()
{
    PressSeq.Play(AsShared());
}

void SIconButtonWidget::HandleReleased()
{
    PressSeq.Reverse();
}

float SIconButtonWidget::GetHoverAlpha() const
{
    return HoverAlpha.GetLerp();
}

float SIconButtonWidget::GetPressAlpha() const
{
    return PressAlpha.GetLerp();
}

float SIconButtonWidget::GetClickCycleScale() const
{
    const float ShrinkLerp = ClickShrinkAlpha.GetLerp();
    const float ReturnLerp = ClickReturnAlpha.GetLerp();

    if (ReturnLerp > KINDA_SMALL_NUMBER)
    {
        return FMath::Clamp(FMath::Lerp(0.15f, 1.f, ReturnLerp), 0.f, 1.f);
    }

    return FMath::Clamp(FMath::Lerp(1.f, 0.f, ShrinkLerp), 0.f, 1.f);
}

TOptional<FSlateRenderTransform> SIconButtonWidget::GetButtonRenderTransform() const
{
    constexpr float PressScaleAmount = 0.08f;

    const float PressScale = 1.f + (GetPressAlpha() * PressScaleAmount);
    const float CycleScale = GetClickCycleScale();

    const float FinalScale = FMath::Max(0.f, PressScale * CycleScale);

    return FSlateRenderTransform(FScale2D(FinalScale, FinalScale));
}

void SIconButtonWidget::RefreshIconBrush()
{
    IconBrush = FSlateBrush();
    IconBrush.DrawAs = ESlateBrushDrawType::Image;

    UObject* IconObject = IconAsset.IsNull() ? nullptr : IconAsset.LoadSynchronous();

    const float MaxIconSize = ButtonDiameter * IconScale;

    if (UTexture2D* Texture = Cast<UTexture2D>(IconObject))
    {
        FVector2D Size(static_cast<float>(Texture->GetSizeX()), static_cast<float>(Texture->GetSizeY()));
        if (Size.GetMax() <= 0.f)
        {
            Size = FVector2D(MaxIconSize, MaxIconSize);
        }

        const float Scale = FMath::Min(MaxIconSize / Size.X, MaxIconSize / Size.Y);
        if (Scale < 1.f)
        {
            Size *= Scale;
        }

        IconBrush.SetResourceObject(Texture);
        IconBrush.ImageSize = Size;
    }
    else if (UMaterialInterface* Material = Cast<UMaterialInterface>(IconObject))
    {
        IconBrush.SetResourceObject(Material);
        IconBrush.ImageSize = FVector2D(MaxIconSize, MaxIconSize);
    }
    else
    {
        IconBrush.SetResourceObject(nullptr);
        IconBrush.ImageSize = FVector2D::ZeroVector;
    }

    if (IconImage.IsValid())
    {
        const bool bHasIcon = IconBrush.GetResourceObject() != nullptr;
        IconImage->SetImage(bHasIcon ? &IconBrush : nullptr);
        IconImage->SetVisibility(bHasIcon ? EVisibility::Visible : EVisibility::Collapsed);
    }
}
