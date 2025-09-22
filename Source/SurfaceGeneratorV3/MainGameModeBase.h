// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SURFACEGENERATORV3_API AMainGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateChunks(int RenderDistance, int ZRenderDistance, FIntVector PlayerChunkLocation) const;
};
