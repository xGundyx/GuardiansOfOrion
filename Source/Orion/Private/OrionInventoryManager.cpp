

#include "Orion.h"
#include "OrionWeapon.h"
#include "OrionCharacter.h"
#include "OrionInventoryManager.h"
#include "OrionPickup.h"
#include "OrionGameMode.h"
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
	bAlwaysRelevant = true;
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

			if (theGrid1->Inventory[index1].ItemClass)
				theGrid1->Inventory[index1].bDirty = true;
			if (theGrid2->Inventory[index2].ItemClass)
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

			if (PRI && theGrid2 != Grid)
				ClassName = PRI->ClassType;
			else
				ClassName = "";

			FString GridName2 = theGrid2->GetGridName();
			theGrid2->Inventory[index2].SlotIndex = FString::Printf(TEXT("%sy8y%sx8x%i"), *ClassName, *GridName2, index2);

			DrawInventory();

			SaveEquippedSlots();

			SaveInventory();

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

		if (bRet)
			EventPlayUISound();

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

	if (Inv.ItemClass && Grid)
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

	if (PC->GetAchievements())
	{
		if (PC->ILevel >= 100)
			PC->GetAchievements()->UnlockAchievement(ACH_GEARMASTERI, PC);
		if (PC->ILevel >= 200)
			PC->GetAchievements()->UnlockAchievement(ACH_GEARMASTERII, PC);
		if (PC->ILevel >= 300)
			PC->GetAchievements()->UnlockAchievement(ACH_GEARMASTERIII, PC);
		if (PC->ILevel >= 400)
			PC->GetAchievements()->UnlockAchievement(ACH_GEARMASTERIV, PC);
		if (PC->ILevel >= 500)
			PC->GetAchievements()->UnlockAchievement(ACH_GEARMASTERV, PC);
		if (PC->ILevel >= 600)
			PC->GetAchievements()->UnlockAchievement(ACH_GEARMASTERVI, PC);
	}

	AOrionCharacter *Pawn = Cast<AOrionCharacter>(PC->GetPawn());

	if (!Pawn)
		return;

	Pawn->Level = FMath::Max(1, EquippedStats.ItemLevel / 12);

	//primary stats
	float HealthRatio = Pawn->Health / Pawn->HealthMax;

	Pawn->HealthMax = GetLevelScaledValue(FMath::Pow(LEVELPOWER, Pawn->Level / LEVELINTERVAL) * 100.0f, Pawn->Level);
	Pawn->Health = Pawn->HealthMax * HealthRatio;

	float ShieldRatio = Pawn->Shield / Pawn->ShieldMax;

	Pawn->ShieldMax = 100.0f + EquippedStats.Defense * 1.0f;
	Pawn->Shield = Pawn->ShieldMax * ShieldRatio;

	Pawn->MeleeDamageBoost = EquippedStats.Primary[PRIMARYSTAT_STRENGTH] * 0.01f;
	Pawn->GrenadeRechargeRate = EquippedStats.Primary[PRIMARYSTAT_DEXTERITY] * 0.01f;
	Pawn->SkillRechargeRate = EquippedStats.Primary[PRIMARYSTAT_INTELLIGENCE] * 0.01f;

	//secondary stats
	Pawn->CriticalHitChance = 5.0f + EquippedStats.Secondary[SECONDARYSTAT_CRITICALHITCHANCE] + PC->GetSkillValue(SKILL_CRITCHANCE) + (HasStat(RARESTAT_BONUSCRITCHANCE) ? 15.0f : 0.0f);
	Pawn->CriticalHitMultiplier = 25.0f + EquippedStats.Secondary[SECONDARYSTAT_CRITICALHITMULTIPLIER] + PC->GetSkillValue(SKILL_CRITDAMAGE) + (HasStat(RARESTAT_BONUSCRITDAMAGE) ? 50.0f : 0.0f);
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
	Pawn->GoldFind = EquippedStats.Secondary[SECONDARYSTAT_GOLDFIND] + (HasStat(RARESTAT_GOLDFIND) ? 100.0f : 0.0f) + PC->GetSkillValue(SKILL_GOLDFIND); //
	Pawn->MagicFind = EquippedStats.Secondary[SECONDARYSTAT_MAGICFIND] + (HasStat(RARESTAT_MAGICFIND) ? 100.0f : 0.0f) + PC->GetSkillValue(SKILL_MAGICFIND);
	Pawn->LargeDinoBoost = EquippedStats.Secondary[SECONDARYSTAT_LARGEDINOBOOST]; 
	Pawn->RobotBoost = EquippedStats.Secondary[SECONDARYSTAT_ROBOTBOOST];

	MaxItemLevel = GetMaxItemLevel();
}

int32 AOrionInventoryManager::GetPrimaryWeaponDamage()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetOwner());

	if(WeaponSlot1 && WeaponSlot1->Inventory[0].ItemClass && PC)
	{
		return FMath::Max(1, GetLevelScaledValue(FMath::Pow(LEVELPOWER, (WeaponSlot1->Inventory[0].MainStat / 2) / LEVELINTERVAL), WeaponSlot1->Inventory[0].ItemLevel));
	}

	return 1;
}

int32 AOrionInventoryManager::GetSecondaryWeaponDamage()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetOwner());

	if (WeaponSlot2 && WeaponSlot2->Inventory[0].ItemClass && PC)
	{
		return FMath::Max(1, GetLevelScaledValue(FMath::Pow(LEVELPOWER, (WeaponSlot2->Inventory[0].MainStat / 2) / LEVELINTERVAL), WeaponSlot2->Inventory[0].ItemLevel));
	}

	return 1;
}

void AOrionInventoryManager::GiveMoney(int32 Amount)
{
	if (Role == ROLE_Authority)
	{
		Money += Amount;

		AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetOwner());

		if (PC)
		{
			PC->AddCoinAmount(Amount);

			AOrionCharacter *P = Cast<AOrionCharacter>(PC->GetPawn());
			if (P)
			{
				float Rate = float(PC->GetSkillValue(SKILL_GOLDHEAL)) / 100.0f;

				P->AddHealth(Rate * P->HealthMax);
			}
		}

		if (PC && PC->GetAchievements())
		{
			if (Money >= 100000)
				PC->GetAchievements()->UnlockAchievement(ACH_MASTEROFCOINI, PC);
			if (Money >= 250000)
				PC->GetAchievements()->UnlockAchievement(ACH_MASTEROFCOINII, PC);
			if (Money >= 500000)
				PC->GetAchievements()->UnlockAchievement(ACH_MASTEROFCOINIII, PC);
			if (Money >= 1000000)
				PC->GetAchievements()->UnlockAchievement(ACH_MASTEROFCOINIV, PC);
			if (Money >= 10000000)
				PC->GetAchievements()->UnlockAchievement(ACH_MASTEROFCOINV, PC);
		}
	}
}

void AOrionInventoryManager::Init(AOrionPlayerController *PC)
{
	EquippedSlots.Empty();

	//14 slots * 5 classes
	for (int32 i = 0; i < 14 * 6; i++)
	{
		FEquippedSlot NewSlot;

		NewSlot.Item.Reset();
		NewSlot.ClassName = "";

		EquippedSlots.Add(NewSlot);
	}

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

bool AOrionInventoryManager::IsFullyInitialized()
{
	return HelmetSlot && BodySlot && HandsSlot && BeltSlot && LegsSlot && BootsSlot &&
		WeaponSlot1 && WeaponSlot2 && GadgetSlot && AbilitySlot && GrenadeSlot && KnifeSlot &&
		DisplaySlot && ShaderSlot;
}

FInventoryItem AOrionInventoryManager::FillInCraftedStats(FInventoryItem Item, int32 Level)
{
	int32 index = 0;
	TArray<FRareStatsInfo> AvailableRares;
	int32 i = 0;

	FDecodeItemInfo Decoder;
	Decoder.ItemLevel = Level;
	Decoder.Slot = Item.Slot;
	Decoder.Rarity = Item.Rarity;

	Item.ItemLevel = Level;

	Item.PrimaryStats.Empty();
	Item.SecondaryStats.Empty();
	Item.RareStats.Empty();

	if (Item.Slot == ITEM_ABILITY)
		index = 2;

	if (Item.Slot == ITEM_SHADER ||
		Item.Slot == ITEM_USEABLE ||
		Item.Slot == ITEM_ANY ||
		Item.Slot == ITEM_GENERICCRAFTING ||
		Item.Slot == ITEM_DISPLAYARMOR ||
		Item.Slot == ITEM_BREAKDOWNABLE ||
		Item.Slot == ITEM_DISPLAYARMOR)
	{
		Item.ItemLevel = 0;
		index = 0;
	}
	else
	{
		switch (Item.Rarity)
		{
		case RARITY_LEGENDARY:
			index = FMath::RandRange(2, 3);

			for (; i < RareStats.StatsInfo.Num(); i++)
			{
				if (RareStats.StatsInfo[i].Slot == Item.Slot || RareStats.StatsInfo[i].Slot == ITEM_ANY)
					AvailableRares.Add(RareStats.StatsInfo[i]);
			}

			if (AvailableRares.Num() > 0)
				Item.RareStats.Add(AvailableRares[FMath::RandRange(0, AvailableRares.Num() - 1)]);
			break;

		case RARITY_SUPERENHANCED:
			index = FMath::RandRange(2, 3);
			break;

		case RARITY_ENHANCED:
			index = FMath::RandRange(1, 2);
			break;
		default:
			index = 1;
			break;
		}
	}

	TArray<int32> Primary;

	for (int32 j = 0; j < PRIMARYSTAT_NUM; j++)
	{
		//if (j != PRIMARYSTAT_VITALITY && j != PRIMARYSTAT_DISCIPLINE)
			Primary.AddUnique(j);
	}

	i = 0;
	//if (PrimaryStats.Num() > 0)
	//{
	for (; i < index / 2 && Primary.Num() > 0; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, Primary.Num() - 1);

		FPrimaryItemStats StatToAdd;

		StatToAdd.StatType = EPrimaryStats(Primary[RandomIndex]);
		StatToAdd.MaxValue = int32(Decoder.GetMaxStatValue() * Decoder.GetRarityMultiplier());
		StatToAdd.MinValue = FMath::Max(1, int32(StatToAdd.MaxValue * 0.7f));
		StatToAdd.Value = FMath::RandRange(StatToAdd.MinValue, StatToAdd.MaxValue);

		Item.PrimaryStats.Add(StatToAdd);

		Primary.RemoveAt(RandomIndex);
	}
	//}

	TArray<int32> Secondary;

	bool bOneElemental = false;
	for (int32 j = 0; j < SECONDARYSTAT_NUM; j++)
	{
		if (j == SECONDARYSTAT_CORROSIVEDAMAGE || j == SECONDARYSTAT_FIREDAMAGE || j == SECONDARYSTAT_ICEDAMAGE || j == SECONDARYSTAT_LIGHTNINGDAMAGE || j == SECONDARYSTAT_POISONDAMAGE)
		{
			if (!bOneElemental && (Item.Slot == ITEM_PRIMARYWEAPON || Item.Slot == ITEM_SECONDARYWEAPON))
			{
				bOneElemental = true;
			}
			else
				continue;
		}

		if (Decoder.GetMaxSecondaryStatValue(ESecondaryStats(j)) > 0)
			Secondary.AddUnique(j);
	}

	for (; i < index && Secondary.Num() > 0; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, Secondary.Num() - 1);

		FSecondaryItemStats StatToAdd;

		StatToAdd.StatType = ESecondaryStats(Secondary[RandomIndex]);
		StatToAdd.MaxValue = Decoder.GetMaxSecondaryStatValue(ESecondaryStats(StatToAdd.StatType));
		StatToAdd.MinValue = FMath::Max(1, int32(StatToAdd.MaxValue * 0.7f));
		StatToAdd.Value = FMath::RandRange(StatToAdd.MinValue, StatToAdd.MaxValue);

		Item.SecondaryStats.Add(StatToAdd);

		Secondary.RemoveAt(RandomIndex);
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
	case ITEM_PRIMARYWEAPON:
	case ITEM_SECONDARYWEAPON:
	case ITEM_GRENADE:
	case ITEM_KNIFE:
	case ITEM_GADGET:
		Item.MainStat = int32(Decoder.GetRarityMultiplier() * FMath::RandRange(int32(Decoder.GetMaxStatValue(true) * 0.9f), Decoder.GetMaxStatValue(true)));
		break;
	default:
		Item.MainStat = 0;
	}

	return Item;
}

void AOrionInventoryManager::ReduceItemCount(TSubclassOf<UOrionInventoryItem> Item, int32 Amount)
{
	if (!Grid)
		return;

	for (int32 i = 0; i < Grid->Inventory.Num(); i++)
	{
		if (Grid->Inventory[i].ItemClass == Item)
		{
			if (Grid->Inventory[i].Amount >= Amount || Amount == 1)
			{
				Grid->Inventory[i].Amount -= Amount;
				if (Grid->Inventory[i].Amount == 0)
					RemoveItemFromInventory(Grid, i);
				else
				{
#if IS_SERVER
					EventUpdateItemStats(Grid->Inventory[i]);
#endif
				}

				Grid->Inventory[i].bDirty = true;

				return;
			}
			else
			{
				Amount -= Grid->Inventory[i].Amount;
				RemoveItemFromInventory(Grid, i);
			}
		}
	}
}

bool AOrionInventoryManager::UseItem(AOrionInventoryGrid *theGrid, int32 index)
{
	if (Role != ROLE_Authority)
	{
		ServerUseItem(theGrid, index);
		return true;
	}

	if (!theGrid)
		return false;

	if (index < 0 || index > 99)
	{
		ClientRedraw(ITEM_ANY, index);
		return false;
	}

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(OwnerController);
	AOrionCharacter *P = nullptr;
	AOrionPRI *PRI = nullptr;
	if (PC)
	{
		P = Cast<AOrionCharacter>(PC->GetPawn());
		PRI = Cast<AOrionPRI>(PC->PlayerState);
	}

	FInventoryItem Item = GetItemAt(theGrid, index);

	if (Item.ItemClass)
	{
		UOrionInventoryItem *Inv = Item.ItemClass.GetDefaultObject();
		if (!Inv || !Inv->bConsumeable)
		{
			ClientRedraw(ITEM_ANY, index);
			return false;
		}

		switch (Inv->UseableItem.Effect)
		{
		case INVENTORYUSE_GRANTXP:
			if (PC)
			{
				PC->AddXP(Inv->UseableItem.Value, true);
			}
			break;
		case INVENTORYUSE_GRANTITEM:
			//pick a random item slot
			EventGrantRandomItem(Inv->UseableItem.Quality);
			RemoveItemFromInventory(theGrid, index);
			return true;
			break;
		case INVENTORYUSE_HEALTH:
			if (P)
				P->AddBuff(Inv->UseableItem.Buff, PC, 0);
			break;
		case INVENTORYUSE_SHIELD:
			if (P)
				P->AddBuff(Inv->UseableItem.Buff, PC, 0);
			break;
		case INVENTORYUSE_COIN:
#if IS_SERVER
			if (PRI)
				UPlayFabRequestProxy::ServerAddUserVirtualCurrency(PRI->PlayFabID, TEXT("TC"), Inv->UseableItem.Value);
#endif
			break;
		case INVENTORYUSE_BUNDLE:
			if(EventOpenBundle(Inv->UseableItem.BundleItems, Inv->UseableItem.Value))
				RemoveItemFromInventory(theGrid, index);
			return true;
			break;
		}

		ReduceItemCount(Item.ItemClass, 1);
	}

	return true;
}

void AOrionInventoryManager::ServerUseItem_Implementation(AOrionInventoryGrid *theGrid, int32 index)
{
	UseItem(theGrid, index);
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
#if IS_SERVER
		EventConsumeItem(Item);
#endif
	}

	//remove it from our local inventory
	theGrid->Inventory[Index].Reset();
}

void AOrionInventoryManager::DestroyInventory()
{
	if (!Grid)
		return;

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

	/*if (HelmetSlot->Inventory[0].ItemClass)
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
	}*/
}

void AOrionInventoryManager::SaveInventory()
{
	if (bSaveInventoryOperationInProgress)
		GetWorldTimerManager().SetTimer(SaveInventoryTimer, this, &AOrionInventoryManager::SaveInventory, 0.1f, false);
	else if(IsFullyInitialized())
	{
#if IS_SERVER
		EventSaveInventoryToPlayfab();
#else
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(OwnerController);

		if (PC && PC->SavedGame)
			PC->SaveGameToFile(PC->SavedGame->FileName);
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

	if (!theGrid || !Grid)
		return Success;

	//if we are stackable, try to find us a stack to join
	if (newItem.ItemClass.GetDefaultObject() && newItem.ItemClass.GetDefaultObject()->bStackable)
	{
		for (int32 i = 0; i < theGrid->Height; i++)
		{
			for (int32 j = 0; j < theGrid->Width; j++)
			{
				if (theGrid->Inventory[i*theGrid->Width + j].ItemClass == newItem.ItemClass)
				{
					if (theGrid->Inventory[i*theGrid->Width + j].Amount + newItem.Amount <= 5000)
					{
						theGrid->Inventory[i*theGrid->Width + j].Amount += FMath::Min(1, newItem.Amount);
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
				theGrid->Inventory[i*theGrid->Width + j].Reset();
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

	if (!IsFullyInitialized())
		return Items;

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
	if (!Grid)
		return;

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
	if (Cast<AOrionPlayerController>(GetOwner()) && Cast<AOrionPlayerController>(GetOwner())->IsLocalPlayerController())
		Cast<AOrionPlayerController>(GetOwner())->EventRedrawInventory();
}

void AOrionInventoryManager::ClientRedraw_Implementation(EItemType type, int32 index)
{
	if (Cast<AOrionPlayerController>(GetOwner()))
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
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, OwnerController, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionInventoryManager, MaxItemLevel, COND_OwnerOnly);
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

	EquippedStats.bAllLegendary = true;

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

	if (PC && Role == ROLE_Authority && EquippedStats.bAllLegendary)
	{
		if (PC->GetAchievements())
			PC->GetAchievements()->UnlockAchievement(ACH_600MAN, PC);
	}
}

TArray<FCharacterStatEntry> AOrionInventoryManager::GetEquippedStats()
{
	TArray<FCharacterStatEntry> Stats;

	UpdateEquippedStats();

	//fill in the stats
	FCharacterStatEntry Entry;

	float ShieldMultiplier = 1.0f;

	if (OwnerController)
	{
		AOrionCharacter *P = Cast<AOrionCharacter>(OwnerController->GetPawn());
		if (P && P->bDoubleShield)
			ShieldMultiplier = 2.0f;
	}

	Entry.StatName = "DEFENSE"; Entry.Value = EquippedStats.Defense; Entry.Desc = "x8x extra Shields"; Entry.DescValue = 1.0f; Stats.Add(Entry);
	Entry.StatName = "HEALTH"; Entry.Value = 100.0f + GetLevelScaledValue(FMath::Pow(LEVELPOWER, (EquippedStats.ItemLevel / 12) / LEVELINTERVAL) * 100.0f, EquippedStats.ItemLevel / 12); Entry.Desc = ""; Entry.DescValue = 0.0f; Stats.Add(Entry);
	Entry.StatName = "SHIELD"; Entry.Value = ShieldMultiplier * (100.0f + EquippedStats.Defense); Entry.Desc = ""; Entry.DescValue = 0.0f; Stats.Add(Entry);

	Entry.StatName = "STRENGTH"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_STRENGTH]; Entry.Desc = "x8x% stronger melee"; Entry.DescValue = 0.01f; Stats.Add(Entry);
	Entry.StatName = "DEXTERITY"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_DEXTERITY]; Entry.Desc = "x8x% faster grenade charge"; Entry.DescValue = 0.01f; Stats.Add(Entry);
	Entry.StatName = "INTELLIGENCE"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_INTELLIGENCE]; Entry.Desc = "x8x% faster energy charge"; Entry.DescValue = 0.01f; Stats.Add(Entry);
	//Entry.StatName = "DISCIPLINE"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_DISCIPLINE]; Entry.Desc = "x8x extra shields"; Entry.DescValue = 1.0f; Stats.Add(Entry);
	//Entry.StatName = "VITALITY"; Entry.Value = EquippedStats.Primary[PRIMARYSTAT_VITALITY]; Entry.Desc = "x8x extra health"; Entry.DescValue = 1.0f; Stats.Add(Entry);
	Entry.Desc = ""; Entry.DescValue = 0.0f;
	Entry.StatName = "CRITICAL HIT CHANCE"; Entry.Value = 5.0f + EquippedStats.Secondary[SECONDARYSTAT_CRITICALHITCHANCE] + (HasStat(RARESTAT_BONUSCRITCHANCE) ? 15.0f: 0.0f); Stats.Add(Entry);
	Entry.StatName = "CRITICAL DAMAGE BONUS"; Entry.Value = 25.0f + EquippedStats.Secondary[SECONDARYSTAT_CRITICALHITMULTIPLIER] + (HasStat(RARESTAT_BONUSCRITDAMAGE) ? 50.0f: 0.0f); Stats.Add(Entry);
	Entry.StatName = "DAMAGE REDUCTION"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTION]; Stats.Add(Entry);
	Entry.StatName = "BLUNT DAMAGE REDUCTION"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONBLUNT]; Entry.Desc = "tail and stomp attacks"; Stats.Add(Entry);
	Entry.StatName = "PIERCING DAMAGE REDUCTION"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONPIERCING]; Entry.Desc = "bite and claw damage"; Stats.Add(Entry);
	Entry.StatName = "EXPLOSIVE DAMAGE REDUCTION"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONEXPLOSIVE]; Entry.Desc = ""; Stats.Add(Entry);
	Entry.StatName = "ELEMENTAL DAMAGE REDUCTION"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONELEMENTAL]; Stats.Add(Entry);
	Entry.StatName = "POISON DAMAGE REDUCTION"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_DAMAGEREDUCTIONPOISON]; Stats.Add(Entry);
	Entry.StatName = "BONUS XP PER KILL"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_EXPBOOST]; Stats.Add(Entry);
	Entry.StatName = "FIRE DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_FIREDAMAGE]; Stats.Add(Entry);
	Entry.StatName = "LIGHTNING DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_LIGHTNINGDAMAGE]; Stats.Add(Entry);
	Entry.StatName = "ICE DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_ICEDAMAGE]; Stats.Add(Entry);
	Entry.StatName = "POISON DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_POISONDAMAGE]; Stats.Add(Entry);
	Entry.StatName = "CORROSIVE DAMAGE BONUS"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_CORROSIVEDAMAGE]; Stats.Add(Entry);
	Entry.StatName = "ION FIND"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_GOLDFIND] + (HasStat(RARESTAT_GOLDFIND) ? 100.0f: 0.0f); Stats.Add(Entry);
	Entry.StatName = "MAGIC FIND"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_MAGICFIND] + (HasStat(RARESTAT_MAGICFIND) ? 100.0f: 0.0f); Stats.Add(Entry);
	Entry.StatName = "LARGE DINO DAMAGE"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_LARGEDINOBOOST]; Stats.Add(Entry);
	Entry.StatName = "ROBOT DAMAGE"; Entry.Value = EquippedStats.Secondary[SECONDARYSTAT_ROBOTBOOST]; Stats.Add(Entry);

	return Stats;
}

int32 AOrionInventoryManager::GetLevelScaledValue(int32 Value, int32 ItemLevel)
{
	//you can only be a max of 50 ilvls higher than the game's item level
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GetGameState());
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(OwnerController);

	if (GRI && PC)
	{
		int32 iLvl = GRI->ItemLevel;
		int32 Level = ItemLevel;

		return Value / FMath::Pow(LEVELPOWER, float(FMath::Max(0, Level - FMath::Min(iLvl + LEVELSYNC, Level))) / LEVELINTERVAL);
	}

	return Value;
}

void AOrionInventoryManager::GetStatsFromSlot(AOrionInventoryGrid *Slot, FArrayHelper &Stats)
{
	FInventoryItem Item = GetItemAt(Slot, 0);
	if (Item.ItemClass)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(OwnerController);
		if (PC && PC->GetAchievements() && Item.Rarity == RARITY_LEGENDARY)
			PC->GetAchievements()->UnlockAchievement(ACH_LEGEND, PC);

		for (int32 i = 0; i < Item.PrimaryStats.Num(); i++)
		{
			Stats.Primary[Item.PrimaryStats[i].StatType] += GetLevelScaledValue(Item.PrimaryStats[i].Value, Item.ItemLevel);
		}
		for (int32 i = 0; i < Item.SecondaryStats.Num(); i++)
		{
			/*if (Item.SecondaryStats[i].StatType == SECONDARYSTAT_CORROSIVEDAMAGE || Item.SecondaryStats[i].StatType == SECONDARYSTAT_FIREDAMAGE || Item.SecondaryStats[i].StatType == SECONDARYSTAT_ICEDAMAGE || 
				Item.SecondaryStats[i].StatType == SECONDARYSTAT_POISONDAMAGE || Item.SecondaryStats[i].StatType == SECONDARYSTAT_LIGHTNINGDAMAGE)
				Stats.Secondary[Item.SecondaryStats[i].StatType] += GetLevelScaledValue(Item.SecondaryStats[i].Value, Item.ItemLevel);
			else*/
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
		case ITEM_GRENADE:
		case ITEM_KNIFE:
		case ITEM_GADGET:
			Stats.Defense += GetLevelScaledValue(Item.MainStat, Item.ItemLevel);
			break;
		}

		if (Item.Slot != ITEM_SHADER && Item.Slot != ITEM_DISPLAYARMOR)
			Stats.ItemLevel += Item.ItemLevel;
	}

	if (Item.Rarity != RARITY_LEGENDARY || Item.ItemLevel < 600)
		Stats.bAllLegendary = false;
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

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetOwner());

	if (PC && PC->GetStats())
		PC->GetStats()->AddStatValue(STAT_CRAFTITEMS, 1);

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

		FInventoryItem Item = GetItemAt(theGrid, index);

		if (Item.ItemClass == nullptr)
			return false;

		AOrionPickup *Pickup = GetWorld()->SpawnActor<AOrionPickup>(Game->DefaultPickupClass, pos, FRotator::ZeroRotator, SpawnInfo);
		if (Pickup)
		{
			Pickup->SetOwner(OwnerController);
			Pickup->bOnlyRelevantToOwner = false;// true;
			Pickup->PickupMesh->bOnlyOwnerSee = false;//true;

			Pickup->bGlobalItem = true;

			Pickup->Decoder.ItemClass = Item.ItemClass;
			UOrionInventoryItem *Inv = Cast<UOrionInventoryItem>(Pickup->Decoder.ItemClass->GetDefaultObject());

			if (Inv)
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
			Pickup->Decoder.Amount = Item.Amount;
			Pickup->Decoder.ItemID = Item.ItemID;

			//for replication of color
			Pickup->GearType = Item.Rarity;

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

	//mark everything dirty
	for (int32 i = 0; Grid && i < Grid->Inventory.Num(); i++)
	{
		if (Grid->Inventory[i].ItemClass)
		{
			switch (Grid->Inventory[i].Slot)
			{
			case ITEM_GENERICCRAFTING:
			case ITEM_USEABLE:
			case ITEM_DISPLAYARMOR:
			case ITEM_SHADER:
			case ITEM_BREAKDOWNABLE:
				Grid->Inventory[i].PrimaryStats.Empty();
				Grid->Inventory[i].SecondaryStats.Empty();
				Grid->Inventory[i].RareStats.Empty();
				Grid->Inventory[i].MainStat = 0;
				break;
			}

			Grid->Inventory[i].bDirty = true;
			//update our slot indices, since sort will mess them all up
			Grid->Inventory[i].SlotIndex = FString::Printf(TEXT("%sx8x%i"), TEXT("INVENTORY"), i);
		}
	}
	
	SaveInventory();

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

					//check and make sure values are within the limits, mainly to fix outdated gear
					//make sure our main stat is in valid bounds
					FDecodeItemInfo Decoder;
					Decoder.Rarity = Item.Rarity;
					Decoder.Slot = Inv->ItemType;
					Decoder.ItemLevel = Item.ItemLevel;

					float MaxValue = Decoder.GetMaxStatValue(true) * Decoder.GetRarityMultiplier();

					if (MaxValue <= Item.MainStat)
						Item.MainStat = MaxValue;
					else if (Item.MainStat < 0.9f * MaxValue)
						Item.MainStat = 0.9f * MaxValue;
					else if (Item.MainStat < 0)
						Item.MainStat = MaxValue;

					//make sure the elemental damages are valid
					if (Item.Slot == ITEM_PRIMARYWEAPON || Item.Slot == ITEM_SECONDARYWEAPON)
					{
						for (int32 i = 0; i < Item.SecondaryStats.Num(); i++)
						{
							if (Item.SecondaryStats[i].StatType == SECONDARYSTAT_CORROSIVEDAMAGE || Item.SecondaryStats[i].StatType == SECONDARYSTAT_POISONDAMAGE || Item.SecondaryStats[i].StatType == SECONDARYSTAT_FIREDAMAGE ||
								Item.SecondaryStats[i].StatType == SECONDARYSTAT_ICEDAMAGE || Item.SecondaryStats[i].StatType == SECONDARYSTAT_LIGHTNINGDAMAGE)
							{
								float MaxSecondaryValue = Decoder.GetMaxSecondaryStatValue(Item.SecondaryStats[i].StatType);

								if (MaxSecondaryValue <= Item.SecondaryStats[i].Value)
									Item.SecondaryStats[i].Value = MaxSecondaryValue;
							}
						}
					}

					if (Item.Slot == ITEM_GENERICCRAFTING || Item.Slot == ITEM_BREAKDOWNABLE || Item.Slot == ITEM_SHADER || Item.Slot == ITEM_DISPLAYARMOR || Item.Slot == ITEM_USEABLE)
					{
						Item.PrimaryStats.Empty();
						Item.SecondaryStats.Empty();
						Item.RareStats.Empty();
						Item.MainStat = 0;
						Item.ItemLevel = 0;
					}

					//don't add other character's gear to our inventory
					if (OwnerController)
					{
						AOrionPRI *PRI = Cast<AOrionPRI>(OwnerController->PlayerState);

						if (PRI)
						{
							//cache all class equipped gear for changing classes in game
							if (Slot != "INVENTORY")
							{
								int32 index = -1;
								if (cName == "ASSAULT")
									index = 0;
								else if (cName == "SUPPORT")
									index = 1;
								else if (cName == "RECON")
									index = 2;
								else if (cName == "TECH")
									index = 3;
								else if (cName == "PYRO")
									index = 4;
								else if (cName == "MARKSMAN")
									index = 5;

								int32 sIndex = -1;

								if (Slot == "HELMET")
									sIndex = 0;
								else if (Slot == "BODY")
									sIndex = 1;
								else if (Slot == "HANDS")
									sIndex = 2;
								else if (Slot == "BELT")
									sIndex = 3;
								else if (Slot == "LEGS")
									sIndex = 4;
								else if (Slot == "BOOTS")
									sIndex = 5;
								else if (Slot == "PRIMARY")
									sIndex = 6;
								else if (Slot == "SECONDARY")
									sIndex = 7;
								else if (Slot == "GADGET")
									sIndex = 8;
								else if (Slot == "ABILITY")
									sIndex = 9;
								else if (Slot == "SHADER")
									sIndex = 10;
								else if (Slot == "DISPLAY")
									sIndex = 11;
								else if (Slot == "GRENADE")
									sIndex = 12;
								else if (Slot == "KNIFE")
									sIndex = 13;

								if (index >= 0 && sIndex >= 0)
								{
									FEquippedSlot NewSlot;

									NewSlot.Item = Item;
									NewSlot.ClassName = ClassName;

									EquippedSlots[index * 14 + sIndex] = NewSlot;
								}
							}

							if (cName != "" && cName != PRI->ClassType)
								return;
							else if (cName == "" && Slot != "INVENTORY")
								return;
						}
					}

					MaxItemLevel = GetMaxItemLevel();

					if (Inv)
					{
						FInventoryItem NewItem;

						NewItem.Amount = Item.Amount;
						NewItem.ItemClass = InventoryClass;
						NewItem.PrimaryStats = Item.PrimaryStats;
						NewItem.SecondaryStats = Item.SecondaryStats;
						NewItem.RareStats = Item.RareStats;
						NewItem.ItemName = Item.Rarity == RARITY_LEGENDARY ? Inv->LegendaryName : Inv->ItemName;
						NewItem.ItemDesc = Inv->ItemDesc;
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
	{
		ClientRedraw(ITEM_ANY, index);
		return false;
	}

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
		ClientRedraw(ITEM_ANY, index);
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
		ItemLevels.Add(0);
		break;
	case ITEM_PRIMARYWEAPON:
	case ITEM_SECONDARYWEAPON:
	case ITEM_GADGET:
	case ITEM_ABILITY:
	case ITEM_GRENADE:
	case ITEM_KNIFE:
		ItemsToGrant.Add(DefaultWeaponPartsClass);
		Rarities.Add(RARITY_COMMON);
		ItemLevels.Add(0);
		break;
	default:
		ClientRedraw(ITEM_ANY, index);
		return false;
		break;
	}

	//see if there is any special kind of breakdown item
	for (int32 i = 0; i < Item.BreakdownClasses.Num(); i++)
	{
		ItemsToGrant.Add(Item.BreakdownClasses[i]);
		Rarities.Add(RARITY_COMMON);
		ItemLevels.Add(0);
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
	ItemLevels.Add(0);

	int32 NumSpacesToFind = ItemsToGrant.Num();

	//see if we have enough room for the new stuff we're getting
	for (int32 i = 0; Grid && i < Grid->Inventory.Num() && NumSpacesToFind > 0; i++)
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
			NewItem.ItemLevel = 0;// ItemLevels[i];
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
				ClientRedraw(ITEM_ANY, index);
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
		return PRIMARYSTAT_STRENGTH;
	else if (StatName == "VITALITY")
		return PRIMARYSTAT_STRENGTH;

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
	//case PRIMARYSTAT_DISCIPLINE:
	//	return "DISCIPLINE";
	//	break;
	//case PRIMARYSTAT_VITALITY:
	//	return "VITALITY";
	//	break;
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

void AOrionInventoryManager::SaveEquippedSlots()
{
	FInventoryItem DummyItem;

	DummyItem.ItemClass = nullptr;

	if (OwnerController)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(OwnerController->PlayerState);

		if (PRI)
		{
			int32 index = -1;
			if (PRI->ClassType == "ASSAULT")
				index = 0;
			else if (PRI->ClassType == "SUPPORT")
				index = 1;
			else if (PRI->ClassType == "RECON")
				index = 2;
			else if (PRI->ClassType == "TECH")
				index = 3;
			else if (PRI->ClassType == "PYRO")
				index = 4;
			else if (PRI->ClassType == "MARKSMAN")
				index = 5;

			if (index >= 0)
			{
				for (int32 i = 0; i < 14; i++)
				{
					AOrionInventoryGrid *theGrid = nullptr;

					switch (i)
					{
					case 0:
						theGrid = HelmetSlot;
						break;
					case 1:
						theGrid = BodySlot;
						break;
					case 2:
						theGrid = HandsSlot;
						break;
					case 3:
						theGrid = BeltSlot;
						break;
					case 4:
						theGrid = LegsSlot;
						break;
					case 5:
						theGrid = BootsSlot;
						break;
					case 6:
						theGrid = WeaponSlot1;
						break;
					case 7:
						theGrid = WeaponSlot2;
						break;
					case 8:
						theGrid = GadgetSlot;
						break;
					case 9:
						theGrid = AbilitySlot;
						break;
					case 10:
						theGrid = ShaderSlot;
						break;
					case 11:
						theGrid = DisplaySlot;
						break;
					case 12:
						theGrid = GrenadeSlot;
						break;
					case 13:
						theGrid = KnifeSlot;
						break;
					}

					if (theGrid && index >= 0)
					{
						EquippedSlots[index * 14 + i].Item = theGrid->Inventory[0];
						EquippedSlots[index * 14 + i].ClassName = GetPathToClass(theGrid->Inventory[0]);// PRI->ClassType;
					}
				}
			}
		}
	}

	MaxItemLevel = GetMaxItemLevel();
}

//this is called on class spawn to make sure the right equipment is equipped
void AOrionInventoryManager::UpdateEquippedSlots()
{
	FInventoryItem DummyItem;

	DummyItem.ItemClass = nullptr;

	if (OwnerController)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(OwnerController->PlayerState);

		if (PRI)
		{
			int32 index = -1;
			if (PRI->ClassType == "ASSAULT")
				index = 0;
			else if (PRI->ClassType == "SUPPORT")
				index = 1;
			else if (PRI->ClassType == "RECON")
				index = 2;
			else if (PRI->ClassType == "TECH")
				index = 3;
			else if (PRI->ClassType == "PYRO")
				index = 4;
			else if (PRI->ClassType == "MARKSMAN")
				index = 5;

			if (index >= 0)
			{
				for (int32 i = 0; i < 14; i++)
				{
					AOrionInventoryGrid *theGrid = nullptr;

					switch (i)
					{
					case 0:
						theGrid = HelmetSlot;
						break;
					case 1:
						theGrid = BodySlot;
						break;
					case 2:
						theGrid = HandsSlot;
						break;
					case 3:
						theGrid = BeltSlot;
						break;
					case 4:
						theGrid = LegsSlot;
						break;
					case 5:
						theGrid = BootsSlot;
						break;
					case 6:
						theGrid = WeaponSlot1;
						break;
					case 7:
						theGrid = WeaponSlot2;
						break;
					case 8:
						theGrid = GadgetSlot;
						break;
					case 9:
						theGrid = AbilitySlot;
						break;
					case 10:
						theGrid = ShaderSlot;
						break;
					case 11:
						theGrid = DisplaySlot;
						break;
					case 12:
						theGrid = GrenadeSlot;
						break;
					case 13:
						theGrid = KnifeSlot;
						break;
					}

					if (EquippedSlots[index * 14 + i].ClassName != "")
					{
						if (theGrid)
						{
							//grab our class reference from string
							TSubclassOf<UOrionInventoryItem> InventoryClass = LoadClass<UOrionInventoryItem>(nullptr, *EquippedSlots[index * 14 + i].ClassName, nullptr, LOAD_None, nullptr);

							if (InventoryClass)
							{
								UOrionInventoryItem *Inv = InventoryClass.GetDefaultObject();

								if (Inv)
								{
									FInventoryItem NewItem;

									NewItem.Amount = EquippedSlots[index * 14 + i].Item.Amount;
									NewItem.ItemClass = InventoryClass;
									if (Inv->ItemType == ITEM_GENERICCRAFTING)
									{
										NewItem.PrimaryStats.Empty();
										NewItem.SecondaryStats.Empty();
										NewItem.RareStats.Empty();
									}
									else
									{
										NewItem.PrimaryStats = EquippedSlots[index * 14 + i].Item.PrimaryStats;
										NewItem.SecondaryStats = EquippedSlots[index * 14 + i].Item.SecondaryStats;
										NewItem.RareStats = EquippedSlots[index * 14 + i].Item.RareStats;
									}
									NewItem.ItemName = EquippedSlots[index * 14 + i].Item.Rarity == RARITY_LEGENDARY ? Inv->LegendaryName : Inv->ItemName;
									NewItem.ItemDesc = Inv->ItemDesc;
									NewItem.Rarity = EquippedSlots[index * 14 + i].Item.Rarity;
									NewItem.Slot = Inv->ItemType;
									NewItem.ItemLevel = EquippedSlots[index * 14 + i].Item.ItemLevel;
									NewItem.MainStat = EquippedSlots[index * 14 + i].Item.MainStat;
									NewItem.SellValue = 10;
									NewItem.BreakdownClasses = Inv->BreakdownClass;
									NewItem.ItemID = EquippedSlots[index * 14 + i].Item.ItemID;
									NewItem.InstanceID = EquippedSlots[index * 14 + i].Item.InstanceID;
									NewItem.bDirty = false;
									NewItem.SlotIndex = EquippedSlots[index * 14 + i].Item.SlotIndex;

									theGrid->Inventory[0] = NewItem;
								}
								else
									if (theGrid)
										theGrid->Inventory[0] = DummyItem;
							}
							else
								if (theGrid)
									theGrid->Inventory[0] = DummyItem;
						}
					}
					else
					{
						if (theGrid)
							theGrid->Inventory[0] = DummyItem;
					}
				}
			}
		}
	}
}

int32 AOrionInventoryManager::GetMaxItemLevel()
{
	int32 MaxLevel = 1;

	for (int32 i = 0; i < EquippedSlots.Num(); i += 14)
	{
		int32 Level = 0;
		for (int32 j = 0; j < 14; j++)
		{
			if (EquippedSlots[i + j].Item.ItemClass)
				Level += EquippedSlots[i + j].Item.ItemLevel;
		}

		Level /= 12;

		if (Level > MaxLevel)
			MaxLevel = Level;
	}

	return MaxLevel;
}