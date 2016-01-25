// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionInventoryGrid.h"

AOrionInventoryGrid::AOrionInventoryGrid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bOnlyRelevantToOwner = true;
	bReplicates = true;
}

void AOrionInventoryGrid::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOrionInventoryGrid, Inventory);
	DOREPLIFETIME(AOrionInventoryGrid, InventoryType);
	DOREPLIFETIME(AOrionInventoryGrid, Height);
	DOREPLIFETIME(AOrionInventoryGrid, Width);
}

//waiting for epic to fix rep notify of tarrays
void AOrionInventoryGrid::OnRep_Inventory()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetOwner());

	if (PC)
	{
		PC->EventRedrawInventory();
	}
}

FString AOrionInventoryGrid::GetGridName()
{
	switch (InventoryType)
	{
	case ITEM_HELMET:
		return "HELMET";
		break;
	case ITEM_CHEST:
		return "BODY";
		break;
	case ITEM_HANDS:
		return "HANDS";
		break;
	case ITEM_BELT:
		return "BELT";
		break;
	case ITEM_LEGS:
		return "LEGS";
		break;
	case ITEM_BOOTS:
		return "BOOTS";
		break;
	case ITEM_PRIMARYWEAPON:
		return "PRIMARY";
		break;
	case ITEM_SECONDARYWEAPON:
		return "SECONDARY";
		break;
	case ITEM_GADGET:
		return "GADGET";
		break;
	case ITEM_ABILITY:
		return "ABILITY";
		break;
	case ITEM_SHADER:
		return "SHADER";
		break;
	case ITEM_DISPLAYARMOR:
		return "DISPLAY";
		break;
	case ITEM_GRENADE:
		return "GRENADE";
		break;
	case ITEM_KNIFE:
		return "KNIFE";
		break;
	case ITEM_ANY:
		return "INVENTORY";
		break;
	}

	return "ERROR";
}