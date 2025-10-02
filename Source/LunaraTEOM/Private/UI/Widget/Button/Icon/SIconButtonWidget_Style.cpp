#include "UI/Widget/Button/Icon/SIconButtonWidget.h"

#include "UI/Style/LunaraTeomSlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleAsset.h"
#include "Styling/CoreStyle.h"

void SIconButtonWidget::ResolveStyle(const FLunaraTeomSlateStyle* InStyle)
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

    OuterStrokeBase = StyleRef->AccentColor;
    InnerStrokeBase = StyleRef->SecondaryColor;
    FillBase        = StyleRef->PrimaryColor;
}

FLinearColor SIconButtonWidget::GetOuterStrokeColor() const
{
    const FLinearColor HoverColor = OuterStrokeBase * 1.08f;
    const FLinearColor PressColor = OuterStrokeBase * 0.9f;

    FLinearColor Mixed = FMath::Lerp(OuterStrokeBase, HoverColor, GetHoverAlpha());
    Mixed = FMath::Lerp(Mixed, PressColor, GetPressAlpha());
    Mixed.A = OuterStrokeBase.A;

    return Mixed;
}

FLinearColor SIconButtonWidget::GetInnerStrokeColor() const
{
    const FLinearColor HoverColor = InnerStrokeBase * 1.05f;
    const FLinearColor PressColor = InnerStrokeBase * 0.88f;

    FLinearColor Mixed = FMath::Lerp(InnerStrokeBase, HoverColor, GetHoverAlpha());
    Mixed = FMath::Lerp(Mixed, PressColor, GetPressAlpha());
    Mixed.A = InnerStrokeBase.A;

    return Mixed;
}

FLinearColor SIconButtonWidget::GetFillColor() const
{
    const FLinearColor HoverColor = FillBase * 1.12f;
    const FLinearColor PressColor = FillBase * 0.78f;

    FLinearColor Mixed = FMath::Lerp(FillBase, HoverColor, GetHoverAlpha());
    Mixed = FMath::Lerp(Mixed, PressColor, GetPressAlpha());
    Mixed.A = FillBase.A;

    return Mixed;
}
