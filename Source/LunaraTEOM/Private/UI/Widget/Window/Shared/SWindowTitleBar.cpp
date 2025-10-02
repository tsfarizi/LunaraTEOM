#include "UI/Widget/Window/Shared/SWindowTitleBar.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"

#include "Styling/CoreStyle.h"

void SWindowTitleBar::Construct(const FArguments& InArgs)
{
    OnClose = InArgs._OnCloseClicked;
    TitleAttr = InArgs._Title;
    CurrentTitleFont = InArgs._TitleFont;
    CurrentTitleColor = InArgs._TitleColor;
    CurrentIconBrush = InArgs._IconBrush;
    BackgroundColor = InArgs._BackgroundColor;

    ChildSlot
    [
        SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(BackgroundColor)
        .Padding(FMargin(8.f, 6.f))
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SAssignNew(IconWidget, SImage)
                .Image(CurrentIconBrush)
                .Visibility(CurrentIconBrush ? EVisibility::Visible : EVisibility::Collapsed)
                .ColorAndOpacity(FLinearColor::White)
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(FMargin(8.f, 0.f))
            .VAlign(VAlign_Center)
            [
                SAssignNew(TitleText, STextBlock)
                .Text(TitleAttr)
                .ColorAndOpacity(CurrentTitleColor)
                .Font(CurrentTitleFont)
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                SNullWidget::NullWidget
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(SButton)
                .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
                .ContentPadding(FMargin(6.f, 2.f))
                .OnClicked(this, &SWindowTitleBar::HandleCloseClicked)
                [
                    SAssignNew(CloseButtonText, STextBlock)
                    .Text(FText::FromString(TEXT("X")))
                    .ColorAndOpacity(CurrentTitleColor)
                    .Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
                ]
            ]
        ]
    ];
}

void SWindowTitleBar::SetTitleFont(const FSlateFontInfo& InFont)
{
    CurrentTitleFont = InFont;

    if (TitleText.IsValid())
    {
        TitleText->SetFont(CurrentTitleFont);
    }
}

void SWindowTitleBar::SetTitleColor(const FSlateColor& InColor)
{
    CurrentTitleColor = InColor;

    if (TitleText.IsValid())
    {
        TitleText->SetColorAndOpacity(CurrentTitleColor);
    }

    if (CloseButtonText.IsValid())
    {
        CloseButtonText->SetColorAndOpacity(CurrentTitleColor);
    }
}

void SWindowTitleBar::SetIconBrush(const FSlateBrush* InBrush)
{
    CurrentIconBrush = InBrush;

    if (IconWidget.IsValid())
    {
        IconWidget->SetImage(CurrentIconBrush);
        IconWidget->SetVisibility(CurrentIconBrush ? EVisibility::Visible : EVisibility::Collapsed);
    }
}

FReply SWindowTitleBar::HandleCloseClicked()
{
    if (OnClose.IsBound())
    {
        OnClose.Execute();
    }

    return FReply::Handled();
}
