#pragma once

#include "CoreMinimal.h"
#include "Components/ContentWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "UMainWindowWidget.generated.h"

class SMainWindowWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWindowCloseClickedBP);

UCLASS(meta=(DisplayName="Main Window", Category="Lunara|Slate"))
class LUNARATEOM_API UMainWindowWidget : public UContentWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Content")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance", meta=(AllowedClasses="Font,FontFace"))
    TSoftObjectPtr<UObject> TitleFontObject;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance", meta=(ClampMin=1))
    int32 TitleFontSize = 20;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance", meta=(AllowedClasses="Texture2D,MaterialInterface"))
    TSoftObjectPtr<UObject> IconObject;

    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnWindowCloseClickedBP OnCloseClicked;

    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    virtual void SynchronizeProperties() override;

protected:
    virtual void OnSlotAdded(UPanelSlot* InSlot) override;
    virtual void OnSlotRemoved(UPanelSlot* InSlot) override;

private:
    void HandleCloseClicked();

    TSharedPtr<SMainWindowWidget> MySlate;
};
