// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGRI.h"
#include "OrionMenuCharacter.h"


// Sets default values
AOrionMenuCharacter::AOrionMenuCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Base"));
	BaseMesh->AlwaysLoadOnClient = true;
	BaseMesh->AlwaysLoadOnServer = true;
	BaseMesh->bOwnerNoSee = false;
	BaseMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	BaseMesh->bCastDynamicShadow = true;
	BaseMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	BaseMesh->bChartDistanceFactor = true;
	BaseMesh->bGenerateOverlapEvents = false;

	ArmsMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Arms"));
	ArmsMesh->AlwaysLoadOnClient = true;
	ArmsMesh->AlwaysLoadOnServer = true;
	ArmsMesh->bOwnerNoSee = false;
	ArmsMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	ArmsMesh->bCastDynamicShadow = true;
	ArmsMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	ArmsMesh->bChartDistanceFactor = true;
	ArmsMesh->bGenerateOverlapEvents = false;
	ArmsMesh->AttachParent = BaseMesh;
	ArmsMesh->SetMasterPoseComponent(BaseMesh);

	ChestMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Chest"));
	ChestMesh->AlwaysLoadOnClient = true;
	ChestMesh->AlwaysLoadOnServer = true;
	ChestMesh->bOwnerNoSee = false;
	ChestMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	ChestMesh->bCastDynamicShadow = true;
	ChestMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	ChestMesh->bChartDistanceFactor = true;
	ChestMesh->bGenerateOverlapEvents = false;
	ChestMesh->AttachParent = BaseMesh;
	ChestMesh->SetMasterPoseComponent(BaseMesh);

	LegsMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Legs"));
	LegsMesh->AlwaysLoadOnClient = true;
	LegsMesh->AlwaysLoadOnServer = true;
	LegsMesh->bOwnerNoSee = false;
	LegsMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	LegsMesh->bCastDynamicShadow = true;
	LegsMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	LegsMesh->bChartDistanceFactor = true;
	LegsMesh->bGenerateOverlapEvents = false;
	LegsMesh->AttachParent = BaseMesh;
	LegsMesh->SetMasterPoseComponent(BaseMesh);

	HelmetMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Helmet"));
	HelmetMesh->AlwaysLoadOnClient = true;
	HelmetMesh->AlwaysLoadOnServer = true;
	HelmetMesh->bOwnerNoSee = false;
	HelmetMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	HelmetMesh->bCastDynamicShadow = true;
	HelmetMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	HelmetMesh->bChartDistanceFactor = true;
	HelmetMesh->bGenerateOverlapEvents = false;
	HelmetMesh->AttachParent = BaseMesh;
	HelmetMesh->SetMasterPoseComponent(BaseMesh);

}

// Called when the game starts or when spawned
void AOrionMenuCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrionMenuCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AOrionMenuCharacter::UpdateData(int32 index)
{
#if !IS_SERVER
	FCharacterData Data = UOrionTCPLink::CharacterDatas[index];

	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	//set our armors
	if (GRI)
	{
		AOrionPlayFabInventoryMapper *Mapper = GRI->Mapper;

		if (Mapper)
		{
			FInventoryMapping *Item = Mapper->InventoryMappings.FindByKey(Data.HelmetID.ItemName);
			if (Item && Item->ItemLink)
			{
				UOrionInventoryItem *HelmetItem = Cast<UOrionInventoryItem>(Item->ItemLink->GetDefaultObject());
				EventSetArmor(ITEM_HELMET, HelmetItem);
			}

			Item = Mapper->InventoryMappings.FindByKey(Data.ChestID.ItemName);
			if (Item && Item->ItemLink)
			{
				UOrionInventoryItem *ChestItem = Cast<UOrionInventoryItem>(Item->ItemLink->GetDefaultObject());
				EventSetArmor(ITEM_CHEST, ChestItem);

				Item = Mapper->InventoryMappings.FindByKey(Data.ChestID.ItemName);
			}

			Item = Mapper->InventoryMappings.FindByKey(Data.HandsID.ItemName);
			if (Item && Item->ItemLink)
			{
				UOrionInventoryItem *ArmsItem = Cast<UOrionInventoryItem>(Item->ItemLink->GetDefaultObject());
				EventSetArmor(ITEM_HANDS, ArmsItem);
			}

			Item = Mapper->InventoryMappings.FindByKey(Data.LegsID.ItemName);
			if (Item && Item->ItemLink)
			{
				UOrionInventoryItem *LegsItem = Cast<UOrionInventoryItem>(Item->ItemLink->GetDefaultObject());
				EventSetArmor(ITEM_LEGS, LegsItem);
			}
		}
	}
#endif
}

