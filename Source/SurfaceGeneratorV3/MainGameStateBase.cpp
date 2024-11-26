// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameStateBase.h"

AMainGameStateBase::AMainGameStateBase()
{
	
}

void AMainGameStateBase::SpawnChunk(const FIntVector ChunkLocation)
{
	AChunk* Ptr = GetWorld()->SpawnActor<AChunk>(AChunk::MakeWorldLocation(ChunkLocation), FRotator::ZeroRotator);
	ChunksMap.Add(ChunkLocation, Ptr);
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
	AChunk* Ptr;
	if (ChunksPool.Dequeue(Ptr))
	{
		Ptr->SetActorLocation(AChunk::MakeWorldLocation(ChunkLocation));
		Ptr->LoadChunk();
		ChunksMap.Add(ChunkLocation, Ptr);
	}
	else
		SpawnChunk(ChunkLocation);
}

void AMainGameStateBase::ExtractChunk(const FIntVector ChunkLocation)
{
	if(ChunksMap.Contains(ChunkLocation))
	{
		(*ChunksMap.Find(ChunkLocation))->UnloadChunk();
		ChunksPool.Enqueue(*ChunksMap.Find(ChunkLocation));
		ChunksMap.Remove(ChunkLocation);
	}
}

AChunk* AMainGameStateBase::GetChunk(const FIntVector ChunkLocation)
{
	if (!ChunksMap.Contains(ChunkLocation))
		return nullptr;
	AChunk* Ptr = *ChunksMap.Find(ChunkLocation);
	if (!Ptr)
		ChunksMap.Remove(ChunkLocation);
	return Ptr;
}
