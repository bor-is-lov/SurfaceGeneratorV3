// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SURFACEGENERATORV3_API UMainCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	float FluidFriction = 1.0f;
	int FluidCollisionsAmount = 0;
	
	public:
    virtual void PhysSwimming(float deltaTime, int32 Iterations) override;

	UFUNCTION(BlueprintNativeEvent)
	void StartSwimming(float NewFluidFriction);
	UFUNCTION(BlueprintNativeEvent)
	void StopSwimming();
};
