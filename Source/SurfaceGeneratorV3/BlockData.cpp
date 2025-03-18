#include "BlockData.h"

#include "MainGameStateBase.h"

void FBlockData::SetBlock(const int Index)
{
	this->DefaultsIndex = Index;
}

void FBlockData::EmptyBlock()
{
	DefaultsIndex = -1;
}

FBlockDefaults* FBlockData::GetDefaults(const UWorld* World) const
{
	return &World->GetGameStateChecked<AMainGameStateBase>()->BlocksDefaults[DefaultsIndex];
}

FBlockDefaults* FBlockData::GetDefaults(const UWorld* World, const int Index)
{
	return &World->GetGameStateChecked<AMainGameStateBase>()->BlocksDefaults[Index];
}

int FBlockData::GetTextureIndex(const UWorld* World, const EFaceDirection FaceDir) const
{
	FBlockDefaults& Defaults =  World->GetGameStateChecked<AMainGameStateBase>()->BlocksDefaults[DefaultsIndex];

	switch (FaceDir)
	{
	case EFaceDirection::ZPositive:
		if(Defaults.TexturesIndices.Contains("Top"))
			return *Defaults.TexturesIndices.Find("Top");
		break;
		
	case EFaceDirection::ZNegative:
		if(Defaults.TexturesIndices.Contains("Bottom"))
			return *Defaults.TexturesIndices.Find("Bottom");
		break;
		
	case EFaceDirection::XPositive:
		if(Defaults.TexturesIndices.Contains("Side"))
			return *Defaults.TexturesIndices.Find("Side");
		break;
		
	case EFaceDirection::XNegative:
		if(Defaults.TexturesIndices.Contains("Side"))
			return *Defaults.TexturesIndices.Find("Side");
		break;
		
	case EFaceDirection::YPositive:
		if(Defaults.TexturesIndices.Contains("Side"))
			return *Defaults.TexturesIndices.Find("Side");
		break;
		
	case EFaceDirection::YNegative:
		if(Defaults.TexturesIndices.Contains("Side"))
			return *Defaults.TexturesIndices.Find("Side");
		break;
		
	default:
		if(Defaults.TexturesIndices.Contains("Default"))
			return *Defaults.TexturesIndices.Find("Default");
		break;
	}
	
	if(Defaults.TexturesIndices.Contains("Default"))
		return *Defaults.TexturesIndices.Find("Default");
	
	return 0;
}

bool FBlockData::ShouldAddFace(const UWorld* World, const FBlockData* TouchingBlock) const
{
	const FBlockDefaults* Defaults = GetDefaults(World);
	const FBlockDefaults* TouchingDefaults = TouchingBlock ? GetDefaults(World, TouchingBlock->DefaultsIndex) : nullptr;
	if(Defaults->Shape == EShape::Cube && (TouchingDefaults && !TouchingDefaults->bIsSolid))
		return true;
	return false;
}
