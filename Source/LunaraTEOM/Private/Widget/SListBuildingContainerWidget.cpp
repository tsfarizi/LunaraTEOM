#include "Widget/SListBuildingContainerWidget.h"

#include "UI/LunaraTeomSlateWidgetStyle.h"
#include "Widget/SBeveledBorder.h"

#include "SlateOptMacros.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"
#include "Layout/Clipping.h"
#include "Brushes/SlateColorBrush.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBackgroundBlur.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

namespace ListBuildingContainerWidgetPrivate
{
    static const FSlateColorBrush GlassTintBrush(FLinearColor(1.f, 1.f, 1.f, 0.08f));
    static const FSlateColorBrush GlassSheenBrush(FLinearColor(1.f, 1.f, 1.f, 0.18f));
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SListBuildingContainerWidget::Construct(const FArguments& InArgs)
{
    const FLunaraTeomSlateStyle& Style = FLunaraTeomSlateStyle::GetDefault();

    FLinearColor OuterBorderColor = Style.AccentColor;
    OuterBorderColor.A = 0.45f;

    FLinearColor InnerBorderColor = FLinearColor::LerpUsingHSV(Style.PrimaryColor, FLinearColor::White, 0.2f);
    InnerBorderColor.A = 0.35f;

    FLinearColor BackgroundColor = FLinearColor::LerpUsingHSV(Style.PrimaryColor, FLinearColor::White, 0.65f);
    BackgroundColor.A = 0.18f;

    const FLinearColor GlassTintColor = BackgroundColor.CopyWithNewOpacity(0.16f);
    const FLinearColor GlassSheenColor(1.f, 1.f, 1.f, 0.28f);

    const FVector4 GlassCornerRadius(22.f, 22.f, 22.f, 22.f);

    // Kontainer utama isi (custom content + baris tombol)
    TSharedRef<SVerticalBox> ContentContainer = SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(CustomContentSlot, SBox)
            .Visibility(EVisibility::Collapsed)
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Fill)
        .Padding(FMargin(0.f, 8.f, 0.f, 0.f))
        [
            SAssignNew(ButtonScrollBox, SScrollBox)
            .Orientation(EOrientation::Orient_Horizontal)
            .ScrollBarVisibility(EVisibility::Collapsed)
            .AllowOverscroll(EAllowOverscroll::No)
            .ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
            .Clipping(EWidgetClipping::ClipToBounds)
            + SScrollBox::Slot()
            [
                SAssignNew(ButtonListContainer, SHorizontalBox)
            ]
        ];

    TSharedRef<SOverlay> GlassSurface = SNew(SOverlay)
        + SOverlay::Slot()
        [
            SNew(SImage)
            .Image(&ListBuildingContainerWidgetPrivate::GlassTintBrush)
            .ColorAndOpacity(GlassTintColor)
        ]
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Top)
        [
            SNew(SBox)
            .HeightOverride(72.f)
            [
                SNew(SImage)
                .Image(&ListBuildingContainerWidgetPrivate::GlassSheenBrush)
                .ColorAndOpacity(GlassSheenColor)
            ]
        ]
        + SOverlay::Slot()
        [
            SNew(SBorder)
            .Padding(FMargin(12.f, 10.f, 12.f, 16.f))
            .BorderImage(FCoreStyle::Get().GetBrush("NoBrush"))
            [
                ContentContainer
            ]
        ];

    SAssignNew(ContentBorder, SBeveledBorder)
        .Bevel(6.f)
        .NotchDepth(4.f)
        .NotchHeight(10.f)
        .Color(BackgroundColor)
        .Padding(FMargin(10.f, 10.f, 10.f, 14.f))
        [
            SNew(SBackgroundBlur)
            .BlurStrength(12.f)
            .BlurRadius(32)
            .bApplyAlphaToBlur(true)
            .CornerRadius(GlassCornerRadius)
            .LowQualityFallbackBrush(&ListBuildingContainerWidgetPrivate::GlassTintBrush)
            [
                GlassSurface
            ]
        ];

    ChildSlot
    [
        SNew(SBeveledBorder)
        .Bevel(10.f)
        .NotchDepth(8.f)
        .NotchHeight(18.f)
        .RightNotchCount(1)
        .LeftNotchCount(1)
        .Color(OuterBorderColor)
        .Padding(FMargin(4.f))
        [
            SNew(SBeveledBorder)
            .Bevel(8.f)
            .NotchDepth(6.f)
            .NotchHeight(14.f)
            .RightNotchCount(1)
            .LeftNotchCount(1)
            .Color(InnerBorderColor)
            .Padding(FMargin(6.f))
            [
                ContentBorder.ToSharedRef()
            ]
        ]
    ];

    if (InArgs._Content.Widget != SNullWidget::NullWidget)
    {
        SetContent(InArgs._Content.Widget);
    }
    else
    {
        ClearContent();
    }

    RebuildButtonList();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SListBuildingContainerWidget::SetContent(const TSharedRef<SWidget>& InContent)
{
    CachedContent = InContent;

    if (CustomContentSlot.IsValid())
    {
        CustomContentSlot->SetContent(InContent);
        CustomContentSlot->SetVisibility(EVisibility::Visible);
    }

    RebuildButtonList();
}

void SListBuildingContainerWidget::ClearContent()
{
    CachedContent.Reset();

    if (CustomContentSlot.IsValid())
    {
        CustomContentSlot->SetContent(SNullWidget::NullWidget);
        CustomContentSlot->SetVisibility(EVisibility::Collapsed);
    }

    RebuildButtonList();
}

void SListBuildingContainerWidget::SetButtonItems(const TArray<FListBuildingButtonItem>& InItems)
{
    ButtonItems = InItems;
    RebuildButtonList();
}

void SListBuildingContainerWidget::RebuildButtonList()
{
    if (!ButtonListContainer.IsValid())
    {
        return;
    }

    const FLunaraTeomSlateStyle& Style = FLunaraTeomSlateStyle::GetDefault();
    const bool bHasCustomContent = CachedContent.IsValid();

    ButtonListContainer->ClearChildren();
    ButtonIconBrushes.Reset(ButtonItems.Num());

    for (int32 ItemIndex = 0; ItemIndex < ButtonItems.Num(); ++ItemIndex)
    {
        const FListBuildingButtonItem& Item = ButtonItems[ItemIndex];

        TSharedPtr<FSlateBrush> IconBrush;
        if (ShouldDisplayIcon(Item.Icon))
        {
            IconBrush = MakeShared<FSlateBrush>(Item.Icon);
            ButtonIconBrushes.Add(IconBrush);
        }
        else
        {
            ButtonIconBrushes.Add(nullptr);
        }

        TSharedRef<SHorizontalBox> ButtonContent = SNew(SHorizontalBox);

        if (IconBrush.IsValid())
        {
            const TSharedPtr<FSlateBrush>& StoredBrush = ButtonIconBrushes.Last();

            ButtonContent->AddSlot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(FMargin(0.f, 0.f, 8.f, 0.f))
            [
                SNew(SImage)
                .Image(StoredBrush.Get())
                .ColorAndOpacity(FLinearColor::White)
            ];
        }

        ButtonContent->AddSlot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(Item.Label)
            .Font(Style.CinzelRegular)
            .ColorAndOpacity(FSlateColor(FLinearColor::White))
            .ShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.6f))
            .ShadowOffset(FVector2D(1.f, 1.f))
        ];

        const float LeftPadding = ItemIndex > 0 ? 12.f : (bHasCustomContent ? 8.f : 0.f);

        ButtonListContainer->AddSlot()
        .AutoWidth()
        .Padding(FMargin(LeftPadding, 0.f, 0.f, 0.f))
        [
            SNew(SButton)
            .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
            .HAlign(HAlign_Left)
            .ContentPadding(FMargin(12.f, 10.f))
            [
                ButtonContent
            ]
        ];
    }

    if (ButtonScrollBox.IsValid())
    {
        ButtonScrollBox->ScrollToStart();
    }
}

bool SListBuildingContainerWidget::ShouldDisplayIcon(const FSlateBrush& Brush) const
{
    return Brush.GetResourceObject() != nullptr
        || Brush.GetImageSize() != FVector2D::ZeroVector
        || Brush.DrawAs != ESlateBrushDrawType::NoDrawType
        || Brush.GetResourceName().IsValid();
}
