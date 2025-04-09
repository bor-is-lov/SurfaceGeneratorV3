#pragma once

#include "CoreMinimal.h"
#include "BlockDefaults.generated.h"

UENUM(BlueprintType)
enum class EShape : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Cube UMETA(DisplayName = "Cube"),
	Liquid UMETA(DisplayName = "Liquid")
};

USTRUCT(BlueprintType)
struct FBlockDefaults
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EShape Shape;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BlockName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsSolid;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, int> TexturesIndices;
};
