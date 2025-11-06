#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SBox;

class LUNARATEOM_API SWindowContentPanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SWindowContentPanel) {}
        SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetContent(const TSharedRef<SWidget>& InContent);
    void ClearContent();

private:
    TSharedPtr<SBox> ContentBox;
};
