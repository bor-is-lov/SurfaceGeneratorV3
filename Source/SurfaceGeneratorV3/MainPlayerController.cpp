// Fill out your copyright notice in the Description page of Project Settings.

#include "MainPlayerController.h"

#include "Chunk.h"
#include "EnhancedInputSubsystems.h"
#include "MainGameModeBase.h"
#include "MainGameStateBase.h"

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
	GetWorld()->GetGameStateChecked<AMainGameStateBase>()->TerrainGenerator->UpdateHeightMap(ActualRenderDistance, PlayerChunkLocation);
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerChunkLocation != AChunk::ActorLocationToChunkLocation(GetPawn()->GetActorLocation()) || ActualRenderDistance != RenderDistance || ActualZScale != ZScale)
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
		
		PlayerChunkLocation = AChunk::ActorLocationToChunkLocation(GetPawn()->GetActorLocation());
		Cast<AMainGameModeBase>(GetWorld()->GetAuthGameMode())->UpdateChunks(ActualRenderDistance, ActualZScale, PlayerChunkLocation);
		GetWorld()->GetGameStateChecked<AMainGameStateBase>()->TerrainGenerator->UpdateHeightMap(ActualRenderDistance, PlayerChunkLocation);
	}
}
