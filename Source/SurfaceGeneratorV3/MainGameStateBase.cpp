// Fill out your copyright notice in the Description page of Project Settings.

#include "MainGameStateBase.h"

#include "MainPlayerController.h"
#include "Kismet/GameplayStatics.h"

AMainGameStateBase::AMainGameStateBase()
{
	srand(time(nullptr));
	TerrainGenerator = new FTerrainGenerator(rand());
	
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
	{
		const int count = pow(Controller->GetRenderDistance() * 2 + 1, 3) * Controller->GetZScale();
		for (int i = 0; i < count; i++)
		{
			AChunk* Ptr = GetWorld()->SpawnActor<AChunk>();
			ChunksPool.Enqueue(Ptr);
		}

		Controller->GetPawn()->SetActorLocation(TerrainGenerator->GetSpawnPosition());
	}
}

void AMainGameStateBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AChunk* Chunk;
	
	for(int i = 0; i < 8; i++)
	{
		if(UnloadChunksQueue.Dequeue(Chunk))
		{
			Chunk->UnloadPlanes();
			Chunk->EndUnloading();
		}
		else if(!LoadChunksQueue.IsEmpty())
		{
			Chunk = LoadChunksQueue.GetHead()->GetValue();
			TerrainGenerator->GenerateChunk(Chunk);
			Chunk->LoadPlanes();
			Chunk->EndLoading();
			LoadChunksQueue.RemoveNode(LoadChunksQueue.GetHead());
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

void AMainGameStateBase::AddToLoadChunks(AChunk* Chunk)
{
	LoadChunksQueue.AddTail(Chunk);
}

void AMainGameStateBase::AddToUnloadChunks(AChunk* Chunk)
{
	UnloadChunksQueue.Enqueue(Chunk);
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
