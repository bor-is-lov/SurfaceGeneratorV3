#pragma once

#include "CoreMinimal.h"
#include "BlockDefaults.h"
#include "BlockData.generated.h"

UENUM(BlueprintType)
enum class EFaceDirection : uint8
{
	XPositive UMETA(DisplayName = "XPositive"),
	XNegative UMETA(DisplayName = "XNegative"),
	YPositive UMETA(DisplayName = "YPositive"),
	YNegative UMETA(DisplayName = "YNegative"),
	ZPositive UMETA(DisplayName = "ZPositive"),
	ZNegative UMETA(DisplayName = "ZNegative")
};

USTRUCT(BlueprintType)
struct FBlockData
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int DefaultsIndex;
	
	void SetBlock(const int TypeIndex);
	void EmptyBlock();

	FBlockDefaults* GetDefaults(const UWorld* World) const;
	static FBlockDefaults* GetDefaults(const UWorld* World, const int Index);
	int GetTextureIndex(const UWorld* World, const EFaceDirection FaceDir) const;
	bool ShouldAddFace(const UWorld* World, const FBlockData* TouchingBlock) const;
};
