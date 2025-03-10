// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk.h"
#include "MainGameStateBase.h"
#include "Components/BoxComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

void AChunk::LoadPlanes()
{
	LoadPlanesZPositive();
	LoadPlanesXPositive();
	LoadPlanesYPositive();
	LoadPlanesZNegative();
	LoadPlanesXNegative();
	LoadPlanesYNegative();
}

void AChunk::UnloadPlanes()
{
	Planes->ClearInstances();
}

void AChunk::LoadPlanesZPositive()
{
	bool ShouldAddPlane[256] = {false};
	bool PlaneAdded[256] = {false};
	for(int z = 0; z < 16; z++)
	{
		for(int i = 0; i < 256; i++)
			PlaneAdded[i] = false;
		for(int x = 0; x < 16; x++)
			for(int y = 0; y < 16; y++)
				ShouldAddPlane[x * 16 + y] = BlocksData[LocationToBlockIndex(x, y, z)].ShouldAddFace(GetWorld(),
					z == 15 ? nullptr : &BlocksData[LocationToBlockIndex(x, y, z + 1)]);
		
		for(int i = 0; i < 256; i++)
		{
			if(ShouldAddPlane[i] && !PlaneAdded[i])
			{
				const int x1 = i / 16;
				const int y1 = i % 16;
				int x2 = x1;
				int y2 = y1;
				const int TextureIndex = BlocksData[LocationToBlockIndex(x1, y1, z)].GetTextureIndex(GetWorld(), EFaceDirection::ZPositive);
				
				for(; x2 < 16; x2++)
					if (!(ShouldAddPlane[x2 * 16 + y1] && !PlaneAdded[x2 * 16 + y1]
						&& BlocksData[LocationToBlockIndex(x2, y1, z)].GetTextureIndex(GetWorld(), EFaceDirection::ZPositive) == TextureIndex))
						break;
				x2--;
				
				for(; y2 < 16; y2++)
				{
					bool ShouldAddRow = true;
					for(int x = x1; x <= x2; x++)
						if(!(ShouldAddPlane[x * 16 + y2] && !PlaneAdded[x * 16 + y2]
							&& BlocksData[LocationToBlockIndex(x, y2, z)].GetTextureIndex(GetWorld(), EFaceDirection::ZPositive) == TextureIndex))
							ShouldAddRow = false;
					if(!ShouldAddRow)
						break;
				}
				y2--;
				
				for(int x = x1; x <= x2; x++)
					for(int y = y1; y <= y2; y++)
						PlaneAdded[x * 16 + y] = true;

				const float PosX = (x1 + x2) / 2.0f * 100.0f + 50.0f;
				const float PosY = (y1 + y2) / 2.0f * 100.0f + 50.0f;
				const float ScaleX = x2 - x1 + 1.0f;
				const float ScaleY = y2 - y1 + 1.0f;
				const int InstanceIndex = Planes->AddInstance(FTransform(
					{0, 0, 0},
					{PosX, PosY, z * 100.0f + 100.0f},
					{ScaleX, ScaleY, 1.0f}));

				if(BlocksMaterial)
				{
					Planes->SetCustomDataValue(InstanceIndex, 0, ScaleX);
					Planes->SetCustomDataValue(InstanceIndex, 1, ScaleY);
					Planes->SetCustomDataValue(InstanceIndex, 2, TextureIndex);
				}
			}
		}
	}
}

void AChunk::LoadPlanesXPositive()
{
	bool ShouldAddPlane[256] = {false};
	bool PlaneAdded[256] = {false};
	for(int x = 0; x < 16; x++)
	{
		for(int i = 0; i < 256; i++)
			ShouldAddPlane[i] = PlaneAdded[i] = false;
		for(int z = 0; z < 16; z++)
			for(int y = 0; y < 16; y++)
				ShouldAddPlane[z * 16 + y] = BlocksData[LocationToBlockIndex(x, y, z)].ShouldAddFace(GetWorld(),
					x == 15 ? nullptr : &BlocksData[LocationToBlockIndex(x + 1, y, z)]);
		
		for(int i = 0; i < 256; i++)
		{
			if(ShouldAddPlane[i] && !PlaneAdded[i])
			{
				const int z1 = i / 16;
				const int y1 = i % 16;
				int z2 = z1;
				int y2 = y1;
				const int TextureIndex = BlocksData[LocationToBlockIndex(x, y1, z1)].GetTextureIndex(GetWorld(), EFaceDirection::XPositive);
				
				for(; z2 < 16; z2++)
					if (!(ShouldAddPlane[z2 * 16 + y1] && !PlaneAdded[z2 * 16 + y1]
						&& BlocksData[LocationToBlockIndex(x, y1, z2)].GetTextureIndex(GetWorld(), EFaceDirection::XPositive) == TextureIndex))
						break;
				z2--;
				
				for(; y2 < 16; y2++)
				{
					bool ShouldAddRow = true;
					for(int z = z1; z <= z2; z++)
						if(!(ShouldAddPlane[z * 16 + y2] && !PlaneAdded[z * 16 + y2]
							&& BlocksData[LocationToBlockIndex(x, y2, z)].GetTextureIndex(GetWorld(), EFaceDirection::XPositive) == TextureIndex))
							ShouldAddRow = false;
					if(!ShouldAddRow)
						break;
				}
				y2--;
				
				for(int z = z1; z <= z2; z++)
					for(int y = y1; y <= y2; y++)
						PlaneAdded[z * 16 + y] = true;

				const float PosZ = (z1 + z2) / 2.0f * 100.0f + 50.0f;
				const float PosY = (y1 + y2) / 2.0f * 100.0f + 50.0f;
				const float ScaleY = z2 - z1 + 1.0f;
				const float ScaleX = y2 - y1 + 1.0f;
				const int InstanceIndex = Planes->AddInstance(FTransform(
					{0, -90, 90},
					{x * 100.0f + 100.0f, PosY, PosZ},
					{ScaleX, ScaleY, 1.0f}));

				if(BlocksMaterial)
				{
					Planes->SetCustomDataValue(InstanceIndex, 0, ScaleX);
					Planes->SetCustomDataValue(InstanceIndex, 1, ScaleY);
					Planes->SetCustomDataValue(InstanceIndex, 2, TextureIndex);
				}
			}
		}
	}
}

void AChunk::LoadPlanesYPositive()
{
	bool ShouldAddPlane[256] = {false};
	bool PlaneAdded[256] = {false};
	for(int y = 0; y < 16; y++)
	{
		for(int i = 0; i < 256; i++)
			ShouldAddPlane[i] = PlaneAdded[i] = false;
		for(int x = 0; x < 16; x++)
			for(int z = 0; z < 16; z++)
				ShouldAddPlane[x * 16 + z] = BlocksData[LocationToBlockIndex(x, y, z)].ShouldAddFace(GetWorld(),
					y == 15 ? nullptr : &BlocksData[LocationToBlockIndex(x, y + 1, z)]);
		
		for(int i = 0; i < 256; i++)
		{
			if(ShouldAddPlane[i] && !PlaneAdded[i])
			{
				const int x1 = i / 16;
				const int z1 = i % 16;
				int x2 = x1;
				int z2 = z1;
				const int TextureIndex = BlocksData[LocationToBlockIndex(x1, y, z2)].GetTextureIndex(GetWorld(), EFaceDirection::YPositive);
				
				for(; x2 < 16; x2++)
					if (!(ShouldAddPlane[x2 * 16 + z1] && !PlaneAdded[x2 * 16 + z1]
						&& BlocksData[LocationToBlockIndex(x2, y, z1)].GetTextureIndex(GetWorld(), EFaceDirection::YPositive) == TextureIndex))
						break;
				x2--;
				
				for(; z2 < 16; z2++)
				{
					bool ShouldAddRow = true;
					for(int x = x1; x <= x2; x++)
						if(!(ShouldAddPlane[x * 16 + z2] && !PlaneAdded[x * 16 + z2]
							&& BlocksData[LocationToBlockIndex(x, y, z2)].GetTextureIndex(GetWorld(), EFaceDirection::YPositive) == TextureIndex))
							ShouldAddRow = false;
					if(!ShouldAddRow)
						break;
				}
				z2--;
				
				for(int x = x1; x <= x2; x++)
					for(int z = z1; z <= z2; z++)
						PlaneAdded[x * 16 + z] = true;

				const float PosX = (x1 + x2) / 2.0f * 100.0f + 50.0f;
				const float PosZ = (z1 + z2) / 2.0f * 100.0f + 50.0f;
				const float ScaleX = x2 - x1 + 1.0f;
				const float ScaleY = z2 - z1 + 1.0f;
				const int InstanceIndex = Planes->AddInstance(FTransform(
					{0, 0, 90},
					{PosX, y * 100.0f + 100.0f, PosZ},
					{ScaleX, ScaleY, 1.0f}));

				if(BlocksMaterial)
				{
					Planes->SetCustomDataValue(InstanceIndex, 0, ScaleX);
					Planes->SetCustomDataValue(InstanceIndex, 1, ScaleY);
					Planes->SetCustomDataValue(InstanceIndex, 2, TextureIndex);
				}
			}
		}
	}
}

void AChunk::LoadPlanesZNegative()
{
	bool ShouldAddPlane[256] = {false};
	bool PlaneAdded[256] = {false};
	for(int z = 0; z < 16; z++)
	{
		for(int i = 0; i < 256; i++)
			ShouldAddPlane[i] = PlaneAdded[i] = false;
		for(int x = 0; x < 16; x++)
			for(int y = 0; y < 16; y++)
				ShouldAddPlane[x * 16 + y] = BlocksData[LocationToBlockIndex(x, y, z)].ShouldAddFace(GetWorld(),
					z == 0 ? nullptr : &BlocksData[LocationToBlockIndex(x, y, z - 1)]);
		
		for(int i = 0; i < 256; i++)
		{
			if(ShouldAddPlane[i] && !PlaneAdded[i])
			{
				const int x1 = i / 16;
				const int y1 = i % 16;
				int x2 = x1;
				int y2 = y1;
				const int TextureIndex = BlocksData[LocationToBlockIndex(x1, y1, z)].GetTextureIndex(GetWorld(), EFaceDirection::ZNegative);
				
				for(; x2 < 16; x2++)
					if (!(ShouldAddPlane[x2 * 16 + y1] && !PlaneAdded[x2 * 16 + y1]
						&& BlocksData[LocationToBlockIndex(x2, y1, z)].GetTextureIndex(GetWorld(), EFaceDirection::ZNegative) == TextureIndex))
						break;
				x2--;
				
				for(; y2 < 16; y2++)
				{
					bool ShouldAddRow = true;
					for(int x = x1; x <= x2; x++)
						if(!(ShouldAddPlane[x * 16 + y2] && !PlaneAdded[x * 16 + y2]
							&& BlocksData[LocationToBlockIndex(x, y2, z)].GetTextureIndex(GetWorld(), EFaceDirection::ZNegative) == TextureIndex))
							ShouldAddRow = false;
					if(!ShouldAddRow)
						break;
				}
				y2--;
				
				for(int x = x1; x <= x2; x++)
					for(int y = y1; y <= y2; y++)
						PlaneAdded[x * 16 + y] = true;

				const float PosX = (x1 + x2) / 2.0f * 100.0f + 50.0f;
				const float PosY = (y1 + y2) / 2.0f * 100.0f + 50.0f;
				const float ScaleX = x2 - x1 + 1.0f;
				const float ScaleY = y2 - y1 + 1.0f;
				const int InstanceIndex = Planes->AddInstance(FTransform(
					{180, 0, 0},
					{PosX, PosY, z * 100.0f},
					{ScaleX, ScaleY, 1.0f}));
				
				if(BlocksMaterial)
				{
					Planes->SetCustomDataValue(InstanceIndex, 0, ScaleX);
					Planes->SetCustomDataValue(InstanceIndex, 1, ScaleY);
					Planes->SetCustomDataValue(InstanceIndex, 2, TextureIndex);
				}
			}
		}
	}
}

void AChunk::LoadPlanesXNegative()
{
	bool ShouldAddPlane[256] = {false};
	bool PlaneAdded[256] = {false};
	for(int x = 0; x < 16; x++)
	{
		for(int i = 0; i < 256; i++)
			ShouldAddPlane[i] = PlaneAdded[i] = false;
		for(int z = 0; z < 16; z++)
			for(int y = 0; y < 16; y++)
				ShouldAddPlane[z * 16 + y] = BlocksData[LocationToBlockIndex(x, y, z)].ShouldAddFace(GetWorld(),
					x == 0 ? nullptr : &BlocksData[LocationToBlockIndex(x - 1, y, z)]);
		
		for(int i = 0; i < 256; i++)
		{
			if(ShouldAddPlane[i] && !PlaneAdded[i])
			{
				const int z1 = i / 16;
				const int y1 = i % 16;
				int z2 = z1;
				int y2 = y1;
				const int TextureIndex = BlocksData[LocationToBlockIndex(x, y1, z1)].GetTextureIndex(GetWorld(), EFaceDirection::XNegative);
				
				for(; z2 < 16; z2++)
					if (!(ShouldAddPlane[z2 * 16 + y1] && !PlaneAdded[z2 * 16 + y1]
						&& BlocksData[LocationToBlockIndex(x, y1, z2)].GetTextureIndex(GetWorld(), EFaceDirection::XNegative) == TextureIndex))
						break;
				z2--;
				
				for(; y2 < 16; y2++)
				{
					bool ShouldAddRow = true;
					for(int z = z1; z <= z2; z++)
						if(!(ShouldAddPlane[z * 16 + y2] && !PlaneAdded[z * 16 + y2]
							&& BlocksData[LocationToBlockIndex(x, y2, z)].GetTextureIndex(GetWorld(), EFaceDirection::XNegative) == TextureIndex))
							ShouldAddRow = false;
					if(!ShouldAddRow)
						break;
				}
				y2--;
				
				for(int z = z1; z <= z2; z++)
					for(int y = y1; y <= y2; y++)
						PlaneAdded[z * 16 + y] = true;

				const float PosZ = (z1 + z2) / 2.0f * 100.0f + 50.0f;
				const float PosY = (y1 + y2) / 2.0f * 100.0f + 50.0f;
				const float ScaleY = z2 - z1 + 1.0f;
				const float ScaleX = y2 - y1 + 1.0f;
				const int InstanceIndex = Planes->AddInstance(FTransform(
					{0, 90, 90},
					{x * 100.0f, PosY, PosZ},
					{ScaleX, ScaleY, 1.0f}));
				
				if(BlocksMaterial)
				{
					Planes->SetCustomDataValue(InstanceIndex, 0, ScaleX);
					Planes->SetCustomDataValue(InstanceIndex, 1, ScaleY);
					Planes->SetCustomDataValue(InstanceIndex, 2, TextureIndex);
				}
			}
		}
	}
}

void AChunk::LoadPlanesYNegative()
{
	bool ShouldAddPlane[256] = {false};
	bool PlaneAdded[256] = {false};
	for(int y = 0; y < 16; y++)
	{
		for(int i = 0; i < 256; i++)
			ShouldAddPlane[i] = PlaneAdded[i] = false;
		for(int x = 0; x < 16; x++)
			for(int z = 0; z < 16; z++)
				ShouldAddPlane[x * 16 + z] = BlocksData[LocationToBlockIndex(x, y, z)].ShouldAddFace(GetWorld(),
					y == 0 ? nullptr : &BlocksData[LocationToBlockIndex(x, y - 1, z)]);
		
		for(int i = 0; i < 256; i++)
		{
			if(ShouldAddPlane[i] && !PlaneAdded[i])
			{
				const int x1 = i / 16;
				const int z1 = i % 16;
				int x2 = x1;
				int z2 = z1;
				const int TextureIndex = BlocksData[LocationToBlockIndex(x1, y, z2)].GetTextureIndex(GetWorld(), EFaceDirection::YNegative);
				
				for(; x2 < 16; x2++)
					if (!(ShouldAddPlane[x2 * 16 + z1] && !PlaneAdded[x2 * 16 + z1]
						&& BlocksData[LocationToBlockIndex(x2, y, z1)].GetTextureIndex(GetWorld(), EFaceDirection::YNegative) == TextureIndex))
						break;
				x2--;
				
				for(; z2 < 16; z2++)
				{
					bool ShouldAddRow = true;
					for(int x = x1; x <= x2; x++)
						if(!(ShouldAddPlane[x * 16 + z2] && !PlaneAdded[x * 16 + z2]
							&& BlocksData[LocationToBlockIndex(x, y, z2)].GetTextureIndex(GetWorld(), EFaceDirection::YNegative) == TextureIndex))
							ShouldAddRow = false;
					if(!ShouldAddRow)
						break;
				}
				z2--;
				
				for(int x = x1; x <= x2; x++)
					for(int z = z1; z <= z2; z++)
						PlaneAdded[x * 16 + z] = true;

				const float PosX = (x1 + x2) / 2.0f * 100.0f + 50.0f;
				const float PosZ = (z1 + z2) / 2.0f * 100.0f + 50.0f;
				const float ScaleX = x2 - x1 + 1.0f;
				const float ScaleY = z2 - z1 + 1.0f;
				const int InstanceIndex = Planes->AddInstance(FTransform(
					{0, 180, 90},
					{PosX, y * 100.0f, PosZ},
					{ScaleX, ScaleY, 1.0f}));
				
				if(BlocksMaterial)
				{
					Planes->SetCustomDataValue(InstanceIndex, 0, ScaleX);
					Planes->SetCustomDataValue(InstanceIndex, 1, ScaleY);
					Planes->SetCustomDataValue(InstanceIndex, 2, TextureIndex);
				}
			}
		}
	}
}

AChunk::AChunk()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;
	
	Border = CreateDefaultSubobject<UBoxComponent>("Border");
	Border->SetupAttachment(RootComponent);
	Border->SetBoxExtent(FVector(800, 800, 800), false);
	Border->SetRelativeLocation(FVector(800, 800, 800));
	Border->SetGenerateOverlapEvents(false);
	Border->SetComponentTickEnabled(false);

	Planes = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Planes");
	Planes->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if(MeshAsset.Succeeded())
		Planes->SetStaticMesh(MeshAsset.Object);
	Planes->SetGenerateOverlapEvents(false);
	Planes->SetComponentTickEnabled(false);
	Planes->NumCustomDataFloats = 3;
	
	BlocksData.SetNum(4096);
}

void AChunk::BeginPlay()
{
	Super::BeginPlay();
	
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	BlocksMaterial = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->BlocksMaterial;
	if(BlocksMaterial)
		Planes->SetMaterial(0, BlocksMaterial);
	Border->SetCollisionResponseToAllChannels(ECR_Block);
	Border->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AChunk::StartLoading()
{
	State = EState::Loading;
	GetWorld()->GetGameStateChecked<AMainGameStateBase>()->TerrainGenerator->GenerateChunk(this);
	GetWorld()->GetGameStateChecked<AMainGameStateBase>()->AddToLoadMeshes(this);
	Border->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AChunk::EndLoading()
{
	State = EState::Loaded;
	SetActorHiddenInGame(false);
	Border->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AChunk::StartUnloading()
{
	State = EState::Unloading;
	SetActorHiddenInGame(true);
	Border->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	GetWorld()->GetGameStateChecked<AMainGameStateBase>()->AddToUnloadMeshes(this);
}

void AChunk::EndUnloading()
{
	State = EState::Unloaded;
	SetActorHiddenInGame(true);
	Border->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AChunk::CloseLoading()
{
	TDoubleLinkedList<AChunk*>* LoadMeshesQueue = &GetWorld()->GetGameStateChecked<AMainGameStateBase>()->LoadMeshesQueue;
	if(auto NodeToRemove = LoadMeshesQueue->FindNode(this))
		LoadMeshesQueue->RemoveNode(NodeToRemove);
}

void AChunk::SetBlock(const size_t InChunkIndex, const size_t TypeIndex)
{
	const auto GameState = GetWorld()->GetGameStateChecked<AMainGameStateBase>();
	if(GameState->BlocksDefaults.Num() >= TypeIndex)
		BlocksData[InChunkIndex].SetBlock(TypeIndex);
}

FIntVector AChunk::ActorLocationToChunkLocation(const FVector& ActorLocation)
{
	FIntVector ChunkLocation;
	ChunkLocation.X = ActorLocation.X / 1600.0f;
	ChunkLocation.Y = ActorLocation.Y / 1600.0f;
	ChunkLocation.Z = ActorLocation.Z / 1600.0f;
	if (ActorLocation.X < 0)
		ChunkLocation.X--;
	if (ActorLocation.Y < 0)
		ChunkLocation.Y--;
	if (ActorLocation.Z < 0)
		ChunkLocation.Z--;
	return ChunkLocation;
}

FIntVector AChunk::ChunkWorldLocationToChunkLocation(const FVector& WorldLocation)
{
	return FIntVector(WorldLocation.X / 1600.0f, WorldLocation.Y / 1600.0f, WorldLocation.Z / 1600.0f);
}

FVector AChunk::MakeWorldLocation(const FIntVector& ChunkLocation)
{
	return FVector(ChunkLocation.X * 1600, ChunkLocation.Y * 1600, ChunkLocation.Z * 1600);
}

FIntVector AChunk::BlockIndexToLocation(const size_t& Index)
{
	return FIntVector(Index / 256, Index / 16 % 16, Index % 16);
}

size_t AChunk::LocationToBlockIndex(const FIntVector& Location)
{
	return Location.X * 256 + Location.Y * 16 + Location.Z;
}

size_t AChunk::LocationToBlockIndex(const int x, const int y, const int z)
{
	return x * 256 + y * 16 + z;
}
