// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionAOEVolume.h"

AOrionAOEVolume::AOrionAOEVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SphereMesh = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	SphereMesh->bGenerateOverlapEvents = true;
	SphereMesh->OnComponentBeginOverlap.AddDynamic(this, &AOrionAOEVolume::OnEnter);
	SphereMesh->OnComponentEndOverlap.AddDynamic(this, &AOrionAOEVolume::OnLeave);

	RootComponent = SphereMesh;

	PrimaryActorTick.bCanEverTick = true;

	AOEScale = 1.0f;

	Duration = 20.0f;
}

void AOrionAOEVolume::OnEnter(AActor *Other, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &Hit)
{
	//apply any special status effects
	AOrionCharacter *Pawn = Cast<AOrionCharacter>(Other);
	if (Pawn && Role == ROLE_Authority)
	{
		if (BuffClass)
		{
			AController *cOwner = Cast<AController>(GetOwner());
			if (!cOwner)
			{
				APawn *P = Cast<APawn>(GetOwner());
				if (P)
					cOwner = P->Controller;
			}

			Pawn->AddBuff(BuffClass, cOwner, TeamIndex);
		}

		Players.AddUnique(Pawn);
	}
}

void AOrionAOEVolume::OnLeave(AActor *Other, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	//remove status effects
	AOrionCharacter *Pawn = Cast<AOrionCharacter>(Other);
	if (Pawn)
	{
		Players.Remove(Pawn);
	}
}

void AOrionAOEVolume::BeginPlay()
{
	Super::BeginPlay();

	//spawn the emitter at our locaion
	if (AOEEffect)
	{
		//AOEPSC = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AOEEffect, GetActorLocation(), GetActorRotation());

		if (AOEPSC)
		{
			AOEPSC->SetWorldScale3D(FVector(AOEScale));
		}
	}

	FTimerHandle DestroyHandle;
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &AOrionAOEVolume::DestroyAOE, Duration, false);

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AOrionAOEVolume::ReTickPlayers, 1.5f, true);
}

void AOrionAOEVolume::DestroyAOE()
{
	if (AOEPSC)
	{
		AOEPSC->DeactivateSystem();
		AOEPSC->DestroyComponent();
		AOEPSC = nullptr;
	}

	Destroy();
}

void AOrionAOEVolume::ReTickPlayers()
{
	AController *cOwner = Cast<AController>(GetOwner());

	if (BuffClass)
	{
		if (!cOwner)
		{
			APawn *P = Cast<APawn>(GetOwner());
			if (P)
				cOwner = P->Controller;
		}
	}

	for (int32 i = 0; i < Players.Num(); i++)
	{
		Players[i]->AddBuff(BuffClass, cOwner, TeamIndex);
	}
}

void AOrionAOEVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
