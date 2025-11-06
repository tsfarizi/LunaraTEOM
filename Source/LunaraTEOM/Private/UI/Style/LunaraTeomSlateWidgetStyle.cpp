#include "UI/Style/LunaraTeomSlateWidgetStyle.h"
#include "Styling/CoreStyle.h"

const FName FLunaraTeomSlateStyle::TypeName(TEXT("FLunaraTeomSlateStyle"));

static FSlateFontInfo ToFont(TSoftObjectPtr<UFontFace> Face, int32 Size)
{
	if (UFontFace* Resolved = Face.LoadSynchronous()) return FSlateFontInfo(Resolved, Size);
	return FCoreStyle::GetDefaultFontStyle("Regular", Size);
}

FLunaraTeomSlateStyle::FLunaraTeomSlateStyle()
{
	CinzelRegular   = ToFont(CinzelRegularFace,   DefaultFontSize);
	CinzelMedium    = ToFont(CinzelMediumFace,    DefaultFontSize);
	CinzelSemiBold  = ToFont(CinzelSemiBoldFace,  DefaultFontSize);
	CinzelBold      = ToFont(CinzelBoldFace,      DefaultFontSize);
	CinzelExtraBold = ToFont(CinzelExtraBoldFace, DefaultFontSize);
	CinzelBlack     = ToFont(CinzelBlackFace,     DefaultFontSize);
}

FLunaraTeomSlateStyle::~FLunaraTeomSlateStyle() {}

const FLunaraTeomSlateStyle& FLunaraTeomSlateStyle::GetDefault()
{
	static FLunaraTeomSlateStyle Default;
	return Default;
}

void FLunaraTeomSlateStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const {}
