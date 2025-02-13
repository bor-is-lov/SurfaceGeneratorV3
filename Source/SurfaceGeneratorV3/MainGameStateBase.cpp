// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameStateBase.h"

#include "MainPlayerController.h"
#include "Components/InstancedStaticMeshComponent.h"
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

	if(!ClearInstancesQueue.IsEmpty())
	{
		FBlockTypeInChunk BlockType = ClearInstancesQueue.GetHead()->GetValue();
		BlockType.GetBlockType()->ClearInstances();
		ClearInstancesQueue.RemoveNode(ClearInstancesQueue.GetHead());
		bool ToEndUnloading = true;
		for(FBlockTypeInChunk& BlockType2 : ClearInstancesQueue)
			if(BlockType2.Chunk == BlockType.Chunk)
			{
				ToEndUnloading = false;
				break;
			}
		if(ToEndUnloading)
			BlockType.Chunk->EndUnloading();
	}
	else
		if(!SpawnInstancesQueue.IsEmpty())
		{
			FBlocksInfoInChunk& BlocksToSpawnInChunk = SpawnInstancesQueue.GetHead()->GetValue();
			static TArray<TArray<FTransform>> BlocksTransforms;
			BlocksTransforms.Empty();
			BlocksTransforms.SetNum(BlocksClasses.Num());
			for(int i = 0; i < 256; i++)
			{
				if (i >= BlocksToSpawnInChunk.BlocksInfo.Num())
					break;
				FVector Location = FVector(
					50 + BlocksToSpawnInChunk.BlocksInfo[i].Position.X * 100,
					50 + BlocksToSpawnInChunk.BlocksInfo[i].Position.Y * 100,
					50 + BlocksToSpawnInChunk.BlocksInfo[i].Position.Z * 100);
				BlocksTransforms[BlocksToSpawnInChunk.BlocksInfo[i].ISMIndex].Emplace(Location);
			}
			for(int i = 0; i < BlocksTransforms.Num(); i++)
				BlocksToSpawnInChunk.GetBlockType(i)->AddInstances(BlocksTransforms[i], false);
			
			BlocksToSpawnInChunk.BlocksInfo.RemoveAt(0, FMath::Min(256, BlocksToSpawnInChunk.BlocksInfo.Num()));
			if (BlocksToSpawnInChunk.BlocksInfo.IsEmpty())
			{
				AChunk* ChunkPtr = BlocksToSpawnInChunk.Chunk;
				SpawnInstancesQueue.RemoveNode(SpawnInstancesQueue.GetHead());
				bool ToEndLoading = true;
				for(auto BlocksToSpawnInChunk2 : SpawnInstancesQueue)
					if(BlocksToSpawnInChunk2.Chunk == ChunkPtr)
					{
						ToEndLoading = false;
						break;
					}
				if(ToEndLoading)
					ChunkPtr->EndLoading();
			}
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

void AMainGameStateBase::AddToSpawnInstancesQueue(AChunk* Chunk, TArray<FBlockInfo>& ToSpawn)
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
