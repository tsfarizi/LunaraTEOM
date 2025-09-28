#include "Wrapper/UMainWindowWidget.h"
#include "Widget/SMainWindowWidget.h"

TSharedRef<SWidget> UMainWindowWidget::RebuildWidget()
{
    SAssignNew(MySlate, SMainWindowWidget)
        .Title(TAttribute<FText>::CreateLambda([this]{ return Title; }))
        .TitleFontObject(TitleFontObject)
        .TitleFontSize(TitleFontSize)
        .IconObject(IconObject)
        .OnCloseClicked(FSimpleDelegate::CreateUObject(this, &UMainWindowWidget::HandleCloseClicked));

    if (GetContent())
    {
        MySlate->SetContent(GetContent()->TakeWidget());
    }

    MySlate->SetTitleFont(TitleFontObject, TitleFontSize);
    MySlate->SetIcon(IconObject);

    return MySlate.ToSharedRef();
}

void UMainWindowWidget::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    MySlate.Reset();
}

void UMainWindowWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    if (MySlate.IsValid())
    {
        MySlate->SetTitleFont(TitleFontObject, TitleFontSize);
        MySlate->SetIcon(IconObject);
    }
}

void UMainWindowWidget::OnSlotAdded(UPanelSlot* InSlot)
{
    if (MySlate.IsValid() && InSlot && InSlot->Content)
    {
        MySlate->SetContent(InSlot->Content->TakeWidget());
    }
}

void UMainWindowWidget::OnSlotRemoved(UPanelSlot* InSlot)
{
    if (MySlate.IsValid())
    {
        MySlate->ClearContent();
    }
}

void UMainWindowWidget::HandleCloseClicked()
{
    OnCloseClicked.Broadcast();
}
