#include "TerrainGenerator.h"
#include "CoreMinimal.h"

#include "Chunk.h"

FTerrainGenerator::FTerrainGenerator(const unsigned int Seed)
	: Seed(Seed),
	Perlin(siv::PerlinNoise(this->Seed))
{
	
}

void FTerrainGenerator::GenerateChunk(AChunk* Chunk) const
{
	const FIntVector ChunkLocation = AChunk::ChunkWorldLocationToChunkLocation(Chunk->GetActorLocation());
	for(int x = 0; x < 16; x++)
		for(int y = 0; y < 16; y++)
		{
			const int Height = 64 * Perlin.octave2D_11(
				(ChunkLocation.X * 16 + x) * 0.003,
				(ChunkLocation.Y * 16 + y) * 0.003,
				5);
			for(int z = 0; z < 16; z++)
			{
				const int InChunkHeight = ChunkLocation.Z * 16 + z;
				const int BlockIndex = AChunk::LocationToBlockIndex(x, y, z);
				if (InChunkHeight <= Height - 5)
					Chunk->SetBlock(BlockIndex, 1);
				else if (InChunkHeight <= Height - 1)
					Chunk->SetBlock(BlockIndex, 2);
				else if (InChunkHeight == Height)
					if(Height >= 0)
						Chunk->SetBlock(BlockIndex, 3);
					else
						Chunk->SetBlock(BlockIndex, 2);
				else
					Chunk->SetBlock(BlockIndex, 0);
			}
		}
}

FVector FTerrainGenerator::GetSpawnPosition() const
{
	return {50.0f, 50.0f, 6400.0f * Perlin.octave2D_11(0.0, 0.0, 5) + 200.0f};
}
