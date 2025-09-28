#include "Widget/Window/Main/SMainWindowWidget.h"

#include "Widget/SBeveledBorder.h"

#include "UI/LunaraTeomSlateWidgetStyle.h"

#include "Widget/Window/Shared/SWindowContentPanel.h"
#include "Widget/Window/Shared/SWindowTitleBar.h"

#include "Widgets/Layout/SBorder.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"

#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMainWindowWidget::BuildLayout()
{
    if (!StyleRef)
    {
        StyleRef = &FLunaraTeomSlateStyle::GetDefault();
    }

    const FLinearColor DeepBlue = StyleRef->DeepBlue;
    const FLinearColor MoonGold = StyleRef->MoonGold;
    const FLinearColor TitleColor = StyleRef->HighlightGold;
    const FLinearColor BaseGold = StyleRef->BaseGold;
    const FLinearColor HigLightGold = StyleRef->HighlightGold;

    TSharedPtr<SWidget> LocalCachedContent = CachedContent;

    SAssignNew(ContentPanel, SWindowContentPanel);
    if (LocalCachedContent.IsValid())
    {
        ContentPanel->SetContent(LocalCachedContent.ToSharedRef());
    }
    else
    {
        ContentPanel->ClearContent();
    }

    SAssignNew(TitleBar, SWindowTitleBar)
        .Title(TitleAttr)
        .TitleColor(TitleColor)
        .TitleFont(TitleFont)
        .IconBrush(nullptr)
        .BackgroundColor(DeepBlue)
        .OnCloseClicked(FSimpleDelegate::CreateSP(this, &SMainWindowWidget::HandleCloseButton));
    TitleBar->SetTitleColor(FSlateColor(TitleColor));

    constexpr float TitleOuterBevel = 6.f;
    constexpr float TitleMiddleBevel = 5.f;
    constexpr float TitleInnerBevel = 4.f;
    constexpr float TitleOuterNotchDepth = 8.f;
    constexpr float TitleMiddleNotchDepth = 6.f;
    constexpr float TitleInnerNotchDepth = 4.f;
    constexpr float TitleNotchHeight = 18.f;

    TSharedRef<SWidget> TitleFrame =
        SNew(SBeveledBorder)
        .Bevel(TitleOuterBevel)
        .NotchDepth(TitleOuterNotchDepth)
        .NotchHeight(TitleNotchHeight)
        .RightNotchCount(3)
        .LeftNotchCount(1)
        .Color(MoonGold * 0.9f)
        .Padding(FMargin(2.f))
        [
            SNew(SBeveledBorder)
            .Bevel(TitleMiddleBevel)
            .NotchDepth(TitleMiddleNotchDepth)
            .NotchHeight(TitleNotchHeight)
            .RightNotchCount(3)
            .LeftNotchCount(1)
            .Color(BaseGold)
            .Padding(FMargin(2.f))
            [
                SNew(SBeveledBorder)
                .Bevel(TitleInnerBevel)
                .NotchDepth(TitleInnerNotchDepth)
                .NotchHeight(TitleNotchHeight)
                .RightNotchCount(3)
                .LeftNotchCount(1)
                .Color(HigLightGold)
                .Padding(FMargin(2.f, 2.f, 2.f, 1.f))
                [
                    TitleBar.ToSharedRef()
                ]
            ]
        ];

    TSharedRef<SOverlay> WindowOverlay =
        SNew(SOverlay)
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Top)
        [
            SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
            .BorderBackgroundColor(MoonGold * 0.5f)
            .Padding(FMargin(0.f, 0.f, 0.f, 1.f))
        ]
        + SOverlay::Slot()
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                TitleFrame
            ]
            + SVerticalBox::Slot()
            .FillHeight(1.f)
            .Padding(FMargin(6.f, 4.f, 6.f, 6.f))
            [
                ContentPanel.ToSharedRef()
            ]
        ];

    ChildSlot
    [
        SNew(SBeveledBorder)
        .Bevel(10.f)
        .NotchDepth(12.f)
        .RightNotchCount(3)
        .LeftNotchCount(2)
        .Color(DeepBlue)
        .Padding(FMargin(8.f))
        [
            SNew(SBeveledBorder)
            .Bevel(8.f)
            .NotchDepth(12.f)
            .RightNotchCount(3)
            .LeftNotchCount(2)
            .Color(MoonGold * 0.9f)
            .Padding(FMargin(6.f))
            [
                SNew(SBeveledBorder)
                .Bevel(8.f)
                .NotchDepth(12.f)
                .RightNotchCount(3)
                .LeftNotchCount(2)
                .Color(BaseGold)
                .Padding(FMargin(6.f))
                [
                    SNew(SBeveledBorder)
                    .Bevel(8.f)
                    .NotchDepth(12.f)
                    .RightNotchCount(3)
                    .LeftNotchCount(2)
                    .Color(HigLightGold)
                    .Padding(FMargin(6.f))
                    [
                        WindowOverlay
                    ]
                ]
            ]
        ]
    ];

    RefreshTitleFont();
    RefreshIconBrush();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
