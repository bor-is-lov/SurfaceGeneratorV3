// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameModeBase.h"

#include "MainGameStateBase.h"
#include "Kismet/GameplayStatics.h"

void AMainGameModeBase::UpdateChunks_Implementation(int RenderDistance, float ZScale, FIntVector PlayerChunkLocation) const
{
	UWorld* World = GetWorld();
	TArray<AActor*>Chunks;
	UGameplayStatics::GetAllActorsOfClass(World, AChunk::StaticClass(), Chunks);
	FIntVector ChunkLocation;
	for (auto actor : Chunks)
	{
		ChunkLocation = AChunk::ChunkWorldLocationToChunkLocation(actor->GetActorLocation());
		if (pow(ChunkLocation.X - PlayerChunkLocation.X, 2) + pow(ChunkLocation.Y - PlayerChunkLocation.Y, 2) > pow(RenderDistance + 0.5, 2)
		|| ChunkLocation.Z < PlayerChunkLocation.Z - RenderDistance * ZScale || ChunkLocation.Z > PlayerChunkLocation.Z + RenderDistance * ZScale)
			Cast<AMainGameStateBase>(GameState)->ExtractChunk(ChunkLocation);
	}
	for (int x = PlayerChunkLocation.X - RenderDistance; x <= PlayerChunkLocation.X + RenderDistance; x++)
		for (int y = PlayerChunkLocation.Y - RenderDistance; y <= PlayerChunkLocation.Y + RenderDistance; y++)
			for (int z = PlayerChunkLocation.Z - RenderDistance * ZScale; z <= PlayerChunkLocation.Z + RenderDistance * ZScale; z++)
				if(pow(x - PlayerChunkLocation.X, 2) + pow(y - PlayerChunkLocation.Y, 2) <= pow(RenderDistance + 0.5, 2))
					if (!Cast<AMainGameStateBase>(GameState)->GetChunk(FIntVector(x, y, z)))
						Cast<AMainGameStateBase>(GameState)->PlaceChunk(FIntVector(x, y, z));
}
