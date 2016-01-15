// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionInventoryList.h"
#include "OrionArmor.h"
#include "OrionPickup.h"

// Sets default values
AOrionPickup::AOrionPickup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickupMesh = ObjectInitializer.CreateOptionalDefaultSubobject<UStaticMeshComponent>(this, TEXT("Pickup"));
	PickupMesh->AlwaysLoadOnClient = true;
	PickupMesh->AlwaysLoadOnServer = true;
	PickupMesh->bCastDynamicShadow = true;
	PickupMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PickupMesh->bGenerateOverlapEvents = false;
	PickupMesh->Mobility = EComponentMobility::Type::Movable;

	RootComponent = PickupMesh;

	BoxCollision = ObjectInitializer.CreateOptionalDefaultSubobject<UBoxComponent>(this, TEXT("Box"));
	BoxCollision->AttachParent = PickupMesh;

}

// Called when the game starts or when spawned
void AOrionPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrionPickup::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AOrionPickup::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AOrionPickup, Inventory, COND_Custom);
}

bool AOrionPickup::Init(UClass *LootTable, int32 Level)
{
	if (Role != ROLE_Authority)
		return false;

	if (!LootTable)
		return false;

	UOrionInventoryList *List = Cast<UOrionInventoryList>(LootTable->GetDefaultObject());

	if (!List)
		return false;

	//first pick an item type
	Decoder.ItemClass = List->PickItemType(Level);
	UOrionInventoryItem *Inv = Cast<UOrionInventoryItem>(Decoder.ItemClass->GetDefaultObject());

	Decoder.ItemPath = Inv->GetPathName();

	if (Inv == nullptr)
		return false;

	//pick which quality, also set the shape/color of the pickup here, also determines name?
	if (Inv->bHasQuality)
	{
		float RandomChance = FMath::FRand();
		if (RandomChance < 0.05f)
		{
			Decoder.Rarity = RARITY_LEGENDARY;
			Decoder.ItemName = Inv->LegendaryName;
		}
		else if (RandomChance < 0.15f)
		{
			Decoder.Rarity = RARITY_SUPERENHANCED;
			Decoder.ItemName = Inv->ItemName;
		}
		else if (RandomChance < 0.3f)
		{
			Decoder.Rarity = RARITY_ENHANCED;
			Decoder.ItemName = Inv->ItemName;
		}
		else
		{
			Decoder.Rarity = RARITY_COMMON;
			Decoder.ItemName = Inv->ItemName;
		}
	}
	else
	{
		Decoder.Rarity = RARITY_COMMON;
		Decoder.ItemName = Inv->ItemName;
	}

	Decoder.Slot = Inv->ItemType;
	if (Decoder.Slot == ITEM_SHADER ||
		Decoder.Slot == ITEM_USEABLE ||
		Decoder.Slot == ITEM_ANY ||
		Decoder.Slot == ITEM_GENERICCRAFTING ||
		Decoder.Slot == ITEM_DISPLAYARMOR ||
		Decoder.Slot == ITEM_BREAKDOWNABLE ||
		Decoder.Slot == ITEM_DISPLAYARMOR)
		Decoder.ItemLevel = 0;
	else
		Decoder.ItemLevel = Level;

	Decoder.ItemDesc = Inv->ItemDesc;

	//assign stats based on quality and item type
	Inv->CalcStats(Decoder);

	//encode this item
	EncodedValue = EncodeItem(Decoder);

	EventSetColor(Decoder.Rarity);

	//might not need this anymore
	/*if (Inv->ItemClass)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Inventory = GetWorld()->SpawnActor<AOrionInventory>(Inv->ItemClass, SpawnInfo);

		if (Inventory)
		{
			Inventory->Image = Inv->ItemIcon;
			Inventory->InventoryType = Inv->ItemType;
			Inventory->ItemName = Decoder.Rarity == RARITY_LEGENDARY ? Inv->LegendaryName : Inv->ItemName;
			Inventory->ItemDescription = Inv->ItemDesc;
			Inventory->bStackable = Inv->bStackable;
			Inventory->StackAmount = 1;
			Inventory->RequiredLevel = Level;
			Inventory->Rarity = Decoder.Rarity;
			Inventory->EncodedValue = EncodedValue;

			AOrionArmor *Armor = Cast<AOrionArmor>(Inventory);
			if (Armor)
			{
				Armor->Mesh = Inv->ItemMesh;
				////Armor->Mesh1P = Inv->ItemMesh1P;
			}
		}
	}*/

	return true;
}

//let the server know!
void AOrionPickup::ClientGrabItem()
{
	if (Role != ROLE_Authority)
		ServerGrabItem();
	else
		GrabItem();
}

bool AOrionPickup::ServerGrabItem_Validate()
{
	return true;
}

void AOrionPickup::ServerGrabItem_Implementation()
{
	GrabItem();
}

void AOrionPickup::GrabItem()
{
	if (GetOwner() && !Decoder.ItemPath.IsEmpty())
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetOwner());
		if (PC)
		{
			AOrionInventoryManager *InvMan = PC->GetInventoryManager();
			if (InvMan)
			{
				FInventoryItem Item;
				Item.Amount = 1;
				Item.ItemClass = Decoder.ItemClass;
				Item.PrimaryStats = Decoder.PrimaryStats;
				Item.SecondaryStats = Decoder.SecondaryStats;
				Item.RareStats = Decoder.RareStats;
				Item.ItemName = Decoder.ItemName;
				Item.ItemDesc = Decoder.ItemDesc;
				Item.Rarity = Decoder.Rarity;
				Item.Slot = Decoder.Slot;
				Item.ItemLevel = Decoder.ItemLevel;
				Item.MainStat = Decoder.MainStat;

				if (InvMan->AddItemToInventory(InvMan->Grid, Item) >= 0)
				{
					Destroy();
				}
				//inventory is full, spawn some kind of message
				else
				{

				}
			}
		}
	}
}


