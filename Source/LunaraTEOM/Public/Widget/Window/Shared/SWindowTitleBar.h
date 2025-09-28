#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateColor.h"

class STextBlock;
class SImage;

class LUNARATEOM_API SWindowTitleBar : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SWindowTitleBar)
        : _Title(FText())
        , _TitleColor(FSlateColor(FLinearColor::White))
        , _TitleFont(FSlateFontInfo())
        , _IconBrush(nullptr)
        , _BackgroundColor(FLinearColor::Transparent)
    {}
        SLATE_ATTRIBUTE(FText, Title)
        SLATE_ARGUMENT(FSlateColor, TitleColor)
        SLATE_ARGUMENT(FSlateFontInfo, TitleFont)
        SLATE_ARGUMENT(const FSlateBrush*, IconBrush)
        SLATE_ARGUMENT(FSlateColor, BackgroundColor)
        SLATE_EVENT(FSimpleDelegate, OnCloseClicked)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetTitleFont(const FSlateFontInfo& InFont);
    void SetTitleColor(const FSlateColor& InColor);
    void SetIconBrush(const FSlateBrush* InBrush);

private:
    FReply HandleCloseClicked();

    TAttribute<FText> TitleAttr;
    FSlateFontInfo CurrentTitleFont;
    FSlateColor CurrentTitleColor;
    const FSlateBrush* CurrentIconBrush = nullptr;
    FSlateColor BackgroundColor;
    FSimpleDelegate OnClose;

    TSharedPtr<STextBlock> TitleText;
    TSharedPtr<STextBlock> CloseButtonText;
    TSharedPtr<SImage> IconWidget;
};
