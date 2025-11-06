#include "UI/Widget/Window/Main/SMainWindowWidget.h"

#include "UI/Widget/SBeveledBorder.h"

#include "UI/Style/LunaraTeomSlateWidgetStyle.h"

#include "UI/Widget/Window/Shared/SWindowContentPanel.h"
#include "UI/Widget/Window/Shared/SWindowTitleBar.h"

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

    const FLinearColor PrimaryColor = StyleRef->PrimaryColor;
    const FLinearColor SecondaryColor = StyleRef->SecondaryColor;
    const FLinearColor TitleColor = StyleRef->AccentColor;
    const FLinearColor SurfaceColor = StyleRef->SurfaceColor;
    const FLinearColor AccentColor = StyleRef->AccentColor;

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
        .BackgroundColor(PrimaryColor)
        .OnCloseClicked(FSimpleDelegate::CreateSP(this, &SMainWindowWidget::HandleCloseButton));
    TitleBar->SetTitleColor(FSlateColor(TitleColor));

    TSharedRef<SOverlay> WindowOverlay =
        SNew(SOverlay)
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Top)
        [
            SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
            .BorderBackgroundColor(SecondaryColor * 0.5f)
            .Padding(FMargin(0.f, 0.f, 0.f, 1.f))
        ]
        + SOverlay::Slot()
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                TitleBar.ToSharedRef()
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
        .Color(PrimaryColor)
        .Padding(FMargin(8.f))
        [
            SNew(SBeveledBorder)
            .Bevel(8.f)
            .NotchDepth(12.f)
            .RightNotchCount(3)
            .LeftNotchCount(2)
            .Color(SecondaryColor * 0.9f)
            .Padding(FMargin(6.f))
            [
                SNew(SBeveledBorder)
                .Bevel(8.f)
                .NotchDepth(12.f)
                .RightNotchCount(3)
                .LeftNotchCount(2)
                .Color(SurfaceColor)
                .Padding(FMargin(6.f))
                [
                    SNew(SBeveledBorder)
                    .Bevel(8.f)
                    .NotchDepth(12.f)
                    .RightNotchCount(3)
                    .LeftNotchCount(2)
                    .Color(AccentColor)
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
