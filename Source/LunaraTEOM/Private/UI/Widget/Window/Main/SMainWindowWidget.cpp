#include "UI/Widget/Window/Main/SMainWindowWidget.h"

#include "UI/Widget/Window/Shared/SWindowContentPanel.h"
#include "UI/Widget/Window/Shared/SWindowTitleBar.h"

#include "Widgets/SNullWidget.h"

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

void SMainWindowWidget::SetContent(const TSharedRef<SWidget>& InContent)
{
    CachedContent = InContent;

    if (ContentPanel.IsValid())
    {
        ContentPanel->SetContent(InContent);
    }
}

void SMainWindowWidget::ClearContent()
{
    CachedContent.Reset();

    if (ContentPanel.IsValid())
    {
        ContentPanel->ClearContent();
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

void SMainWindowWidget::HandleCloseButton()
{
    if (OnClose.IsBound())
    {
        OnClose.Execute();
    }
}
