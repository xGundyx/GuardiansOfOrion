

#include "Orion.h"
#include "OrionWeapon.h"
#include "OrionCharacter.h"
#include "OrionInventoryManager.h"
#include "OrionInventoryArmor.h"


AOrionInventoryManager::AOrionInventoryManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bOnlyRelevantToOwner = true;
	bReplicates = true;
}

bool AOrionInventoryManager::ServerSwapItems_Validate(AOrionInventoryGrid *theGrid1, int32 index1, AOrionInventoryGrid *theGrid2, int32 index2)
{
	return true;
}

void AOrionInventoryManager::ServerSwapItems_Implementation(AOrionInventoryGrid *theGrid1, int32 index1, AOrionInventoryGrid *theGrid2, int32 index2)
{
	SwapItems(theGrid1, index1, theGrid2, index2);
}

bool AOrionInventoryManager::SwapItems(AOrionInventoryGrid *theGrid1, int32 index1, AOrionInventoryGrid *theGrid2, int32 index2)
{
	if (Role != ROLE_Authority)
	{
		ServerSwapItems(theGrid1, index1, theGrid2, index2);
		return false;
	}

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

			DrawInventory();

			return true;
		}
	}

	return false;
}

AOrionInventory *AOrionInventoryManager::GetItemAt(AOrionInventoryGrid *theGrid, int32 index)
{
	if (theGrid && theGrid->Inventory.Num() > index)
		return theGrid->Inventory[index];

	return nullptr;
}

bool AOrionInventoryManager::ServerHandleRightClick_Validate(AOrionInventoryGrid *theGrid, int32 index)
{
	return true;
}

void AOrionInventoryManager::ServerHandleRightClick_Implementation(AOrionInventoryGrid *theGrid, int32 index)
{
	HandleRightClick(theGrid, index);
}

bool AOrionInventoryManager::HandleRightClick(AOrionInventoryGrid *theGrid, int32 index)
{
	if (Role != ROLE_Authority)
	{
		ServerHandleRightClick(theGrid, index);
		return false;
	}

	//don't let the player unequip weapons just yet
	if (theGrid == nullptr || theGrid == WeaponSlot1 || theGrid == WeaponSlot2)
		return false;

	//make sure it's a valid index
	if (theGrid->Inventory.Num() <= index)
		return false;

	AOrionInventory *Inv = theGrid->Inventory[index];

	if (!OwnerController)
		OwnerController = Cast<AOrionPlayerController>(GetOwner());

	//ignore clicks on null inventories
	if (!Inv || !OwnerController)
		return false;

	//only allow this when we have a valid pawn?
	AOrionCharacter *MyPawn = Cast<AOrionCharacter>(OwnerController->GetPawn());

	//TODO: allow the player to change gear while respawning
	if (!MyPawn)
		return false;

	bool bRet = false;
	//right clicking from the main inventory means we're trying to equip something, or trying to use a consumeable
	if (theGrid == Grid)
	{
		bRet = TryToEquip(MyPawn, theGrid, index);
		DrawInventory();
		return bRet;
	}
	//otherwise we're trying to unequip something
	else
	{
		bRet = TryToUnEquip(MyPawn, theGrid, index);
		DrawInventory();
		return bRet;
	}

	return true;
}

bool AOrionInventoryManager::TryToEquip(AOrionCharacter *MyPawn, AOrionInventoryGrid *theGrid, int32 index)
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

bool AOrionInventoryManager::TryToUnEquip(AOrionCharacter *MyPawn, AOrionInventoryGrid *theGrid, int32 index)
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
			aPawn->EquipArmor(Armor);
		else
			aPawn->UnEquipArmor(ITEM_HELMET);
		if(SlotType !=ITEM_ANY)
			break;

	case ITEM_CHEST:
		Armor = Cast<AOrionArmor>(BodySlot->Inventory[0]);
		if (Armor)
			aPawn->EquipArmor(Armor);
		else
			aPawn->UnEquipArmor(ITEM_CHEST);
		if (SlotType != ITEM_ANY)
			break;

	case ITEM_HANDS:
		Armor = Cast<AOrionArmor>(HandsSlot->Inventory[0]);
		if (Armor)
			aPawn->EquipArmor(Armor);
		else
			aPawn->UnEquipArmor(ITEM_HANDS);
		if (SlotType != ITEM_ANY)
			break;

	case ITEM_LEGS:
		Armor = Cast<AOrionArmor>(LegsSlot->Inventory[0]);
		if (Armor)
			aPawn->EquipArmor(Armor);
		else
			aPawn->UnEquipArmor(ITEM_LEGS);
		if (SlotType != ITEM_ANY)
			break;

	//disable until finalized and default weapons are in
	/*case ITEM_PRIMARYWEAPON:
		Weapon = Cast<AOrionWeapon>(WeaponSlot1->Inventory[0]);
		OtherWeapon = aPawn->GetWeaponFromType(ITEM_PRIMARYWEAPON);

		if (OtherWeapon == nullptr || (Weapon && Weapon->InventoryType == OtherWeapon->InventoryType))
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
			break;*/
	}
}

void AOrionInventoryManager::GiveMoney(int32 Amount)
{
	if (Role == ROLE_Authority)
	{
		Money += Amount;
	}
}

void AOrionInventoryManager::Init(AOrionPlayerController *PC)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;
	SpawnInfo.Owner = PC;

	Grid = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (Grid)
		Grid->CreateInventory(10, 10, 96, 1, ITEM_ANY);

	HelmetSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (HelmetSlot)
		HelmetSlot->CreateInventory(1, 1, 96, 1, ITEM_HELMET);

	HandsSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (HandsSlot)
		HandsSlot->CreateInventory(1, 1, 96, 1, ITEM_HANDS);

	BodySlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (BodySlot)
		BodySlot->CreateInventory(1, 1, 96, 1, ITEM_CHEST);

	LegsSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (LegsSlot)
		LegsSlot->CreateInventory(1, 1, 96, 1, ITEM_LEGS);

	WeaponSlot1 = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (WeaponSlot1)
		WeaponSlot1->CreateInventory(1, 1, 96, 1, ITEM_PRIMARYWEAPON);

	WeaponSlot2 = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (WeaponSlot2)
		WeaponSlot2->CreateInventory(1, 1, 96, 1, ITEM_SECONDARYWEAPON);

	RingSlot1 = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (RingSlot1)
		RingSlot1->CreateInventory(1, 1, 96, 1, ITEM_RING);

	RingSlot2 = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (RingSlot2)
		RingSlot2->CreateInventory(1, 1, 96, 1, ITEM_RING);

	ShieldSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (ShieldSlot)
		ShieldSlot->CreateInventory(1, 1, 96, 1, ITEM_SHIELD);

	NeckSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (NeckSlot)
		NeckSlot->CreateInventory(1, 1, 96, 1, ITEM_NECK);

	BeltSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (BeltSlot)
		BeltSlot->CreateInventory(1, 1, 96, 1, ITEM_BELT);

	GadgetSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
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
int32 AOrionInventoryManager::AddItemToInventory(AOrionInventoryGrid *theGrid, AOrionInventory* newItem, int32 Index)
{
	if (Index > 0)
	{
		if (theGrid->Inventory.Num() > Index && theGrid->Inventory[Index] == nullptr)
		{
			theGrid->Inventory[Index] = newItem;
			return Index;
		}
		else
		{
			newItem->Destroy();
			newItem = NULL;
			return -1;
		}
	}

	for (int32 i = 0; i < theGrid->Height; i++)
	{
		for (int32 j = 0; j < theGrid->Width; j++)
		{
			if (theGrid->Inventory[i*theGrid->Width + j] == NULL)
			{
				theGrid->Inventory[i*theGrid->Width + j] = newItem;

				//tell the client about this!
				//ClientRedraw(theGrid->InventoryType, i*theGrid->Width + j);
				DrawInventory();

				return i * theGrid->Width + j;
			}
		}
	}

	newItem->Destroy();
	newItem = NULL;
	return -1;
}

void AOrionInventoryManager::DrawInventory()
{
	if (OwnerController && OwnerController->IsLocalPlayerController())
		Cast<AOrionPlayerController>(OwnerController)->EventRedrawInventory();
}

void AOrionInventoryManager::ClientRedraw_Implementation(EItemType type, int32 index)
{
	if (GetOwner())
		Cast<AOrionPlayerController>(GetOwner())->EventRedrawInventory();
}

void AOrionInventoryManager::RemoveItemFromInventory(AOrionInventoryGrid *theGrid, int32 index)
{
	if (theGrid->Inventory.Num() > index)
	{
		theGrid->Inventory[index] = NULL;
	}
}

void AOrionInventoryManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AOrionInventoryManager, HandsSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, Grid, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, HelmetSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, BodySlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, LegsSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, WeaponSlot1, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, WeaponSlot2, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, RingSlot1, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, RingSlot2, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, ShieldSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, NeckSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, BeltSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, GadgetSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, PrimaryAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, SecondaryAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, Money, COND_OwnerOnly);
}
