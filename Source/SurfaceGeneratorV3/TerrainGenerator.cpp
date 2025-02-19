#include "TerrainGenerator.h"
#include "CoreMinimal.h"

#include "Chunk.h"
#include "MainGameStateBase.h"

FTerrainGenerator::FTerrainGenerator(const unsigned int Seed, AMainGameStateBase* Owner)
	: Seed(Seed),
	Perlin(siv::PerlinNoise(this->Seed)),
	OwnerGameStateBase(Owner),
	HeightMap(new TMap<FIntVector2, TStaticArray<int, 256>>())
{
	
}

FTerrainGenerator::~FTerrainGenerator()
{
	delete HeightMap;
}

void FTerrainGenerator::GenerateChunk(AChunk* Chunk) const
{
	const FIntVector ChunkLocation = AChunk::ChunkWorldLocationToChunkLocation(Chunk->GetActorLocation());
	const FIntVector2 HeightMapKey(ChunkLocation.X, ChunkLocation.Y);
	if(HeightMap->Contains(HeightMapKey))
	{
		TStaticArray<int, 256>* HeightData = HeightMap->Find(HeightMapKey);
		for(int x = 0; x < 16; x++)
			for(int y = 0; y < 16; y++)
				for(int z = 0; z < 16; z++)
					if(ChunkLocation.Z * 16 + z <= (*HeightData)[x * 16 + y])
						Chunk->SetBlock(AChunk::LocationToBlockIndex(x, y, z), 1);
					else
						Chunk->SetBlock(AChunk::LocationToBlockIndex(x, y, z), 0);
	}
	else
	{
		TStaticArray<int, 256>* HeightData = &HeightMap->Add(HeightMapKey, {});
		for(int x = 0; x < 16; x++)
			for(int y = 0; y < 16; y++)
			{
				const int Height = 64 * Perlin.octave2D_11(
					(ChunkLocation.X * 16 + x) * 0.003,
					(ChunkLocation.Y * 16 + y) * 0.003,
					6);
				(*HeightData)[x * 16 + y] = Height;
				for(int z = 0; z < 16; z++)
					if (ChunkLocation.Z * 16 + z <= Height)
						Chunk->SetBlock(AChunk::LocationToBlockIndex(x, y, z), 1);
					else
						Chunk->SetBlock(AChunk::LocationToBlockIndex(x, y, z), 0);
			}
	}
}

void FTerrainGenerator::UpdateHeightMap(const int RenderDistance, const FIntVector& PlayerChunkLocation)
{
	TDoubleLinkedList<FIntVector2> KeysToDel;
	for (auto Node : *HeightMap)
	{
		if (pow(Node.Key.X - PlayerChunkLocation.X, 2) +
			pow(Node.Key.Y - PlayerChunkLocation.Y, 2) >
			pow(RenderDistance, 2))
			KeysToDel.AddHead(Node.Key);
	}
	for(const FIntVector2 Key : KeysToDel)
		HeightMap->Remove(Key);
}
