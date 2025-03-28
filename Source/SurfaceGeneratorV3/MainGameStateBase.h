// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlockDefaults.h"
#include "Chunk.h"
#include "TerrainGenerator.h"
#include "GameFramework/GameStateBase.h"
#include "MainGameStateBase.generated.h"

/**
 * Contains:
 * Map of Chunks and their location;
 * Pool of unused Chunks;
 * TerrainGenerator instance;
 * Array of classes for all Blocks in game
 */

UCLASS()
class SURFACEGENERATORV3_API AMainGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	TMap<FIntVector, AChunk*> ChunksMap;
	TQueue<AChunk*> ChunksPool;

	friend void AChunk::CloseLoading();
	TDoubleLinkedList<AChunk*> LoadChunksQueue;
	TQueue<AChunk*> UnloadChunksQueue;
	
	void SpawnChunk(const FIntVector ChunkLocation);
	void DestroyChunk(const FIntVector ChunkLocation);
	
public:
	FTerrainGenerator* TerrainGenerator;
	
	UPROPERTY(EditAnywhere)
	TArray<FBlockDefaults> BlocksDefaults;
	UPROPERTY(EditAnywhere)
	UMaterial* BlocksMaterial;
	
	AMainGameStateBase();
	virtual ~AMainGameStateBase() override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	void PlaceChunk(const FIntVector ChunkLocation);
	void ExtractChunk(const FIntVector ChunkLocation);
	
	void AddToLoadChunks(AChunk* Chunk);
	void AddToUnloadChunks(AChunk* Chunk);
	
	UFUNCTION(BlueprintCallable)
	AChunk* GetChunk(const FIntVector ChunkLocation);
};
