// Fill out your copyright notice in the Description page of Project Settings.

#include "MainPlayerController.h"

#include "Chunk.h"
#include "EnhancedInputSubsystems.h"
#include "MainCharacter.h"
#include "MainGameModeBase.h"
#include "MainGameStateBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>())
			if (!InputMapping.IsNull())
				InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), 0);
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerChunkLocation = AChunk::ActorLocationToChunkLocation(GetPawn()->GetActorLocation());
	Cast<AMainGameModeBase>(GetWorld()->GetAuthGameMode())->UpdateChunks(ActualRenderDistance, ActualZScale, PlayerChunkLocation);
	UpdateChunksCollision();
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FIntVector ActualPlayerChunkLocation = AChunk::ActorLocationToChunkLocation(GetPawn()->GetActorLocation());

	if(Cast<AMainCharacter>(GetCharacter()))
	{
		if(const AChunk* Chunk = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk(ActualPlayerChunkLocation);
			Chunk && Chunk->GetState() == AChunk::EState::Loaded)
		{
			if(!bCanMove)
			{
				GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				bCanMove = true;
			}
		}
		else if(bCanMove)
		{
			UCharacterMovementComponent* Movement = GetCharacter()->GetCharacterMovement();
			Movement->StopMovementImmediately();
			Movement->DisableMovement();
			bCanMove = false;
		}
	}

	if (PlayerChunkLocation != ActualPlayerChunkLocation || ActualRenderDistance != RenderDistance || ActualZScale != ZScale)
	{
		if(ActualRenderDistance < RenderDistance)
			ActualRenderDistance++;
		else if(ActualRenderDistance > RenderDistance)
			ActualRenderDistance--;
		
		if(ActualZScale < ZScale)
			if(ZScale - ActualZScale <= 0.1f)
				ActualZScale = ZScale;
			else
				ActualZScale += 0.1f;
		if(ActualZScale > ZScale)
			if(ActualZScale - ZScale <= 0.1f)
				ActualZScale = ZScale;
			else
				ActualZScale -= 0.1f;
		
		PlayerChunkLocation = ActualPlayerChunkLocation;
		Cast<AMainGameModeBase>(GetWorld()->GetAuthGameMode())->UpdateChunks(ActualRenderDistance, ActualZScale, PlayerChunkLocation);
		UpdateChunksCollision();
	}
}

void AMainPlayerController::UpdateChunksCollision()
{
	TStaticArray<AChunk*, 27> ToDisableCollisionChunks;
	for(int i = 0; i < 27; i++)
		ToDisableCollisionChunks[i] = CollisionEnabledChunks[i];

	{
		int i = 0;
		for(int x = -1; x <= 1; x++)
			for(int y = -1; y <= 1; y++)
				for(int z = -1; z <= 1; z++)
					CollisionEnabledChunks[i++] = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({
					PlayerChunkLocation.X + x,
					PlayerChunkLocation.Y + y,
					PlayerChunkLocation.Z + z});
	}

	for(AChunk* ToDisableCollisionChunk : ToDisableCollisionChunks)
		if(ToDisableCollisionChunk)
		{
			bool ShouldStopCollision = true;
			for(AChunk* CollisionEnabledChunk : CollisionEnabledChunks)
				if(ToDisableCollisionChunk == CollisionEnabledChunk)
				{
					ShouldStopCollision = false;
					break;
				}
			if(ShouldStopCollision)
				ToDisableCollisionChunk->SetbCollisionEnabled(false);
		}
	
	for(AChunk* Chunk : CollisionEnabledChunks)
		if(Chunk)
			Chunk->SetbCollisionEnabled(true);
}
