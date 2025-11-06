#include "UI/Widget/Window/Main/SMainWindowWidget.h"

#include "UI/Style/LunaraTeomSlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleAsset.h"
#include "Styling/CoreStyle.h"

#include "UI/Widget/Window/Shared/SWindowTitleBar.h"

#include "Engine/Font.h"
#include "Engine/FontFace.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInterface.h"

void SMainWindowWidget::ResolveStyle(const FLunaraTeomSlateStyle* InStyle)
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
}

void SMainWindowWidget::RefreshTitleFont()
{
    TitleFont = FSlateFontInfo();

    UObject* FontObj = TitleFontObject.IsNull() ? nullptr : TitleFontObject.LoadSynchronous();
    if (UFontFace* Face = Cast<UFontFace>(FontObj))
    {
        TitleFont = FSlateFontInfo(Face, TitleFontSize);
    }
    else if (UFont* Font = Cast<UFont>(FontObj))
    {
        TitleFont = FSlateFontInfo(Font, TitleFontSize);
    }

    if (!TitleFont.HasValidFont())
    {
        TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", TitleFontSize);
    }

    if (TitleBar.IsValid())
    {
        TitleBar->SetTitleFont(TitleFont);
    }
}

void SMainWindowWidget::RefreshIconBrush()
{
    IconBrush = FSlateBrush();
    IconBrush.DrawAs = ESlateBrushDrawType::Image;

    UObject* IconObjResolved = IconObject.IsNull() ? nullptr : IconObject.LoadSynchronous();
    constexpr float MaxIconDimension = 28.f;

    if (UTexture2D* Tex = Cast<UTexture2D>(IconObjResolved))
    {
        FVector2D Size(static_cast<float>(Tex->GetSizeX()), static_cast<float>(Tex->GetSizeY()));
        if (Size.GetMax() <= 0.f)
        {
            Size = FVector2D(MaxIconDimension, MaxIconDimension);
        }

        const float Scale = FMath::Min(MaxIconDimension / Size.X, MaxIconDimension / Size.Y);
        if (Scale < 1.f)
        {
            Size *= Scale;
        }

        IconBrush.SetResourceObject(Tex);
        IconBrush.ImageSize = Size;
    }
    else if (UMaterialInterface* Mat = Cast<UMaterialInterface>(IconObjResolved))
    {
        IconBrush.SetResourceObject(Mat);
        IconBrush.ImageSize = FVector2D(MaxIconDimension, MaxIconDimension);
    }
    else
    {
        IconBrush.SetResourceObject(nullptr);
        IconBrush.ImageSize = FVector2D::ZeroVector;
    }

    if (TitleBar.IsValid())
    {
        TitleBar->SetIconBrush(IconBrush.GetResourceObject() ? &IconBrush : nullptr);
    }
}
