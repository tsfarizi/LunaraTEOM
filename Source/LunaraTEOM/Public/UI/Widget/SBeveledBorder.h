#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

// Gunakan 'class' sesuai deklarasi asli UE untuk hindari C4099.
class FPaintArgs;
class FSlateRect;
class FSlateWindowElementList;
class FWidgetStyle;
// FGeometry sudah tersedia via SCompoundWidget.h; forward-declare opsional:
// struct FGeometry;

/**
 * Beveled + notch border (Slate)
 * Header ini hanya deklarasi; seluruh logic rendering ada di .cpp
 */
class SBeveledBorder : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SBeveledBorder)
        : _Bevel(8.f)
        , _Color(FLinearColor::White)
        , _Padding(FMargin(0))
        , _NotchDepth(8.f)
        , _NotchHeight(16.f)
        , _RightNotchCount(1)
        , _LeftNotchCount(1)
    {}
        SLATE_ATTRIBUTE(float,        Bevel)
        SLATE_ATTRIBUTE(FLinearColor, Color)
        SLATE_ATTRIBUTE(FMargin,      Padding)
        SLATE_ATTRIBUTE(float,        NotchDepth)
        SLATE_ATTRIBUTE(float,        NotchHeight)
        SLATE_ATTRIBUTE(int32,        RightNotchCount)
        SLATE_ATTRIBUTE(int32,        LeftNotchCount)
        SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    virtual int32 OnPaint(
        const FPaintArgs& Args,
        const FGeometry& Geo,
        const FSlateRect& CullingRect,
        FSlateWindowElementList& Out,
        int32 LayerId,
        const FWidgetStyle& Style,
        bool bParentEnabled) const override;

private:
    TAttribute<float>        BevelAttr;
    TAttribute<FLinearColor> ColorAttr;
    TAttribute<FMargin>      PaddingAttr;
    TAttribute<float>        NotchDepthAttr;
    TAttribute<float>        NotchHeightAttr;
    TAttribute<int32>        RightNotchCountAttr;
    TAttribute<int32>        LeftNotchCountAttr;
};
