#pragma once
#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "UTextButtonWidget.generated.h"

class STextButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClickedBP);

UCLASS(meta=(DisplayName="Text Button (Slate)"))
class LUNARATEOM_API UTextButtonWidget : public UWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Content")
    FText Label = FText::FromString(TEXT("Button"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance", meta=(AllowedClasses="Font,FontFace"))
    TSoftObjectPtr<UObject> FontObject;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance", meta=(ClampMin=1))
    int32 FontSize = 18;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnButtonClickedBP OnClicked;

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    virtual void SynchronizeProperties() override;

private:
    FReply HandleSlateClicked();
    FText GetLabel() const;

	TSharedPtr<STextButtonWidget> MySlate;
};
