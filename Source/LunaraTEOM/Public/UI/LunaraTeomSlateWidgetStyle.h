#pragma once
#include "CoreMinimal.h"
#include "Styling/SlateWidgetStyle.h"
#include "Fonts/SlateFontInfo.h"
#include "Engine/FontFace.h"
#include "LunaraTeomSlateWidgetStyle.generated.h"

USTRUCT(BlueprintType)
struct FLunaraTeomSlateStyle : public FSlateWidgetStyle
{
	GENERATED_BODY()

	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; }
	static const FLunaraTeomSlateStyle& GetDefault();

	FLunaraTeomSlateStyle();
	virtual ~FLunaraTeomSlateStyle();
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Colors")
        FLinearColor SurfaceColor = FLinearColor(0.658f, 0.478f, 0.176f);

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Colors")
        FLinearColor PrimaryColor = FLinearColor(0.110f, 0.184f, 0.360f);

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Colors")
        FLinearColor HighlightColor = FLinearColor(0.306f, 0.772f, 0.945f);

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Colors")
        FLinearColor BackgroundColor = FLinearColor(0.086f, 0.133f, 0.262f);

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Colors")
        FLinearColor AccentColor = FLinearColor(0.851f, 0.650f, 0.302f);

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Colors")
        FLinearColor SecondaryColor = FLinearColor(0.721f, 0.537f, 0.227f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
	TSoftObjectPtr<UFontFace> CinzelRegularFace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
	TSoftObjectPtr<UFontFace> CinzelMediumFace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
	TSoftObjectPtr<UFontFace> CinzelSemiBoldFace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
	TSoftObjectPtr<UFontFace> CinzelBoldFace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
	TSoftObjectPtr<UFontFace> CinzelExtraBoldFace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
	TSoftObjectPtr<UFontFace> CinzelBlackFace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
	int32 DefaultFontSize = 24;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lunara|Fonts")
	FSlateFontInfo CinzelRegular;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lunara|Fonts")
	FSlateFontInfo CinzelMedium;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lunara|Fonts")
	FSlateFontInfo CinzelSemiBold;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lunara|Fonts")
	FSlateFontInfo CinzelBold;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lunara|Fonts")
	FSlateFontInfo CinzelExtraBold;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lunara|Fonts")
	FSlateFontInfo CinzelBlack;
};
