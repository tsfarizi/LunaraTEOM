#include "Widget/SMainWindowWidget.h"
#include "Widget/SBeveledBorder.h"

#include "UI/LunaraTeomSlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleAsset.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"

#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"

#include "SlateOptMacros.h"

#include "Engine/Texture2D.h"
#include "Engine/Font.h"
#include "Engine/FontFace.h"
#include "Materials/MaterialInterface.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

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

void SMainWindowWidget::Construct(const FArguments& InArgs)
{
    OnClose = InArgs._OnCloseClicked;
    ResolveStyle(InArgs._Style);

    TitleAttr       = InArgs._Title;
    TitleFontObject = InArgs._TitleFontObject;
    TitleFontSize   = FMath::Max(1, InArgs._TitleFontSize);
    IconObject      = InArgs._IconObject;

    BuildLayout();

    if (InArgs._Content.Widget != SNullWidget::NullWidget)
    {
        SetContent(InArgs._Content.Widget);
    }
    else
    {
        ClearContent();
    }

    RefreshTitleFont();
    RefreshIconBrush();
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

    if (TitleText.IsValid())
    {
        TitleText->SetFont(TitleFont);
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

    if (IconWidget.IsValid())
    {
        IconWidget->SetImage(IconBrush.GetResourceObject() ? &IconBrush : nullptr);
        IconWidget->SetVisibility(IconBrush.GetResourceObject() ? EVisibility::Visible : EVisibility::Collapsed);
    }
}

void SMainWindowWidget::SetTitleFont(TSoftObjectPtr<UObject> InFontObject, int32 InFontSize)
{
    TitleFontObject = InFontObject;
    TitleFontSize   = FMath::Max(1, InFontSize);
    RefreshTitleFont();
}

void SMainWindowWidget::SetIcon(TSoftObjectPtr<UObject> InIconObject)
{
    IconObject = InIconObject;
    RefreshIconBrush();
}

void SMainWindowWidget::SetContent(const TSharedRef<SWidget>& InContent)
{
    CachedContent = InContent;

    if (ContentBox.IsValid())
    {
        ContentBox->SetContent(InContent);
    }
}

void SMainWindowWidget::ClearContent()
{
    CachedContent.Reset();

    if (ContentBox.IsValid())
    {
        ContentBox->SetContent(SNullWidget::NullWidget);
    }
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

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

    SAssignNew(ContentBox, SBox);
    if (LocalCachedContent.IsValid())
    {
        ContentBox->SetContent(LocalCachedContent.ToSharedRef());
    }
    else
    {
        ContentBox->SetContent(SNullWidget::NullWidget);
    }

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
                SNew(SBorder)
                .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                .BorderBackgroundColor(DeepBlue)
                .Padding(FMargin(8.f, 6.f))
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    [
                        SAssignNew(IconWidget, SImage)
                        .Image(nullptr)
                        .Visibility(EVisibility::Collapsed)
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(FMargin(8.f, 0.f))
                    .VAlign(VAlign_Center)
                    [
                        SAssignNew(TitleText, STextBlock)
                        .Text(TitleAttr)
                        .ColorAndOpacity(TitleColor)
                        .Font(TitleFont)
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
                        .OnClicked_Lambda([this]()
                        {
                            if (OnClose.IsBound())
                            {
                                OnClose.Execute();
                            }
                            return FReply::Handled();
                        })
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("X")))
                            .ColorAndOpacity(TitleColor)
                            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
                        ]
                    ]
                ]
            ]
            + SVerticalBox::Slot()
            .FillHeight(1.f)
            .Padding(FMargin(6.f, 4.f, 6.f, 6.f))
            [
                ContentBox.ToSharedRef()
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
