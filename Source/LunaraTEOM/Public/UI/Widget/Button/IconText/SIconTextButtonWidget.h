#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateColor.h"
#include "Styling/SlateBrush.h"

class LUNARATEOM_API SIconTextButtonWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SIconTextButtonWidget)
        : _OnClicked()
        , _IconBrush(nullptr)
        , _IconColor(FSlateColor(FLinearColor::White))
        , _Label(FText::GetEmpty())
        , _Font()
        , _LabelColor(FSlateColor(FLinearColor::White))
        , _IconSize(48.f)
        , _Spacing(6.f)
        , _LabelMaxWidth(0.f)
    {}
        SLATE_EVENT(FOnClicked, OnClicked)
        SLATE_ATTRIBUTE(const FSlateBrush*, IconBrush)
        SLATE_ATTRIBUTE(FSlateColor, IconColor)
        SLATE_ATTRIBUTE(FText, Label)
        SLATE_ATTRIBUTE(FSlateFontInfo, Font)
        SLATE_ATTRIBUTE(FSlateColor, LabelColor)
        SLATE_ARGUMENT(float, IconSize)
        SLATE_ARGUMENT(float, Spacing)
        SLATE_ARGUMENT(float, LabelMaxWidth)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TAttribute<const FSlateBrush*> IconBrushAttr;
    TAttribute<FSlateColor> IconColorAttr;
    TAttribute<FText> LabelAttr;
    TAttribute<FSlateFontInfo> FontAttr;
    TAttribute<FSlateColor> LabelColorAttr;
    FOnClicked ClickHandler;
    float IconSize = 48.f;
    float Spacing = 6.f;
    float LabelMaxWidth = 0.f;
};
