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

const FBlockDefaults* FBlockData::GetDefaults(const AMainGameStateBase* MainGameState) const
{
	return &MainGameState->BlocksDefaults[DefaultsIndex];
}

const FBlockDefaults* FBlockData::GetDefaults(const AMainGameStateBase* MainGameState, const int Index)
{
	return &MainGameState->BlocksDefaults[Index];
}

int FBlockData::GetTextureIndex(const AMainGameStateBase* MainGameState, const EFaceDirection FaceDir) const
{
	const FBlockDefaults& Defaults =  MainGameState->BlocksDefaults[DefaultsIndex];

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

bool FBlockData::ShouldAddFace(const AMainGameStateBase* MainGameState, const FBlockData* TouchingBlock) const
{
	const FBlockDefaults* Defaults = GetDefaults(MainGameState);
	const FBlockDefaults* TouchingDefaults = TouchingBlock ? GetDefaults(MainGameState, TouchingBlock->DefaultsIndex) : nullptr;
	if(Defaults->Shape == EShape::Cube && (TouchingDefaults && !TouchingDefaults->bIsSolid))
		return true;
	if(Defaults->Shape == EShape::Liquid && (TouchingDefaults && !TouchingDefaults->bIsSolid && TouchingDefaults != Defaults))
		return true;
	return false;
}
