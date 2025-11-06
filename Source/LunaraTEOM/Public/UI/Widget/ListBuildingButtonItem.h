#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "ListBuildingButtonItem.generated.h"

USTRUCT(BlueprintType)
struct FListBuildingButtonItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "List Building")
    FText Label = FText::GetEmpty();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "List Building")
    FSlateBrush Icon;
};
