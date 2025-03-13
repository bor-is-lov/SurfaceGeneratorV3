// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk.h"
#include "MainGameStateBase.h"
#include "Components/BoxComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

void AChunk::LoadPlanes()
{
	FIntVector ChunkLocation = ChunkWorldLocationToChunkLocation(GetActorLocation());
	AChunk* Touching = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({ChunkLocation.X + 1, ChunkLocation.Y, ChunkLocation.Z});
	for(int x = 0; x < 16; x++)
		LoadPlanesAtIndex(EFaceDirection::XPositive, x, Touching);
	if(Touching)
		Touching->LoadPlanesAtIndex(EFaceDirection::XNegative, 0, this);
	
	Touching = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({ChunkLocation.X, ChunkLocation.Y + 1, ChunkLocation.Z});
	for(int y = 0; y < 16; y++)
		LoadPlanesAtIndex(EFaceDirection::YPositive, y, Touching);
	if(Touching)
		Touching->LoadPlanesAtIndex(EFaceDirection::YNegative, 0, this);
	
	Touching = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({ChunkLocation.X, ChunkLocation.Y, ChunkLocation.Z + 1});
	for(int z = 0; z < 16; z++)
		LoadPlanesAtIndex(EFaceDirection::ZPositive, z, Touching);
	if(Touching)
		Touching->LoadPlanesAtIndex(EFaceDirection::ZNegative, 0, this);
	
	Touching = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({ChunkLocation.X - 1, ChunkLocation.Y, ChunkLocation.Z});
	for(int x = 0; x < 16; x++)
		LoadPlanesAtIndex(EFaceDirection::XNegative, x, Touching);
	if(Touching)
		Touching->LoadPlanesAtIndex(EFaceDirection::XPositive, 15, this);
	
	Touching = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({ChunkLocation.X, ChunkLocation.Y - 1, ChunkLocation.Z});
	for(int y = 0; y < 16; y++)
		LoadPlanesAtIndex(EFaceDirection::YNegative, y, Touching);
	if(Touching)
		Touching->LoadPlanesAtIndex(EFaceDirection::YPositive, 15, this);
	
	Touching = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({ChunkLocation.X, ChunkLocation.Y, ChunkLocation.Z - 1});
	for(int z = 0; z < 16; z++)
		LoadPlanesAtIndex(EFaceDirection::ZNegative, z, Touching);
	if(Touching)
		Touching->LoadPlanesAtIndex(EFaceDirection::ZPositive, 15, this);

}

void AChunk::UnloadPlanes()
{
	Planes->ClearInstances();
	
	UnloadPlanesTouching(EFaceDirection::XPositive);
	UnloadPlanesTouching(EFaceDirection::YPositive);
	UnloadPlanesTouching(EFaceDirection::ZPositive);
	UnloadPlanesTouching(EFaceDirection::XNegative);
	UnloadPlanesTouching(EFaceDirection::YNegative);
	UnloadPlanesTouching(EFaceDirection::ZNegative);
}

void AChunk::LoadPlanesAtIndex(const EFaceDirection FaceDir, const int FaceIndex, const AChunk* Touching)
{
	bool ShouldAddPlane[256] = {false};
	bool PlaneAdded[256] = {false};
	
	{
		int x = 0, y = 0, z = 0;
		int *first = nullptr, *second = nullptr, *third = nullptr;

		if(FaceDir == EFaceDirection::XPositive || FaceDir == EFaceDirection::XNegative)
		{
			first = &x;
			second = &z;
			third = &y;
		}
		else if(FaceDir == EFaceDirection::YPositive || FaceDir == EFaceDirection::YNegative)
		{
			first = &y;
			second = &x;
			third = &z;
		}
		else if(FaceDir == EFaceDirection::ZPositive || FaceDir == EFaceDirection::ZNegative)
		{
			first = &z;
			second = &x;
			third = &y;
		}
		else
			__debugbreak();
		*first = FaceIndex;
	
		for(; *second < 16; (*second)++)
		{
			*third = 0;
			for(; *third < 16; (*third)++)
				if(FaceDir == EFaceDirection::XPositive || FaceDir == EFaceDirection::YPositive || FaceDir == EFaceDirection::ZPositive)
					if(*first == 15)
						if(Touching)
						{
							const int tempX = x, tempY = y, tempZ = z;
							*first = 0;
							ShouldAddPlane[*second * 16 + *third] = BlocksData[LocationToBlockIndex(tempX, tempY, tempZ)].ShouldAddFace(GetWorld(),
								&Touching->BlocksData[LocationToBlockIndex(x, y, z)]);
							*first = FaceIndex;
						}
						else
							ShouldAddPlane[*second * 16 + *third] = BlocksData[LocationToBlockIndex(x, y, z)].ShouldAddFace(GetWorld(),
								nullptr);
						else
						{
							const int tempX = x, tempY = y, tempZ = z;
							(*first)++;
							ShouldAddPlane[*second * 16 + *third] = BlocksData[LocationToBlockIndex(tempX, tempY, tempZ)].ShouldAddFace(GetWorld(),
								&BlocksData[LocationToBlockIndex(x, y, z)]);
							(*first)--;
						}
			else
				if(*first == 0)
					if(Touching)
					{
						const int tempX = x, tempY = y, tempZ = z;
						*first = 15;
						ShouldAddPlane[*second * 16 + *third] = BlocksData[LocationToBlockIndex(tempX, tempY, tempZ)].ShouldAddFace(GetWorld(),
							&Touching->BlocksData[LocationToBlockIndex(x, y, z)]);
						*first = FaceIndex;
					}
					else
						ShouldAddPlane[*second * 16 + *third] = BlocksData[LocationToBlockIndex(x, y, z)].ShouldAddFace(GetWorld(),
							nullptr);
					else
					{
						const int tempX = x, tempY = y, tempZ = z;
						(*first)--;
						ShouldAddPlane[*second * 16 + *third] = BlocksData[LocationToBlockIndex(tempX, tempY, tempZ)].ShouldAddFace(GetWorld(),
							&BlocksData[LocationToBlockIndex(x, y, z)]);
						(*first)++;
					}
		}
	}
	
	int first = FaceIndex;
	for(int i = 0; i < 256; i++)
	{
		if(ShouldAddPlane[i] && !PlaneAdded[i])
		{
			int second1 = i / 16;
			int third1 = i % 16;
			int second2 = second1;
			int third2 = third1;

			int *x1, *x2, *y1, *y2, *z1, *z2;
			if(FaceDir == EFaceDirection::XPositive || FaceDir == EFaceDirection::XNegative)
			{
				x1 = &first;
				x2 = &first;
				y1 = &third1;
				y2 = &third2;
				z1 = &second1;
				z2 = &second2;
			}
			else if(FaceDir == EFaceDirection::YPositive || FaceDir == EFaceDirection::YNegative)
			{
				x1 = &second1;
				x2 = &second2;
				y1 = &first;
				y2 = &first;
				z1 = &third1;
				z2 = &third2;
			}
			else
			{
				x1 = &second1;
				x2 = &second2;
				y1 = &third1;
				y2 = &third2;
				z1 = &first;
				z2 = &first;
			}
			const int TextureIndex = BlocksData[LocationToBlockIndex(*x1, *y1, *z1)].GetTextureIndex(GetWorld(), FaceDir);
			
			for(; second2 < 16; second2++)
			{
				int x, y, z;
				if(FaceDir == EFaceDirection::XPositive || FaceDir == EFaceDirection::XNegative)
				{
					x = *x1;
					y = *y1;
					z = *z2;
				}
				else
				{
					x = *x2;
					y = *y1;
					z = *z1;
				}
				if (!(ShouldAddPlane[second2 * 16 + third1] && !PlaneAdded[second2 * 16 + third1]
						&& BlocksData[LocationToBlockIndex(x, y, z)].GetTextureIndex(GetWorld(), FaceDir) == TextureIndex))
					break;
			}
			second2--;
			
			for(; third2 < 16; third2++)
			{
				bool ShouldAddRow = true;
				int x, y, z;
				for(int secondIter = second1; secondIter <= second2; secondIter++)
				{
					if(FaceDir == EFaceDirection::XPositive || FaceDir == EFaceDirection::XNegative)
					{
						x = *x1;
						y = *y2;
						z = secondIter;
					}
					else if(FaceDir == EFaceDirection::YPositive || FaceDir == EFaceDirection::YNegative)
					{
						x = secondIter;
						y = *y1;
						z = *z2;
					}
					else
					{
						x = secondIter;
						y = *y2;
						z = *z1;
					}
					if(!(ShouldAddPlane[secondIter * 16 + third2] && !PlaneAdded[secondIter * 16 + third2]
						&& BlocksData[LocationToBlockIndex(x, y, z)].GetTextureIndex(GetWorld(), FaceDir) == TextureIndex))
						ShouldAddRow = false;
				}
				if(!ShouldAddRow)
					break;
			}
			third2--;
			
			for(int secondIter = second1; secondIter <= second2; secondIter++)
				for(int thirdIter = third1; thirdIter <= third2; thirdIter++)
					PlaneAdded[secondIter * 16 + thirdIter] = true;

			const float PosSecond = (second1 + second2) / 2.0f * 100.0f + 50.0f;
			const float PosThird = (third1 + third2) / 2.0f * 100.0f + 50.0f;
			float ScaleX = second2 - second1 + 1.0f;
			float ScaleY = third2 - third1 + 1.0f;
			FTransform Transform;
			switch(FaceDir)
			{
			case EFaceDirection::XPositive:
				{
					const float temp = ScaleX;
					ScaleX = ScaleY;
					ScaleY = temp;
					Transform =
						{{0, -90, 90},
						{FaceIndex * 100.0f + 100.0f, PosThird, PosSecond},
						{ScaleX, ScaleY, 1.0f}};
					break;
				}
			case EFaceDirection::XNegative:
				{
					const float temp = ScaleX;
					ScaleX = ScaleY;
					ScaleY = temp;
					Transform =
						{{0, 90, 90},
						{FaceIndex * 100.0f, PosThird, PosSecond},
						{ScaleX, ScaleY, 1.0f}};
					break;
				}
			case EFaceDirection::YPositive:
				Transform =
					{{0, 0, 90},
					{PosSecond, FaceIndex * 100.0f + 100.0f, PosThird},
					{ScaleX, ScaleY, 1.0f}};
				break;
			case EFaceDirection::YNegative:
				Transform =
					{{0, 180, 90},
					{PosSecond, FaceIndex * 100.0f, PosThird},
					{ScaleX, ScaleY, 1.0f}};
				break;
			case EFaceDirection::ZPositive:
				Transform =
					{{0, 0, 0},
					{PosSecond, PosThird, FaceIndex * 100.0f + 100.0f},
					{ScaleX, ScaleY, 1.0f}};
				break;
			case EFaceDirection::ZNegative:
				Transform =
					{{180, 0, 0},
					{PosSecond, PosThird, FaceIndex * 100.0f},
					{ScaleX, ScaleY, 1.0f}};
				break;
			}
			const int InstanceIndex = Planes->AddInstance(Transform);

			if(BlocksMaterial)
			{
				Planes->SetCustomDataValue(InstanceIndex, 0, ScaleX);
				Planes->SetCustomDataValue(InstanceIndex, 1, ScaleY);
				Planes->SetCustomDataValue(InstanceIndex, 2, TextureIndex);
			}
		}
	}
}

void AChunk::UnloadPlanesTouching(const EFaceDirection FaceDir) const
{
	FIntVector ChunkLocation = ChunkWorldLocationToChunkLocation(GetActorLocation());
	AChunk* Touching = nullptr;
	switch(FaceDir)
	{
		case EFaceDirection::XPositive:
			Touching = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({ChunkLocation.X - 1, ChunkLocation.Y, ChunkLocation.Z});
			break;
		case EFaceDirection::XNegative:
			Touching = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({ChunkLocation.X + 1, ChunkLocation.Y, ChunkLocation.Z});
			break;
		case EFaceDirection::YPositive:
			Touching = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({ChunkLocation.X, ChunkLocation.Y - 1, ChunkLocation.Z});
			break;
		case EFaceDirection::YNegative:
			Touching = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({ChunkLocation.X, ChunkLocation.Y + 1, ChunkLocation.Z});
			break;
		case EFaceDirection::ZPositive:
			Touching = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({ChunkLocation.X, ChunkLocation.Y, ChunkLocation.Z - 1});
			break;
		case EFaceDirection::ZNegative:
			Touching = GetWorld()->GetGameStateChecked<AMainGameStateBase>()->GetChunk({ChunkLocation.X, ChunkLocation.Y, ChunkLocation.Z + 1});
			break;
		default:
			__debugbreak();
	}
	if(Touching)
	{
		FTransform Transform;
		TArray<int32> ToRemove;
		for(int i = 0; i < Touching->Planes->GetInstanceCount(); i++)
			if(Touching->Planes->GetInstanceTransform(i, Transform))
				switch(FaceDir)
				{
					case EFaceDirection::XPositive:
						if(Transform.GetLocation().X == 1600.0f)
							ToRemove.Add(i);
						break;
					case EFaceDirection::XNegative:
						if(Transform.GetLocation().X == 0.0f)
							ToRemove.Add(i);
						break;
					case EFaceDirection::YPositive:
						if(Transform.GetLocation().Y == 1600.0f)
							ToRemove.Add(i);
						break;
					case EFaceDirection::YNegative:
						if(Transform.GetLocation().Y == 0.0f)
							ToRemove.Add(i);
						break;
					case EFaceDirection::ZPositive:
						if(Transform.GetLocation().Z == 1600.0f)
							ToRemove.Add(i);
						break;
					case EFaceDirection::ZNegative:
						if(Transform.GetLocation().Z == 0.0f)
							ToRemove.Add(i);
						break;
				}
		Touching->Planes->RemoveInstances(ToRemove);
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
