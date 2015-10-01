// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"

AOrionImpactEffect::AOrionImpactEffect(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	bAutoDestroyWhenFinished = true;
}

void AOrionImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	AOrionCharacter *Pawn = Cast<AOrionCharacter>(SurfaceHit.GetActor());
	if (Pawn)
	{
		if (Pawn->Shield > 0.0f)
			HitSurfaceType = ORION_SURFACE_Shield;
		else
			HitSurfaceType = ORION_SURFACE_Flesh;
	}

	AOrionCharacter *Droid = Cast<AOrionDroidPawn>(SurfaceHit.GetActor());
	if (Droid)
		HitSurfaceType = ORION_SURFACE_Shield;

	if (Pawn && Pawn->bIsHealableMachine)
		HitSurfaceType = ORION_SURFACE_Shield;

	// show particles
	UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);
	if (ImpactFX)
	{
		UParticleSystemComponent *PSC = UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());

		if (PSC)
		{
			PSC->SetWorldScale3D(GetImpactScale(HitSurfaceType));
		}
	}

	// play sound
	USoundCue* ImpactSound = GetImpactSound(HitSurfaceType);
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (DefaultDecal.DecalMaterial)
	{
		FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
		RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

		UGameplayStatics::SpawnDecalAttached(DefaultDecal.DecalMaterial, FVector(DefaultDecal.DecalSize, DefaultDecal.DecalSize, 1.0f),
			SurfaceHit.Component.Get(), SurfaceHit.BoneName,
			SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
			DefaultDecal.LifeSpan);
	}
}

UParticleSystem* AOrionImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	UParticleSystem* ImpactFX = NULL;

	switch (SurfaceType)
	{
		case ORION_SURFACE_Concrete:	ImpactFX = ConcreteFX; break;
		case ORION_SURFACE_Dirt:		ImpactFX = DirtFX; break;
		case ORION_SURFACE_Water:		ImpactFX = WaterFX; break;
		case ORION_SURFACE_Metal:		ImpactFX = MetalFX; break;
		case ORION_SURFACE_Wood:		ImpactFX = WoodFX; break;
		case ORION_SURFACE_Grass:		ImpactFX = GrassFX; break;
		case ORION_SURFACE_Glass:		ImpactFX = GlassFX; break;
		case ORION_SURFACE_Flesh:		ImpactFX = FleshFX; break;
		case ORION_SURFACE_Shield:		ImpactFX = ShieldFX; break; 
		default:						ImpactFX = DefaultFX; break;
	}

	return ImpactFX;
}

USoundCue* AOrionImpactEffect::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	USoundCue* ImpactSound = NULL;

	switch (SurfaceType)
	{
		case ORION_SURFACE_Concrete:	ImpactSound = ConcreteSound; break;
		case ORION_SURFACE_Dirt:		ImpactSound = DirtSound; break;
		case ORION_SURFACE_Water:		ImpactSound = WaterSound; break;
		case ORION_SURFACE_Metal:		ImpactSound = MetalSound; break;
		case ORION_SURFACE_Wood:		ImpactSound = WoodSound; break;
		case ORION_SURFACE_Grass:		ImpactSound = GrassSound; break;
		case ORION_SURFACE_Glass:		ImpactSound = GlassSound; break;
		case ORION_SURFACE_Flesh:		ImpactSound = FleshSound; break;
		case ORION_SURFACE_Shield:		ImpactSound = ShieldSound; break;
		default:						ImpactSound = DefaultSound; break;
	}

	return ImpactSound;
}

FVector AOrionImpactEffect::GetImpactScale(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	FVector Scale = FVector(1.0f);

	switch (SurfaceType)
	{
	case ORION_SURFACE_Concrete:	Scale = FVector(1.0f); break;
	case ORION_SURFACE_Dirt:		Scale = FVector(1.0f); break;
	case ORION_SURFACE_Water:		Scale = FVector(1.0f); break;
	case ORION_SURFACE_Metal:		Scale = FVector(1.0f); break;
	case ORION_SURFACE_Wood:		Scale = FVector(1.0f); break;
	case ORION_SURFACE_Grass:		Scale = FVector(1.0f); break;
	case ORION_SURFACE_Glass:		Scale = FVector(1.0f); break;
	case ORION_SURFACE_Flesh:		Scale = FVector(2.5f); break;
	case ORION_SURFACE_Shield:		Scale = FVector(2.5f); break;
	default:						Scale = FVector(1.0f); break;
	}

	return Scale;
}
