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

public:
	enum class EState{ Unloaded, Loading, Loaded, Unloading };
	
private:	
	EState State;

	UMaterial* BlocksMaterial;

	void UnloadPlanesTouching(const EFaceDirection FaceDir) const;
	
public:
	AChunk();
	virtual void BeginPlay() override;

	void LoadPlanes();
	void UnloadPlanes();

private:
	friend void AChunk::LoadPlanes();
	void LoadPlanesAtIndex(const EFaceDirection FaceDir, const int FaceIndex, const AChunk* Touching);
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FBlockData> BlocksData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UHierarchicalInstancedStaticMeshComponent* Planes;
	
	void StartLoading();
	void EndLoading();
	void StartUnloading();
	void EndUnloading();
	// Stops loading in a safe manner. Doesn't unload chunk.
	void CloseLoading();

	inline EState GetState() const { return State; }

	void SetBlock(const size_t InChunkIndex, const size_t TypeIndex);
	
	static FIntVector ActorLocationToChunkLocation(const FVector& ActorLocation);
	static FIntVector ChunkWorldLocationToChunkLocation(const FVector& WorldLocation);
	static FVector MakeWorldLocation(const FIntVector& ChunkLocation);

	static FIntVector BlockIndexToLocation(const size_t& Index);
	static size_t LocationToBlockIndex(const FIntVector& Location);
	static size_t LocationToBlockIndex(const int x, const int y, const int z);
};
