#include "UI/Wrapper/UIconButtonWidget.h"
#include "UI/Widget/Button/Icon/SIconButtonWidget.h"

TSharedRef<SWidget> UIconButtonWidget::RebuildWidget()
{
    SAssignNew(MySlate, SIconButtonWidget)
        .IconAsset(IconAsset)
        .Diameter(Diameter)
        .OnClicked(FOnSlateIconButtonClicked::CreateUObject(this, &UIconButtonWidget::HandleSlateClicked))
        .OnPressed(FSimpleDelegate::CreateUObject(this, &UIconButtonWidget::HandleSlatePressed))
        .OnReleased(FSimpleDelegate::CreateUObject(this, &UIconButtonWidget::HandleSlateReleased))
        .OnHovered(FSimpleDelegate::CreateUObject(this, &UIconButtonWidget::HandleSlateHovered))
        .OnUnhovered(FSimpleDelegate::CreateUObject(this, &UIconButtonWidget::HandleSlateUnhovered));

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

void UIconButtonWidget::HandleSlatePressed()
{
    OnPressed.Broadcast();
}

void UIconButtonWidget::HandleSlateReleased()
{
    OnReleased.Broadcast();
}

void UIconButtonWidget::HandleSlateHovered()
{
    OnHovered.Broadcast();
}

void UIconButtonWidget::HandleSlateUnhovered()
{
    OnUnhovered.Broadcast();
}
