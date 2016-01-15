

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
		Grid->CreateInventory(15, 5, 64, 1, ITEM_ANY);

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

TArray<FCharacterStatEntry> AOrionInventoryManager::GetEquippedStats()
{
	TArray<FCharacterStatEntry> Stats;

	EquippedStats.Reset();

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

	Entry.StatName = "DEFENSE"; Entry.Value = EquippedStats.Defense; Stats.Add(Entry);

	Entry.StatName = "STRENGTH"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_STRENGTH]; Stats.Add(Entry);
	Entry.StatName = "DEXTERITY"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_DEXTERITY]; Stats.Add(Entry);
	Entry.StatName = "INTELLIGENCE"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_INTELLIGENCE]; Stats.Add(Entry);
	Entry.StatName = "DISCIPLINE"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_DISCIPLINE]; Stats.Add(Entry);
	Entry.StatName = "VITALITY"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_VITALITY]; Stats.Add(Entry);
	Entry.StatName = "CRITICAL HIT CHANCE"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_CRITICALHITCHANCE]; Stats.Add(Entry);
	Entry.StatName = "CRITICAL DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_CRITICALHITMULTIPLIER]; Stats.Add(Entry);
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
	Entry.StatName = "BONUS GOLD"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_GOLDFIND]; Stats.Add(Entry);
	Entry.StatName = "BONUS ITEMS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_MAGICFIND]; Stats.Add(Entry);
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
