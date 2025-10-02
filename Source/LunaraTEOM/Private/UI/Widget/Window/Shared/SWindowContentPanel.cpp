#include "UI/Widget/Window/Shared/SWindowContentPanel.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/SNullWidget.h"

void SWindowContentPanel::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SAssignNew(ContentBox, SBox)
        [
            InArgs._Content.Widget
        ]
    ];
}

void SWindowContentPanel::SetContent(const TSharedRef<SWidget>& InContent)
{
    if (ContentBox.IsValid())
    {
        ContentBox->SetContent(InContent);
    }
}

void SWindowContentPanel::ClearContent()
{
    if (ContentBox.IsValid())
    {
        ContentBox->SetContent(SNullWidget::NullWidget);
    }
}
