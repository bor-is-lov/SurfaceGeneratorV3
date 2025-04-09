// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk.h"
#include "MainGameStateBase.h"
#include "Components/BoxComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

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
							ShouldAddPlane[*second * 16 + *third] = BlocksData[LocationToBlockIndex(tempX, tempY, tempZ)].
								ShouldAddFace(MainGameState, &Touching->BlocksData[LocationToBlockIndex(x, y, z)]);
							*first = FaceIndex;
						}
						else
							ShouldAddPlane[*second * 16 + *third] =BlocksData[LocationToBlockIndex(x, y, z)].
								ShouldAddFace(MainGameState, nullptr);
						else
						{
							const int tempX = x, tempY = y, tempZ = z;
							(*first)++;
							ShouldAddPlane[*second * 16 + *third] = BlocksData[LocationToBlockIndex(tempX, tempY, tempZ)].
								ShouldAddFace(MainGameState, &BlocksData[LocationToBlockIndex(x, y, z)]);
							(*first)--;
						}
			else
				if(*first == 0)
					if(Touching)
					{
						const int tempX = x, tempY = y, tempZ = z;
						*first = 15;
						ShouldAddPlane[*second * 16 + *third] = BlocksData[LocationToBlockIndex(tempX, tempY, tempZ)].
							ShouldAddFace(MainGameState, &Touching->BlocksData[LocationToBlockIndex(x, y, z)]);
						*first = FaceIndex;
					}
					else
						ShouldAddPlane[*second * 16 + *third] = BlocksData[LocationToBlockIndex(x, y, z)].
							ShouldAddFace(MainGameState, nullptr);
					else
					{
						const int tempX = x, tempY = y, tempZ = z;
						(*first)--;
						ShouldAddPlane[*second * 16 + *third] = BlocksData[LocationToBlockIndex(tempX, tempY, tempZ)].
							ShouldAddFace(MainGameState, &BlocksData[LocationToBlockIndex(x, y, z)]);
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
			const int TextureIndex = BlocksData[LocationToBlockIndex(*x1, *y1, *z1)].GetTextureIndex(MainGameState, FaceDir);
			
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
						&& BlocksData[LocationToBlockIndex(x, y, z)].GetTextureIndex(MainGameState, FaceDir) == TextureIndex))
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
						&& BlocksData[LocationToBlockIndex(x, y, z)].GetTextureIndex(MainGameState, FaceDir) == TextureIndex))
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
			
			if(BlocksData[LocationToBlockIndex(*x1, *y1, *z1)].GetDefaults(MainGameState)->bIsSolid)
			{
				const int InstanceIndex = SolidPlanes->AddInstance(Transform);
				if(SolidBlocksMaterial)
				{
					SolidPlanes->SetCustomDataValue(InstanceIndex, 0, ScaleX);
					SolidPlanes->SetCustomDataValue(InstanceIndex, 1, ScaleY);
					SolidPlanes->SetCustomDataValue(InstanceIndex, 2, TextureIndex);
				}
			}
			else
			{
				UStaticMeshComponent* Plane = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
				Plane->SetStaticMesh(PlaneMesh);
				Plane->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
				Plane->RegisterComponent();
				Plane->SetComponentTickEnabled(false);
				Plane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				Plane->SetRelativeTransform(Transform);
				TransparentPlanes.Add(Plane);

				if(TransparentBlocksMaterial)
				{
					Plane->SetMaterial(0, TransparentBlocksMaterial);
					Plane->SetScalarParameterValueOnMaterials("U_Tiling", ScaleX);
					Plane->SetScalarParameterValueOnMaterials("V_Tiling", ScaleY);
					Plane->SetScalarParameterValueOnMaterials("TextureIndex", TextureIndex);
				}
			}
		}
	}
}

void AChunk::UnloadSolidPlanesTouching(const EFaceDirection FaceDir) const
{
	FIntVector ChunkLocation = ChunkWorldLocationToChunkLocation(GetActorLocation());
	switch(FaceDir)
	{
	case EFaceDirection::XPositive:
		ChunkLocation.X--;
		break;
	case EFaceDirection::XNegative:
		ChunkLocation.X++;
		break;
	case EFaceDirection::YPositive:
		ChunkLocation.Y--;
		break;
	case EFaceDirection::YNegative:
		ChunkLocation.Y++;
		break;
	case EFaceDirection::ZPositive:
		ChunkLocation.Z--;
		break;
	case EFaceDirection::ZNegative:
		ChunkLocation.Z++;
		break;
	default:
		__debugbreak();
	}
	if(const AChunk* Touching = MainGameState->GetChunk(ChunkLocation))
	{
		FTransform Transform;
		TArray<int32> ToRemove;
		for(int i = 0; i < Touching->SolidPlanes->GetInstanceCount(); i++)
			if(Touching->SolidPlanes->GetInstanceTransform(i, Transform))
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
		Touching->SolidPlanes->RemoveInstances(ToRemove);
	}
}

void AChunk::UnloadTransparentPlanesTouching(const EFaceDirection FaceDir) const
{
	FIntVector ChunkLocation = ChunkWorldLocationToChunkLocation(GetActorLocation());
	switch(FaceDir)
	{
	case EFaceDirection::XPositive:
		ChunkLocation.X--;
		break;
	case EFaceDirection::XNegative:
		ChunkLocation.X++;
		break;
	case EFaceDirection::YPositive:
		ChunkLocation.Y--;
		break;
	case EFaceDirection::YNegative:
		ChunkLocation.Y++;
		break;
	case EFaceDirection::ZPositive:
		ChunkLocation.Z--;
		break;
	case EFaceDirection::ZNegative:
		ChunkLocation.Z++;
		break;
	default:
		__debugbreak();
	}
	if(AChunk* Touching = MainGameState->GetChunk(ChunkLocation))
	{
		TArray<TObjectPtr<UStaticMeshComponent>> NewTransparentPlanes;
		for(int i = 0; i < Touching->TransparentPlanes.Num(); i++)
		{
			FTransform Transform = Touching->TransparentPlanes[i]->GetRelativeTransform();
			switch(FaceDir)
			{
			case EFaceDirection::XPositive:
				if(Transform.GetLocation().X != 1600.0f)
					NewTransparentPlanes.Add(Touching->TransparentPlanes[i]);
				break;
			case EFaceDirection::XNegative:
				if(Transform.GetLocation().X != 0.0f)
					NewTransparentPlanes.Add(Touching->TransparentPlanes[i]);
				break;
			case EFaceDirection::YPositive:
				if(Transform.GetLocation().Y != 1600.0f)
					NewTransparentPlanes.Add(Touching->TransparentPlanes[i]);
				break;
			case EFaceDirection::YNegative:
				if(Transform.GetLocation().Y != 0.0f)
					NewTransparentPlanes.Add(Touching->TransparentPlanes[i]);
				break;
			case EFaceDirection::ZPositive:
				if(Transform.GetLocation().Z != 1600.0f)
					NewTransparentPlanes.Add(Touching->TransparentPlanes[i]);
				break;
			case EFaceDirection::ZNegative:
				if(Transform.GetLocation().Z != 0.0f)
					NewTransparentPlanes.Add(Touching->TransparentPlanes[i]);
				break;
			}
		}
		for(auto& Plane : Touching->TransparentPlanes)
			if(!NewTransparentPlanes.Contains(Plane))
		Touching->TransparentPlanes = NewTransparentPlanes;
	}
}

void AChunk::LoadPlanes()
{
	FIntVector ChunkLocation = ChunkWorldLocationToChunkLocation(GetActorLocation());
	AChunk* Touching = MainGameState->GetChunk({ChunkLocation.X + 1, ChunkLocation.Y, ChunkLocation.Z});
	if(Touching && Touching->State == EState::Loaded)
	{
		for(int x = 0; x < 16; x++)
			LoadPlanesAtIndex(EFaceDirection::XPositive, x, Touching);
		Touching->LoadPlanesAtIndex(EFaceDirection::XNegative, 0, this);
	}
	else
		for(int x = 0; x < 16; x++)
			LoadPlanesAtIndex(EFaceDirection::XPositive, x, nullptr);
	
	Touching = MainGameState->GetChunk({ChunkLocation.X, ChunkLocation.Y + 1, ChunkLocation.Z});
	if(Touching && Touching->State == EState::Loaded)
	{
		for(int y = 0; y < 16; y++)
			LoadPlanesAtIndex(EFaceDirection::YPositive, y, Touching);
		Touching->LoadPlanesAtIndex(EFaceDirection::YNegative, 0, this);
	}
	else
		for(int y = 0; y < 16; y++)
			LoadPlanesAtIndex(EFaceDirection::YPositive, y, nullptr);
	
	Touching = MainGameState->GetChunk({ChunkLocation.X, ChunkLocation.Y, ChunkLocation.Z + 1});
	if(Touching && Touching->State == EState::Loaded)
	{
		for(int z = 0; z < 16; z++)
			LoadPlanesAtIndex(EFaceDirection::ZPositive, z, Touching);
		Touching->LoadPlanesAtIndex(EFaceDirection::ZNegative, 0, this);
	}
	else
		for(int z = 0; z < 16; z++)
			LoadPlanesAtIndex(EFaceDirection::ZPositive, z, nullptr);
	
	Touching = MainGameState->GetChunk({ChunkLocation.X - 1, ChunkLocation.Y, ChunkLocation.Z});
	if(Touching && Touching->State == EState::Loaded)
	{
		for(int x = 0; x < 16; x++)
			LoadPlanesAtIndex(EFaceDirection::XNegative, x, Touching);
		Touching->LoadPlanesAtIndex(EFaceDirection::XPositive, 15, this);
	}
	else
		for(int x = 0; x < 16; x++)
			LoadPlanesAtIndex(EFaceDirection::XNegative, x, nullptr);
	
	Touching = MainGameState->GetChunk({ChunkLocation.X, ChunkLocation.Y - 1, ChunkLocation.Z});
	if(Touching && Touching->State == EState::Loaded)
	{
		for(int y = 0; y < 16; y++)
			LoadPlanesAtIndex(EFaceDirection::YNegative, y, Touching);
		Touching->LoadPlanesAtIndex(EFaceDirection::YPositive, 15, this);
	}
	else
		for(int y = 0; y < 16; y++)
			LoadPlanesAtIndex(EFaceDirection::YNegative, y, nullptr);
	
	Touching = MainGameState->GetChunk({ChunkLocation.X, ChunkLocation.Y, ChunkLocation.Z - 1});
	if(Touching && Touching->State == EState::Loaded)
	{
		for(int z = 0; z < 16; z++)
			LoadPlanesAtIndex(EFaceDirection::ZNegative, z, Touching);
		Touching->LoadPlanesAtIndex(EFaceDirection::ZPositive, 15, this);
	}
	else
		for(int z = 0; z < 16; z++)
			LoadPlanesAtIndex(EFaceDirection::ZNegative, z, nullptr);
}

void AChunk::UnloadPlanes()
{
	SolidPlanes->ClearInstances();
	for(auto& Plane : TransparentPlanes)
		Plane->ConditionalBeginDestroy();
	TransparentPlanes.Empty();
	
	UnloadSolidPlanesTouching(EFaceDirection::XPositive);
	UnloadSolidPlanesTouching(EFaceDirection::YPositive);
	UnloadSolidPlanesTouching(EFaceDirection::ZPositive);
	UnloadSolidPlanesTouching(EFaceDirection::XNegative);
	UnloadSolidPlanesTouching(EFaceDirection::YNegative);
	UnloadSolidPlanesTouching(EFaceDirection::ZNegative);
	UnloadTransparentPlanesTouching(EFaceDirection::XPositive);
	UnloadTransparentPlanesTouching(EFaceDirection::YPositive);
	UnloadTransparentPlanesTouching(EFaceDirection::ZPositive);
	UnloadTransparentPlanesTouching(EFaceDirection::XNegative);
	UnloadTransparentPlanesTouching(EFaceDirection::YNegative);
	UnloadTransparentPlanesTouching(EFaceDirection::ZNegative);
}

void AChunk::LoadSolidBoxes()
{
	bool ShouldAddBox[4096] = {false}, BoxAdded[4096] = {false};
	for(int i = 0; i < 4096; i++)
		ShouldAddBox[i] =  BlocksData[i].GetDefaults(MainGameState)->bIsSolid
						&& BlocksData[i].GetDefaults(MainGameState)->Shape == EShape::Cube;

	for(int i = 0; i < 4096; i++)
		if(ShouldAddBox[i] && !BoxAdded[i])
		{
			int x1, y1, z1;
			BlockIndexToLocation(i, x1, y1, z1);
			int x2 = x1, y2 = y1, z2 = z1;

			for(; x2 < 16; x2++)
				if(!(ShouldAddBox[LocationToBlockIndex(x2, y1, z1)] && !BoxAdded[LocationToBlockIndex(x2, y1, z1)]))
					break;
			x2--;

			for(; y2 < 16; y2++)
			{
				bool ShouldAddRow = true;
				for(int x = x1; x <= x2; x++)
					if(!(ShouldAddBox[LocationToBlockIndex(x, y2, z1)] && !BoxAdded[LocationToBlockIndex(x, y2, z1)]))
						ShouldAddRow = false;
				if(!ShouldAddRow)
					break;
			}
			y2--;

			for(; z2 < 16; z2++)
			{
				bool ShouldAddSquare = true;
				for(int x = x1; x <= x2; x++)
					for(int y = y1; y <= y2; y++)
						if(!(ShouldAddBox[LocationToBlockIndex(x, y, z2)] && !BoxAdded[LocationToBlockIndex(x, y, z2)]))
							ShouldAddSquare = false;
				if(!ShouldAddSquare)
					break;
			}
			z2--;

			for(int x = x1; x <= x2; x++)
				for(int y = y1; y <= y2; y++)
					for(int z = z1; z <= z2; z++)
						BoxAdded[LocationToBlockIndex(x, y, z)] = true;

			UBoxComponent* Box = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass());
			Box->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			Box->RegisterComponent();
			Box->SetComponentTickEnabled(false);
			if(bCollisionEnabled)
				Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			else
				Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Box->SetCollisionResponseToAllChannels(ECR_Block);
			Box->SetRelativeLocation({(x1 + x2 + 1) * 50.0f, (y1 + y2 + 1) * 50.0f, (z1 + z2 + 1) * 50.0f});
			Box->SetBoxExtent({(x2 - x1 + 1) * 50.0f, (y2 - y1 + 1) * 50.0f, (z2 - z1 + 1) * 50.0f});
			SolidBoxes.Add(Box);
		}
}

void AChunk::LoadLiquidBoxes()
{
	bool ShouldAddBox[4096] = {false}, BoxAdded[4096] = {false};
	for(int i = 0; i < 4096; i++)
		ShouldAddBox[i] = BlocksData[i].GetDefaults(MainGameState)->Shape == EShape::Liquid;

	for(int i = 0; i < 4096; i++)
		if(ShouldAddBox[i] && !BoxAdded[i])
		{
			int x1, y1, z1;
			BlockIndexToLocation(i, x1, y1, z1);
			int x2 = x1, y2 = y1, z2 = z1;

			for(; x2 < 16; x2++)
				if(!(ShouldAddBox[LocationToBlockIndex(x2, y1, z1)] && !BoxAdded[LocationToBlockIndex(x2, y1, z1)]))
					break;
			x2--;

			for(; y2 < 16; y2++)
			{
				bool ShouldAddRow = true;
				for(int x = x1; x <= x2; x++)
					if(!(ShouldAddBox[LocationToBlockIndex(x, y2, z1)] && !BoxAdded[LocationToBlockIndex(x, y2, z1)]))
						ShouldAddRow = false;
				if(!ShouldAddRow)
					break;
			}
			y2--;

			for(; z2 < 16; z2++)
			{
				bool ShouldAddSquare = true;
				for(int x = x1; x <= x2; x++)
					for(int y = y1; y <= y2; y++)
						if(!(ShouldAddBox[LocationToBlockIndex(x, y, z2)] && !BoxAdded[LocationToBlockIndex(x, y, z2)]))
							ShouldAddSquare = false;
				if(!ShouldAddSquare)
					break;
			}
			z2--;

			for(int x = x1; x <= x2; x++)
				for(int y = y1; y <= y2; y++)
					for(int z = z1; z <= z2; z++)
						BoxAdded[LocationToBlockIndex(x, y, z)] = true;

			UBoxComponent* Box = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass());
			Box->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			Box->RegisterComponent();
			Box->SetComponentTickEnabled(false);
			if(bCollisionEnabled)
				Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			else
				Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Box->SetCollisionResponseToAllChannels(ECR_Overlap);
			Box->SetRelativeLocation({(x1 + x2 + 1) * 50.0f, (y1 + y2 + 1) * 50.0f, (z1 + z2 + 1) * 50.0f});
			Box->SetBoxExtent({(x2 - x1 + 1) * 50.0f, (y2 - y1 + 1) * 50.0f, (z2 - z1 + 1) * 50.0f});
			LiquidBoxes.Add(Box);
		}
}

void AChunk::UnloadBoxes()
{
	for(auto& Box : SolidBoxes)
		Box->ConditionalBeginDestroy();
	SolidBoxes.Empty();
	for(auto& Box : LiquidBoxes)
		Box->ConditionalBeginDestroy();
	LiquidBoxes.Empty();
}

AChunk::AChunk() : InLoadChunksQueue(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;

	SolidPlanes = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("SolidPlanes");
	SolidPlanes->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if(MeshAsset.Succeeded())
	{
		SolidPlanes->SetStaticMesh(MeshAsset.Object);
		PlaneMesh = MeshAsset.Object;
	}
	SolidPlanes->SetGenerateOverlapEvents(false);
	SolidPlanes->SetComponentTickEnabled(false);
	SolidPlanes->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SolidPlanes->NumCustomDataFloats = 3;
	
	BlocksData.SetNum(4096);
}

void AChunk::BeginPlay()
{
	Super::BeginPlay();
	
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	MainGameState = GetWorld()->GetGameStateChecked<AMainGameStateBase>();
	SolidBlocksMaterial = MainGameState->SolidBlocksMaterial;
	if(SolidBlocksMaterial)
		SolidPlanes->SetMaterial(0, SolidBlocksMaterial);
	TransparentBlocksMaterial = MainGameState->TransparentBlocksMaterial;
}

void AChunk::LoadMeshes()
{
	LoadSolidBoxes();
	LoadLiquidBoxes();
	LoadPlanes();
}

void AChunk::UnloadMeshes()
{
	UnloadPlanes();
	UnloadBoxes();
}

void AChunk::StartLoading()
{
	State = EState::Loading;
	MainGameState->AddToLoadChunks(this);
}

void AChunk::EndLoading()
{
	State = EState::Loaded;
	SetActorHiddenInGame(false);
}

void AChunk::StartUnloading()
{
	State = EState::Unloading;
	SetActorHiddenInGame(true);
	
	MainGameState->AddToUnloadChunks(this);
}

void AChunk::EndUnloading()
{
	State = EState::Unloaded;
	SetActorHiddenInGame(true);
}

void AChunk::SetbCollisionEnabled(bool NewCollisionEnabled)
{
	if(NewCollisionEnabled != bCollisionEnabled)
	{
		if(NewCollisionEnabled)
		{
			for(auto& Box : SolidBoxes)
				Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			for(auto& Box : LiquidBoxes)
				Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		else
		{
			for(auto& Box : SolidBoxes)
				Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			for(auto& Box : LiquidBoxes)
				Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		bCollisionEnabled = NewCollisionEnabled;
	}
}

void AChunk::SetBlock(const size_t InChunkIndex, const size_t TypeIndex)
{
	if(MainGameState->BlocksDefaults.Num() >= TypeIndex)
		BlocksData[InChunkIndex].SetBlock(TypeIndex);
	else
		BlocksData[InChunkIndex].SetBlock(0);
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

void AChunk::BlockIndexToLocation(const size_t& Index, int& x, int& y, int& z)
{
	x = Index / 256;
	y = Index / 16 % 16;
	z = Index % 16;
}

size_t AChunk::LocationToBlockIndex(const FIntVector& Location)
{
	return Location.X * 256 + Location.Y * 16 + Location.Z;
}

size_t AChunk::LocationToBlockIndex(const int x, const int y, const int z)
{
	return x * 256 + y * 16 + z;
}
