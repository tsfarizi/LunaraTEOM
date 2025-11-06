#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateBrush.h"

struct FLunaraTeomSlateStyle;
class SWindowTitleBar;
class SWindowContentPanel;

class LUNARATEOM_API SMainWindowWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMainWindowWidget)
        : _Title(FText())
        , _Style(nullptr)
        , _TitleFontObject(nullptr)
        , _TitleFontSize(20)
        , _IconObject(nullptr)
    {}
        SLATE_ATTRIBUTE(FText, Title)
        SLATE_ARGUMENT(const FLunaraTeomSlateStyle*, Style)
        SLATE_ARGUMENT(TSoftObjectPtr<UObject>, TitleFontObject)
        SLATE_ARGUMENT(int32, TitleFontSize)
        SLATE_ARGUMENT(TSoftObjectPtr<UObject>, IconObject)
        SLATE_EVENT(FSimpleDelegate, OnCloseClicked)
        SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetContent(const TSharedRef<SWidget>& InContent);
    void ClearContent();

    void SetTitleFont(TSoftObjectPtr<UObject> InFontObject, int32 InFontSize);
    void SetIcon(TSoftObjectPtr<UObject> InIconObject);

private:
    void ResolveStyle(const FLunaraTeomSlateStyle* InStyle);
    void RefreshTitleFont();
    void RefreshIconBrush();
    void BuildLayout();
    void HandleCloseButton();

private:
    const FLunaraTeomSlateStyle* StyleRef = nullptr;
    FSimpleDelegate OnClose;

    TAttribute<FText> TitleAttr;
    TSharedPtr<SWindowContentPanel> ContentPanel;
    TSharedPtr<SWindowTitleBar> TitleBar;
    TSharedPtr<SWidget> CachedContent;

    FSlateFontInfo TitleFont;
    TSoftObjectPtr<UObject> TitleFontObject;
    int32 TitleFontSize = 20;
    TSoftObjectPtr<UObject> IconObject;
    FSlateBrush IconBrush;
};
