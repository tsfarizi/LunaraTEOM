#include "Wrapper/UListBuildingContainerWidget.h"
#include "Widget/SListBuildingContainerWidget.h"

TSharedRef<SWidget> UListBuildingContainerWidget::RebuildWidget()
{
    SAssignNew(MySlate, SListBuildingContainerWidget);

    if (GetContent())
    {
        MySlate->SetContent(GetContent()->TakeWidget());
    }
    else
    {
        MySlate->ClearContent();
    }

    MySlate->SetButtonItems(ButtonItems);

    return MySlate.ToSharedRef();
}

void UListBuildingContainerWidget::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    MySlate.Reset();
}

void UListBuildingContainerWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    if (MySlate.IsValid())
    {
        MySlate->SetButtonItems(ButtonItems);
    }
}

void UListBuildingContainerWidget::OnSlotAdded(UPanelSlot* InSlot)
{
    if (MySlate.IsValid() && InSlot && InSlot->Content)
    {
        MySlate->SetContent(InSlot->Content->TakeWidget());
    }
}

void UListBuildingContainerWidget::OnSlotRemoved(UPanelSlot* InSlot)
{
    if (MySlate.IsValid())
    {
        MySlate->ClearContent();
    }
}

void UListBuildingContainerWidget::SetButtonItems(const TArray<FListBuildingButtonItem>& InItems)
{
    ButtonItems = InItems;

    if (MySlate.IsValid())
    {
        MySlate->SetButtonItems(ButtonItems);
    }
}

void UListBuildingContainerWidget::AddButtonItem(const FListBuildingButtonItem& NewItem)
{
    ButtonItems.Add(NewItem);

    if (MySlate.IsValid())
    {
        MySlate->SetButtonItems(ButtonItems);
    }
}

void UListBuildingContainerWidget::UpdateButtonItem(int32 Index, const FListBuildingButtonItem& UpdatedItem)
{
    if (!ButtonItems.IsValidIndex(Index))
    {
        return;
    }

    ButtonItems[Index] = UpdatedItem;

    if (MySlate.IsValid())
    {
        MySlate->SetButtonItems(ButtonItems);
    }
}

void UListBuildingContainerWidget::RemoveButtonItemAt(int32 Index)
{
    if (!ButtonItems.IsValidIndex(Index))
    {
        return;
    }

    ButtonItems.RemoveAt(Index);

    if (MySlate.IsValid())
    {
        MySlate->SetButtonItems(ButtonItems);
    }
}

void UListBuildingContainerWidget::ClearButtonItems()
{
    if (ButtonItems.Num() == 0)
    {
        return;
    }

    ButtonItems.Reset();

    if (MySlate.IsValid())
    {
        MySlate->SetButtonItems(ButtonItems);
    }
}
