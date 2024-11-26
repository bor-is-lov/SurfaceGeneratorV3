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
	AChunk();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* Border;
	
	enum class EState{ Unloaded, Loading, Loaded, Unloading };
	
protected:
	virtual void BeginPlay() override;

	EState State;

public:	
	static FIntVector ActorLocationToChunkLocation(const FVector& ActorLocation);
	static FIntVector ChunkWorldLocationToChunkLocation(const FVector& WorldLocation);
	static FVector MakeWorldLocation(const FIntVector& ChunkLocation);
	
	void LoadChunk();
	void UnloadChunk();
};
