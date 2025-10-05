#include "UI/Widget/SListBuildingContainerWidget.h"

#include "UI/Style/LunaraTeomSlateWidgetStyle.h"
#include "UI/Widget/SBeveledBorder.h"

#include "SlateOptMacros.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"
#include "Layout/Clipping.h"
#include "Brushes/SlateColorBrush.h"
#include "Brushes/SlateRoundedBoxBrush.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBackgroundBlur.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SScaleBox.h"
#include "InputCoreTypes.h"

namespace ListBuildingContainerWidgetPrivate
{
    static const FVector4 GlassCornerRadius(22.f, 22.f, 22.f, 22.f);
    static const FSlateRoundedBoxBrush GlassTintBrush(FLinearColor::White, GlassCornerRadius);
    static const FSlateRoundedBoxBrush GlassSheenBrush(
        FLinearColor::White,
        FVector4(22.f, 22.f, 6.f, 6.f)
    );
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SListBuildingContainerWidget::Construct(const FArguments& InArgs)
{
    const FLunaraTeomSlateStyle& Style = FLunaraTeomSlateStyle::GetDefault();

    const FLinearColor OuterBorderColor = Style.AccentColor.CopyWithNewOpacity(0.32f);

    const FLinearColor InnerBorderBase = FLinearColor::LerpUsingHSV(Style.PrimaryColor, Style.HighlightColor, 0.2f);
    const FLinearColor InnerBorderColor = InnerBorderBase.CopyWithNewOpacity(0.12f);

    const FLinearColor GlassBaseColor = Style.BackgroundColor.CopyWithNewOpacity(0.18f);
    const FLinearColor GlassBackdropColor = Style.BackgroundColor.CopyWithNewOpacity(0.08f);

    const FMargin OuterBorderRimPadding(1.5f);
    const FLinearColor OuterBorderRimColor = Style.HighlightColor.CopyWithNewOpacity(0.22f);

    const FLinearColor GlassTintColor = Style.BackgroundColor.CopyWithNewOpacity(0.12f);
    const FLinearColor GlassSheenColor = Style.HighlightColor.CopyWithNewOpacity(0.35f);

    const FVector4 GlassCornerRadius(22.f, 22.f, 22.f, 22.f);

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
            SAssignNew(ScrollInteractionLayer, SBorder)
            .Padding(FMargin(0.f))
            .BorderImage(FCoreStyle::Get().GetBrush("NoBrush"))
            .OnMouseButtonDown(this, &SListBuildingContainerWidget::HandleScrollAreaMouseButtonDown)
            .OnMouseButtonUp(this, &SListBuildingContainerWidget::HandleScrollAreaMouseButtonUp)
            .OnMouseMove(this, &SListBuildingContainerWidget::HandleScrollAreaMouseMove)
            [
                SAssignNew(ButtonScrollBox, SScrollBox)
                .Orientation(EOrientation::Orient_Horizontal)
                .ScrollBarVisibility(EVisibility::Collapsed)
                .AllowOverscroll(EAllowOverscroll::No)
                .ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
                .AnimateWheelScrolling(true)
                .Clipping(EWidgetClipping::ClipToBounds)
                + SScrollBox::Slot()
                [
                    SAssignNew(ButtonListContainer, SHorizontalBox)
                ]
            ]
        ];

    TSharedRef<SOverlay> GlassSurface = SNew(SOverlay)
        + SOverlay::Slot()
        [
            SNew(SImage)
            .Image(&ListBuildingContainerWidgetPrivate::GlassTintBrush)
            .ColorAndOpacity(GlassBackdropColor)
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
                SNew(SOverlay)
                + SOverlay::Slot()
                [
                    SNew(SImage)
                    .Image(&ListBuildingContainerWidgetPrivate::GlassTintBrush)
                    .ColorAndOpacity(GlassTintColor)
                ]
                + SOverlay::Slot()
                [
                    ContentContainer
                ]
            ]
        ];

    SAssignNew(ContentBorder, SBeveledBorder)
        .Bevel(6.f)
        .NotchDepth(4.f)
        .NotchHeight(10.f)
        .Color(GlassBaseColor)
        .Padding(FMargin(10.f, 10.f, 10.f, 14.f))
        [
            SNew(SBackgroundBlur)
            .BlurStrength(18.f)
            .BlurRadius(64)
            .bApplyAlphaToBlur(true)
            .CornerRadius(ListBuildingContainerWidgetPrivate::GlassCornerRadius)
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
            SNew(SOverlay)
            + SOverlay::Slot()
            .Padding(OuterBorderRimPadding)
            [
                SNew(SBeveledBorder)
                .Bevel(9.f)
                .NotchDepth(7.f)
                .NotchHeight(16.f)
                .RightNotchCount(1)
                .LeftNotchCount(1)
                .Color(OuterBorderRimColor)
            ]
            + SOverlay::Slot()
            .Padding(FMargin(2.f))
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
    ButtonListContainer->ClearChildren();
    ButtonIconBrushes.Reset(ButtonItems.Num());

    constexpr float ButtonBaseExtent = 116.f;
    const FMargin ButtonContentPadding(12.f);
    const float ButtonHorizontalPadding = ButtonContentPadding.GetTotalSpaceAlong<Orient_Horizontal>();
    const float IconBoxSize = 48.f;
    const FLinearColor ButtonFillColor(0.929f, 0.780f, 0.216f, 0.95f);
    const FLinearColor ButtonTintOverlay(0.929f, 0.780f, 0.216f, 0.18f);
    const FLinearColor ButtonSheenColor = Style.HighlightColor.CopyWithNewOpacity(0.2f);
    const float MinSlotSpacing = 20.f;

    const TAttribute<FOptionalSize> ButtonExtentAttribute = TAttribute<FOptionalSize>::Create(TAttribute<FOptionalSize>::FGetter::CreateSP(this, &SListBuildingContainerWidget::GetButtonExtent));
    const TAttribute<FOptionalSize> LabelWidthAttribute = TAttribute<FOptionalSize>::Create(TAttribute<FOptionalSize>::FGetter::CreateLambda([this, ButtonBaseExtent, ButtonHorizontalPadding]() -> FOptionalSize
    {
        const FOptionalSize ExtentOptional = GetButtonExtent();
        const float ExtentValue = ExtentOptional.IsSet() ? FMath::Max(1.f, ExtentOptional.Get()) : ButtonBaseExtent;
        return FOptionalSize(FMath::Max(1.f, ExtentValue - ButtonHorizontalPadding));
    }));

    const FOptionalSize ButtonExtentOptional = GetButtonExtent();
    const float ButtonExtentValue = ButtonExtentOptional.IsSet() ? FMath::Max(1.f, ButtonExtentOptional.Get()) : ButtonBaseExtent;
    const float ContainerWidth = GetContainerWidth();

    float ComputedGap = MinSlotSpacing;
    if (ButtonItems.Num() > 0 && ContainerWidth > KINDA_SMALL_NUMBER)
    {
        const float TotalButtonWidth = ButtonExtentValue * ButtonItems.Num();
        const float AvailableWidth = ContainerWidth - TotalButtonWidth;
        if (AvailableWidth > 0.f)
        {
            const float SpaceAroundGap = AvailableWidth / static_cast<float>(ButtonItems.Num() + 1);
            ComputedGap = FMath::Max(MinSlotSpacing, SpaceAroundGap);
        }
    }

    if (ButtonItems.Num() <= 0)
    {
        if (ButtonScrollBox.IsValid())
        {
            ButtonScrollBox->ScrollToStart();
        }
        return;
    }

    const float HalfGap = ComputedGap * 0.5f;

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

        TSharedRef<SVerticalBox> ButtonContent = SNew(SVerticalBox);

        if (IconBrush.IsValid())
        {
            const TSharedPtr<FSlateBrush>& StoredBrush = ButtonIconBrushes.Last();

            ButtonContent->AddSlot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            [
                SNew(SBox)
                .WidthOverride(IconBoxSize)
                .HeightOverride(IconBoxSize)
                [
                    SNew(SImage)
                    .Image(StoredBrush.Get())
                    .ColorAndOpacity(FLinearColor::White)
                ]
            ];
        }

        ButtonContent->AddSlot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        .Padding(FMargin(0.f, IconBrush.IsValid() ? 6.f : 0.f, 0.f, 0.f))
        [
            SNew(SBox)
            .WidthOverride(LabelWidthAttribute)
            [
                SNew(SScaleBox)
                .Stretch(EStretch::ScaleToFit)
                .StretchDirection(EStretchDirection::Both)
                [
                    SNew(STextBlock)
                    .Text(Item.Label)
                    .Font(Style.CinzelRegular)
                    .Justification(ETextJustify::Center)
                    .AutoWrapText(false)
                    .ColorAndOpacity(FSlateColor(FLinearColor::White))
                    .ShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.6f))
                    .ShadowOffset(FVector2D(1.f, 1.f))
                ]
            ]
        ];

        const float LeftPadding = (ItemIndex == 0) ? ComputedGap : HalfGap;
        const float RightPadding = (ItemIndex == ButtonItems.Num() - 1) ? ComputedGap : HalfGap;

        ButtonListContainer->AddSlot()
        .AutoWidth()
        .Padding(FMargin(LeftPadding, 0.f, RightPadding, 0.f))
        .HAlign(HAlign_Center)
        [
            SNew(SBox)
            .WidthOverride(ButtonExtentAttribute)
            .HeightOverride(ButtonExtentAttribute)
            [
                SNew(SButton)
                .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
                .ButtonColorAndOpacity(FLinearColor::Transparent)
                .IsFocusable(false)
                .ContentPadding(FMargin(0.f))
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                [
                    SNew(SOverlay)
                    + SOverlay::Slot()
                    [
                        SNew(SBeveledBorder)
                        .Bevel(7.f)
                        .NotchDepth(6.f)
                        .NotchHeight(14.f)
                        .RightNotchCount(1)
                        .LeftNotchCount(2)
                        .Color(ButtonFillColor)
                        .Padding(ButtonContentPadding)
                        [
                            SNew(SOverlay)
                            + SOverlay::Slot()
                            [
                                SNew(SBorder)
                                .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                                .BorderBackgroundColor(ButtonTintOverlay)
                                .Padding(FMargin(0.f))
                            ]
                            + SOverlay::Slot()
                            [
                                ButtonContent
                            ]
                        ]
                    ]
                    + SOverlay::Slot()
                    .VAlign(VAlign_Top)
                    .HAlign(HAlign_Fill)
                    [
                        SNew(SBox)
                        .HeightOverride(24.f)
                        [
                            SNew(SImage)
                            .Image(&ListBuildingContainerWidgetPrivate::GlassSheenBrush)
                            .ColorAndOpacity(ButtonSheenColor)
                        ]
                    ]
                ]
            ]
        ];
    }

    if (ButtonScrollBox.IsValid())
    {
        ButtonScrollBox->ScrollToStart();
    }
}

FReply SListBuildingContainerWidget::HandleScrollAreaMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
    if (PointerEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bPendingScrollDrag = true;
        ScrollDragStartPosition = PointerEvent.GetScreenSpacePosition();
        ScrollDragStartOffset = ButtonScrollBox.IsValid() ? ButtonScrollBox->GetScrollOffset() : 0.f;
    }

    return FReply::Unhandled();
}

FReply SListBuildingContainerWidget::HandleScrollAreaMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
    if (PointerEvent.GetEffectingButton() != EKeys::LeftMouseButton)
    {
        return FReply::Unhandled();
    }

    const bool bWasDragging = bIsDraggingScroll;
    EndScrollDrag();

    if (bWasDragging && ScrollInteractionLayer.IsValid())
    {
        return FReply::Handled().ReleaseMouseCapture();
    }

    return FReply::Unhandled();
}

FReply SListBuildingContainerWidget::HandleScrollAreaMouseMove(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
    if (!PointerEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        if (bIsDraggingScroll && ScrollInteractionLayer.IsValid())
        {
            FSlateApplication::Get().ReleaseAllPointerCapture();
        }

        if (bPendingScrollDrag || bIsDraggingScroll)
        {
            EndScrollDrag();
        }
        return FReply::Unhandled();
    }

    if (!ButtonScrollBox.IsValid() || !ScrollInteractionLayer.IsValid())
    {
        return FReply::Unhandled();
    }

    const FVector2D CurrentPosition = PointerEvent.GetScreenSpacePosition();
    const FVector2D DragDelta = CurrentPosition - ScrollDragStartPosition;

    if (!bIsDraggingScroll)
    {
        if (bPendingScrollDrag && FMath::Abs(DragDelta.X) >= FSlateApplication::Get().GetDragTriggerDistance())
        {
            bPendingScrollDrag = false;
            bIsDraggingScroll = true;
            return FReply::Handled().CaptureMouse(ScrollInteractionLayer.ToSharedRef());
        }

        return FReply::Unhandled();
    }

    const float MaxOffset = FMath::Max(0.f, ButtonScrollBox->GetScrollOffsetOfEnd());
    const float TargetOffset = FMath::Clamp(ScrollDragStartOffset - DragDelta.X, 0.f, MaxOffset);
    ButtonScrollBox->SetScrollOffset(TargetOffset);
    return FReply::Handled();
}

void SListBuildingContainerWidget::EndScrollDrag()
{
    bPendingScrollDrag = false;
    bIsDraggingScroll = false;
}

void SListBuildingContainerWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    if (!ScrollInteractionLayer.IsValid())
    {
        return;
    }

    const FVector2D CurrentSize = ScrollInteractionLayer->GetCachedGeometry().GetLocalSize();
    if (!CurrentSize.IsNearlyZero() && !CurrentSize.Equals(CachedScrollSize, 0.5f))
    {
        CachedScrollSize = CurrentSize;
        CachedButtonExtent = FMath::Max(1.f, CurrentSize.Y);
        CachedContainerWidth = CurrentSize.X;
        RebuildButtonList();
    }
}

FOptionalSize SListBuildingContainerWidget::GetButtonExtent() const
{
    constexpr float DefaultExtent = 116.f;
    if (ScrollInteractionLayer.IsValid())
    {
        const FVector2D LocalSize = ScrollInteractionLayer->GetCachedGeometry().GetLocalSize();
        if (LocalSize.X > KINDA_SMALL_NUMBER)
        {
            CachedContainerWidth = LocalSize.X;
        }
        if (LocalSize.Y > KINDA_SMALL_NUMBER)
        {
            CachedScrollSize = LocalSize;
            CachedButtonExtent = FMath::Max(1.f, LocalSize.Y);
        }
    }

    const float Extent = CachedButtonExtent > 0.f ? CachedButtonExtent : DefaultExtent;
    return FOptionalSize(FMath::Max(1.f, Extent));
}

float SListBuildingContainerWidget::GetContainerWidth() const
{
    if (ScrollInteractionLayer.IsValid())
    {
        const FVector2D LocalSize = ScrollInteractionLayer->GetCachedGeometry().GetLocalSize();
        if (LocalSize.X > KINDA_SMALL_NUMBER)
        {
            CachedContainerWidth = LocalSize.X;
        }
    }

    return CachedContainerWidth;
}

bool SListBuildingContainerWidget::ShouldDisplayIcon(const FSlateBrush& Brush) const
{
    return Brush.GetResourceObject() != nullptr
        || Brush.GetImageSize() != FVector2D::ZeroVector
        || Brush.DrawAs != ESlateBrushDrawType::NoDrawType
        || Brush.GetResourceName().IsValid();
}
