#include "Orion.h"
#include "OrionPlayFabInventoryMapper.h"

AOrionPlayFabInventoryMapper::AOrionPlayFabInventoryMapper(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	//populate these here and file in the actual linking in blueprints
	InventoryMappings.Add(FInventoryMapping(TEXT("MarineHelmetCommon")));
	InventoryMappings.Add(FInventoryMapping(TEXT("MarineChestCommon")));
	InventoryMappings.Add(FInventoryMapping(TEXT("MarineArmsCommon")));
	InventoryMappings.Add(FInventoryMapping(TEXT("MarineLegsCommon")));
}