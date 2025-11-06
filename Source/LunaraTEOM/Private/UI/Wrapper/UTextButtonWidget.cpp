#include "UI/Wrapper/UTextButtonWidget.h"
#include "UI/Widget/Button/Text/STextButtonWidget.h"

TSharedRef<SWidget> UTextButtonWidget::RebuildWidget()
{
    SAssignNew(MySlate, STextButtonWidget)
        .Label(TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateUObject(this, &UTextButtonWidget::GetLabel)))
        .OnClicked(FOnSlateClicked::CreateUObject(this, &UTextButtonWidget::HandleSlateClicked))
        .OnPressed(FSimpleDelegate::CreateUObject(this, &UTextButtonWidget::HandleSlatePressed))
        .OnReleased(FSimpleDelegate::CreateUObject(this, &UTextButtonWidget::HandleSlateReleased))
        .OnHovered(FSimpleDelegate::CreateUObject(this, &UTextButtonWidget::HandleSlateHovered))
        .OnUnhovered(FSimpleDelegate::CreateUObject(this, &UTextButtonWidget::HandleSlateUnhovered))
        .FontObject(FontObject)
        .FontSize(FontSize);
    return MySlate.ToSharedRef();
}

void UTextButtonWidget::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    MySlate.Reset();
}

void UTextButtonWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    if (MySlate.IsValid())
    {
        MySlate->SetExplicitFont(FontObject, FontSize);
    }
}

FReply UTextButtonWidget::HandleSlateClicked()
{
    OnClicked.Broadcast();
    return FReply::Handled();
}

void UTextButtonWidget::HandleSlatePressed()
{
    OnPressed.Broadcast();
}

void UTextButtonWidget::HandleSlateReleased()
{
    OnReleased.Broadcast();
}

void UTextButtonWidget::HandleSlateHovered()
{
    OnHovered.Broadcast();
}

void UTextButtonWidget::HandleSlateUnhovered()
{
    OnUnhovered.Broadcast();
}

FText UTextButtonWidget::GetLabel() const
{
    return Label;
}
