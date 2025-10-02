// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "Widget/ListBuildingButtonItem.h"

/**
 *
 */
class LUNARATEOM_API SListBuildingContainerWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SListBuildingContainerWidget)
    {}
        // Content slot yang diekspos ke WBP.
        SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_END_ARGS()

    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs);

    void SetContent(const TSharedRef<SWidget>& InContent);
    void ClearContent();

    void SetButtonItems(const TArray<FListBuildingButtonItem>& InItems);
    const TArray<FListBuildingButtonItem>& GetButtonItems() const { return ButtonItems; }

private:
    TSharedPtr<class SBorder> ContentBorder;
    TSharedPtr<SWidget> CachedContent;
    TSharedPtr<class SBox> CustomContentSlot;
    TSharedPtr<class SVerticalBox> ButtonListContainer;
    TArray<FListBuildingButtonItem> ButtonItems;
    TArray<TSharedPtr<FSlateBrush>> ButtonIconBrushes;

    void RebuildButtonList();
    bool ShouldDisplayIcon(const FSlateBrush& Brush) const;
};
