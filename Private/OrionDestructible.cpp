// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "PhysicsPublic.h"
#include "Engine/DestructibleMesh.h"
#include "Engine/DestructibleFractureSettings.h"
#include "OrionDestructible.h"


// Sets default values
AOrionDestructible::AOrionDestructible(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AOrionDestructible::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void AOrionDestructible::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AOrionDestructible::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

float AOrionDestructible::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (Cast<AOrionPlayerController>(EventInstigator))
		Damage *= 4.0f;

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}
