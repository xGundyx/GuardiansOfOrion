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

	InventoryMappings.Add(FInventoryMapping(TEXT("AssaultHelmetCommon")));
	InventoryMappings.Add(FInventoryMapping(TEXT("AssaultChestCommon")));
	InventoryMappings.Add(FInventoryMapping(TEXT("AssaultArmsCommon")));
	InventoryMappings.Add(FInventoryMapping(TEXT("AssaultLegsCommon")));

	InventoryMappings.Add(FInventoryMapping(TEXT("ReconHelmetCommon")));
	InventoryMappings.Add(FInventoryMapping(TEXT("ReconChestCommon")));
	InventoryMappings.Add(FInventoryMapping(TEXT("ReconArmsCommon")));
	InventoryMappings.Add(FInventoryMapping(TEXT("ReconLegsCommon")));
}