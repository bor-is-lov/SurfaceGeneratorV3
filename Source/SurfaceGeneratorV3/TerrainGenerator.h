#pragma once
#include "PerlinNoise.hpp"

class AMainGameStateBase;
class AChunk;

class FTerrainGenerator
{
	const siv::PerlinNoise::seed_type Seed;
	const siv::PerlinNoise* Perlin;
	AMainGameStateBase* OwnerGameStateBase;
public:
	FTerrainGenerator(unsigned int Seed, AMainGameStateBase* Owner);
	~FTerrainGenerator();

	void GenerateChunk(const AChunk* Chunk, TArray<TTuple<size_t, FIntVector>>& ToSpawn) const;
	void AddChunkToSpawnQueue(AChunk* Chunk) const;
};
