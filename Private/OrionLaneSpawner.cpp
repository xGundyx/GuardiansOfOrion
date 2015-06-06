// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionLaneSpawner.h"


// Sets default values
AOrionLaneSpawner::AOrionLaneSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TheScene = ObjectInitializer.CreateOptionalDefaultSubobject<USceneComponent>(this, TEXT("TheScene"));

	RootComponent = TheScene;

	PreviewMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Preview"));
	PreviewMesh->AlwaysLoadOnClient = false;
	PreviewMesh->AlwaysLoadOnServer = true;
	PreviewMesh->bHiddenInGame = true;
	PreviewMesh->AttachParent = TheScene;

	TheSpline = ObjectInitializer.CreateOptionalDefaultSubobject<USplineComponent>(this, TEXT("TheSpline"));
	TheSpline->AttachParent = TheScene;
}

// Called when the game starts or when spawned
void AOrionLaneSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrionLaneSpawner::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

AOrionCharacter *AOrionLaneSpawner::GetSpawnObject()
{
	if (ClassToSpawn)
		return Cast<AOrionCharacter>(ClassToSpawn->GetDefaultObject());

	return nullptr;
}

#if WITH_EDITOR
void AOrionLaneSpawner::LoadedFromAnotherClass(const FName& OldClassName)
{
}

void AOrionLaneSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UProperty* PropertyThatChanged = PropertyChangedEvent.Property;
	FName PropertyName = PropertyThatChanged != NULL ? PropertyThatChanged->GetFName() : NAME_None;

	if (PropertyName == FName(TEXT("ClassToSpawn")))
	{
		EventChangeMesh();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

