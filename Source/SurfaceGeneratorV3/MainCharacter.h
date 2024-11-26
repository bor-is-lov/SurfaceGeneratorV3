// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.generated.h"

UCLASS()
class SURFACEGENERATORV3_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MoveInputAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* LookInputAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* JumpInputAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* SprintInputAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* CrouchInputAction;
	
	UPROPERTY(EditAnywhere, Category = "Speed")
	float WalkSpeed = 600.0f;
	UPROPERTY(EditAnywhere, Category = "Speed")
	float SprintSpeed = 1000.0f;
	

	void Look(const struct FInputActionInstance& Instance);
	void Move(const struct FInputActionInstance& Instance);
	inline void BeginSprint() { GetCharacterMovement()->MaxWalkSpeed = SprintSpeed; }
	inline void EndSprint() { GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; }
	void BeginCrouch() { Crouch(); }
	void EndCrouch() { UnCrouch(); }
	
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

