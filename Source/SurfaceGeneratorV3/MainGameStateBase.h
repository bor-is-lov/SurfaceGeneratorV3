// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "GameFramework/GameStateBase.h"
#include "MainGameStateBase.generated.h"

/**
 * Contains map of Chunks and their location
 */
UCLASS()
class SURFACEGENERATORV3_API AMainGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	TMap<FIntVector, AChunk*> ChunksMap;
	TQueue<AChunk*> ChunksPool;
	
	void SpawnChunk(const FIntVector ChunkLocation);
	void DestroyChunk(const FIntVector ChunkLocation);
	
public:
	AMainGameStateBase();
	
	void PlaceChunk(const FIntVector ChunkLocation);
	void ExtractChunk(const FIntVector ChunkLocation);
	
	UFUNCTION(BlueprintCallable)
	AChunk* GetChunk(const FIntVector ChunkLocation);
};
