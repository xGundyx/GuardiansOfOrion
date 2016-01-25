

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
	static ConstructorHelpers::FObjectFinder<UBlueprint> WeaponPartsObject(TEXT("/Game/InventoryItems/Crafting/WeaponMaterial"));
	if (WeaponPartsObject.Object != NULL)
	{
		DefaultWeaponPartsClass = (UClass*)WeaponPartsObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> ArmorPartsObject(TEXT("/Game/InventoryItems/Crafting/ArmorMaterial"));
	if (ArmorPartsObject.Object != NULL)
	{
		DefaultArmorPartsClass = (UClass*)ArmorPartsObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> CommonObject(TEXT("/Game/InventoryItems/Crafting/CommonMaterial"));
	if (CommonObject.Object != NULL)
	{
		DefaultCommonClass = (UClass*)CommonObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> EnhancedObject(TEXT("/Game/InventoryItems/Crafting/EnhancedCraftingMat"));
	if (EnhancedObject.Object != NULL)
	{
		DefaultEnhancedClass = (UClass*)EnhancedObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> SuperEnhancedObject(TEXT("/Game/InventoryItems/Crafting/SuperEnhancedCraftingMat"));
	if (SuperEnhancedObject.Object != NULL)
	{
		DefaultSuperEnhancedClass = (UClass*)SuperEnhancedObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> RareObject(TEXT("/Game/InventoryItems/Crafting/LegendaryCraftingMat"));
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

			theGrid1->Inventory[index1].bDirty = true;
			theGrid2->Inventory[index2].bDirty = true;

			//check for any equipment changes
			EItemType Type = theGrid1->InventoryType;

			if (Type == ITEM_ANY)
				Type = theGrid2->InventoryType;

			if (Type != ITEM_ANY)
				EquipItems();

			AOrionPRI *PRI = Cast<AOrionPRI>(OwnerController->PlayerState);

			FString GridName1 = theGrid1->GetGridName();
			FString ClassName = "";

			if (PRI && theGrid1 != Grid)
				ClassName = PRI->ClassType;

			theGrid1->Inventory[index1].SlotIndex = FString::Printf(TEXT("%sy8y%sx8x%i"), *ClassName, *GridName1, index1);

			if (PRI && theGrid1 != Grid)
				ClassName = PRI->ClassType;
			else
				ClassName = "";

			FString GridName2 = theGrid2->GetGridName();
			theGrid2->Inventory[index2].SlotIndex = FString::Printf(TEXT("%sy8y%sx8x%i"), *ClassName, *GridName2, index2);

			SaveInventory();

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
	case ITEM_KNIFE:
		return GetItemAt(KnifeSlot, 0);
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
		case ITEM_KNIFE:
			return SwapItems(theGrid, index, KnifeSlot, 0);
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
//called when spawning, and when gear changes
void AOrionInventoryManager::EquipItems()
{
	UpdateEquippedStats();

	//update our max health and max shield based on equipment changes
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetOwner());

	if (!PC)
		return;

	PC->ILevel = FMath::Max(1, EquippedStats.ItemLevel / 12);
	PC->ClientSetItemLevel(PC->ILevel);

	AOrionCharacter *Pawn = Cast<AOrionCharacter>(PC->GetPawn());

	if (!Pawn)
		return;

	//primary stats
	float HealthRatio = Pawn->Health / Pawn->HealthMax;

	Pawn->HealthMax = 1000.0f + EquippedStats.Primary[PRIMARYSTAT_VITALITY] * 1.0f;
	Pawn->Health = Pawn->HealthMax * HealthRatio;

	float ShieldRatio = Pawn->Shield / Pawn->ShieldMax;

	Pawn->ShieldMax = 1000.0f + EquippedStats.Primary[PRIMARYSTAT_DISCIPLINE] * 1.0f;
	Pawn->Shield = Pawn->ShieldMax * ShieldRatio;

	Pawn->MeleeDamageBoost = 1.0f + EquippedStats.Primary[PRIMARYSTAT_STRENGTH] * 0.1f;
	Pawn->GrenadeRechargeRate = 1.0f + EquippedStats.Primary[PRIMARYSTAT_DEXTERITY] * 0.1f;
	Pawn->SkillRechargeRate = 1.0f + EquippedStats.Primary[PRIMARYSTAT_INTELLIGENCE] * 0.1f;

	//secondary stats
	Pawn->CriticalHitChance = 0.0f + EquippedStats.Secondary[SECONDARYSTAT_CRITICALHITCHANCE] + (HasStat(RARESTAT_BONUSCRITCHANCE) ? 15.0f : 0.0f);
	Pawn->CriticalHitMultiplier = 0.0f + EquippedStats.Secondary[SECONDARYSTAT_CRITICALHITMULTIPLIER] + (HasStat(RARESTAT_BONUSCRITDAMAGE) ? 50.0f : 0.0f);
	Pawn->DamageReduction = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTION]; 
	Pawn->BluntDamageReduction = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONBLUNT]; 
	Pawn->PiercingDamageReduction = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONPIERCING]; 
	Pawn->ExplosiveDamageReduction = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONEXPLOSIVE]; 
	Pawn->ElementalDamageReduction = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONELEMENTAL]; 
	Pawn->PoisonDamageReduction = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONPOISON]; 
	Pawn->XPPerKill = EquippedStats.Secondary[SECONDARYSTAT_EXPBOOST]; 
	Pawn->FireDamage = EquippedStats.Secondary[SECONDARYSTAT_FIREDAMAGE]; //
	Pawn->LightningDamage = EquippedStats.Secondary[SECONDARYSTAT_LIGHTNINGDAMAGE]; //
	Pawn->IceDamage = EquippedStats.Secondary[SECONDARYSTAT_ICEDAMAGE]; //
	Pawn->PoisonDamage = EquippedStats.Secondary[SECONDARYSTAT_POISONDAMAGE]; //
	Pawn->CorrosiveDamage = EquippedStats.Secondary[SECONDARYSTAT_CORROSIVEDAMAGE]; //
	Pawn->GoldFind = EquippedStats.Secondary[SECONDARYSTAT_GOLDFIND] + (HasStat(RARESTAT_GOLDFIND) ? 100.0f : 0.0f); //
	Pawn->MagicFind = EquippedStats.Secondary[SECONDARYSTAT_MAGICFIND] + (HasStat(RARESTAT_MAGICFIND) ? 100.0f : 0.0f); 
	Pawn->LargeDinoBoost = EquippedStats.Secondary[SECONDARYSTAT_LARGEDINOBOOST]; 
	Pawn->RobotBoost = EquippedStats.Secondary[SECONDARYSTAT_ROBOTBOOST];

	Pawn->Level = FMath::Max(1, EquippedStats.ItemLevel / 12);
}

int32 AOrionInventoryManager::GetPrimaryWeaponDamage()
{
	if (WeaponSlot1->Inventory[0].ItemClass)
	{
		return WeaponSlot1->Inventory[0].MainStat;
	}

	return 1;
}

int32 AOrionInventoryManager::GetSecondaryWeaponDamage()
{
	if (WeaponSlot2->Inventory[0].ItemClass)
	{
		return WeaponSlot2->Inventory[0].MainStat;
	}

	return 1;
}

void AOrionInventoryManager::GiveMoney(int32 Amount)
{
	if (Role == ROLE_Authority)
	{
		Money += Amount;

		if (Cast<AOrionPlayerController>(OwnerController))
		{
			Cast<AOrionPlayerController>(OwnerController)->AddCoinAmount(Amount);
		}
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

	KnifeSlot = GetWorld()->SpawnActor<AOrionInventoryGrid>(AOrionInventoryGrid::StaticClass(), SpawnInfo);
	if (KnifeSlot)
		KnifeSlot->CreateInventory(1, 1, 64, 1, ITEM_KNIFE);

#if IS_SERVER
	EventReadInventoryFromPlayfab();
#endif
}

void AOrionInventoryManager::RemoveItemFromInventory(AOrionInventoryGrid *theGrid, int32 Index)
{
	if (!theGrid)
		return;

	if (theGrid->Inventory.Num() < Index)
		return;

	FInventoryItem Item = GetItemAt(theGrid, Index);

	if (Item.ItemClass)
	{
		//tell playfab to consume a use of this item and therefore remove it from our inventory
		EventConsumeItem(Item);
	}
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

void AOrionInventoryManager::SaveInventory()
{
	if (bSaveInventoryOperationInProgress)
		GetWorldTimerManager().SetTimer(SaveInventoryTimer, this, &AOrionInventoryManager::SaveInventory, 0.1f, false);
	else
	{
#if IS_SERVER
		EventSaveInventoryToPlayfab();
#endif
		for (int32 i = 0; i < Grid->Inventory.Num(); i++)
		{
			Grid->Inventory[i].bDirty = false;
		}

		HelmetSlot->Inventory[0].bDirty = false;
		BodySlot->Inventory[0].bDirty = false;
		HandsSlot->Inventory[0].bDirty = false;
		BeltSlot->Inventory[0].bDirty = false;
		LegsSlot->Inventory[0].bDirty = false;
		BootsSlot->Inventory[0].bDirty = false;
		WeaponSlot1->Inventory[0].bDirty = false;
		WeaponSlot2->Inventory[0].bDirty = false;
		GadgetSlot->Inventory[0].bDirty = false;
		AbilitySlot->Inventory[0].bDirty = false;
		ShaderSlot->Inventory[0].bDirty = false;
		DisplaySlot->Inventory[0].bDirty = false;
		GrenadeSlot->Inventory[0].bDirty = false;
		KnifeSlot->Inventory[0].bDirty = false;
	}
}

//returns index we were added at, -1 means failure
int32 AOrionInventoryManager::AddItemToInventory(AOrionInventoryGrid *theGrid, FInventoryItem newItem, int32 Index)
{
	/*if (Index > 0)
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
	}*/

	int32 Success = -1;

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
						theGrid->Inventory[i*theGrid->Width + j].bDirty = true;
#if IS_SERVER
						EventUpdateItemStats(theGrid->Inventory[i*theGrid->Width + j]);
#endif
						DrawInventory();
						return i * theGrid->Width + j;
					}
				}
			}
		}
	}

	for (int32 i = 0; i < theGrid->Height && Success < 0; i++)
	{
		for (int32 j = 0; j < theGrid->Width && Success < 0; j++)
		{
			if (theGrid->Inventory[i*theGrid->Width + j].ItemClass == NULL)
			{
				theGrid->Inventory[i*theGrid->Width + j] = newItem;

				//tell the client about this!
				//ClientRedraw(theGrid->InventoryType, i*theGrid->Width + j);
				DrawInventory();

				Success = i * theGrid->Width + j;
			}
		}
	}

	static int32 UniqueID = 1;

	if (Success >= 0)
	{
		theGrid->Inventory[Success].TempUniqueID = UniqueID;
		UniqueID++;

		theGrid->Inventory[Success].bDirty = true;

		FString GridName = theGrid->GetGridName();

		AOrionPRI *PRI = Cast<AOrionPRI>(OwnerController->PlayerState);
		FString ClassName = "";

		if (PRI && theGrid != Grid)
			ClassName == PRI->ClassType;

		theGrid->Inventory[Success].SlotIndex = FString::Printf(TEXT("%sy8y%sx8x%i"), *ClassName, *GridName, Success);

		GrantItemQueue.Add(theGrid->Inventory[Success]);

		GetWorldTimerManager().SetTimer(GrantItemTimer, this, &AOrionInventoryManager::HandleGrantItemQueue, 0.001f, false);
	}

	//newItem->Destroy();
	//newItem = NULL;
	return Success;
}

FString AOrionInventoryManager::GetPathToClass(FInventoryItem Item)
{
	if (Item.ItemClass)
		return Item.ItemClass->GetPathName(nullptr);
	else
		return "ERROR";
}

TArray<FInventoryItem> AOrionInventoryManager::GetFullInventoryList()
{
	TArray<FInventoryItem> Items;

	for (int32 i = 0; i < Grid->Inventory.Num(); i++)
	{
		if (Grid->Inventory[i].ItemClass && Grid->Inventory[i].bDirty)
			Items.Add(Grid->Inventory[i]);
	}

	if (HelmetSlot->Inventory[0].ItemClass && HelmetSlot->Inventory[0].bDirty) Items.Add(HelmetSlot->Inventory[0]);
	if (BodySlot->Inventory[0].ItemClass && BodySlot->Inventory[0].bDirty) Items.Add(BodySlot->Inventory[0]);
	if (HandsSlot->Inventory[0].ItemClass && HandsSlot->Inventory[0].bDirty) Items.Add(HandsSlot->Inventory[0]);
	if (BeltSlot->Inventory[0].ItemClass && BeltSlot->Inventory[0].bDirty) Items.Add(BeltSlot->Inventory[0]);
	if (LegsSlot->Inventory[0].ItemClass && LegsSlot->Inventory[0].bDirty) Items.Add(LegsSlot->Inventory[0]);
	if (BootsSlot->Inventory[0].ItemClass && BootsSlot->Inventory[0].bDirty) Items.Add(BootsSlot->Inventory[0]);
	if (WeaponSlot1->Inventory[0].ItemClass && WeaponSlot1->Inventory[0].bDirty) Items.Add(WeaponSlot1->Inventory[0]);
	if (WeaponSlot2->Inventory[0].ItemClass && WeaponSlot2->Inventory[0].bDirty) Items.Add(WeaponSlot2->Inventory[0]);
	if (GadgetSlot->Inventory[0].ItemClass && GadgetSlot->Inventory[0].bDirty) Items.Add(GadgetSlot->Inventory[0]);
	if (AbilitySlot->Inventory[0].ItemClass && AbilitySlot->Inventory[0].bDirty) Items.Add(AbilitySlot->Inventory[0]);
	if (ShaderSlot->Inventory[0].ItemClass && ShaderSlot->Inventory[0].bDirty) Items.Add(ShaderSlot->Inventory[0]);
	if (DisplaySlot->Inventory[0].ItemClass && DisplaySlot->Inventory[0].bDirty) Items.Add(DisplaySlot->Inventory[0]);
	if (GrenadeSlot->Inventory[0].ItemClass && GrenadeSlot->Inventory[0].bDirty) Items.Add(GrenadeSlot->Inventory[0]);
	if (KnifeSlot->Inventory[0].ItemClass && KnifeSlot->Inventory[0].bDirty) Items.Add(KnifeSlot->Inventory[0]);

	return Items;
}

AOrionInventoryGrid *AOrionInventoryManager::GetGridFromSlot(EItemType Slot)
{
	switch (Slot)
	{
	case ITEM_HELMET:
		return HelmetSlot;
		break;
	case ITEM_CHEST:
		return BodySlot;
		break;
	case ITEM_HANDS:
		return HandsSlot;
		break;
	case ITEM_BELT:
		return BeltSlot;
		break;
	case ITEM_LEGS:
		return LegsSlot;
		break;
	case ITEM_BOOTS:
		return BootsSlot;
		break;
	case ITEM_PRIMARYWEAPON:
		return WeaponSlot1;
		break;
	case ITEM_SECONDARYWEAPON:
		return WeaponSlot2;
		break;
	case ITEM_GADGET:
		return GadgetSlot;
		break;
	case ITEM_ABILITY:
		return AbilitySlot;
		break;
	case ITEM_SHADER:
		return ShaderSlot;
		break;
	case ITEM_DISPLAYARMOR:
		return DisplaySlot;
		break;
	case ITEM_GRENADE:
		return GrenadeSlot;
		break;
	case ITEM_KNIFE:
		return KnifeSlot;
		break;
	}

	return nullptr;
}

AOrionInventoryGrid *AOrionInventoryManager::GetGridFromName(FString Slot)
{
	if (Slot == "HELMET")
		return HelmetSlot;
	else if (Slot == "BODY")
		return BodySlot;
	else if (Slot == "HANDS")
		return HandsSlot;
	else if (Slot == "BELT")
		return BeltSlot;
	else if (Slot == "LEGS")
		return LegsSlot;
	else if (Slot == "BOOTS")
		return BootsSlot;
	else if (Slot == "PRIMARY")
		return WeaponSlot1;
	else if (Slot == "SECONDARY")
		return WeaponSlot2;
	else if (Slot == "GADGET")
		return GadgetSlot;
	else if (Slot == "ABILITY")
		return AbilitySlot;
	else if (Slot == "SHADER")
		return ShaderSlot;
	else if (Slot == "DISPLAY")
		return DisplaySlot;
	else if (Slot == "GRENADE")
		return GrenadeSlot;
	else if (Slot == "KNIFE")
		return KnifeSlot;
	else if (Slot == "INVENTORY")
		return Grid;
	else
		return nullptr;
}

void AOrionInventoryManager::SetItemInstanceID(FInventoryItem Item, FString ID)
{
	for (int32 i = 0; i < Grid->Inventory.Num(); i++)
	{
		if (Grid->Inventory[i].TempUniqueID == Item.TempUniqueID)
		{
			Grid->Inventory[i].InstanceID = ID;
			return;
		}
	}

	AOrionInventoryGrid *pGrid = GetGridFromSlot(Item.Slot);

	if (!pGrid)
		return;

	//if we get here, it means we couldnt find the item inside the inventory, so check if it's equipped
	if (pGrid->Inventory[0].TempUniqueID == Item.TempUniqueID) 
		pGrid->Inventory[0].InstanceID = ID;

}

void AOrionInventoryManager::HandleGrantItemQueue()
{
	if (bGrantItemOperationInProgress)
		GetWorldTimerManager().SetTimer(GrantItemTimer, this, &AOrionInventoryManager::HandleGrantItemQueue, 0.1f, false);
	else
	{
#if IS_SERVER
		EventGiveItemToUser(GrantItemQueue);
#endif
		GrantItemQueue.Empty();
	}
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

/*void AOrionInventoryManager::RemoveItemFromInventory(AOrionInventoryGrid *theGrid, int32 index)
{
	if (theGrid->Inventory.Num() > index)
	{
		theGrid->Inventory[index].Reset();
	}
}*/

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
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, KnifeSlot, COND_OwnerOnly);
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

void AOrionInventoryManager::UpdateEquippedStats()
{
	EquippedStats.Reset();

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetOwner());

	if (!PC)
		return;

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
	GetStatsFromSlot(KnifeSlot, EquippedStats);
	GetStatsFromSlot(AbilitySlot, EquippedStats);
}

TArray<FCharacterStatEntry> AOrionInventoryManager::GetEquippedStats()
{
	TArray<FCharacterStatEntry> Stats;

	UpdateEquippedStats();

	//fill in the stats
	FCharacterStatEntry Entry;

	Entry.StatName = "DEFENSE"; Entry.Value = EquippedStats.Defense; Stats.Add(Entry); Entry.Desc = ""; Entry.DescValue = 0.0f;

	Entry.StatName = "STRENGTH"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_STRENGTH]; Entry.Desc = "x8x% stronger melee"; Entry.DescValue = 0.1f; Stats.Add(Entry);
	Entry.StatName = "DEXTERITY"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_DEXTERITY]; Entry.Desc = "x8x% faster grenade charge"; Entry.DescValue = 0.1f; Stats.Add(Entry);
	Entry.StatName = "INTELLIGENCE"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_INTELLIGENCE]; Entry.Desc = "x8x% faster energy charge"; Entry.DescValue = 0.1f; Stats.Add(Entry);
	Entry.StatName = "DISCIPLINE"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_DISCIPLINE]; Entry.Desc = "x8x extra shields"; Entry.DescValue = 1.0f; Stats.Add(Entry);
	Entry.StatName = "VITALITY"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_VITALITY]; Entry.Desc = "x8x extra health"; Entry.DescValue = 1.0f; Stats.Add(Entry);
	Entry.Desc = ""; Entry.DescValue = 0.0f;
	Entry.StatName = "CRITICAL HIT CHANCE"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_CRITICALHITCHANCE] + (HasStat(RARESTAT_BONUSCRITCHANCE) ? 15.0f: 0.0f); Stats.Add(Entry);
	Entry.StatName = "CRITICAL DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_CRITICALHITMULTIPLIER] + (HasStat(RARESTAT_BONUSCRITDAMAGE) ? 50.0f: 0.0f); Stats.Add(Entry);
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
	Entry.StatName = "BONUS GOLD"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_GOLDFIND] + (HasStat(RARESTAT_GOLDFIND) ? 100.0f: 0.0f); Stats.Add(Entry);
	Entry.StatName = "BONUS ITEMS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_MAGICFIND] + (HasStat(RARESTAT_MAGICFIND) ? 100.0f: 0.0f); Stats.Add(Entry);
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

		if (Item.Slot != ITEM_SHADER && Item.Slot != ITEM_DISPLAYARMOR)
			Stats.ItemLevel += Item.ItemLevel;
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

void AOrionInventoryManager::ServerDropItem_Implementation(AOrionInventoryGrid *theGrid, int32 index)
{
	DropItem(theGrid, index);
}

bool AOrionInventoryManager::DropItem(AOrionInventoryGrid *theGrid, int32 index)
{
	if (Role != ROLE_Authority)
	{
		ServerDropItem(theGrid, index);
		return true;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

	if (!Game || !OwnerController)
		return false;

	FVector pos;
	FRotator rot;

	AOrionCharacter *Pawn = Cast<AOrionCharacter>(OwnerController->GetPawn());

	if (Pawn)
		pos = Pawn->GetActorLocation() + 250.0f * Pawn->GetActorRotation().Vector();
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
			{
				DrawInventory();
				return false;
			}

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

	DrawInventory();

	return bRet;
}

void AOrionInventoryManager::ServerSellItem_Implementation(AOrionInventoryGrid *theGrid, int32 index)
{
	SellItem(theGrid, index);
}

bool AOrionInventoryManager::SellItem(AOrionInventoryGrid *theGrid, int32 index)
{
	if (Role != ROLE_Authority)
	{
		ServerSellItem(theGrid, index);
		return true;
	}

	FInventoryItem Item = GetItemAt(theGrid, index);

	if (Item.ItemClass == nullptr)
		return false;

	//give the player some cash money
	//todo : add this if needed
	////GiveMoney(Item.SellValue);

	//remove item
	RemoveItemFromInventory(theGrid, index);

	DrawInventory();

	return true;
}

void AOrionInventoryManager::ServerSortInventory_Implementation()
{
	SortInventory();
}

bool AOrionInventoryManager::SortInventory()
{
	if (Role != ROLE_Authority)
	{
		ServerSortInventory();
		return true;
	}

	Grid->Inventory.Sort();

	DrawInventory();

	return true;
}

void AOrionInventoryManager::ServerBreakdownItem_Implementation(AOrionInventoryGrid *theGrid, int32 index)
{
	BreakdownItem(theGrid, index);
}

FRareStatsInfo AOrionInventoryManager::GetRareStat(int32 Index)
{
	return RareStats.StatsInfo[Index];
}

void AOrionInventoryManager::ForceAddInventoryItem(FInventoryItem Item, FString ClassName)
{
	//decode the weapon slot
	FString WeaponSlot = Item.SlotIndex;

	int32 index = WeaponSlot.Find("x8x");

	//need some kind of error handling here
	if (index == INDEX_NONE)
		return;

	int32 index2 = WeaponSlot.Find("y8y");

	FString cName = "";
	FString Slot = WeaponSlot.Left(index);

	if (index2 != INDEX_NONE)
	{
		cName = WeaponSlot.Left(index2);
		Slot = WeaponSlot.Mid(index2 + 3, index - index2 - 3);
	}

	//don't add other character's gear to our inventory
	if (OwnerController)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(OwnerController->PlayerState);

		if (PRI)
		{
			if (cName != "" && cName == PRI->ClassType)
				return;
		}
	}

	int32 InventoryIndex = FCString::Atoi(*WeaponSlot.Mid(index + 3));

	AOrionInventoryGrid *pGrid = GetGridFromName(Slot);

	if (pGrid)
	{
		if (InventoryIndex >= 0)
		{
			if (pGrid->Inventory.Num() <= InventoryIndex)// || Item.ItemClass == nullptr)
			{
				return;
			}
			else
			{
				//grab our class reference from string
				TSubclassOf<UOrionInventoryItem> InventoryClass = LoadClass<UOrionInventoryItem>(nullptr, *ClassName, nullptr, LOAD_None, nullptr);

				if (InventoryClass)
				{
					UOrionInventoryItem *Inv = InventoryClass.GetDefaultObject();

					if (Inv)
					{
						FInventoryItem NewItem;

						NewItem.Amount = Item.Amount;
						NewItem.ItemClass = InventoryClass;
						NewItem.PrimaryStats = Item.PrimaryStats;
						NewItem.SecondaryStats = Item.SecondaryStats;
						NewItem.RareStats = Item.RareStats;
						NewItem.ItemName = Item.Rarity == RARITY_LEGENDARY ? Inv->LegendaryName : Inv->ItemName;
						NewItem.ItemDesc = Item.ItemDesc;
						NewItem.Rarity = Item.Rarity;
						NewItem.Slot = Inv->ItemType;
						NewItem.ItemLevel = Item.ItemLevel;
						NewItem.MainStat = Item.MainStat;
						NewItem.SellValue = 10;
						NewItem.BreakdownClasses = Inv->BreakdownClass;
						NewItem.ItemID = Item.ItemID;
						NewItem.InstanceID = Item.InstanceID;
						NewItem.bDirty = false;
						NewItem.SlotIndex = Item.SlotIndex;

						pGrid->Inventory[InventoryIndex] = NewItem;
					}
				}
			}
		}
	}
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

	//some items can't be broken down
	switch (Item.Slot)
	{
	case ITEM_GENERICCRAFTING:
	case ITEM_SHADER:
	case ITEM_DISPLAYARMOR:
	case ITEM_USEABLE:
		return false;
		break;
	case ITEM_HELMET:
	case ITEM_CHEST:
	case ITEM_HANDS:
	case ITEM_LEGS:
	case ITEM_BELT:
	case ITEM_BOOTS:
		ItemsToGrant.Add(DefaultArmorPartsClass);
		Rarities.Add(RARITY_COMMON);
		ItemLevels.Add(1);
		break;
	case ITEM_PRIMARYWEAPON:
	case ITEM_SECONDARYWEAPON:
	case ITEM_GADGET:
	case ITEM_ABILITY:
	case ITEM_GRENADE:
		ItemsToGrant.Add(DefaultWeaponPartsClass);
		Rarities.Add(RARITY_COMMON);
		ItemLevels.Add(1);
		break;
	default:
		return false;
		break;
	}

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
			UOrionInventoryItem *pItem = Cast<UOrionInventoryItem>(ItemsToGrant[i].GetDefaultObject());

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
			NewItem.ItemID = pItem->ItemID;
			//NewItem.InstanceID = Item.InstanceID;
			//NewItem.bDirty = false;
			//NewItem.SlotIndex = Item.SlotIndex;

			if (AddItemToInventory(Grid, NewItem) < 0)
			{
				DrawInventory();
				return false;
			}
		}

		DrawInventory();
		return true;
	}

	return false;
}

EPrimaryStats AOrionInventoryManager::GetPrimaryStat(FString StatName)
{
	if (StatName == "STRENGTH")
		return PRIMARYSTAT_STRENGTH;
	else if (StatName == "DEXTERITY")
		return PRIMARYSTAT_DEXTERITY;
	else if (StatName == "INTELLIGENCE")
		return PRIMARYSTAT_INTELLIGENCE;
	else if (StatName == "DISCIPLINE")
		return PRIMARYSTAT_DISCIPLINE;
	else if (StatName == "VITALITY")
		return PRIMARYSTAT_VITALITY;

	return PRIMARYSTAT_NUM;
}

ESecondaryStats AOrionInventoryManager::GetSecondaryStat(FString StatName)
{
	if (StatName == "CHC")
		return SECONDARYSTAT_CRITICALHITCHANCE;
	else if (StatName == "CHD")
		return SECONDARYSTAT_CRITICALHITMULTIPLIER;
	else if (StatName == "DR")
		return SECONDARYSTAT_DAMAGEREDUCTION;
	else if (StatName == "DRBLUNT")
		return SECONDARYSTAT_DAMAGEREDUCTIONBLUNT;
	else if (StatName == "DRPIERCING")
		return SECONDARYSTAT_DAMAGEREDUCTIONPIERCING;
	else if (StatName == "DREXPLOSIVE")
		return SECONDARYSTAT_DAMAGEREDUCTIONEXPLOSIVE;
	else if (StatName == "DRELEMENTAL")
		return SECONDARYSTAT_DAMAGEREDUCTIONELEMENTAL;
	else if (StatName == "DRPOISON")
		return SECONDARYSTAT_DAMAGEREDUCTIONPOISON;
	else if (StatName == "EXPBOOST")
		return SECONDARYSTAT_EXPBOOST;
	else if (StatName == "FIREDAMAGE")
		return SECONDARYSTAT_FIREDAMAGE;
	else if (StatName == "LIGHTNINGDAMAGE")
		return SECONDARYSTAT_LIGHTNINGDAMAGE;
	else if (StatName == "ICEDAMAGE")
		return SECONDARYSTAT_ICEDAMAGE;
	else if (StatName == "POISONDAMAGE")
		return SECONDARYSTAT_POISONDAMAGE;
	else if (StatName == "CORROSIVEDAMAGE")
		return SECONDARYSTAT_CORROSIVEDAMAGE;
	else if (StatName == "GOLDFIND")
		return SECONDARYSTAT_GOLDFIND;
	else if (StatName == "MAGICFIND")
		return SECONDARYSTAT_MAGICFIND;
	else if (StatName == "LARGEDINOBOOST")
		return SECONDARYSTAT_LARGEDINOBOOST;
	else if (StatName == "ROBOTBOOST")
		return SECONDARYSTAT_ROBOTBOOST;
	
	return SECONDARYSTAT_NUM;
}

//these can be a max of 20 characters
FString AOrionInventoryManager::GetPrimaryStatName(EPrimaryStats Stat)
{
	switch (Stat)
	{
	case PRIMARYSTAT_STRENGTH:
		return "STRENGTH";
		break;
	case PRIMARYSTAT_DEXTERITY:
		return "DEXTERITY";
		break;
	case PRIMARYSTAT_INTELLIGENCE:
		return "INTELLIGENCE";
		break;
	case PRIMARYSTAT_DISCIPLINE:
		return "DISCIPLINE";
		break;
	case PRIMARYSTAT_VITALITY:
		return "VITALITY";
		break;
	}

	return "ERROR";
}

//these can only be a max of 20 characters long
FString AOrionInventoryManager::GetSecondaryStatName(ESecondaryStats Stat)
{
	switch (Stat)
	{
	case SECONDARYSTAT_CRITICALHITCHANCE:
		return "CHC";
		break;
	case SECONDARYSTAT_CRITICALHITMULTIPLIER:
		return "CHD";
		break;
	case SECONDARYSTAT_DAMAGEREDUCTION:
		return "DR";
		break;
	case SECONDARYSTAT_DAMAGEREDUCTIONBLUNT:
		return "DRBLUNT";
		break;
	case SECONDARYSTAT_DAMAGEREDUCTIONPIERCING:
		return "DRPIERCING";
		break;
	case SECONDARYSTAT_DAMAGEREDUCTIONEXPLOSIVE:
		return "DREXPLOSIVE";
		break;
	case SECONDARYSTAT_DAMAGEREDUCTIONELEMENTAL:
		return "DRELEMENTAL";
		break;
	case SECONDARYSTAT_DAMAGEREDUCTIONPOISON:
		return "DRPOISON";
		break;
	case SECONDARYSTAT_EXPBOOST:
		return "EXPBOOST";
		break;
	case SECONDARYSTAT_FIREDAMAGE:
		return "FIREDAMAGE";
		break;
	case SECONDARYSTAT_LIGHTNINGDAMAGE:
		return "LIGHTNINGDAMAGE";
		break;
	case SECONDARYSTAT_ICEDAMAGE:
		return "ICEDAMAGE";
		break;
	case SECONDARYSTAT_POISONDAMAGE:
		return "POISONDAMAGE";
		break;
	case SECONDARYSTAT_CORROSIVEDAMAGE:
		return "CORROSIVEDAMAGE";
		break;
	case SECONDARYSTAT_GOLDFIND:
		return "GOLDFIND";
		break;
	case SECONDARYSTAT_MAGICFIND:
		return "MAGICFIND";
		break;
	case SECONDARYSTAT_LARGEDINOBOOST:
		return "LARGEDINOBOOST";
		break;
	case SECONDARYSTAT_ROBOTBOOST:
		return "ROBOTBOOST";
		break;
	}

	return "ERROR";
}