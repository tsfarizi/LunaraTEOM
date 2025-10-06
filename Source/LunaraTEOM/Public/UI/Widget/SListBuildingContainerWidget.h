#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Types/SlateStructs.h"
#include "Fonts/SlateFontInfo.h"

#include "UI/Widget/ListBuildingButtonItem.h"

class SBeveledBorder;
class SScrollBox;
class SHorizontalBox;
class SBorder;

class LUNARATEOM_API SListBuildingContainerWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SListBuildingContainerWidget)
    {}
        SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetContent(const TSharedRef<SWidget>& InContent);
    void ClearContent();

    void SetButtonItems(const TArray<FListBuildingButtonItem>& InItems);
    const TArray<FListBuildingButtonItem>& GetButtonItems() const { return ButtonItems; }

    void SetButtonFont(const FSlateFontInfo& InFont);

private:
    TSharedPtr<class SBeveledBorder> ContentBorder;
    TSharedPtr<SWidget> CachedContent;
    TSharedPtr<class SBox> CustomContentSlot;
    TSharedPtr<class SScrollBox> ButtonScrollBox;
    TSharedPtr<class SHorizontalBox> ButtonListContainer;
    TSharedPtr<class SBorder> ScrollInteractionLayer;
    TArray<FListBuildingButtonItem> ButtonItems;
    TArray<TSharedPtr<FSlateBrush>> ButtonIconBrushes;

    FSlateFontInfo ButtonLabelFont;

    bool bPendingScrollDrag = false;
    bool bIsDraggingScroll = false;
    FVector2D ScrollDragStartPosition = FVector2D::ZeroVector;
    float ScrollDragStartOffset = 0.f;
    mutable float CachedButtonExtent = 116.f;
    mutable float CachedContainerWidth = 0.f;
    mutable FVector2D CachedScrollSize = FVector2D::ZeroVector;

    void RebuildButtonList();
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
    bool ShouldDisplayIcon(const FSlateBrush& Brush) const;
    FOptionalSize GetButtonExtent() const;
    float GetContainerWidth() const;

    FReply HandleScrollAreaMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
    FReply HandleScrollAreaMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
    FReply HandleScrollAreaMouseMove(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
    void EndScrollDrag();
};
