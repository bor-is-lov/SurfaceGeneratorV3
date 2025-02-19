// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlockData.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"

/**
 * 
 */


UCLASS()
class SURFACEGENERATORV3_API AChunk : public AActor
{
	GENERATED_BODY()

protected:
	void UpdatePlanes();
	
public:
	AChunk();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* Border;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FBlockData> BlocksData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UStaticMeshComponent>> Planes;
	

	void LoadChunk();
	void UnloadChunk();

	void SetBlock(const size_t InChunkIndex, const size_t TypeIndex);
	
	static FIntVector ActorLocationToChunkLocation(const FVector& ActorLocation);
	static FIntVector ChunkWorldLocationToChunkLocation(const FVector& WorldLocation);
	static FVector MakeWorldLocation(const FIntVector& ChunkLocation);

	static FIntVector BlockIndexToLocation(const size_t& Index);
	static size_t LocationToBlockIndex(const FIntVector& Location);
	static size_t LocationToBlockIndex(const int x, const int y, const int z);
};
