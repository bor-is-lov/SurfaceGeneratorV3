// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* Border;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UInstancedStaticMeshComponent*> Blocks;
	
	enum class EState{ Unloaded, Loading, Loaded, Unloading };
	
protected:
	virtual void BeginPlay() override;

	EState State;

public:
	AChunk();
	
	static FIntVector ActorLocationToChunkLocation(const FVector& ActorLocation);
	static FIntVector ChunkWorldLocationToChunkLocation(const FVector& WorldLocation);
	static FVector MakeWorldLocation(const FIntVector& ChunkLocation);
	
	bool LoadChunk();
	bool UnloadChunk();

	inline EState GetState() const { return State; }
};
