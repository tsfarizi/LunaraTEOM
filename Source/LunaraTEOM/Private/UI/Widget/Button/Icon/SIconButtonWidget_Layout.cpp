#include "UI/Widget/Button/Icon/SIconButtonWidget.h"

#include "UI/Style/LunaraTeomSlateWidgetStyle.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"

#include "Styling/CoreStyle.h"
#include "Brushes/SlateRoundedBoxBrush.h"

#include "Framework/Application/SlateApplication.h"

#include "Rendering/DrawElements.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SIconButtonWidget::BuildLayout()
{
    ChildSlot
    [
        SAssignNew(ButtonBox, SBox)
        .WidthOverride(ButtonDiameter)
        .HeightOverride(ButtonDiameter)
        .RenderTransformPivot(FVector2D(0.5f, 0.5f))
        .RenderTransform(this, &SIconButtonWidget::GetButtonRenderTransform)
        [
            SNew(SButton)
            .ButtonStyle(&ButtonStyle)
            .ContentPadding(0.f)
            .OnClicked(this, &SIconButtonWidget::HandleClick)
            .OnHovered(this, &SIconButtonWidget::HandleHovered)
            .OnUnhovered(this, &SIconButtonWidget::HandleUnhovered)
            .OnPressed(this, &SIconButtonWidget::HandlePressed)
            .OnReleased(this, &SIconButtonWidget::HandleReleased)
            [
                SNew(SOverlay)
                + SOverlay::Slot()
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    SAssignNew(IconBox, SBox)
                    .WidthOverride(ButtonDiameter * IconScale)
                    .HeightOverride(ButtonDiameter * IconScale)
                    [
                        SAssignNew(IconImage, SImage)
                        .Image(nullptr)
                        .ColorAndOpacity(FLinearColor::White)
                        .Visibility(EVisibility::Collapsed)
                    ]
                ]
            ]
        ]
    ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

int32 SIconButtonWidget::OnPaint(const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled) const
{
    const FVector2D Size = AllottedGeometry.GetLocalSize();
    const float Diameter = FMath::Min(Size.X, Size.Y);

    if (Diameter <= KINDA_SMALL_NUMBER)
    {
        return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
    }

    const float OuterStroke = FMath::Clamp(Diameter * 0.08f, 2.f, 8.f);
    const float InnerStroke = FMath::Clamp(Diameter * 0.05f, 1.f, 6.f);

    const FVector2D OuterSize(Diameter, Diameter);
    const FVector2D OuterOffset((Size - OuterSize) * 0.5f);

    const FVector2D MiddleSize = OuterSize - FVector2D(OuterStroke * 2.f, OuterStroke * 2.f);
    const FVector2D MiddleOffset = OuterOffset + FVector2D(OuterStroke, OuterStroke);

    const FVector2D InnerSize = MiddleSize - FVector2D(InnerStroke * 2.f, InnerStroke * 2.f);
    const FVector2D InnerOffset = MiddleOffset + FVector2D(InnerStroke, InnerStroke);

    if (InnerSize.X <= 0.f || InnerSize.Y <= 0.f)
    {
        return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
    }

    const FLinearColor Tint = InWidgetStyle.GetColorAndOpacityTint();

    const FLinearColor OuterColor = GetOuterStrokeColor() * Tint;
    const FLinearColor InnerColor = GetInnerStrokeColor() * Tint;
    const FLinearColor FillColor  = GetFillColor() * Tint;

    const float OuterRadius = Diameter * 0.5f;
    const float MiddleRadius = FMath::Max(1.f, MiddleSize.X * 0.5f);
    const float InnerRadius  = FMath::Max(1.f, InnerSize.X * 0.5f);

    const FVector2D ShadowOffset(2.f, 2.f);
    const FLinearColor ShadowColor(0.f, 0.f, 0.f, 0.25f * Tint.A);

    auto ToGeometry = [&](const FVector2D& Offset, const FVector2D& LocalSize)
    {
        return AllottedGeometry.ToPaintGeometry(FVector2f(LocalSize), FSlateLayoutTransform(Offset));
    };

    auto PaintCircle = [&](int32& InLayer, const FVector2D& Offset, const FVector2D& CircSize, float Radius, const FLinearColor& Color)
    {
        FSlateRoundedBoxBrush Brush(Color, Radius);
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            InLayer++,
            ToGeometry(Offset, CircSize),
            &Brush,
            ESlateDrawEffect::None,
            Color);
    };

    auto PaintCircleWithShadow = [&](int32& InLayer, const FVector2D& Offset, const FVector2D& CircSize, float Radius, const FLinearColor& Color)
    {
        FSlateRoundedBoxBrush ShadowBrush(ShadowColor, Radius);
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            InLayer++,
            ToGeometry(Offset + ShadowOffset, CircSize),
            &ShadowBrush,
            ESlateDrawEffect::None,
            ShadowColor);

        PaintCircle(InLayer, Offset, CircSize, Radius, Color);
    };

    auto PaintNotchedFill = [&](int32& InLayer, const FVector2D& Offset, const FVector2D& CircSize, float BaseRadius, float MaxRadius, float NotchDepth, float NotchHalfAngle, const FLinearColor& BaseColor)
    {
        const FVector2f LocalCenter(CircSize * 0.5f);
        constexpr int32 SegmentCount = 96;

        TArray<FSlateVertex> Vertices;
        TArray<SlateIndex> Indices;
        Vertices.Reserve(SegmentCount + 1);
        Indices.Reserve(SegmentCount * 3);

        const float AllowedGrowth = FMath::Max(0.f, MaxRadius - BaseRadius);
        const float MaxDepth = FMath::Clamp(NotchDepth, 0.f, AllowedGrowth);

        const float NotchAngles[4] = { 0.f, PI * 0.5f, PI, PI * 1.5f };

        const FPaintGeometry FillGeometry = AllottedGeometry.ToPaintGeometry(FVector2f::ZeroVector, FSlateLayoutTransform(Offset));
        const FSlateRenderTransform FillTransform = FillGeometry.GetAccumulatedRenderTransform();

        for (int32 i = 0; i < SegmentCount; ++i)
        {
            const float Angle = (2.f * PI * i) / SegmentCount;
            float Growth = 0.f;

            for (float NotchAngle : NotchAngles)
            {
                const float Dist = FMath::Abs(FMath::FindDeltaAngleRadians(Angle, NotchAngle));
                if (Dist < NotchHalfAngle)
                {
                    const float Weight = 1.f - (Dist / NotchHalfAngle);
                    Growth = FMath::Max(Growth, MaxDepth * Weight);
                }
            }

            float Radius = BaseRadius + Growth;
            Radius = FMath::Min(Radius, MaxRadius);
            Radius = FMath::Max(1.f, Radius);

            const float ShadeFactor = FMath::Clamp(1.f + 0.18f * FMath::Sin(Angle), 0.75f, 1.2f);
            FLinearColor VertexColor = BaseColor * ShadeFactor;
            VertexColor.A = BaseColor.A;

            const FVector2f Position = LocalCenter + FVector2f(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius);
            Vertices.Add(FSlateVertex::Make(FillTransform, Position, FVector2f::ZeroVector, VertexColor.ToFColor(true)));
        }

        const int32 CenterIndex = Vertices.Num();
        Vertices.Add(FSlateVertex::Make(FillTransform, LocalCenter, FVector2f::ZeroVector, BaseColor.ToFColor(true)));

        for (int32 i = 0; i < SegmentCount; ++i)
        {
            const int32 Next = (i + 1) % SegmentCount;
            Indices.Add(CenterIndex);
            Indices.Add(i);
            Indices.Add(Next);
        }

        const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush("WhiteBrush");
        FSlateResourceHandle Handle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(*WhiteBrush);

        FSlateDrawElement::MakeCustomVerts(
            OutDrawElements,
            InLayer++,
            Handle,
            Vertices,
            Indices,
            nullptr,
            0,
            0);
    };

    int32 CurrentLayer = LayerId;

    PaintCircleWithShadow(CurrentLayer, OuterOffset, OuterSize, OuterRadius, OuterColor);
    PaintCircleWithShadow(CurrentLayer, MiddleOffset, MiddleSize, MiddleRadius, InnerColor);

    const float FillInset   = FMath::Clamp(Diameter * 0.05f, 2.f, InnerRadius * 0.45f);
    const float FillBaseRadius = InnerRadius - FillInset;
    const float FillOuterLimit = InnerRadius - (FillInset * 0.15f);
    const float NotchDepth  = FMath::Clamp(Diameter * 0.08f, 3.f, FillOuterLimit - FillBaseRadius);
    const float NotchHalfAngle = PI / 18.f;

    PaintNotchedFill(CurrentLayer, InnerOffset, InnerSize, FillBaseRadius, FillOuterLimit, NotchDepth, NotchHalfAngle, FillColor);

    return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, CurrentLayer, InWidgetStyle, bParentEnabled);
}
