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
	//DestructibleComponent = ObjectInitializer.CreateOptionalDefaultSubobject<UDestructibleComponent>(this, TEXT("Destructible"));

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AOrionDestructible::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetDestructibleComponent())
	{
		//GetDestructibleComponent()->DestructibleMesh->DefaultDestructibleParameters.Flags.bDebrisTimeout = true;
		//GetDestructibleComponent()->DestructibleMesh->DefaultDestructibleParameters.SpecialHierarchyDepths.bEnableDebris = true;
		//GetDestructibleComponent()->DestructibleMesh->DefaultDestructibleParameters.SpecialHierarchyDepths.DebrisDepth = 1;
	}
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

#if WITH_EDITOR
void ADestructibleActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ADestructibleActor, bAffectNavigation))
	{
		GetDestructibleComponent()->SetCanEverAffectNavigation(bAffectNavigation);
	}
}
#endif // WITH_EDITOR

void ADestructibleActor::PostLoad()
{
	Super::PostLoad();

	if (GetDestructibleComponent())
	{
		GetDestructibleComponent()->bCanEverAffectNavigation = bAffectNavigation;
	}
}
