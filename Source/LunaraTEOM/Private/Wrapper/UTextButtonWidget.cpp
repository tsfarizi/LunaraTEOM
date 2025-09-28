#include "Wrapper/UTextButtonWidget.h"
#include "Widget/Button/Text/STextButtonWidget.h"

TSharedRef<SWidget> UTextButtonWidget::RebuildWidget()
{
    SAssignNew(MySlate, STextButtonWidget)
        .Label(TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateUObject(this, &UTextButtonWidget::GetLabel)))
        .OnClicked(FOnSlateClicked::CreateUObject(this, &UTextButtonWidget::HandleSlateClicked))
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

FText UTextButtonWidget::GetLabel() const
{
	return Label;
}
