

#include "Orion.h"
#include "OrionInventory.h"

AOrionInventory::AOrionInventory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
}

FString AOrionInventory::GetSlotName()
{
	switch (InventoryType)
	{
	case ITEM_HELMET:
		return TEXT("Helmet");
		break;
	case ITEM_CHEST:
		return TEXT("Body");
		break;
	case ITEM_HANDS:
		return TEXT("Arms");
		break;
	case ITEM_LEGS:
		return TEXT("Legs");
		break;
	case ITEM_PRIMARYWEAPON:
		return TEXT("Primary Weapon");
		break;
	case ITEM_SECONDARYWEAPON:
		return TEXT("Secondary Weapon");
		break;
	case ITEM_GADGET:
		return TEXT("Gadget");
		break;
	case ITEM_BELT:
		return TEXT("Waist");
		break;
	case ITEM_CAPE:
		return TEXT("Cape");
		break;
	case ITEM_USEABLE:
		return TEXT("Consumeable");
		break;
	case ITEM_SHADER:
		return TEXT("Shader");
		break;
	case ITEM_DISPLAYARMOR:
		return TEXT("DisplayArmor");
		break;
	case ITEM_ABILITY:
		return TEXT("Ability");
		break;
	}

	return TEXT("");
}

void AOrionInventory::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOrionInventory, Image);
	DOREPLIFETIME(AOrionInventory, ItemName);
	DOREPLIFETIME(AOrionInventory, ItemDescription);
	DOREPLIFETIME(AOrionInventory, bStackable);
	DOREPLIFETIME(AOrionInventory, Rarity);
	DOREPLIFETIME(AOrionInventory, StackAmount);
	DOREPLIFETIME(AOrionInventory, RequiredLevel);
	DOREPLIFETIME(AOrionInventory, InventoryType);
}

