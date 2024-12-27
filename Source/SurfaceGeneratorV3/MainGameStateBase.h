// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "GameFramework/GameStateBase.h"
#include "MainGameStateBase.generated.h"

/**
 * Contains:
 * Map of Chunks and their location;
 * Array of Classes for all blocks in game;
 * Queue for staggered clearing Blocks instances;
 * Queue for staggered adding Block instances
 */
UCLASS()
class SURFACEGENERATORV3_API AMainGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	TMap<FIntVector, AChunk*> ChunksMap;
	TQueue<AChunk*> ChunksPool;
	
	TDoubleLinkedList<TTuple<AChunk*, size_t>> ClearInstancesQueue;
	
	friend void AChunk::CloseLoading();
	TDoubleLinkedList<TTuple<AChunk*, size_t, FIntVector>> SpawnInstancesQueue;
	
	void SpawnChunk(const FIntVector ChunkLocation);
	void DestroyChunk(const FIntVector ChunkLocation);
	
public:
	AMainGameStateBase();

	void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UInstancedStaticMeshComponent>> BlocksClasses;
	
	void PlaceChunk(const FIntVector ChunkLocation);
	void ExtractChunk(const FIntVector ChunkLocation);

	void AddToUnloadBlocksQueue(AChunk* Chunk, size_t BlockIndex);
	void AddToSpawnInstancesQueue(AChunk* Chunk, size_t BlockIndex, FIntVector Location);
	
	UFUNCTION(BlueprintCallable)
	AChunk* GetChunk(const FIntVector ChunkLocation);
};
