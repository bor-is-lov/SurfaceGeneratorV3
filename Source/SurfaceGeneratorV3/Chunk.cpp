// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk.h"
#include "Components/BoxComponent.h"

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
}

void AChunk::BeginPlay()
{
	Super::BeginPlay();
	
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

void AChunk::LoadChunk()
{
	State = EState::Loading;

	SetHidden(false);
	// TODO load blocks here
	
	State = EState::Loaded;
}

void AChunk::UnloadChunk()
{
	State = EState::Unloading;
	
	SetHidden(true);
	// TODO unload blocks here
	
	State = EState::Unloaded;
}
