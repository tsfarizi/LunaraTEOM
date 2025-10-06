#pragma once

#include "CoreMinimal.h"
#include "Components/ContentWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "UI/Widget/ListBuildingButtonItem.h"
#include "UListBuildingContainerWidget.generated.h"

class SListBuildingContainerWidget;

UCLASS(meta = (DisplayName = "List Building Container", Category = "Lunara|Slate"))
class LUNARATEOM_API UListBuildingContainerWidget : public UContentWidget
{
    GENERATED_BODY()

public:
    UListBuildingContainerWidget();

    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    virtual void SynchronizeProperties() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "List Building", meta = (ExposeOnSpawn = "true"))
    TArray<FListBuildingButtonItem> ButtonItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "List Building", meta = (ExposeOnSpawn = "true"))
    FSlateFontInfo ButtonFont;

    UFUNCTION(BlueprintCallable, Category = "List Building")
    void SetButtonItems(const TArray<FListBuildingButtonItem>& InItems);

    UFUNCTION(BlueprintCallable, Category = "List Building")
    void SetButtonFont(const FSlateFontInfo& InFont);

    UFUNCTION(BlueprintCallable, Category = "List Building")
    void AddButtonItem(const FListBuildingButtonItem& NewItem);

    UFUNCTION(BlueprintCallable, Category = "List Building")
    void UpdateButtonItem(int32 Index, const FListBuildingButtonItem& UpdatedItem);

    UFUNCTION(BlueprintCallable, Category = "List Building")
    void RemoveButtonItemAt(int32 Index);

    UFUNCTION(BlueprintCallable, Category = "List Building")
    void ClearButtonItems();

protected:
    virtual void OnSlotAdded(UPanelSlot* InSlot) override;
    virtual void OnSlotRemoved(UPanelSlot* InSlot) override;

private:
    TSharedPtr<SListBuildingContainerWidget> MySlate;
};
