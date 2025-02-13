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

// Stores pointer to Chunk and index in array of block types (ISM components) for that Chunk
// Used in ClearInstancesQueue
struct FBlockTypeInChunk
{
	AChunk* Chunk;
	size_t ISMIndex;

	FBlockTypeInChunk(AChunk* Chunk, const size_t ISMIndex) : Chunk(Chunk), ISMIndex(ISMIndex) {}
	TObjectPtr<UInstancedStaticMeshComponent> GetBlockType() const {return Chunk->BlockTypes[ISMIndex];}
};

// Stores index in array of block types (ISM components) and position for that block to be placed in Chunk
// Used in SpawnInstancesQueue
struct FBlockInfo
{
	size_t ISMIndex;
	FIntVector Position;

	FBlockInfo(const size_t ISMIndex, const FIntVector& Position) : ISMIndex(ISMIndex), Position(Position) {}
};

// Stores pointer to Chunk and array of FBlockInfos to know where and what type of blocks should be placed in Chunk
// Used in SpawnInstancesQueue
struct FBlocksInfoInChunk
{
	AChunk* Chunk;
	TArray<FBlockInfo> BlocksInfo;

	FBlocksInfoInChunk(AChunk* Chunk, const TArray<FBlockInfo>& BlocksInfo) : Chunk(Chunk), BlocksInfo(BlocksInfo) {}
	TObjectPtr<UInstancedStaticMeshComponent> GetBlockType(const int Index) {return Chunk->BlockTypes[BlocksInfo[Index].ISMIndex];}
};

UCLASS()
class SURFACEGENERATORV3_API AMainGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	TMap<FIntVector, AChunk*> ChunksMap;
	TQueue<AChunk*> ChunksPool;
	
	TDoubleLinkedList<FBlockTypeInChunk> ClearInstancesQueue;
	
	friend void AChunk::CloseLoading();
	TDoubleLinkedList<FBlocksInfoInChunk> SpawnInstancesQueue;
	
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
	void AddToSpawnInstancesQueue(AChunk* Chunk, TArray<FBlockInfo>& ToSpawn);
	
	UFUNCTION(BlueprintCallable)
	AChunk* GetChunk(const FIntVector ChunkLocation);
};
