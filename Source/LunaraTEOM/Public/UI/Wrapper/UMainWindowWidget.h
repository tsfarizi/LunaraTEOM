#pragma once

#include "CoreMinimal.h"
#include "Components/ContentWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "UMainWindowWidget.generated.h"

class SMainWindowWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWindowCloseClickedBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWindowShownBP);

UCLASS(meta=(DisplayName="Main Window", Category="Lunara|Slate"))
class LUNARATEOM_API UMainWindowWidget : public UContentWidget
{
    GENERATED_BODY()

public:
    /** Text that appears in the window's title bar. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Content")
    FText Title;

    /** Font asset applied to the window title text. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance", meta=(AllowedClasses="Font,FontFace"))
    TSoftObjectPtr<UObject> TitleFontObject;

    /** Size of the window title font in points. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance", meta=(ClampMin=1))
    int32 TitleFontSize = 20;

    /** Optional icon displayed alongside the window title. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance", meta=(AllowedClasses="Texture2D,MaterialInterface"))
    TSoftObjectPtr<UObject> IconObject;

    /** Event fired when the close button is clicked. */
    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnWindowCloseClickedBP OnCloseClicked;

    /** Event fired after the window becomes visible to the user. */
    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnWindowShownBP OnShown;

    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    virtual void SynchronizeProperties() override;
    virtual void OnWidgetRebuilt() override;

protected:
    virtual void OnSlotAdded(UPanelSlot* InSlot) override;
    virtual void OnSlotRemoved(UPanelSlot* InSlot) override;

private:
    void HandleCloseClicked();

    TSharedPtr<SMainWindowWidget> MySlate;
    bool bHasBroadcastShown = false;
};
