// Fill out your copyright notice in the Description page of Project Settings.

#include "MainGameStateBase.h"

#include "MainPlayerController.h"
#include "Kismet/GameplayStatics.h"

AMainGameStateBase::AMainGameStateBase()
{
	srand(time(NULL));
	TerrainGenerator = new FTerrainGenerator(rand(), this);
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

AMainGameStateBase::~AMainGameStateBase()
{
	delete TerrainGenerator;
}

void AMainGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	
	AMainPlayerController* Controller = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(Controller)
		for (int i = 0; i < pow(Controller->GetRenderDistance() * 2 + 1, 3); i++)
		{
			AChunk* Ptr = GetWorld()->SpawnActor<AChunk>();
			ChunksPool.Enqueue(Ptr);
		}
}

void AMainGameStateBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AChunk* Chunk;
	for(int i = 0; i < 8; i++)
	{
		if(UnloadMeshesQueue.Dequeue(Chunk))
		{
			Chunk->UnloadPlanes();
			Chunk->EndUnloading();
		}
		else if(!LoadMeshesQueue.IsEmpty())
		{
			Chunk = LoadMeshesQueue.GetHead()->GetValue();
			Chunk->LoadPlanes();
			Chunk->EndLoading();
			LoadMeshesQueue.RemoveNode(LoadMeshesQueue.GetHead());
		}
		else
			break;
	}
}

void AMainGameStateBase::SpawnChunk(const FIntVector ChunkLocation)
{
	AChunk* Chunk = GetWorld()->SpawnActor<AChunk>(AChunk::MakeWorldLocation(ChunkLocation), FRotator::ZeroRotator);
	Chunk->StartLoading();
	ChunksMap.Add(ChunkLocation, Chunk);
}

void AMainGameStateBase::DestroyChunk(const FIntVector ChunkLocation)
{
	if(ChunksMap.Contains(ChunkLocation))
	{
		(*ChunksMap.Find(ChunkLocation))->Destroy();
		ChunksMap.Remove(ChunkLocation);
	}
}

void AMainGameStateBase::PlaceChunk(const FIntVector ChunkLocation)
{
	if (AChunk* Chunk; ChunksPool.Dequeue(Chunk))
	{
		if(Chunk->GetState() == AChunk::EState::Unloaded)
		{
			Chunk->SetActorLocation(AChunk::MakeWorldLocation(ChunkLocation));
			Chunk->StartLoading();
			ChunksMap.Add(ChunkLocation, Chunk);
		}
		else
		{
			ChunksPool.Enqueue(Chunk);
			SpawnChunk(ChunkLocation);
		}
	}
	else
		SpawnChunk(ChunkLocation);
}

void AMainGameStateBase::ExtractChunk(const FIntVector ChunkLocation)
{
	if(ChunksMap.Contains(ChunkLocation))
	{
		AChunk* Chunk = *ChunksMap.Find(ChunkLocation);
		switch(Chunk->GetState())
		{
		case AChunk::EState::Loaded:
			Chunk->StartUnloading();
			break;
		case AChunk::EState::Loading:
			Chunk->CloseLoading();
			Chunk->StartUnloading();
			break;
		default:
			break;
		}
		ChunksPool.Enqueue(Chunk);
		ChunksMap.Remove(ChunkLocation);
	}
}

void AMainGameStateBase::AddToLoadMeshes(AChunk* Chunk)
{
	LoadMeshesQueue.AddTail(Chunk);
}

void AMainGameStateBase::AddToUnloadMeshes(AChunk* Chunk)
{
	UnloadMeshesQueue.Enqueue(Chunk);
}

AChunk* AMainGameStateBase::GetChunk(const FIntVector ChunkLocation)
{
	if (!ChunksMap.Contains(ChunkLocation))
		return nullptr;
	AChunk* Chunk = *ChunksMap.Find(ChunkLocation);
	if (!Chunk)
		ChunksMap.Remove(ChunkLocation);
	return Chunk;
}
