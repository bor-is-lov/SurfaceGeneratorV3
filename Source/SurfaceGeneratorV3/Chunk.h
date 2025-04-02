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
	bool bCollisionEnabled = false;
	
	UMaterial* BlocksMaterial;
	class AMainGameStateBase* MainGameState;

	void LoadPlanesAtIndex(const EFaceDirection FaceDir, const int FaceIndex, const AChunk* Touching);
	void UnloadPlanesTouching(const EFaceDirection FaceDir) const;
	
	void LoadPlanes();
	void UnloadPlanes();
	void LoadBoxes();
	void UnloadBoxes();
	
public:
	TDoubleLinkedList<AChunk*>::TDoubleLinkedListNode* InLoadChunksQueue;
	
	AChunk();
	virtual void BeginPlay() override;

	void LoadMeshes();
	void UnloadMeshes();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FBlockData> BlocksData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UHierarchicalInstancedStaticMeshComponent* Planes;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class UBoxComponent>> SolidBoxes;
	
	void StartLoading();
	void EndLoading();
	void StartUnloading();
	void EndUnloading();

	inline EState GetState() const { return State; }
	inline bool GetbCollisionEnabled() const { return bCollisionEnabled; }
	void SetbCollisionEnabled(bool NewCollisionEnabled);

	void SetBlock(const size_t InChunkIndex, const size_t TypeIndex);
	
	static FIntVector ActorLocationToChunkLocation(const FVector& ActorLocation);
	static FIntVector ChunkWorldLocationToChunkLocation(const FVector& WorldLocation);
	static FVector MakeWorldLocation(const FIntVector& ChunkLocation);

	static FIntVector BlockIndexToLocation(const size_t& Index);
	static void BlockIndexToLocation(const size_t& Index, int& x, int& y, int& z);
	static size_t LocationToBlockIndex(const FIntVector& Location);
	static size_t LocationToBlockIndex(const int x, const int y, const int z);
};
