

#include "Orion.h"
#include "OrionWeapon.h"
#include "OrionCharacter.h"
#include "OrionInventoryManager.h"
#include "OrionPickup.h"
#include "OrionInventoryArmor.h"

class AOrionGameMode;

AOrionInventoryManager::AOrionInventoryManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> WeaponPartsObject(TEXT("/Game/Pickups/Blueprints/BasePickup"));
	if (WeaponPartsObject.Object != NULL)
	{
		DefaultWeaponPartsClass = (UClass*)WeaponPartsObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> ArmorPartsObject(TEXT("/Game/Pickups/Blueprints/BasePickup"));
	if (ArmorPartsObject.Object != NULL)
	{
		DefaultArmorPartsClass = (UClass*)ArmorPartsObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> CommonObject(TEXT("/Game/Pickups/Blueprints/BasePickup"));
	if (CommonObject.Object != NULL)
	{
		DefaultCommonClass = (UClass*)CommonObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> EnhancedObject(TEXT("/Game/Pickups/Blueprints/BasePickup"));
	if (EnhancedObject.Object != NULL)
	{
		DefaultEnhancedClass = (UClass*)EnhancedObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> SuperEnhancedObject(TEXT("/Game/Pickups/Blueprints/BasePickup"));
	if (SuperEnhancedObject.Object != NULL)
	{
		DefaultSuperEnhancedClass = (UClass*)SuperEnhancedObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> RareObject(TEXT("/Game/Pickups/Blueprints/BasePickup"));
	if (RareObject.Object != NULL)
	{
		DefaultRareClass = (UClass*)RareObject.Object->GeneratedClass;
	}

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

	if (theGrid2->InventoryType == ITEM_ANY || theGrid1->Inventory[index1].ItemClass == nullptr || theGrid1->Inventory[index1].Slot == theGrid2->InventoryType)
	{
		if (theGrid1->InventoryType == ITEM_ANY || theGrid2->Inventory[index2].ItemClass == nullptr || theGrid2->Inventory[index2].Slot == theGrid1->InventoryType)
		{
			//if (theGrid1->InventoryType != theGrid2->InventoryType)
			//	return false;

			FInventoryItem tempInv = theGrid1->Inventory[index1];

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

FInventoryItem AOrionInventoryManager::GetItemAtSlot(EItemType Slot)
{
	switch (Slot)
	{
	case ITEM_HELMET:
		return GetItemAt(HelmetSlot, 0);
		break;
	case ITEM_CHEST:
		return GetItemAt(BodySlot, 0);
		break;
	case ITEM_HANDS:
		return GetItemAt(HandsSlot, 0);
		break;
	case ITEM_BELT:
		return GetItemAt(BeltSlot, 0);
		break;
	case ITEM_LEGS:
		return GetItemAt(LegsSlot, 0);
		break;
	case ITEM_BOOTS:
		return GetItemAt(BootsSlot, 0);
		break;
	case ITEM_PRIMARYWEAPON:
		return GetItemAt(WeaponSlot1, 0);
		break;
	case ITEM_SECONDARYWEAPON:
		return GetItemAt(WeaponSlot2, 0);
		break;
	case ITEM_GADGET:
		return GetItemAt(GadgetSlot, 0);
		break;
	case ITEM_ABILITY:
		return GetItemAt(AbilitySlot, 0);
		break;
	case ITEM_SHADER:
		return GetItemAt(ShaderSlot, 0);
		break;
	case ITEM_DISPLAYARMOR:
		return GetItemAt(DisplaySlot, 0);
		break;
	case ITEM_GRENADE:
		return GetItemAt(GrenadeSlot, 0);
		break;
	}

	FInventoryItem Dummy;
	Dummy.Reset();

	return Dummy;
}

FInventoryItem AOrionInventoryManager::GetItemAt(AOrionInventoryGrid *theGrid, int32 index)
{
	if (theGrid && theGrid->Inventory.Num() > index)
		return theGrid->Inventory[index];

	FInventoryItem Dummy;
	Dummy.Reset();

	return Dummy;
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

	if (theGrid == nullptr)// || theGrid == WeaponSlot1 || theGrid == WeaponSlot2)
		return false;

	//make sure it's a valid index
	if (theGrid->Inventory.Num() <= index)
		return false;

	FInventoryItem Inv = theGrid->Inventory[index];

	if (!OwnerController)
		OwnerController = Cast<AOrionPlayerController>(GetOwner());

	//ignore clicks on null inventories
	if (!Inv.ItemClass || !OwnerController)
		return false;

	//only allow this when we have a valid pawn?
	//AOrionCharacter *MyPawn = Cast<AOrionCharacter>(OwnerController->GetPawn());

	//TODO: allow the player to change gear while respawning
	//if (!MyPawn)
	//	return false;

	bool bRet = false;
	//right clicking from the main inventory means we're trying to equip something, or trying to use a consumeable
	if (theGrid == Grid)
	{
		bRet = TryToEquip(theGrid, index);
		DrawInventory();
		return bRet;
	}
	//otherwise we're trying to unequip something
	else
	{
		bRet = TryToUnEquip(theGrid, index);
		DrawInventory();
		return bRet;
	}

	return true;
}

bool AOrionInventoryManager::TryToEquip(AOrionInventoryGrid *theGrid, int32 index)
{
	FInventoryItem Inv = theGrid->Inventory[index];

	if (Inv.ItemClass)
	{
		//first make sure we are able to wear this piece of gear
		//if (MyPawn->Level < Inv->RequiredLevel)
		//{
		//	return false;
		//}

		switch (Inv.Slot)
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
		case ITEM_BELT:
			return SwapItems(theGrid, index, BeltSlot, 0);
			break;
		case ITEM_BOOTS:
			return SwapItems(theGrid, index, BootsSlot, 0);
			break;
		case ITEM_PRIMARYWEAPON:
			return SwapItems(theGrid, index, WeaponSlot1, 0);
			break;
		case ITEM_SECONDARYWEAPON:
			return SwapItems(theGrid, index, WeaponSlot2, 0);
			break;
		case ITEM_GADGET:
			return SwapItems(theGrid, index, GadgetSlot, 0);
			break;
		case ITEM_ABILITY:
			return SwapItems(theGrid, index, AbilitySlot, 0);
			break;
		case ITEM_SHADER:
			return SwapItems(theGrid, index, ShaderSlot, 0);
			break;
		case ITEM_DISPLAYARMOR:
			return SwapItems(theGrid, index, DisplaySlot, 0);
			break;
		case ITEM_GRENADE:
			return SwapItems(theGrid, index, GrenadeSlot, 0);
			break;
		default:
			break;
		};
	}

	return false;
}

bool AOrionInventoryManager::TryToUnEquip(AOrionInventoryGrid *theGrid, int32 index)
{
	FInventoryItem Inv = theGrid->Inventory[index];

	if (Inv.ItemClass)
	{
		//make sure we have an empty inventory slot for this item
		bool bSpotOpen = false;
		int32 SlotIndex = 0;
		for (int32 i = 0; i < Grid->Inventory.Num() && !bSpotOpen; i++)
		{
			if (Grid->Inventory[i].ItemClass == nullptr)
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
	/*AOrionArmor *Armor = nullptr;
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
	}*/
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
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.Owner = PC;

	Grid = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (Grid)
		Grid->CreateInventory(10, 10, 64, 1, ITEM_ANY);

	HelmetSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (HelmetSlot)
		HelmetSlot->CreateInventory(1, 1, 64, 1, ITEM_HELMET);

	HandsSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (HandsSlot)
		HandsSlot->CreateInventory(1, 1, 64, 1, ITEM_HANDS);

	BodySlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (BodySlot)
		BodySlot->CreateInventory(1, 1, 64, 1, ITEM_CHEST);

	LegsSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (LegsSlot)
		LegsSlot->CreateInventory(1, 1, 64, 1, ITEM_LEGS);

	WeaponSlot1 = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (WeaponSlot1)
		WeaponSlot1->CreateInventory(1, 1, 64, 1, ITEM_PRIMARYWEAPON);

	WeaponSlot2 = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (WeaponSlot2)
		WeaponSlot2->CreateInventory(1, 1, 64, 1, ITEM_SECONDARYWEAPON);

	BeltSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (BeltSlot)
		BeltSlot->CreateInventory(1, 1, 64, 1, ITEM_BELT);

	GadgetSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (GadgetSlot)
		GadgetSlot->CreateInventory(1, 1, 64, 1, ITEM_GADGET);

	ShaderSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (ShaderSlot)
		ShaderSlot->CreateInventory(1, 1, 64, 1, ITEM_SHADER);

	DisplaySlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (DisplaySlot)
		DisplaySlot->CreateInventory(1, 1, 64, 1, ITEM_DISPLAYARMOR);

	AbilitySlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (AbilitySlot)
		AbilitySlot->CreateInventory(1, 1, 64, 1, ITEM_ABILITY);

	BootsSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (BootsSlot)
		BootsSlot->CreateInventory(1, 1, 64, 1, ITEM_BOOTS);

	GrenadeSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (GrenadeSlot)
		GrenadeSlot->CreateInventory(1, 1, 64, 1, ITEM_GRENADE);
}

void AOrionInventoryManager::DestroyInventory()
{
	for (int32 i = 0; i < Grid->Height; i++)
	{
		for (int32 j = 0; j < Grid->Width; j++)
		{
			if (Grid->Inventory[i*Grid->Width + j].ItemClass)
			{
				Grid->Inventory[i*Grid->Width + j].Reset();
			}
		}
	}

	if (HelmetSlot->Inventory[0].ItemClass)
	{
		HelmetSlot->Inventory[0].Reset();
	}

	if (BodySlot->Inventory[0].ItemClass)
	{
		BodySlot->Inventory[0].Reset();
	}

	if (HandsSlot->Inventory[0].ItemClass)
	{
		HandsSlot->Inventory[0].Reset();
	}

	if (LegsSlot->Inventory[0].ItemClass)
	{
		LegsSlot->Inventory[0].Reset();
	}
}

//returns index we were added at, -1 means failure
int32 AOrionInventoryManager::AddItemToInventory(AOrionInventoryGrid *theGrid, FInventoryItem newItem, int32 Index)
{
	if (Index > 0)
	{
		if (theGrid->Inventory.Num() > Index && theGrid->Inventory[Index].ItemClass == nullptr)
		{
			theGrid->Inventory[Index] = newItem;
			return Index;
		}
		else
		{
			newItem.Reset();// ->Destroy();
			//newItem = NULL;
			return -1;
		}
	}

	//if we are stackable, try to find us a stack to join
	if (newItem.ItemClass.GetDefaultObject() && newItem.ItemClass.GetDefaultObject()->bStackable)
	{
		for (int32 i = 0; i < theGrid->Height; i++)
		{
			for (int32 j = 0; j < theGrid->Width; j++)
			{
				if (theGrid->Inventory[i*theGrid->Width + j].ItemClass == newItem.ItemClass)
				{
					if (theGrid->Inventory[i*theGrid->Width + j].Amount < 500)
					{
						theGrid->Inventory[i*theGrid->Width + j].Amount++;

						DrawInventory();
						return i * theGrid->Width + j;
					}
				}
			}
		}
	}

	for (int32 i = 0; i < theGrid->Height; i++)
	{
		for (int32 j = 0; j < theGrid->Width; j++)
		{
			if (theGrid->Inventory[i*theGrid->Width + j].ItemClass == NULL)
			{
				theGrid->Inventory[i*theGrid->Width + j] = newItem;

				//tell the client about this!
				//ClientRedraw(theGrid->InventoryType, i*theGrid->Width + j);
				DrawInventory();

				return i * theGrid->Width + j;
			}
		}
	}

	//newItem->Destroy();
	//newItem = NULL;
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
		theGrid->Inventory[index].Reset();
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
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, BeltSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, GadgetSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, ShaderSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, DisplaySlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, AbilitySlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, BootsSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, GrenadeSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, Money, COND_OwnerOnly);
}

bool AOrionInventoryManager::HasStat(ESuperRareStat Stat)
{
	for (int32 i = 0; i < EquippedStats.LegendaryStats.Num(); i++)
	{
		if (EquippedStats.LegendaryStats[i] == Stat)
			return true;
	}

	return false;
}

TArray<FStatHelper> AOrionInventoryManager::GetChangedStats(FInventoryItem InventoryItem, FInventoryItem EquippedItem)
{
	TArray<FStatHelper> Stats;

	FStatHelper MainStat;

	MainStat.bMainStat = true;
	MainStat.bInvert = false;
	MainStat.bPrimary = false;
	MainStat.Value = InventoryItem.MainStat - EquippedItem.MainStat;

	Stats.Add(MainStat);

	for (int32 i = 0; i < InventoryItem.PrimaryStats.Num(); i++)
	{
		FStatHelper Helper;
		bool bFound = false;

		for (int32 j = 0; j < EquippedItem.PrimaryStats.Num() && !bFound; j++)
		{
			if (InventoryItem.PrimaryStats[i].StatType == EquippedItem.PrimaryStats[j].StatType)
			{
				Helper.bPrimary = true;
				Helper.bInvert = false;
				Helper.PrimaryStat = InventoryItem.PrimaryStats[i].StatType;
				Helper.Value = InventoryItem.PrimaryStats[i].Value - EquippedItem.PrimaryStats[j].Value;

				if (Helper.Value != 0.0f) Stats.AddUnique(Helper);

				bFound = true;

				break;
			}
		}

		if (!bFound)
		{
			Helper.bPrimary = true;
			Helper.bInvert = false;
			Helper.PrimaryStat = InventoryItem.PrimaryStats[i].StatType;
			Helper.Value = InventoryItem.PrimaryStats[i].Value;

			if (Helper.Value != 0.0f) Stats.AddUnique(Helper);
		}
	}

	for (int32 i = 0; i < EquippedItem.PrimaryStats.Num(); i++)
	{
		FStatHelper Helper;
		bool bFound = false;

		for (int32 j = 0; j < InventoryItem.PrimaryStats.Num() && !bFound; j++)
		{
			if (InventoryItem.PrimaryStats[j].StatType == EquippedItem.PrimaryStats[i].StatType)
			{
				Helper.bPrimary = true;
				Helper.bInvert = true;
				Helper.PrimaryStat = EquippedItem.PrimaryStats[i].StatType;
				Helper.Value = EquippedItem.PrimaryStats[i].Value - InventoryItem.PrimaryStats[j].Value;

				if (Helper.Value != 0.0f) Stats.AddUnique(Helper);

				bFound = true;

				break;
			}
		}

		if (!bFound)
		{
			Helper.bPrimary = true;
			Helper.bInvert = true;
			Helper.PrimaryStat = EquippedItem.PrimaryStats[i].StatType;
			Helper.Value = EquippedItem.PrimaryStats[i].Value;

			if (Helper.Value != 0.0f) Stats.AddUnique(Helper);
		}
	}

	for (int32 i = 0; i < InventoryItem.SecondaryStats.Num(); i++)
	{
		FStatHelper Helper;
		bool bFound = false;

		for (int32 j = 0; j < EquippedItem.SecondaryStats.Num() && !bFound; j++)
		{
			if (InventoryItem.SecondaryStats[i].StatType == EquippedItem.SecondaryStats[j].StatType)
			{
				Helper.bPrimary = false;
				Helper.bInvert = false;
				Helper.SecondaryStat = InventoryItem.SecondaryStats[i].StatType;
				Helper.Value = InventoryItem.SecondaryStats[i].Value - EquippedItem.SecondaryStats[j].Value;

				if (Helper.Value != 0.0f) Stats.AddUnique(Helper);

				bFound = true;

				break;
			}
		}

		if (!bFound)
		{
			Helper.bPrimary = false;
			Helper.bInvert = false;
			Helper.SecondaryStat = InventoryItem.SecondaryStats[i].StatType;
			Helper.Value = InventoryItem.SecondaryStats[i].Value;

			if (Helper.Value != 0.0f) Stats.AddUnique(Helper);
		}
	}

	for (int32 i = 0; i < EquippedItem.SecondaryStats.Num(); i++)
	{
		FStatHelper Helper;
		bool bFound = false;

		for (int32 j = 0; j < InventoryItem.SecondaryStats.Num() && !bFound; j++)
		{
			if (InventoryItem.SecondaryStats[j].StatType == EquippedItem.SecondaryStats[i].StatType)
			{
				Helper.bPrimary = false;
				Helper.bInvert = true;
				Helper.SecondaryStat = EquippedItem.SecondaryStats[i].StatType;
				Helper.Value = EquippedItem.SecondaryStats[i].Value - InventoryItem.SecondaryStats[j].Value;

				if (Helper.Value != 0.0f) Stats.AddUnique(Helper);

				bFound = true;

				break;
			}
		}

		if (!bFound)
		{
			Helper.bPrimary = false;
			Helper.bInvert = true;
			Helper.SecondaryStat = EquippedItem.SecondaryStats[i].StatType;
			Helper.Value = EquippedItem.SecondaryStats[i].Value;

			if (Helper.Value != 0.0f) Stats.AddUnique(Helper);
		}
	}

	return Stats;
}

TArray<FCharacterStatEntry> AOrionInventoryManager::GetEquippedStats()
{
	TArray<FCharacterStatEntry> Stats;

	EquippedStats.Reset();

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetOwner());

	if (!PC)
		return Stats;

	//cycle through each slot and add up all the stats from gear
	GetStatsFromSlot(HelmetSlot, EquippedStats);
	GetStatsFromSlot(BodySlot, EquippedStats);
	GetStatsFromSlot(HandsSlot, EquippedStats);
	GetStatsFromSlot(BeltSlot, EquippedStats);
	GetStatsFromSlot(LegsSlot, EquippedStats);
	GetStatsFromSlot(BootsSlot, EquippedStats);
	GetStatsFromSlot(WeaponSlot1, EquippedStats);
	GetStatsFromSlot(WeaponSlot2, EquippedStats);
	GetStatsFromSlot(GadgetSlot, EquippedStats);
	GetStatsFromSlot(GrenadeSlot, EquippedStats);
	GetStatsFromSlot(AbilitySlot, EquippedStats);

	//fill in the stats
	FCharacterStatEntry Entry;

	Entry.StatName = "DEFENSE"; Entry.Value = EquippedStats.Defense; Stats.Add(Entry); Entry.Desc = ""; Entry.DescValue = 0.0f;

	Entry.StatName = "STRENGTH"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_STRENGTH]; Entry.Desc = "x8x% stronger melee"; Entry.DescValue = 0.1f; Stats.Add(Entry);
	Entry.StatName = "DEXTERITY"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_DEXTERITY]; Entry.Desc = "x8x% faster grenade charge"; Entry.DescValue = 0.1f; Stats.Add(Entry);
	Entry.StatName = "INTELLIGENCE"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_INTELLIGENCE]; Entry.Desc = "x8x% faster energy charge"; Entry.DescValue = 0.1f; Stats.Add(Entry);
	Entry.StatName = "DISCIPLINE"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_DISCIPLINE]; Entry.Desc = "x8x extra shields"; Entry.DescValue = 1.0f; Stats.Add(Entry);
	Entry.StatName = "VITALITY"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_VITALITY]; Entry.Desc = "x8x extra health"; Entry.DescValue = 1.0f; Stats.Add(Entry);
	Entry.Desc = ""; Entry.DescValue = 0.0f;
	Entry.StatName = "CRITICAL HIT CHANCE"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_CRITICALHITCHANCE] + HasStat(RARESTAT_BONUSCRITCHANCE) ? 15.0f: 0.0f; Stats.Add(Entry); Stats.Add(Entry);
	Entry.StatName = "CRITICAL DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_CRITICALHITMULTIPLIER] + HasStat(RARESTAT_BONUSCRITDAMAGE) ? 50.0f: 0.0f; Stats.Add(Entry);
	Entry.StatName = "DAMAGE REDUCTION"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTION]; Stats.Add(Entry);
	Entry.StatName = "BLUNT DAMAGE REDUCTION"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONBLUNT]; Stats.Add(Entry);
	Entry.StatName = "PIERCING DAMAGE REDUCTION"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONPIERCING]; Stats.Add(Entry);
	Entry.StatName = "EXPLOSIVE DAMAGE REDUCTION"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONEXPLOSIVE]; Stats.Add(Entry);
	Entry.StatName = "ELEMENTAL DAMAGE REDUCTION"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONELEMENTAL]; Stats.Add(Entry);
	Entry.StatName = "POISON DAMAGE REDUCTION"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONPOISON]; Stats.Add(Entry);
	Entry.StatName = "BONUS XP PER KILL"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_EXPBOOST]; Stats.Add(Entry);
	Entry.StatName = "FIRE DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_FIREDAMAGE]; Stats.Add(Entry);
	Entry.StatName = "LIGHTNING DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_LIGHTNINGDAMAGE]; Stats.Add(Entry);
	Entry.StatName = "ICE DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_ICEDAMAGE]; Stats.Add(Entry);
	Entry.StatName = "POISON DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_POISONDAMAGE]; Stats.Add(Entry);
	Entry.StatName = "CORROSIVE DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_CORROSIVEDAMAGE]; Stats.Add(Entry);
	Entry.StatName = "BONUS GOLD"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_GOLDFIND] + HasStat(RARESTAT_GOLDFIND) ? 100.0f: 0.0f; Stats.Add(Entry);
	Entry.StatName = "BONUS ITEMS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_MAGICFIND] + HasStat(RARESTAT_MAGICFIND) ? 100.0f: 0.0f; Stats.Add(Entry);
	Entry.StatName = "LARGE DINO DAMAGE"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_LARGEDINOBOOST]; Stats.Add(Entry);
	Entry.StatName = "ROBOT DAMAGE"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_ROBOTBOOST]; Stats.Add(Entry);

	return Stats;
}

void AOrionInventoryManager::GetStatsFromSlot(AOrionInventoryGrid *Slot, FArrayHelper &Stats)
{
	FInventoryItem Item = GetItemAt(Slot, 0);
	if (Item.ItemClass)
	{
		for (int32 i = 0; i < Item.PrimaryStats.Num(); i++)
		{
			Stats.Primary[Item.PrimaryStats[i].StatType] += Item.PrimaryStats[i].Value;
		}
		for (int32 i = 0; i < Item.SecondaryStats.Num(); i++)
		{
			Stats.Secondary[Item.SecondaryStats[i].StatType] += Item.SecondaryStats[i].Value;
		}
		for (int32 i = 0; i < Item.RareStats.Num(); i++)
		{
			Stats.LegendaryStats.Add(Item.RareStats[0].Index);
		}

		switch (Item.Slot)
		{
		case ITEM_HELMET:
		case ITEM_CHEST:
		case ITEM_HANDS:
		case ITEM_LEGS:
		case ITEM_BELT:
		case ITEM_BOOTS:
		case ITEM_ABILITY:
		//case ITEM_GRENADE:
			Stats.Defense += Item.MainStat;
			break;
		}
	}
}

bool AOrionInventoryManager::CraftItem(FInventoryItem ItemToCraft, TArray<FInventoryHelper> ItemsToRemove)
{
	if (!Grid)
		return false;

	//first make sure that there is room in the player's inventory for this item
	int32 NumOpenSlots = 0;

	for (int32 i = 0; i < Grid->Inventory.Num(); i++)
	{
		if (Grid->Inventory[i].ItemClass == nullptr)
			NumOpenSlots++;
	}

	if (NumOpenSlots < 1)
		return false;

	//subtract the items from the user's inventory
	for (int32 i = 0; i < ItemsToRemove.Num(); i++)
		RemoveItemFromInventory(ItemsToRemove[i].Grid, ItemsToRemove[i].Index);

	//add the crafted item to our inventory
	return AddItemToInventory(Grid, ItemToCraft, -1) >= 0;
}

bool AOrionInventoryManager::DropItem(AOrionInventoryGrid *theGrid, int32 index)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

	if (!Game || !OwnerController)
		return false;

	FVector pos;
	FRotator rot;

	AOrionCharacter *Pawn = Cast<AOrionCharacter>(OwnerController->GetPawn());

	if (Pawn)
		pos = Pawn->GetActorLocation() + 150.0f * Pawn->GetActorRotation().Vector();
	else
		OwnerController->GetPlayerViewPoint(pos, rot);

	bool bRet = false;

	//for (TActorIterator<AOrionPlayerController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	//{
	//	AOrionPlayerController *PC = *ActorItr;

	//	if (!PC)
	//		continue;

		AOrionPickup *Pickup = GetWorld()->SpawnActor<AOrionPickup>(Game->DefaultPickupClass, pos, FRotator::ZeroRotator, SpawnInfo);
		if (Pickup)
		{
			Pickup->SetOwner(OwnerController);
			Pickup->bOnlyRelevantToOwner = false;// true;
			Pickup->PickupMesh->bOnlyOwnerSee = false;//true;

			Pickup->bGlobalItem = true;

			FInventoryItem Item = GetItemAt(theGrid, index);

			Pickup->Decoder.ItemClass = Item.ItemClass;
			UOrionInventoryItem *Inv = Cast<UOrionInventoryItem>(Pickup->Decoder.ItemClass->GetDefaultObject());

			Pickup->Decoder.ItemPath = Inv->GetPathName();

			if (Inv == nullptr)
				return false;

			//pick which quality, also set the shape/color of the pickup here, also determines name?
			Pickup->Decoder.Rarity = Item.Rarity;
			Pickup->Decoder.ItemName = Item.ItemName;
			Pickup->Decoder.Slot = Item.Slot;
			Pickup->Decoder.ItemLevel = Item.ItemLevel;
			Pickup->Decoder.ItemDesc = Item.ItemDesc;
			Pickup->Decoder.PrimaryStats = Item.PrimaryStats;
			Pickup->Decoder.SecondaryStats = Item.SecondaryStats;
			Pickup->Decoder.RareStats = Item.RareStats;
			Pickup->Decoder.MainStat = Item.MainStat;

			Pickup->EventSetColor(Pickup->Decoder.Rarity);

			RemoveItemFromInventory(theGrid, index);

			bRet = true;
		}
	//}

	return bRet;
}

bool AOrionInventoryManager::SellItem(AOrionInventoryGrid *theGrid, int32 index)
{
	FInventoryItem Item = GetItemAt(theGrid, index);

	if (Item.ItemClass == nullptr)
		return false;

	//give the player some cash money
	GiveMoney(Item.SellValue);

	//remove item
	RemoveItemFromInventory(theGrid, index);

	return true;
}

bool AOrionInventoryManager::SortInventory()
{
	Grid->Inventory.Sort();

	return true;
}

void AOrionInventoryManager::ServerBreakdownItem_Implementation(AOrionInventoryGrid *theGrid, int32 index)
{
	BreakdownItem(theGrid, index);
}

bool AOrionInventoryManager::BreakdownItem(AOrionInventoryGrid *theGrid, int32 index)
{
	if (Role != ROLE_Authority)
	{
		ServerBreakdownItem(theGrid, index);
		return true;
	}

	FInventoryItem Item = GetItemAt(theGrid, index);

	if (Item.ItemClass == nullptr)
		return false;

	TArray< TSubclassOf<UOrionInventoryItem> > ItemsToGrant;
	TArray<EItemRarity> Rarities;
	TArray<int32> ItemLevels;

	//see if there is any special kind of breakdown item
	for (int32 i = 0; i < Item.BreakdownClasses.Num(); i++)
	{
		ItemsToGrant.Add(Item.BreakdownClasses[i]);
		Rarities.Add(RARITY_COMMON);
		ItemLevels.Add(1);
	}

	//give some breakdown items based on quality of item
	switch (Item.Rarity)
	{
	case RARITY_COMMON:
		ItemsToGrant.Add(DefaultCommonClass);
		break;
	case RARITY_ENHANCED:
		ItemsToGrant.Add(DefaultEnhancedClass);
		break;
	case RARITY_SUPERENHANCED:
		ItemsToGrant.Add(DefaultSuperEnhancedClass);
		break;
	case RARITY_LEGENDARY:
		ItemsToGrant.Add(DefaultRareClass);
		break;
	}

	Rarities.Add(Item.Rarity);
	ItemLevels.Add(Item.ItemLevel);

	int32 NumSpacesToFind = ItemsToGrant.Num();

	//see if we have enough room for the new stuff we're getting
	for (int32 i = 0; i < Grid->Inventory.Num() && NumSpacesToFind > 0; i++)
	{
		if (Grid->Inventory[i].ItemClass == nullptr)
			NumSpacesToFind--;
		else
		{
			for (int32 j = 0; j < ItemsToGrant.Num(); j++)
			{
				if (Grid->Inventory[i].ItemClass == ItemsToGrant[j])
				{
					if (Grid->Inventory[i].Amount < 500)
					{
						NumSpacesToFind--;
						break;
					}
				}
			}
		}
	}

	if (NumSpacesToFind <= 0)
	{
		//remove the breakdown item first!
		RemoveItemFromInventory(theGrid, index);

		for (int32 i = 0; i < ItemsToGrant.Num(); i++)
		{
			UOrionInventoryItem *pItem = ItemsToGrant[i].GetDefaultObject();

			if (!pItem)
				continue;

			FInventoryItem NewItem;

			NewItem.Amount = 1;
			NewItem.ItemClass = ItemsToGrant[i];
			NewItem.PrimaryStats.Empty();
			NewItem.SecondaryStats.Empty();
			NewItem.RareStats.Empty();
			NewItem.ItemName = pItem->ItemName;
			NewItem.ItemDesc = pItem->ItemDesc;
			NewItem.Rarity = Rarities[i];
			NewItem.Slot = pItem->ItemType;
			NewItem.ItemLevel = ItemLevels[i];
			NewItem.MainStat = 0;
			NewItem.SellValue = 10;
			NewItem.BreakdownClasses = pItem->BreakdownClass;

			if (AddItemToInventory(Grid, NewItem) < 0)
				return false;
		}

		return true;
	}

	return false;
}