#include "UI/Widget/SBeveledBorder.h"            

#include "Rendering/DrawElements.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/CoreStyle.h"

void SBeveledBorder::Construct(const FArguments& InArgs)
{
    BevelAttr       = InArgs._Bevel;
    ColorAttr       = InArgs._Color;
    PaddingAttr     = InArgs._Padding;
    NotchDepthAttr  = InArgs._NotchDepth;
    NotchHeightAttr = InArgs._NotchHeight;
    RightNotchCountAttr = InArgs._RightNotchCount;
    LeftNotchCountAttr  = InArgs._LeftNotchCount;

    ChildSlot
    .Padding(PaddingAttr)
    [
        InArgs._Content.Widget
    ];
}

int32 SBeveledBorder::OnPaint(
    const FPaintArgs& Args,
    const FGeometry& Geo,
    const FSlateRect& CullingRect,
    FSlateWindowElementList& Out,
    int32 LayerId,
    const FWidgetStyle& Style,
    bool bParentEnabled) const
{
    const FVector2D S = Geo.GetLocalSize();
    const float W = S.X, H = S.Y;

    const float B  = FMath::Clamp(BevelAttr.Get(), 0.f, 0.5f * FMath::Min(W, H));
    const float ND = FMath::Max(0.f, NotchDepthAttr.Get());
    const float NH = FMath::Max(0.f, NotchHeightAttr.Get());

    const FLinearColor LinCol = ColorAttr.Get() * Style.GetColorAndOpacityTint();

    if (B <= 0.5f || ND <= 0.5f || NH <= 0.5f)
    {
        const FSlateBrush* White = FCoreStyle::Get().GetBrush("WhiteBrush");
        FSlateDrawElement::MakeBox(Out, LayerId, Geo.ToPaintGeometry(), White, ESlateDrawEffect::None, LinCol);
        return SCompoundWidget::OnPaint(Args, Geo, CullingRect, Out, LayerId + 1, Style, bParentEnabled);
    }

    const float AvailableHeight = FMath::Max(0.f, H - (2.f * B));
    const float ClampedND = FMath::Clamp(ND, 0.f, FMath::Max(0.f, (W * 0.5f) - B));

    struct FNotchLayout
    {
        int32 Count = 0;
        float Height = 0.f;
        float Spacing = 0.f;
    };

    auto CalcLayout = [&](int32 InCount) -> FNotchLayout
    {
        FNotchLayout Layout;
        const int32 Count = FMath::Max(0, InCount);
        if (Count <= 0 || AvailableHeight <= KINDA_SMALL_NUMBER)
        {
            return Layout;
        }

        const float TargetHeight = FMath::Max(0.f, NH);
        if (TargetHeight <= KINDA_SMALL_NUMBER)
        {
            return Layout;
        }

        const float MaxHeightPerNotch = AvailableHeight / static_cast<float>(Count);
        const float NotchHeight = FMath::Min(TargetHeight, MaxHeightPerNotch);
        if (NotchHeight <= KINDA_SMALL_NUMBER)
        {
            return Layout;
        }

        const float Spacing = FMath::Max(0.f, (AvailableHeight - (NotchHeight * Count)) / static_cast<float>(Count + 1));

        Layout.Count = Count;
        Layout.Height = NotchHeight;
        Layout.Spacing = Spacing;
        return Layout;
    };

    const FNotchLayout RightLayout = CalcLayout(RightNotchCountAttr.Get());
    const FNotchLayout LeftLayout  = CalcLayout(LeftNotchCountAttr.Get());

    TArray<FVector2D> Outline;
    Outline.Reserve(12 + (RightLayout.Count + LeftLayout.Count) * 3);

    auto AddPoint = [&](float X, float Y)
    {
        const FVector2D Candidate(X, Y);
        if (Outline.Num() == 0 || !Outline.Last().Equals(Candidate, KINDA_SMALL_NUMBER))
        {
            Outline.Add(Candidate);
        }
    };

    AddPoint(B, 0.f);
    AddPoint(W - B, 0.f);
    AddPoint(W, B);

    if (RightLayout.Count > 0)
    {
        for (int32 Index = 0; Index < RightLayout.Count; ++Index)
        {
            const float Top = B + (RightLayout.Spacing * (Index + 1)) + (RightLayout.Height * Index);
            const float Bottom = Top + RightLayout.Height;
            const float Mid = Top + (RightLayout.Height * 0.5f);

            AddPoint(W, Top);
            AddPoint(W - ClampedND, Mid);
            AddPoint(W, Bottom);
        }
    }

    AddPoint(W, H - B);
    AddPoint(W - B, H);
    AddPoint(B, H);
    AddPoint(0.f, H - B);

    if (LeftLayout.Count > 0)
    {
        for (int32 Index = LeftLayout.Count - 1; Index >= 0; --Index)
        {
            const float Top = B + (LeftLayout.Spacing * (Index + 1)) + (LeftLayout.Height * Index);
            const float Bottom = Top + LeftLayout.Height;
            const float Mid = Top + (LeftLayout.Height * 0.5f);

            AddPoint(0.f, Bottom);
            AddPoint(ClampedND, Mid);
            AddPoint(0.f, Top);
        }
    }

    AddPoint(0.f, B);

    const FColor Col = LinCol.ToFColor(true);
    const FSlateRenderTransform Xform = Geo.ToPaintGeometry().GetAccumulatedRenderTransform();

    TArray<FSlateVertex> Verts;
    TArray<SlateIndex> Indices;

    Verts.Reserve(Outline.Num() + 1);
    Indices.Reserve(Outline.Num() * 3);

    for (const FVector2D& P : Outline)
    {
        Verts.Add(FSlateVertex::Make(Xform, FVector2f(P), FVector2f(0.f, 0.f), Col));
    }

    const int32 CenterIndex = Verts.Num();
    Verts.Add(FSlateVertex::Make(Xform, FVector2f(Geo.GetLocalSize() * 0.5f), FVector2f(0.f, 0.f), Col));

    for (int32 i = 0; i < CenterIndex; ++i)
    {
        const int32 a = i;
        const int32 b = (i + 1) % CenterIndex;
        Indices.Add(CenterIndex);
        Indices.Add(a);
        Indices.Add(b);
    }

    const FSlateBrush* White = FCoreStyle::Get().GetBrush("WhiteBrush");
    FSlateResourceHandle Handle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(*White);
    FSlateDrawElement::MakeCustomVerts(Out, LayerId, Handle, Verts, Indices, nullptr, 0, 0);

    return SCompoundWidget::OnPaint(Args, Geo, CullingRect, Out, LayerId + 1, Style, bParentEnabled);
}
