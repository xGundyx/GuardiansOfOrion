// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionDestructible.h"


// Sets default values
AOrionDestructible::AOrionDestructible(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DestructibleComponent = ObjectInitializer.CreateOptionalDefaultSubobject<UDestructibleComponent>(this, TEXT("Destructible"));

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

void AOrionDestructible::EditorReplacedActor(AActor * OldActor)
{
	Super::EditorReplacedActor(OldActor);

	ADestructibleActor *DActor = Cast<ADestructibleActor>(OldActor);
	if (DActor)
	{
		DestructibleComponent->SetSkeletalMesh(DActor->GetDestructibleComponent()->SkeletalMesh);
	}
	//EventCopyProperties(OldActor);
}
