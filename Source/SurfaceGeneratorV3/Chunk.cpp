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
	Border->SetGenerateOverlapEvents(false);
}

void AChunk::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);

	SetActorHiddenInGame(true);
	if (auto GameState = GetWorld()->GetGameState<AMainGameStateBase>())
	{
		TArray<TSubclassOf<UInstancedStaticMeshComponent>>* BlocksClasses = &GameState->BlocksClasses;
		if(!BlocksClasses->IsEmpty())
			for(auto BlockClass : *BlocksClasses)
			{
				auto Ptr = NewObject<UInstancedStaticMeshComponent>(this, BlockClass);
				Ptr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
				Ptr->RegisterComponent();
				Ptr->SetComponentTickEnabled(false);
				BlockTypes.Add(Ptr);
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

void AChunk::StartLoading()
{
	if (State != EState::Unloaded)
		return;
	
	State = EState::Loading;
	Border->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetWorld()->GetGameStateChecked<AMainGameStateBase>()->TerrainGenerator->AddChunkToSpawnQueue(this);
}

void AChunk::StartUnloading()
{
	if (State != EState::Loaded)
		return;
	
	State = EState::Unloading;
	Border->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	SetActorHiddenInGame(true);
	if(!BlockTypes.IsEmpty())
		for(size_t i = 0; i < BlockTypes.Num(); i++)
			GetWorld()->GetGameStateChecked<AMainGameStateBase>()->AddToUnloadBlocksQueue(this, i);
}

void AChunk::CloseLoading()
{
	if (State != EState::Loading)
		return;
	for(auto BlocksToSpawnInChunk = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->SpawnInstancesQueue.GetHead(); BlocksToSpawnInChunk != nullptr;)
	{
		auto ToDel = BlocksToSpawnInChunk;
		BlocksToSpawnInChunk = BlocksToSpawnInChunk->GetNextNode();
		if(this == ToDel->GetValue().Chunk)
		{
			GetWorld()->GetGameStateChecked<AMainGameStateBase>()->SpawnInstancesQueue.RemoveNode(ToDel);
			break;
		}
	}
	State = EState::Unloaded;
}

void AChunk::EndLoading()
{
	if (State != EState::Loading)
		return;
	
	SetActorHiddenInGame(false);
	Border->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	State = EState::Loaded;
}

void AChunk::EndUnloading()
{
	if (State != EState::Unloading)
		return;
	
	Border->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	State = EState::Unloaded;
}