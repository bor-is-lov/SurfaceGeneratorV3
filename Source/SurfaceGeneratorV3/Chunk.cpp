// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk.h"

#include "MainGameStateBase.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

AChunk::AChunk()
{
	PrimaryActorTick.bCanEverTick = false;
	
	State = EState::Unloaded;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;
	
	Border = CreateDefaultSubobject<UBoxComponent>("Border");
	Border->SetupAttachment(RootComponent);
	Border->SetBoxExtent(FVector(800, 800, 800), false);
	Border->SetRelativeLocation(FVector(800, 800, 800));
	Border->bHiddenInGame = false;
	Border->SetGenerateOverlapEvents(false);
	Border->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
}

void AChunk::BeginPlay()
{
	Super::BeginPlay();

	if (auto GameState = GetWorld()->GetGameState<AMainGameStateBase>())
	{
		TArray<TSubclassOf<UInstancedStaticMeshComponent>>* BlocksClasses = &GameState->BlocksClasses;
		if(!BlocksClasses->IsEmpty())
			for(auto BlockClass : *BlocksClasses)
			{
				auto Ptr = NewObject<UInstancedStaticMeshComponent>(this, BlockClass);
				Ptr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
				Ptr->RegisterComponent();
				Blocks.Add(Ptr);
			}
	}
}

FIntVector AChunk::ActorLocationToChunkLocation(const FVector& ActorLocation)
{
	FIntVector ChunkLocation;
	ChunkLocation.X = ActorLocation.X / 1600.0f;
	ChunkLocation.Y = ActorLocation.Y / 1600.0f;
	ChunkLocation.Z = ActorLocation.Z / 1600.0f;
	if (ActorLocation.X < 0)
		ChunkLocation.X--;
	if (ActorLocation.Y < 0)
		ChunkLocation.Y--;
	if (ActorLocation.Z < 0)
		ChunkLocation.Z--;
	return ChunkLocation;
}

FIntVector AChunk::ChunkWorldLocationToChunkLocation(const FVector& WorldLocation)
{
	return FIntVector(WorldLocation.X / 1600.0f, WorldLocation.Y / 1600.0f, WorldLocation.Z / 1600.0f);
}

FVector AChunk::MakeWorldLocation(const FIntVector& ChunkLocation)
{
	return FVector(ChunkLocation.X * 1600, ChunkLocation.Y * 1600, ChunkLocation.Z * 1600);
}

bool AChunk::LoadChunk()
{
	if (State != EState::Unloaded)
		return false;
	
	State = EState::Loading;

	if(!Blocks.IsEmpty())
		for(int x = 0; x < 16; x++)
			for(int y = 0; y < 16; y++)
				Blocks[0]->AddInstance(FTransform(FVector(x * 100 + 50, y * 100 + 50, 50)));
	Border->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	SetHidden(false);
	
	State = EState::Loaded;
	return true;
}

bool AChunk::UnloadChunk()
{
	if (State != EState::Loaded)
		return false;
	
	State = EState::Unloading;
	
	Border->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	SetHidden(true);
	if(!Blocks.IsEmpty())
		for(auto Block : Blocks)
			Block->ClearInstances();
	
	State = EState::Unloaded;
	return true;
}
