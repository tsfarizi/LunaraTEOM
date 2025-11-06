#include "UI/Widget/Button/Text/STextButtonWidget.h"

#include "UI/Style/LunaraTeomSlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleAsset.h"
#include "Styling/CoreStyle.h"

#include "Widgets/Text/STextBlock.h"

#include "Engine/Font.h"
#include "Engine/FontFace.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInterface.h"

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

    TextColor = StyleRef ? FSlateColor(StyleRef->PrimaryColor) : FSlateColor(FLinearColor::White);
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

FLinearColor STextButtonWidget::GetInnerPanelColor() const
{
    const FLinearColor BaseColor = StyleRef ? StyleRef->AccentColor : FLinearColor(0.85f, 0.65f, 0.30f);
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
