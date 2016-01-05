// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionPlaceableItem.h"


// Sets default values
AOrionPlaceableItem::AOrionPlaceableItem(const FObjectInitializer& ObjectInitializer)
{
	USceneComponent *SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("WeaponComponent"));
	RootComponent = SceneComponent;

	ItemMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("ItemMesh"));
	ItemMesh->AttachParent = SceneComponent;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AOrionPlaceableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrionPlaceableItem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

FVector2D AOrionPlaceableItem::GetAim(FVector WorldAim)
{
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(WorldAim);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return FVector2D(AimRotLS.Yaw, AimRotLS.Pitch);
}
