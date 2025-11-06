#include "UI/Widget/Button/IconText/SIconTextButtonWidget.h"

#include "SlateOptMacros.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SIconTextButtonWidget::Construct(const FArguments& InArgs)
{
    IconBrushAttr = InArgs._IconBrush;
    IconColorAttr = InArgs._IconColor;
    LabelAttr = InArgs._Label;
    FontAttr = InArgs._Font;
    LabelColorAttr = InArgs._LabelColor;
    ClickHandler = InArgs._OnClicked;
    IconSize = FMath::Max(0.f, InArgs._IconSize);
    Spacing = FMath::Max(0.f, InArgs._Spacing);
    LabelMaxWidth = FMath::Max(0.f, InArgs._LabelMaxWidth);

    const FSlateBrush* IconBrush = IconBrushAttr.Get(nullptr);
    const bool bHasIcon = IconBrush != nullptr;

    TSharedRef<SVerticalBox> ContentBox = SNew(SVerticalBox);

    if (bHasIcon)
    {
        ContentBox->AddSlot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        [
            SNew(SBox)
            .WidthOverride(IconSize)
            .HeightOverride(IconSize)
            [
                SNew(SImage)
                .Image(IconBrushAttr)
                .ColorAndOpacity(IconColorAttr)
            ]
        ];
    }

    TSharedRef<STextBlock> LabelText = SNew(STextBlock)
        .Text(LabelAttr)
        .Font(FontAttr)
        .Justification(ETextJustify::Center)
        .AutoWrapText(false)
        .ColorAndOpacity(LabelColorAttr)
        .ShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.6f))
        .ShadowOffset(FVector2D(1.f, 1.f))
        .WrapTextAt(0.f);

    const bool bHasLabel = !LabelAttr.Get(FText::GetEmpty()).IsEmpty();

    TSharedRef<SWidget> LabelWidget = StaticCastSharedRef<SWidget>(LabelText);

    if (LabelMaxWidth > 0.f)
    {
        TSharedRef<SBox> LabelBox = SNew(SBox)
            .WidthOverride(LabelMaxWidth)
            [
                LabelText
            ];
        LabelWidget = StaticCastSharedRef<SWidget>(LabelBox);
    }

    ContentBox->AddSlot()
    .AutoHeight()
    .HAlign(HAlign_Center)
    .Padding(FMargin(0.f, (bHasIcon && bHasLabel) ? Spacing : 0.f, 0.f, 0.f))
    [
        LabelWidget
    ];

    ChildSlot
    [
        SNew(SButton)
        .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder"))
        .ButtonColorAndOpacity(FLinearColor::Transparent)
        .ContentPadding(FMargin(0.f))
        .ForegroundColor(FSlateColor::UseForeground())
        .OnClicked(ClickHandler)
        [
            ContentBox
        ]
    ];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
