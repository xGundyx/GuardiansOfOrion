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

bool AOrionPickup::Init(UClass *LootTable, int32 Level)
{
	if (!LootTable)
		return false;

	UOrionInventoryList *List = Cast<UOrionInventoryList>(LootTable->GetDefaultObject());

	if (!List)
		return false;

	//first pick an item type
	UOrionInventoryItem *Inv = Cast<UOrionInventoryItem>(List->PickItemType(Level)->GetDefaultObject());

	if (Inv == nullptr)
		return false;

	//pick which quality, also set the shape/color of the pickup here, also determines name?
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

	Decoder.RequiredLevel = Level;

	//assign stats based on quality and item type
	Inv->CalcStats(Decoder);

	//encode this item
	EncodedValue = EncodeItem(Decoder);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;
	Inventory = GetWorld()->SpawnActor<AOrionInventory>(Inv->ItemClass, SpawnInfo);

	if (Inventory)
	{
		Inventory->Image = Inv->ItemIcon;
		Inventory->InventoryType = Inv->ItemType;
		Inventory->ItemName = Decoder.Rarity == RARITY_LEGENDARY? Inv->LegendaryName : Inv->ItemName;
		Inventory->ItemDescription = Inv->ItemDesc;
		Inventory->bStackable = Inv->bStackable;
		Inventory->StackAmount = 1;
		Inventory->RequiredLevel = Level;
		Inventory->Rarity = Decoder.Rarity;

		AOrionArmor *Armor = Cast<AOrionArmor>(Inventory);
		if (Armor)
		{
			Armor->Mesh = Inv->ItemMesh;
			Armor->Mesh1P = Inv->ItemMesh1P;
		}
	}

	return true;
}


