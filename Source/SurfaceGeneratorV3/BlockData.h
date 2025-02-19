#pragma once

#include "CoreMinimal.h"
#include "BlockDefaults.h"
#include "BlockData.generated.h"

class AMainGameStateBase;

USTRUCT(BlueprintType)
struct FBlockData
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int TypeIndex;
	
	void SetBlock(const int TypeIndex);
	void EmptyBlock();

	const FBlockDefaults* GetDefaults(const UWorld* World) const;
};
