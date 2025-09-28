#include "Wrapper/UIconButtonWidget.h"
#include "Widget/Button/Icon/SIconButtonWidget.h"

TSharedRef<SWidget> UIconButtonWidget::RebuildWidget()
{
    SAssignNew(MySlate, SIconButtonWidget)
        .IconAsset(IconAsset)
        .Diameter(Diameter)
        .OnClicked(FOnSlateIconButtonClicked::CreateUObject(this, &UIconButtonWidget::HandleSlateClicked));

    return MySlate.ToSharedRef();
}

void UIconButtonWidget::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    MySlate.Reset();
}

void UIconButtonWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    if (MySlate.IsValid())
    {
        MySlate->SetDiameter(Diameter);
        MySlate->SetIconAsset(IconAsset);
    }
}

FReply UIconButtonWidget::HandleSlateClicked()
{
    OnClicked.Broadcast();
    return FReply::Handled();
}
