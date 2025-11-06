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

        /** Primary surface tint applied to window bodies and cards. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Colors")
        FLinearColor SurfaceColor = FLinearColor(0.658f, 0.478f, 0.176f);

        /** Accent color for main interactive elements (e.g., buttons). */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Colors")
        FLinearColor PrimaryColor = FLinearColor(0.110f, 0.184f, 0.360f);

        /** Highlight color used for hover states and keylines. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Colors")
        FLinearColor HighlightColor = FLinearColor(0.306f, 0.772f, 0.945f);

        /** Background color for application frames and large panels. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Colors")
        FLinearColor BackgroundColor = FLinearColor(0.086f, 0.133f, 0.262f);

        /** Secondary accent used sparingly for emphasis or status. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Colors")
        FLinearColor AccentColor = FLinearColor(0.851f, 0.650f, 0.302f);

        /** Support color for subtle UI details and dividers. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Colors")
        FLinearColor SecondaryColor = FLinearColor(0.721f, 0.537f, 0.227f);

        /** Soft reference to the Cinzel Regular font face asset. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
        TSoftObjectPtr<UFontFace> CinzelRegularFace;

        /** Soft reference to the Cinzel Medium font face asset. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
        TSoftObjectPtr<UFontFace> CinzelMediumFace;

        /** Soft reference to the Cinzel SemiBold font face asset. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
        TSoftObjectPtr<UFontFace> CinzelSemiBoldFace;

        /** Soft reference to the Cinzel Bold font face asset. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
        TSoftObjectPtr<UFontFace> CinzelBoldFace;

        /** Soft reference to the Cinzel ExtraBold font face asset. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
        TSoftObjectPtr<UFontFace> CinzelExtraBoldFace;

        /** Soft reference to the Cinzel Black font face asset. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
        TSoftObjectPtr<UFontFace> CinzelBlackFace;

        /** Default point size applied when constructing Slate fonts. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lunara|Fonts")
        int32 DefaultFontSize = 24;

        /** Runtime Slate font built from CinzelRegularFace. */
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lunara|Fonts")
        FSlateFontInfo CinzelRegular;

        /** Runtime Slate font built from CinzelMediumFace. */
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lunara|Fonts")
        FSlateFontInfo CinzelMedium;

        /** Runtime Slate font built from CinzelSemiBoldFace. */
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lunara|Fonts")
        FSlateFontInfo CinzelSemiBold;

        /** Runtime Slate font built from CinzelBoldFace. */
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lunara|Fonts")
        FSlateFontInfo CinzelBold;

        /** Runtime Slate font built from CinzelExtraBoldFace. */
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lunara|Fonts")
        FSlateFontInfo CinzelExtraBold;

        /** Runtime Slate font built from CinzelBlackFace. */
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lunara|Fonts")
        FSlateFontInfo CinzelBlack;
};
