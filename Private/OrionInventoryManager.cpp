

#include "Orion.h"
#include "OrionWeapon.h"
#include "OrionCharacter.h"
#include "OrionInventoryManager.h"
#include "OrionInventoryArmor.h"


AOrionInventoryManager::AOrionInventoryManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool AOrionInventoryManager::SwapItems(UOrionInventoryGrid *theGrid1, int32 index1, UOrionInventoryGrid *theGrid2, int32 index2)
{
	if (!theGrid1 || !theGrid2 || !OwnerController)
		return false;

	if (theGrid2->InventoryType == ITEM_ANY || theGrid1->Inventory[index1] == nullptr || theGrid1->Inventory[index1]->InventoryType == theGrid2->InventoryType)
	{
		if (theGrid1->InventoryType == ITEM_ANY || theGrid2->Inventory[index2] == nullptr || theGrid2->Inventory[index2]->InventoryType == theGrid1->InventoryType)
		{
			//if (theGrid1->InventoryType != theGrid2->InventoryType)
			//	return false;

			AOrionInventory *tempInv = theGrid1->Inventory[index1];

			theGrid1->Inventory[index1] = theGrid2->Inventory[index2];
			theGrid2->Inventory[index2] = tempInv;

			//check for any equipment changes
			EItemType Type = theGrid1->InventoryType;

			if (Type == ITEM_ANY)
				Type = theGrid2->InventoryType;

			if (Type != ITEM_ANY)
				EquipItems(Cast<AOrionCharacter>(OwnerController->GetPawn()), Type);

			return true;
		}
	}

	return false;
}

AOrionInventory *AOrionInventoryManager::GetItemAt(UOrionInventoryGrid *theGrid, int32 index)
{
	return theGrid->Inventory[index];
}

bool AOrionInventoryManager::HandleRightClick(UOrionInventoryGrid *theGrid, int32 index)
{
	//don't let the player unequip weapons just yet
	if (theGrid == WeaponSlot1 || theGrid == WeaponSlot2)
		return false;

	//make sure it's a valid index
	if (theGrid->Inventory.Num() <= index)
		return false;

	AOrionInventory *Inv = theGrid->Inventory[index];

	//ignore clicks on null inventories
	if (!Inv || !OwnerController)
		return false;

	//only allow this when we have a valid pawn?
	AOrionCharacter *MyPawn = Cast<AOrionCharacter>(OwnerController->GetPawn());

	//TODO: allow the player to change gear while respawning
	if (!MyPawn)
		return false;

	//right clicking from the main inventory means we're trying to equip something, or trying to use a consumeable
	if (theGrid == Grid)
	{
		return TryToEquip(MyPawn, theGrid, index);
	}
	//otherwise we're trying to unequip something
	else
	{
		return TryToUnEquip(MyPawn, theGrid, index);
	}

	return true;
}

bool AOrionInventoryManager::TryToEquip(AOrionCharacter *MyPawn, UOrionInventoryGrid *theGrid, int32 index)
{
	AOrionInventory *Inv = theGrid->Inventory[index];

	if (Inv)
	{
		//first make sure we are able to wear this piece of gear
		if (MyPawn->Level < Inv->RequiredLevel)
		{
			return false;
		}

		switch (Inv->InventoryType)
		{
		case ITEM_HELMET:
			return SwapItems(theGrid, index, HelmetSlot, 0);
			break;
		case ITEM_CHEST:
			return SwapItems(theGrid, index, BodySlot, 0);
			break;
		case ITEM_LEGS:
			return SwapItems(theGrid, index, LegsSlot, 0);
			break;
		case ITEM_HANDS:
			return SwapItems(theGrid, index, HandsSlot, 0);
			break;
		case ITEM_PRIMARYWEAPON:
			return SwapItems(theGrid, index, WeaponSlot1, 0);
			break;
		case ITEM_SECONDARYWEAPON:
			return SwapItems(theGrid, index, WeaponSlot2, 0);
			break;
		default:
			break;
		};
	}

	return false;
}

bool AOrionInventoryManager::TryToUnEquip(AOrionCharacter *MyPawn, UOrionInventoryGrid *theGrid, int32 index)
{
	AOrionInventory *Inv = theGrid->Inventory[index];

	if (Inv)
	{
		//make sure we have an empty inventory slot for this item
		bool bSpotOpen = false;
		int32 SlotIndex = 0;
		for (int32 i = 0; i < Grid->Inventory.Num() && !bSpotOpen; i++)
		{
			if (Grid->Inventory[i] == nullptr)
			{
				bSpotOpen = true;
				SlotIndex = i;
			}
		}

		if (!bSpotOpen)
			return false;

		return SwapItems(theGrid, index, Grid, SlotIndex);
	}

	return false;
}
//this will equip all items based on what's in the inventory
void AOrionInventoryManager::EquipItems(AOrionCharacter *aPawn, EItemType SlotType)
{
	AOrionArmor *Armor = nullptr;
	AOrionWeapon *Weapon = nullptr;
	AOrionWeapon *OtherWeapon = nullptr;
	bool bSwitchWeapon = false;

	if (!aPawn)
		return;

	switch (SlotType)
	{
	case ITEM_ANY:
	case ITEM_HELMET:
		Armor = Cast<AOrionArmor>(HelmetSlot->Inventory[0]);
		if (Armor)
			aPawn->SetHelmetMesh(Armor->Mesh);
		else
			aPawn->SetHelmetMesh(NULL);
		if(SlotType !=ITEM_ANY)
			break;

	case ITEM_CHEST:
		Armor = Cast<AOrionArmor>(BodySlot->Inventory[0]);
		if (Armor)
			aPawn->SetBodyMesh(Armor->Mesh);
		else
			aPawn->SetBodyMesh(NULL);
		if (SlotType != ITEM_ANY)
			break;

	case ITEM_HANDS:
		Armor = Cast<AOrionArmor>(HandsSlot->Inventory[0]);
		if (Armor)
			aPawn->SetArmsMesh(Armor->Mesh);
		else
			aPawn->SetArmsMesh(NULL);

		if (aPawn->IsLocallyControlled())
		{
			if (Armor)
				aPawn->Set1PArmorMesh(Armor->Mesh1P);
			else
				aPawn->Set1PArmorMesh(NULL);
		}

		if (SlotType != ITEM_ANY)
			break;

	case ITEM_LEGS:
		Armor = Cast<AOrionArmor>(LegsSlot->Inventory[0]);
		if (Armor)
			aPawn->SetLegsMesh(Armor->Mesh);
		else
			aPawn->SetLegsMesh(NULL);
		if (SlotType != ITEM_ANY)
			break;

	case ITEM_PRIMARYWEAPON:
		Weapon = Cast<AOrionWeapon>(WeaponSlot1->Inventory[0]);
		OtherWeapon = aPawn->GetWeaponFromType(ITEM_PRIMARYWEAPON);

		if (OtherWeapon && Weapon && Weapon->InventoryType == OtherWeapon->InventoryType)
			bSwitchWeapon = true;

		if (OtherWeapon)
			aPawn->RemoveWeapon(OtherWeapon);
		aPawn->AddWeapon(Weapon);

		if (bSwitchWeapon)
			aPawn->EquipWeapon(Weapon);

		aPawn->CheckWeaponEquipped();

		bSwitchWeapon = false;

		if (SlotType != ITEM_ANY)
			break;

	case ITEM_SECONDARYWEAPON:
		Weapon = Cast<AOrionWeapon>(WeaponSlot2->Inventory[0]);
		OtherWeapon = aPawn->GetWeaponFromType(ITEM_SECONDARYWEAPON);

		if (OtherWeapon && Weapon && Weapon->InventoryType == OtherWeapon->InventoryType)
			bSwitchWeapon = true;

		if (OtherWeapon)
			aPawn->RemoveWeapon(OtherWeapon);
		aPawn->AddWeapon(Weapon);

		if (bSwitchWeapon)
			aPawn->EquipWeapon(Weapon);

		aPawn->CheckWeaponEquipped();

		bSwitchWeapon = false;

		if (SlotType != ITEM_ANY)
			break;
	}
}

void AOrionInventoryManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//skip owner
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, Money, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, PrimaryAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, SecondaryAmmo, COND_OwnerOnly);
}

void AOrionInventoryManager::GiveMoney(int32 Amount)
{
	if (Role == ROLE_Authority)
	{
		Money += Amount;
	}
}

void AOrionInventoryManager::Init()
{
	Grid = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (Grid)
		Grid->CreateInventory(10, 10, 96, 1, ITEM_ANY);

	HelmetSlot = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (HelmetSlot)
		HelmetSlot->CreateInventory(1, 1, 96, 1, ITEM_HELMET);

	HandsSlot = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (HandsSlot)
		HandsSlot->CreateInventory(1, 1, 96, 1, ITEM_HANDS);

	BodySlot = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (BodySlot)
		BodySlot->CreateInventory(1, 1, 96, 1, ITEM_CHEST);

	LegsSlot = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (LegsSlot)
		LegsSlot->CreateInventory(1, 1, 96, 1, ITEM_LEGS);

	WeaponSlot1 = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (WeaponSlot1)
		WeaponSlot1->CreateInventory(1, 1, 96, 1, ITEM_PRIMARYWEAPON);

	WeaponSlot2 = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (WeaponSlot2)
		WeaponSlot2->CreateInventory(1, 1, 96, 1, ITEM_SECONDARYWEAPON);

	RingSlot1 = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (RingSlot1)
		RingSlot1->CreateInventory(1, 1, 96, 1, ITEM_RING);

	RingSlot2 = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (RingSlot2)
		RingSlot2->CreateInventory(1, 1, 96, 1, ITEM_RING);

	ShieldSlot = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (ShieldSlot)
		ShieldSlot->CreateInventory(1, 1, 96, 1, ITEM_SHIELD);

	NeckSlot = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (NeckSlot)
		NeckSlot->CreateInventory(1, 1, 96, 1, ITEM_NECK);

	BeltSlot = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (BeltSlot)
		BeltSlot->CreateInventory(1, 1, 96, 1, ITEM_BELT);

	GadgetSlot = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (GadgetSlot)
		GadgetSlot->CreateInventory(1, 1, 96, 1, ITEM_GADGET);
}

void AOrionInventoryManager::DestroyInventory()
{
	for (int32 i = 0; i < Grid->Height; i++)
	{
		for (int32 j = 0; j < Grid->Width; j++)
		{
			if (Grid->Inventory[i*Grid->Width + j])
			{
				Grid->Inventory[i*Grid->Width + j]->Destroy();
				Grid->Inventory[i*Grid->Width + j] = NULL;
			}
		}
	}

	if (HelmetSlot->Inventory[0])
	{
		HelmetSlot->Inventory[0]->Destroy();
		HelmetSlot->Inventory[0] = NULL;
	}

	if (BodySlot->Inventory[0])
	{
		BodySlot->Inventory[0]->Destroy();
		BodySlot->Inventory[0] = NULL;
	}

	if (HandsSlot->Inventory[0])
	{
		HandsSlot->Inventory[0]->Destroy();
		HandsSlot->Inventory[0] = NULL;
	}

	if (LegsSlot->Inventory[0])
	{
		LegsSlot->Inventory[0]->Destroy();
		LegsSlot->Inventory[0] = NULL;
	}
}

//returns index we were added at, -1 means failure
int32 AOrionInventoryManager::AddItemToInventory(UOrionInventoryGrid *theGrid, AOrionInventory* newItem)
{
	for (int32 i = 0; i < theGrid->Height; i++)
	{
		for (int32 j = 0; j < theGrid->Width; j++)
		{
			if (theGrid->Inventory[i*theGrid->Width + j] == NULL)
			{
				theGrid->Inventory[i*theGrid->Width + j] = newItem;
				return i * theGrid->Width + j;
			}
		}
	}

	newItem->Destroy();
	newItem = NULL;
	return -1;
}

void AOrionInventoryManager::RemoveItemFromInventory(UOrionInventoryGrid *theGrid, int32 index)
{
	if (theGrid->Inventory.Num() > index)
	{
		theGrid->Inventory[index] = NULL;
	}
}
