// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionPickupOrb.h"


// Sets default values
AOrionPickupOrb::AOrionPickupOrb(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = ObjectInitializer.CreateOptionalDefaultSubobject<USphereComponent>(this, TEXT("Sphere"));
	
	RootComponent = SphereCollision;
}

// Called when the game starts or when spawned
void AOrionPickupOrb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrionPickupOrb::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AOrionPickupOrb::Init()
{
	int32 RandNum = FMath::RandRange(1, 100);

	if (RandNum > 10)//health
		OrbType = ORB_HEALTH;
	else if (RandNum > 30)//shield
		OrbType = ORB_SHIELD;
	else//exp boost
		OrbType = ORB_EXP;

	//setup the orb particle
	EventInitOrb();
}

