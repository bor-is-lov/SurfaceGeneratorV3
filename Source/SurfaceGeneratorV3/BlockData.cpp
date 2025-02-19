#include "BlockData.h"

#include "MainGameStateBase.h"

void FBlockData::SetBlock(const int Index)
{
	this->TypeIndex = Index;
}

void FBlockData::EmptyBlock()
{
	TypeIndex = -1;
}

const FBlockDefaults* FBlockData::GetDefaults(const UWorld* World) const
{
	return &World->GetGameStateChecked<AMainGameStateBase>()->BlocksDefaults[TypeIndex];
}
