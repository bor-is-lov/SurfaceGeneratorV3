// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameStateBase.h"

#include "MainPlayerController.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AMainGameStateBase::AMainGameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AMainPlayerController* Controller = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(Controller)
	    for (int i = 0; i < pow(Controller->GetRenderDistance() * 2 + 1, 3) * 2; i++)
	    {
	    	AChunk* Ptr = GetWorld()->SpawnActor<AChunk>();
	    	ChunksPool.Enqueue(Ptr);
	    }
}

void AMainGameStateBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!ClearInstancesQueue.IsEmpty())
	{
		auto Tuple = ClearInstancesQueue.GetHead()->GetValue();
		Tuple.Get<0>()->Blocks[Tuple.Get<1>()]->ClearInstances();
		ClearInstancesQueue.RemoveNode(ClearInstancesQueue.GetHead());
		bool ToEndUnloading = true;
		for(auto Tuple2 : ClearInstancesQueue)
			if(Tuple2.Get<0>() == Tuple.Get<0>())
			{
				ToEndUnloading = false;
				break;
			}
		if(ToEndUnloading)
			Tuple.Get<0>()->EndUnloading();
	}
	else
		if(!SpawnInstancesQueue.IsEmpty())
		{
			TTuple<AChunk*, TArray<TTuple<size_t, FIntVector>>> Tuple = SpawnInstancesQueue.GetHead()->GetValue();
			for(int i = 0; i < Tuple.Get<1>().Num(); i++)
			{
				FVector Location = FVector(50 + Tuple.Get<1>()[i].Get<1>().X * 100, 50 + Tuple.Get<1>()[i].Get<1>().Y * 100, 50 + Tuple.Get<1>()[i].Get<1>().Z * 100);
				Tuple.Get<0>()->Blocks[Tuple.Get<1>()[i].Get<0>()]->AddInstance(FTransform(Location));
			}
			SpawnInstancesQueue.RemoveNode(SpawnInstancesQueue.GetHead());
			bool ToEndLoading = true;
			for(auto Tuple2 : SpawnInstancesQueue)
				if(Tuple2.Get<0>() == Tuple.Get<0>())
				{
					ToEndLoading = false;
					break;
				}
			if(ToEndLoading)
				Tuple.Get<0>()->EndLoading();
		}
}

void AMainGameStateBase::SpawnChunk(const FIntVector ChunkLocation)
{
	AChunk* Ptr = GetWorld()->SpawnActor<AChunk>(AChunk::MakeWorldLocation(ChunkLocation), FRotator::ZeroRotator);
	Ptr->StartLoading();
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
		AChunk* Ptr = *ChunksMap.Find(ChunkLocation);
		switch(Ptr->GetState())
		{
		case AChunk::EState::Loaded:
			Ptr->StartUnloading();
			break;
		case AChunk::EState::Loading:
			Ptr->CloseLoading();
			Ptr->StartUnloading();
			break;
		default:
			break;
		}
		ChunksPool.Enqueue(Ptr);
		ChunksMap.Remove(ChunkLocation);
	}
}

void AMainGameStateBase::AddToUnloadBlocksQueue(AChunk* Chunk, size_t BlockIndex)
{
	ClearInstancesQueue.AddTail({Chunk, BlockIndex});
}

void AMainGameStateBase::AddToSpawnInstancesQueue(AChunk* Chunk, TArray<TTuple<size_t, FIntVector>>& ToSpawn)
{
	SpawnInstancesQueue.AddTail({Chunk, ToSpawn});
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
