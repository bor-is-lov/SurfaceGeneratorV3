// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

AMainCharacter::AMainCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMainCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, -90.0f, 0.0f)));

	CameraBoom->SetupAttachment(GetMesh());
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	CameraBoom->bUsePawnControlRotation = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;
	
	EndSprint();
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterMovement()->Buoyancy = 0.9f;
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (LookInputAction)
			Input->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
		if(MoveInputAction)
			Input->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		if(MoveVerticalInputAction)
			Input->BindAction(MoveVerticalInputAction, ETriggerEvent::Triggered, this, &AMainCharacter::MoveVertical);
		if(JumpInputAction)
			Input->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		if(SprintInputAction)
		{
			Input->BindAction(SprintInputAction, ETriggerEvent::Triggered, this, &AMainCharacter::BeginSprint);
			Input->BindAction(SprintInputAction, ETriggerEvent::Completed, this, &AMainCharacter::EndSprint);
		}
		if(CrouchInputAction)
		{
			Input->BindAction(CrouchInputAction, ETriggerEvent::Triggered, this, &AMainCharacter::BeginCrouch);
			Input->BindAction(CrouchInputAction, ETriggerEvent::Completed, this, &AMainCharacter::EndCrouch);
		}
	}
}

void AMainCharacter::Look(const FInputActionInstance& Instance)
{
	FVector2D InputAxis = Instance.GetValue().Get<FVector2D>();
	AddControllerYawInput(InputAxis.X);
	AddControllerPitchInput(InputAxis.Y);
}

void AMainCharacter::Move(const struct FInputActionInstance& Instance)
{
	if(Controller)
	{
		FVector2D InputAxis = Instance.GetValue().Get<FVector2D>();
		AddMovementInput(FRotationMatrix(FRotator(0.0, Controller->GetControlRotation().Yaw, 0.0)).GetUnitAxis(EAxis::X), InputAxis.X);
		AddMovementInput(FRotationMatrix(FRotator(0.0, Controller->GetControlRotation().Yaw, 0.0)).GetUnitAxis(EAxis::Y), InputAxis.Y);
	}
}

void AMainCharacter::MoveVertical(const FInputActionInstance& Instance)
{
	if(Controller)
	{
		float InputAxis = Instance.GetValue().Get<float>();
		AddMovementInput(FRotationMatrix(FRotator(0.0, 0.0, Controller->GetControlRotation().Roll)).GetUnitAxis(EAxis::Z), InputAxis);
	}
}

void AMainCharacter::BeginSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	GetCharacterMovement()->MaxSwimSpeed = SprintSwimSpeed;
}

void AMainCharacter::EndSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxSwimSpeed = SwimSpeed;
}

