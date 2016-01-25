// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionProjectile.h"
#include "OrionWeaponFlameThrower.h"

void AOrionWeaponFlameThrower::FireWeapon()
{
	//if (!CanFire())
	//	return;

	//basically we just play a flame particle, and then spawn a bunch of invisible colliding projectiles that do the actual damage
	if (Role == ROLE_Authority)
	{
		if (!ProjectileClass || !MyPawn)
			return;

		//make sure we have a projectile class
		AOrionProjectile *Proj = Cast<AOrionProjectile>(ProjectileClass.GetDefaultObject());
		if (Proj)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Instigator = Instigator;
			SpawnInfo.Owner = Instigator;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FVector Origin(0.0f);

			USkeletalMeshComponent* UseMesh = GetWeaponMesh(MyPawn->IsFirstPerson());

			if (UseMesh)
				Origin = UseMesh->GetSocketLocation("FlameSocket");

			AOrionProjectile *Proj = GetWorld()->SpawnActor<AOrionProjectile>(ProjectileClass, Origin, MyPawn->GetActorRotation(), SpawnInfo);

			if (Proj)
			{
				Proj->SetLifeSpan(0.35f);
				if (Proj->ProjectileMovement)
				{
					FVector vel = GetAdjustedAim() * 1000.0f;

					Proj->ProjectileMovement->Velocity = vel;
					Proj->NewVelocity = vel;
				}
			}
		}
	}

	//make sure the flame effect is playing
	EventPlayFlames();

	GetWorldTimerManager().SetTimer(FireTimer, this, &AOrionWeapon::FireWeapon, 0.1f, false);
}

void AOrionWeaponFlameThrower::StopFire()
{
	Super::StopFire();

	EventStopFlames();
}


