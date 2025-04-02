// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "MainPlayerController.generated.h"

class AChunk;
/**
 * Sets up Input Mapping Context
 * PlayerChunkLocation is Chunk's location the player currently in
 */
UCLASS()
class SURFACEGENERATORV3_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

	int ActualRenderDistance = 0;
	float ActualZScale = 0.5f;
	bool bCanMove = true;
	
	TStaticArray<AChunk*, 27> CollisionEnabledChunks;
	void UpdateChunksCollision();

protected:
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chunk")
	FIntVector PlayerChunkLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	int RenderDistance = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	float ZScale = 0.5f;
	
public:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	inline int GetRenderDistance() const { return RenderDistance; }
	inline float GetZScale() const { return ZScale; }
};
