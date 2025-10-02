#pragma once

#include "CoreMinimal.h"
#include "Components/ContentWidget.h"
#include "Widget/ListBuildingButtonItem.h"
#include "UListBuildingContainerWidget.generated.h"

class SListBuildingContainerWidget;

UCLASS(meta = (DisplayName = "List Building Container", Category = "Lunara|Slate"))
class LUNARATEOM_API UListBuildingContainerWidget : public UContentWidget
{
    GENERATED_BODY()

public:
    // Slot konten yang dapat diisi melalui WBP.
    virtual UPanelSlot* GetContentSlot() const override;

    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    virtual void SynchronizeProperties() override;

    // Properti dan fungsi tombol yang diekspos ke WBP.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "List Building", meta = (ExposeOnSpawn = "true"))
    TArray<FListBuildingButtonItem> ButtonItems;

    UFUNCTION(BlueprintCallable, Category = "List Building")
    void SetButtonItems(const TArray<FListBuildingButtonItem>& InItems);

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
