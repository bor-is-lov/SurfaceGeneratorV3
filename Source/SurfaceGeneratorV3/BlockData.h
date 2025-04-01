#pragma once

#include "CoreMinimal.h"
#include "BlockDefaults.h"
#include "BlockData.generated.h"

class AMainGameStateBase;

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

	const FBlockDefaults* GetDefaults(const AMainGameStateBase* MainGameState) const;
	static const FBlockDefaults* GetDefaults(const AMainGameStateBase* MainGameState, const int Index);
	int GetTextureIndex(const AMainGameStateBase* MainGameState, const EFaceDirection FaceDir) const;
	bool ShouldAddFace(const AMainGameStateBase* MainGameState, const FBlockData* TouchingBlock) const;
};
