#include "Orion.h"
#include "OrionBuff.h"

AOrionBuff::AOrionBuff(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BuffName = TEXT("Buff Name");
	Damage = 1.0f;
	SpeedMultiplier = 1.0f;
	Duration = 3.0f;
	TickInterval = 1.5f;
	bStackable = false;
	StackLimit = 1;
	NumStacks = 0;
	bIsBuff = false;
	bBlockSight = false;
}