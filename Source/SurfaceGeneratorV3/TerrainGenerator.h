#pragma once
#include "PerlinNoise.hpp"

class AMainGameStateBase;
class AChunk;

class FTerrainGenerator
{
	const siv::PerlinNoise::seed_type Seed;
	const siv::PerlinNoise Perlin;

public:
	FTerrainGenerator(unsigned int Seed);

	void GenerateChunk(AChunk* Chunk) const;
};
