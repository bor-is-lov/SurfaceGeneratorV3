#pragma once
#include "PerlinNoise.hpp"

class AMainGameStateBase;
class AChunk;

class FTerrainGenerator
{
	const siv::PerlinNoise::seed_type Seed;
	const siv::PerlinNoise Perlin;
	AMainGameStateBase* OwnerGameStateBase;
	TMap<FIntVector2, TStaticArray<int, 256>>* HeightMap;

public:
	FTerrainGenerator(unsigned int Seed, AMainGameStateBase* Owner);
	~FTerrainGenerator();

	void GenerateChunk(const AChunk* Chunk, TArray<struct FBlockInfo>& ToSpawn) const;
	void AddChunkToSpawnQueue(AChunk* Chunk) const;

	void UpdateHeightMap(int RenderDistance, FIntVector PlayerChunkLocation);
};
