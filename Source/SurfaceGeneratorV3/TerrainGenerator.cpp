#include "TerrainGenerator.h"
#include "CoreMinimal.h"

#include "Chunk.h"
#include "MainGameStateBase.h"

FTerrainGenerator::FTerrainGenerator(const unsigned int Seed, AMainGameStateBase* Owner) : Seed(Seed), OwnerGameStateBase(Owner)
{
	Perlin = new siv::PerlinNoise(this->Seed);
}

FTerrainGenerator::~FTerrainGenerator()
{
	delete Perlin;
}

void FTerrainGenerator::GenerateChunk(const AChunk* Chunk, TArray<TTuple<size_t, FIntVector>>& ToSpawn) const
{
	const FIntVector ChunkLocation = AChunk::ChunkWorldLocationToChunkLocation(Chunk->GetActorLocation());
	for(int x = 0; x < 16; x++)
		for(int y = 0; y < 16; y++)
		{
			const FIntVector2 BlockLocation(ChunkLocation.X * 16 + x, ChunkLocation.Y * 16 + y);
			const int Height = 64 * Perlin->octave2D_11(BlockLocation.X * 0.003, BlockLocation.Y * 0.003, 6);
			for(int z = 0; z < 16; z++)
				if (ChunkLocation.Z * 16 + z <= Height)
					ToSpawn.Add({0, FIntVector(x, y, z)});
		}
}

void FTerrainGenerator::AddChunkToSpawnQueue(AChunk* Chunk) const
{
	TArray<TTuple<size_t, FIntVector>> ToSpawn;
	GenerateChunk(Chunk, ToSpawn);
	OwnerGameStateBase->AddToSpawnInstancesQueue(Chunk, ToSpawn);
}
