// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "TerrainGenerator.h"
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
	TDoubleLinkedList<TTuple<AChunk*, TArray<TTuple<size_t, FIntVector>>>> SpawnInstancesQueue;
	
	void SpawnChunk(const FIntVector ChunkLocation);
	void DestroyChunk(const FIntVector ChunkLocation);
	
public:
	AMainGameStateBase();
	virtual ~AMainGameStateBase() override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	FTerrainGenerator* TerrainGenerator;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UInstancedStaticMeshComponent>> BlocksClasses;
	
	void PlaceChunk(const FIntVector ChunkLocation);
	void ExtractChunk(const FIntVector ChunkLocation);

	void AddToUnloadBlocksQueue(AChunk* Chunk, size_t BlockIndex);
	void AddToSpawnInstancesQueue(AChunk* Chunk, TArray<TTuple<size_t, FIntVector>>& ToSpawn);
	
	UFUNCTION(BlueprintCallable)
	AChunk* GetChunk(const FIntVector ChunkLocation);
};
