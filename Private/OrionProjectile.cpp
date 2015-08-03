// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AOrionProjectile::AOrionProjectile(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	// Use a sphere as a simple collision representation
	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");			// Collision profiles are defined in DefaultEngine.ini
	CollisionComp->OnComponentHit.AddDynamic(this, &AOrionProjectile::OnHit);		// set up a notification for when this component overlaps something
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); //no collision for now, just move to the endpoint and vanish
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	PrimaryActorTick.bCanEverTick = true;
}

void AOrionProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AOrionProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ValidatePosition();
}

void AOrionProjectile::ValidatePosition()
{
	if (FVector::DotProduct((TargetPos - GetActorLocation()).GetSafeNormal(), TargetDir) < 0.8f)
	{
		if (TracerPSC)
		{
			TracerPSC->DestroyComponent();
			TracerPSC = nullptr;
		}

		Destroy();
	}
}

void AOrionProjectile::Init(UParticleSystem* PS, FVector StartPoint, FVector EndPoint)
{
	TargetPos = EndPoint;
	StartPos = StartPoint;

	TargetDir = (TargetPos - StartPos).GetSafeNormal();

	if (PS)
	{
		//FVector Origin = GetMesh()->GetSocketLocation(RepWeaponFireData.socket);
		//FVector vDir = RepWeaponFireData.pos - Origin;
		TracerPSC = UGameplayStatics::SpawnEmitterAttached(PS, CollisionComp);
		//UParticleSystemComponent* TracerPSC = UGameplayStatics::SpawnEmitterAtLocation(this, PS, GetActorLocation(), GetActorRotation());
		if (TracerPSC)
		{
			TracerPSC->SetWorldScale3D(FVector(1.0));
		}
	}
}

void AOrionProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	/*if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}*/
	
	if (TracerPSC)
	{
		TracerPSC->DestroyComponent();
		TracerPSC = nullptr;
	}

	Destroy();
}