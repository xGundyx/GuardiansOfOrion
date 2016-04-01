// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionProjectile.h"
#include "OrionAbility.h"
#include "OrionWeapon.h"
#include "OrionGrenade.h"
#include "OrionLobbyPawn.h"
#include "OrionSkeletalMeshComponent.h"

class AOrionLobbyPawn;

//auto rifle -3.0 30.0 -13.5
//auto pistol -4.7, 12.65, -8.04

AOrionWeapon::AOrionWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	USceneComponent *SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("WeaponComponent"));
	RootComponent = SceneComponent;

	Mesh1P = ObjectInitializer.CreateDefaultSubobject<UOrionSkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	//Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	//Mesh1P->bChartDistanceFactor = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh1P->bPerBoneMotionBlur = false;
	Mesh1P->AttachParent = SceneComponent;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Mesh1P->WeaponFOV = 60.0f;
	//RootComponent = Mesh1P;

	Mesh3P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));
	//Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	//Mesh3P->bChartDistanceFactor = true;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh3P->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	Mesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mesh3P->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	Mesh3P->AttachParent = SceneComponent; 
	Mesh3P->BoundsScale = 2.0f;
	Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;

	KnifeMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("KnifeMesh"));
	//Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	//KnifeMesh->bChartDistanceFactor = true;
	KnifeMesh->bReceivesDecals = false;
	KnifeMesh->CastShadow = true;
	KnifeMesh->SetCollisionObjectType(ECC_WorldDynamic);
	KnifeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	KnifeMesh->AttachParent = SceneComponent;
	KnifeMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	KnifeMesh->SetHiddenInGame(true); //hide knife until animation is needed

	static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> FireRumble(TEXT("ForceFeedbackEffect'/Game/DamageTypes/ControllerRumbles/SmallBite.SmallBite'"));
	if (FireRumble.Object)
		FireRumbleEffect = Cast<UForceFeedbackEffect>(FireRumble.Object);

	//bLoopedMuzzleFX = false;
	//bLoopedFireAnim = false;
	//bPlayingFireAnim = false;
	//bIsEquipped = false;
	//bWantsToFire = false;
	//bPendingReload = false;
	//bPendingEquip = false;
	//CurrentState = EWeaponState::Idle;

	//CurrentAmmo = 0;
	//CurrentAmmoInClip = 0;
	//BurstCounter = 0;
	//LastFireTime = 0.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	//bReplicateInstigator = true;
	bNetUseOwnerRelevancy = true;

	InstantConfig.AimFOV = 70.0f;
	InstantConfig.AimSpread = 0.0f;
	InstantConfig.AllowedViewDotHitDir = 0.25f;
	InstantConfig.AttachPoint = FName("");
	InstantConfig.bAutomatic = false;
	InstantConfig.bSingleShellReload = false;
	InstantConfig.ClientSideHitLeeway = 1.0;
	InstantConfig.ClipSize = 12;
	InstantConfig.CrouchSpread = 0.0f;
	InstantConfig.FiringSpreadIncrement = 0;
	InstantConfig.FiringSpreadMax = 0;
	InstantConfig.FOVTransitionTime = 0.2f;
	InstantConfig.HitDamage = 50;
	InstantConfig.MaxAmmo = 500;
	InstantConfig.MuzzleScale = 1.0f;
	InstantConfig.NormalSpread = 0.0f;
	InstantConfig.NumPellets = 1;
	InstantConfig.RecoilScale = 1.0f;
	InstantConfig.TimeBetweenShots = 0.1;
	InstantConfig.WeaponIndex = 0;
	InstantConfig.WeaponRange = 6000.0f;
	InstantConfig.WeaponScale = 0.5f;

	InventoryType = ITEM_PRIMARYWEAPON;

	LaserAimPSC = nullptr;
}

void AOrionWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitMaterials();

	AmmoInClip = GetClipSize();
	Ammo = InstantConfig.MaxAmmo;

	TargetFOV = 90.0;

	//Mesh1P->SetWorldScale3D(FVector(InstantConfig.WeaponScale));
	Mesh1P->UpdateBounds();

	//PlayWeaponAnimation(IdleAnim);

	DetachMeshFromPawn();
}

void AOrionWeapon::InitClip()
{
	if (MyPawn)
	{
		int32 ClipDelta = FMath::Min(GetClipSize() - AmmoInClip, Ammo - AmmoInClip);

		if (ClipDelta > 0)
		{
			AmmoInClip += ClipDelta;
		}
	}
}

FVector AOrionWeapon::GetLeftHandOffset() const
{
	return InstantConfig.LeftHandOffset;
}

void AOrionWeapon::AttachMeshToPawn()
{
	if (MyPawn)
	{
		// Remove and hide both first and third person meshes
		DetachMeshFromPawn();

		//PlayWeaponAnimation(IdleAnim);

		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
		AOrionLobbyPawn *LPawn = Cast<AOrionLobbyPawn>(MyPawn);
		bool bLobby = false;
		if (GRI) bLobby = GRI->bIsLobby || LPawn != nullptr;

		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		FName AttachPoint = InstantConfig.AttachPoint;// TEXT("WeaponPoint");// MyPawn->GetWeaponAttachPoint();
		if (MyPawn->IsLocallyControlled() == true)
		{
			AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
			USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecifcPawnMesh(true);
			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecifcPawnMesh(false);
			if (Mesh1P)
				Mesh1P->SetHiddenInGame(!MyPawn->IsFirstPerson() /*|| MyPawn->bBlinking*/ || (PC && PC->bHideWeapons) || bLobby || LPawn);
			if (PawnMesh1p)
				PawnMesh1p->SetHiddenInGame(!MyPawn->IsFirstPerson() /*|| MyPawn->bBlinking*/ || (PC && PC->bHideWeapons) || bLobby || LPawn);
			if (Mesh3P)
				Mesh3P->SetHiddenInGame(MyPawn->IsFirstPerson() || bLobby || LPawn);// || MyPawn->bBlinking);
			//Mesh1P->AttachTo(PawnMesh1p, AttachPoint);
			if (PawnMesh1p && PawnMesh1p->SkeletalMesh)
				Mesh1P->AttachTo(PawnMesh1p, AttachPoint); //AttachRootComponentTo(PawnMesh1p, AttachPoint);// , EAttachLocation::KeepWorldPosition);
			if (Mesh3P && Mesh3P->SkeletalMesh)
				Mesh3P->AttachTo(PawnMesh3p, AttachPoint);

			//make sure knife is hidden
			if (KnifeMesh && KnifeMesh->SkeletalMesh)
			{
				KnifeMesh->SetHiddenInGame(true);
				KnifeMesh->AttachTo(PawnMesh3p, "KnifeSocket");
			}

			//add it to the outliner
			if (PC && (!PC->DropPod || PC->DropPod->bHasLanded))
			{
				if (MyPawn->PlayerState && !MyPawn->PlayerState->bIsABot)
				{
					Mesh3P->SetRenderCustomDepth(!bLobby);
					if (PC)
						Mesh3P->CustomDepthStencilValue = PC->ControllerIndex + 1;
				}
			}

			if (PawnMesh1p && PawnMesh1p->SkeletalMesh)
			{
				PawnMesh1p->SetWorldScale3D(FVector(InstantConfig.WeaponScale));
				//PawnMesh1p->SetWorldScale3D(FVector(1.0f, 0.6f, 1.0f)*InstantConfig.WeaponScale);
				//Mesh1P->SetWorldScale3D(FVector(1.0f, 0.6f, 1.0f)*InstantConfig.WeaponScale);
				PawnMesh1p->UpdateBounds();
			}

			//Mesh3P->SetRelativeLocation(FVector(0, 49.9, 1.0));
			//Mesh3P->SetRelativeRotation(FRotator(0, 0, -5));

			StopAiming();
		}
		else
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh(false);
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			if (UseWeaponMesh && UsePawnMesh)
			{
				UseWeaponMesh->AttachTo(UsePawnMesh, AttachPoint);
				UseWeaponMesh->SetHiddenInGame(false || MyPawn->bBlinking || LPawn || bLobby);
			}

			//make sure knife is hidden
			if (KnifeMesh && UsePawnMesh && KnifeMesh->SkeletalMesh)
			{
				KnifeMesh->SetHiddenInGame(true);
				KnifeMesh->AttachTo(UsePawnMesh, "KnifeSocket");
			}

			//make sure it's not in the outliner
			Mesh3P->SetRenderCustomDepth(false);
		}

		InitMaterials();

		//update our cloak values by telling the player about us
		if (MyPawn->CurrentSkill)
			MyPawn->CurrentSkill->EventCheckMaterials();
	}
}

void AOrionWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MyPawn && MyPawn->GetWeapon() == this && MyPawn->IsLocallyControlled())
	{
		//HandleZooms(DeltaSeconds);
		//HandleViewOffsets(DeltaSeconds);

		if (CurrentFiringSpread > 0.0f && GetWorld()->TimeSeconds - LastFireTime > 0.5f)
			CurrentFiringSpread = FMath::Max(0.0f, CurrentFiringSpread - DeltaSeconds*20.0f*InstantConfig.FiringSpreadIncrement);

		//if we need to reload, try to start it automatically
		if (AmmoInClip == 0 && WeaponState != WEAP_RELOADING && Ammo > 0 && CanReload())
			StartReload();
	}

	if (MyPawn && MyPawn->GetWeapon() != this)
	{
		if (LaserAimPSC)
		{
			LaserAimPSC->DeactivateSystem();
			LaserAimPSC = nullptr;
		}

		if (Mesh1P->bHiddenInGame == false)
			Mesh1P->SetHiddenInGame(true);

		if (Mesh3P->bHiddenInGame == false)
			Mesh3P->SetHiddenInGame(true);
	}
}

void AOrionWeapon::HandleZooms(float DeltaSeconds)
{
	if (MyPawn && MyPawn->FirstPersonCameraComponent->FieldOfView != TargetFOV)
	{
		if (MyPawn->FirstPersonCameraComponent->FieldOfView > TargetFOV)
		{
			MyPawn->FirstPersonCameraComponent->FieldOfView = FMath::Max(MyPawn->FirstPersonCameraComponent->FieldOfView - (DeltaSeconds / InstantConfig.FOVTransitionTime)*(90.0f - InstantConfig.AimFOV), TargetFOV);
		}
		else
		{
			MyPawn->FirstPersonCameraComponent->FieldOfView = FMath::Min(MyPawn->FirstPersonCameraComponent->FieldOfView + (DeltaSeconds / InstantConfig.FOVTransitionTime)*(90.0f - InstantConfig.AimFOV), TargetFOV);
		}
	}
}

void AOrionWeapon::HandleViewOffsets(float DeltaSeconds)
{
	float DeltaTime = GetWorld()->GetTimeSeconds() - LastAimTime;
	if (DeltaTime - 0.15f < InstantConfig.FOVTransitionTime)
	{
		if (bAiming)
		{
			Mesh1P->SetRelativeLocation(ViewOffset + (FMath::Clamp((DeltaTime / InstantConfig.FOVTransitionTime), 0.0f, 1.0f)*(AimViewOffset - ViewOffset)));
		}
		else
		{
			Mesh1P->SetRelativeLocation(AimViewOffset + (FMath::Clamp((DeltaTime / InstantConfig.FOVTransitionTime), 0.0f, 1.0f)*(ViewOffset - AimViewOffset)));
		}
	}
	else
	{
		Mesh1P->SetRelativeLocation(bAiming ? AimViewOffset : ViewOffset);
	}
}

bool AOrionWeapon::IsAiming()
{
	return bAiming;
}

int32 AOrionWeapon::GetWeaponIndex()
{
	return InstantConfig.WeaponIndex;
}

void AOrionWeapon::StartAiming(bool bPlaySound)
{
	if (MyPawn && bPlaySound && ZoomInSound && Cast<AOrionLobbyPawn>(MyPawn) == nullptr)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ZoomInSound, MyPawn->GetActorLocation());

	bAiming = true;
	LastAimTime = GetWorld()->TimeSeconds;
	//Mesh1P->SetRelativeLocation(AimViewOffset);

	//MyPawn->FirstPersonCameraComponent->FieldOfView = InstantConfig.AimFOV;
	TargetFOV = InstantConfig.AimFOV;

	//PlayWeaponAnimation(AimAnim);

	//disable lasers for now
	/*if (LaserAimPSC == nullptr && LaserAimFX != nullptr && MyPawn && MyPawn->IsLocallyControlled())
	{
		LaserAimPSC = UGameplayStatics::SpawnEmitterAttached(LaserAimFX, GetWeaponMesh(MyPawn->IsFirstPerson()), "LaserAim");
		if (LaserAimPSC)
		{
		}
	}

	if (LaserAimPSC)
		LaserAimPSC->ActivateSystem();*/
}

void AOrionWeapon::StopAiming()
{
	if (bAiming && MyPawn && ZoomOutSound && Cast<AOrionLobbyPawn>(MyPawn) == nullptr)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ZoomOutSound, MyPawn->GetActorLocation());

	bAiming = false;
	LastAimTime = GetWorld()->TimeSeconds;
	//Mesh1P->SetRelativeLocation(ViewOffset);

	//MyPawn->FirstPersonCameraComponent->FieldOfView = 90;
	TargetFOV = 90.0f;

	//StopWeaponAnimation(AimAnim);

	//have to destroy it since it's a beam, or could hide it maybe?
	if (LaserAimPSC)
	{
		LaserAimPSC->DeactivateSystem();
		LaserAimPSC = nullptr;
	}
}

USkeletalMeshComponent* AOrionWeapon::GetWeaponMesh(bool bFirstPerson) const
{
	if (bFirstPerson && Mesh1P)
		if (Mesh1P->SkeletalMesh == nullptr)
			return nullptr;

	if (!bFirstPerson && Mesh3P)
		if (Mesh3P->SkeletalMesh == nullptr)
			return nullptr;

	return bFirstPerson ? Mesh1P : Mesh3P;
}

void AOrionWeapon::DetachMeshFromPawn()
{
	//remove laser
	if (LaserAimPSC)
	{
		LaserAimPSC->DeactivateSystem();
		LaserAimPSC = nullptr;
	}

	////Mesh1P->DetachFromParent();
	Mesh1P->SetHiddenInGame(true);

	////Mesh3P->DetachFromParent();
	Mesh3P->SetHiddenInGame(true);
}

void AOrionWeapon::Destroyed()
{
	Super::Destroyed();

	StopSimulatingWeaponFire();
}

void AOrionWeapon::Melee()
{
	//no double clutching!
	if (WeaponState == WEAP_MELEE)
		return;

	//no melee during weapon switches?
	if (WeaponState == WEAP_EQUIPPING || WeaponState == WEAP_PUTTINGDOWN)
		return;

	StopFire();

	if (Role < ROLE_Authority)
	{
		ServerMelee();
	}

	DoMelee();
}

float AOrionWeapon::DoMelee()
{
	if (WeaponState == WEAP_RELOADING)
		CancelReload();

	WeaponState = WEAP_MELEE;

	if (bAiming)
		StopAiming();

	if (LaserAimPSC)
	{
		LaserAimPSC->DeactivateSystem();
		LaserAimPSC = nullptr;
	}

	float Len = 0.1f;
	
	if (MyPawn && MyPawn->bKnockedDown)
		Len = PlayWeaponAnimation(MeleeKnockDownAnim, Role == ROLE_Authority);
	else if (MyPawn && MyPawn->bLatchedOnto)
		Len = PlayWeaponAnimation(MyPawn->CutTongueAnim, Role == ROLE_Authority);
	else
		Len = PlayWeaponAnimation(MeleeAnim, Role == ROLE_Authority);

	//hide weapon and show knife
	if (MyPawn && !MyPawn->bKnockedDown)
	{
		bKnifing = true;
		Mesh3P->SetHiddenInGame(true);
		KnifeMesh->SetHiddenInGame(false);
	}

	if (Len < 0.1f)
		Len = 0.1f;

	GetWorldTimerManager().SetTimer(MeleeTimer, this, &AOrionWeapon::ResetMelee, Len, false);

	//if we are being tongued, cut it and cancel it
	if (Role == ROLE_Authority && MyPawn && MyPawn->bLatchedOnto)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AOrionWeapon::BreakTongue, Len, false);
	}

	return Len;
}

void AOrionWeapon::BreakTongue()
{
	if (MyPawn && MyPawn->bLatchedOnto)
	{
		MyPawn->bStopSpecialMove = true;

		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, MyPawn, &AOrionCharacter::ResetStopSpecialMove, 1.0f, false);
	}
}

void AOrionWeapon::ResetMelee()
{
	WeaponState = WEAP_IDLE;

	//unhide weapon and hide knife
	bKnifing = false;
	Mesh3P->SetHiddenInGame(false);
	KnifeMesh->SetHiddenInGame(true);
}

void AOrionWeapon::StartFire()
{
	if (!MyPawn || BurstCounter > 0)
		return;

	//can't fire when punching stuff
	if (WeaponState == WEAP_MELEE)
		return;

	if (MyPawn->IsLocallyControlled())
	{
		if (AmmoInClip == 0 && WeaponState != WEAP_RELOADING && Ammo > 0 && CanReload())
		{
			StartReload();
			return;
		}
	}

	if (CanFire() == false)
		return;

	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}
	else
	{
		if (MyPawn && MyPawn->CurrentSkill && MyPawn->CurrentSkill->IsCloaking())
			MyPawn->CurrentSkill->DeactivateSkill();// DepleteEnergy();
		if (MyPawn && MyPawn->CurrentSecondarySkill && MyPawn->CurrentSecondarySkill->IsCloaking())
			MyPawn->CurrentSecondarySkill->DeactivateSkill();// DepleteEnergy();
	}

	/*if (!bWantsToFire)
	{
	bWantsToFire = true;
	DetermineWeaponState();
	}*/

	if (WeaponState == WEAP_RELOADING)
		CancelReload();

	if (WeaponState == WEAP_MELEE)
		CancelMelee();

	float FireRate = InstantConfig.TimeBetweenShots;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
	if (PC)
	{
		FireRate *= 1.0f - float(PC->GetSkillValue(SKILL_FIRERATE)) / 100.0f;

		if (PC->HasOrbEffect(ORB_ROF))
			FireRate *= 0.75f;

		//AOrionInventoryManager *Inv = PC->GetInventoryManager();
		//if (Inv && Inv->HasStat(RARESTAT_NORELOAD) && InstantConfig.WeaponSlot == 1)
		//	FireRate *= 2.0f;
	}

	if (InstantConfig.bBurst)
	{
		BurstCounter = 3;

		if (GetWorld()->GetTimeSeconds() - LastFireTime < InstantConfig.TimeBetweenShots)
			GetWorldTimerManager().SetTimer(BurstTimer, this, &AOrionWeapon::FireBurst, LastFireTime + FireRate - GetWorld()->GetTimeSeconds(), false);
		else
			FireBurst();
	}
	else if (FireRate > 0 && LastFireTime + FireRate > GetWorld()->GetTimeSeconds())
		GetWorldTimerManager().SetTimer(FireTimer, this, &AOrionWeapon::FireWeapon, LastFireTime + FireRate - GetWorld()->GetTimeSeconds(), false);
	else
		FireWeapon();
}

void AOrionWeapon::CancelReload()
{
	GetWorldTimerManager().ClearTimer(ReloadTimer);// this, &AOrionWeapon::StopReload);
	GetWorldTimerManager().ClearTimer(ReloadStopTimer);
	//GetWorldTimerManager().ClearTimer(this, &AOrionWeapon::ReloadWeapon);

	WeaponState = WEAP_IDLE;
}

void AOrionWeapon::CancelMelee()
{
	GetWorldTimerManager().ClearTimer(MeleeTimer);// this, &AOrionWeapon::ResetMelee);

	WeaponState = WEAP_IDLE;
}

void AOrionWeapon::FireProjectile(FName SocketName, FVector Direction)
{
	if (!ProjectileClass || !MyPawn || SocketName == "")
		return;

	//make sure we have a projectile class
	AOrionGrenade *Grenade = Cast<AOrionGrenade>(ProjectileClass.GetDefaultObject());
	if (Grenade)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = Instigator;
		SpawnInfo.Owner = Instigator;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector Origin(0.0f);

		if (Cast<AOrionDroidPawn>(MyPawn))
			Origin = MyPawn->GetMesh()->GetSocketLocation(SocketName);
		else
		{
			USkeletalMeshComponent* UseMesh = GetWeaponMesh(MyPawn->IsFirstPerson());

			if (UseMesh)
				Origin = UseMesh->GetSocketLocation(SocketName);
		}

		AOrionGrenade *Proj = GetWorld()->SpawnActor<AOrionGrenade>(ProjectileClass, Origin, Direction.Rotation(), SpawnInfo);

		if (Proj)
		{
			Proj->Init(Direction);
			Proj->SetFuseTime(2.5f);
			Proj->GrenadeLife = Grenade->FXTime;
			Proj->GrenadeScale = Grenade->ExplosionScale;
		}
	}
	else
	{
		//rockets and stuff with no gravity
		AOrionProjectile *Proj = Cast<AOrionProjectile>(ProjectileClass.GetDefaultObject());
		if (Proj && Role == ROLE_Authority)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Instigator = Instigator;
			SpawnInfo.Owner = Instigator;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FVector Origin(0.0f);

			if (Cast<AOrionDroidPawn>(MyPawn))
				Origin = MyPawn->GetMesh()->GetSocketLocation(SocketName);
			else
			{
				USkeletalMeshComponent* UseMesh = GetWeaponMesh(MyPawn->IsFirstPerson());

				if (UseMesh)
					Origin = UseMesh->GetSocketLocation(SocketName);
			}

			AOrionProjectile *Rocket = GetWorld()->SpawnActor<AOrionProjectile>(ProjectileClass, Origin, Direction.Rotation(), SpawnInfo);

			if (Proj)
			{
				Proj->Init(nullptr, Origin, Origin + Direction * 10000.0f);
				Proj->SetLifeSpan(4.0f);
				Proj->NewVelocity = Direction * 3000.0f;
			}
		}
	}
}

void AOrionWeapon::FireSpecial(FName SocketName, FVector Direction)
{
	LastFireTime = GetWorld()->GetTimeSeconds();

	if (bProjectile)
	{
		FireProjectile(SocketName, Direction);
		return;
	}

	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = GetCurrentSpread();
	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

	SpecialSocketName = SocketName;

	const FVector AimDir = GetAdjustedAim();
	const FVector StartTrace = GetBarrelLocation(SocketName);
	const FVector ShootDir = Direction;
	const FVector EndTrace = StartTrace + ShootDir * FMath::Max(6000.0f, InstantConfig.WeaponRange);

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);

	CurrentFiringSpread = FMath::Min(InstantConfig.FiringSpreadMax, CurrentFiringSpread + InstantConfig.FiringSpreadIncrement) * GetSpreadModifier();
}

void AOrionWeapon::FireBurst()
{
	if (MyPawn && MyPawn->IsRolling())
	{
		BurstCounter = 0;
		return;
	}

	if (BurstCounter > 0)
		FireWeapon();

	BurstCounter--;

	float FireRate = InstantConfig.TimeBetweenShots;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
	if (PC)
	{
		FireRate *= 1.0f - float(PC->GetSkillValue(SKILL_FIRERATE)) / 100.0f;

		if (PC->HasOrbEffect(ORB_ROF))
			FireRate *= 0.75f;

		//AOrionInventoryManager *Inv = PC->GetInventoryManager();
		//if (Inv && Inv->HasStat(RARESTAT_NORELOAD) && InstantConfig.WeaponSlot == 1)
		//	FireRate *= 2.0f;
	}

	if (BurstCounter > 0)
		GetWorldTimerManager().SetTimer(BurstTimer, this, &AOrionWeapon::FireBurst, FireRate, false);
	else
	{
		LastFireTime = GetWorld()->GetTimeSeconds() + 0.5f;
	}
}

void AOrionWeapon::FireWeapon()
{
	if (!CanFire() || !MyPawn)
		return;

	float FireRate = InstantConfig.TimeBetweenShots;

	if (MyPawn)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
		if (PC)
		{
			FireRate *= 1.0f - float(PC->GetSkillValue(SKILL_FIRERATE)) / 100.0f;

			if (PC->HasOrbEffect(ORB_ROF))
				FireRate *= 0.75f;
		}
	}

	bool bShouldRefire = InstantConfig.WeaponSlot == 2 || (MyPawn && !MyPawn->bDowned);

	if (InstantConfig.bAutomatic && bShouldRefire)
		GetWorldTimerManager().SetTimer(FireTimer, this, &AOrionWeapon::FireWeapon, FireRate, false);

	LastFireTime = GetWorld()->GetTimeSeconds();

	UseAmmo();

	if (bProjectile)
	{
		if (MuzzleAttachPoint.Num() > 0)
			FireProjectile(MuzzleAttachPoint[0], GetAdjustedAim());
		return;
	}

	//making bullets client side only for now
	if (!MyPawn->IsLocallyControlled())
	{
		return;
	}

	for (int32 i = 0; i < InstantConfig.NumPellets; i++)
	{
		const int32 RandomSeed = FMath::Rand();
		FRandomStream WeaponRandomStream(RandomSeed);
		const float CurrentSpread = InstantConfig.WeaponName == "SNIPER RIFLE" ? (!bAiming ? 25.0f : 0.0f) : (bAiming ? 2.5f : 5.0f);// GetCurrentSpread();
		const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

		const FVector AimDir = GetAdjustedAim();
		const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
		const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
		const FVector EndTrace = StartTrace + ShootDir * FMath::Max(6000.0f, InstantConfig.WeaponRange);

		FHitResult Impact;

		AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
		AOrionInventoryManager *Inv = nullptr;
		if(PC)
			Inv = PC->GetInventoryManager();

		if (InstantConfig.WeaponName == "SNIPER RIFLE" && Inv && Inv->HasStat(RARESTAT_MARKPENETRATE))
		{
			FVector vStart = StartTrace;
			int32 Counter = 0;
			AOrionCharacter *LastHitEnemy = nullptr;
			while (true)
			{
				Impact = WeaponPenetrateTrace(vStart, EndTrace);
				ProcessInstantHit(Impact, vStart, ShootDir, RandomSeed, CurrentSpread);

				if ((Impact.ImpactPoint - EndTrace).Size() < 1.0f)
					break;

				if (Impact.ImpactPoint.Size() < 1.0f)
					return;

				FVector dir = (EndTrace - StartTrace).GetSafeNormal();

				if (FVector::DotProduct((EndTrace - Impact.ImpactPoint).GetSafeNormal(), dir) < 0.98f)
					break;

				vStart = Impact.ImpactPoint + dir * 50.0f;

				if (LastHitEnemy && Impact.GetActor() == LastHitEnemy)
					continue;

				LastHitEnemy = Cast<AOrionCharacter>(Impact.GetActor());

				Counter++;

				if (Counter >= 10)
					break;
			}
		}
		else if (MyPawn->CurrentSkill && MyPawn->CurrentSkill->IsThermalVisioning() && PC && PC->GetSkillValue(SKILL_THERMALPENETRATE) > 0)
		{
			Impact = WeaponPenetrateTrace(StartTrace, EndTrace);
			ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);
		}
		else
		{
			Impact = WeaponTrace(StartTrace, EndTrace);
			ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);
		}

		//DrawDebugSphere(GetWorld(), StartTrace, 100, 12, FColor(255, 0, 0, 255), false); 
		//DrawDebugSphere(GetWorld(), EndTrace, 100, 12, FColor(0, 255, 0, 255), false);
	}

	CurrentFiringSpread = FMath::Min(InstantConfig.FiringSpreadMax, CurrentFiringSpread + InstantConfig.FiringSpreadIncrement) * GetSpreadModifier();

	if (MyPawn)
	{
		MyPawn->AddAimKick(FRotator(FMath::FRandRange(1.0f, 2.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f));

		APlayerController* PC = Cast<APlayerController>(MyPawn->Controller);
		if (PC && PC->IsLocalPlayerController())
		{
			if (FireRumbleEffect)
			{
				PC->ClientPlayForceFeedback(FireRumbleEffect, false, "Fired");
			}
		}
	}

	//if we hit a voxel, deform it!
	/*if (Impact.GetActor())
	{
		AVoxelMeshActor* HitActor = (AVoxelMeshActor*)Impact.GetActor();
		if (HitActor != nullptr)
			HitActor->DoHit(Impact.ImpactPoint, 1.0);
	}*/
}

int32 AOrionWeapon::GetCurrentAmmo() const
{
	return Ammo;
}

int32 AOrionWeapon::GetCurrentAmmoInClip() const
{
	return AmmoInClip;
}

int32 AOrionWeapon::GetClipSize() const
{
	if (MyPawn)
	{
		int32 Size = InstantConfig.ClipSize;

		AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
		if (PC)
		{
			Size = int32(float(Size) * (1.0f + float(PC->GetSkillValue(SKILL_CLIPSIZE)) / 100.0f));

			AOrionInventoryManager *Inv = PC->GetInventoryManager();
			if (Inv && Inv->HasStat(RARESTAT_KILLRELOAD) && InstantConfig.WeaponSlot == 2)
				Size /= 2.0f;
		}

		return Size;
	}

	return InstantConfig.ClipSize;
}

int32 AOrionWeapon::GetMaxAmmo() const
{
	return InstantConfig.MaxAmmo;
}


float AOrionWeapon::GetSpreadModifier()
{
	float mod = 1.0f;

	if (MyPawn)
	{
		if (MyPawn->IsDucking())
			mod *= InstantConfig.CrouchSpread;

		if (MyPawn->IsAiming())
			mod *= InstantConfig.AimSpread;
	}

	return mod;
}

UAnimMontage* AOrionWeapon::GetIdleMontage()
{
	return nullptr;// IdleAnim.Pawn3P;
}

UAnimMontage* AOrionWeapon::GetMeleeMontage()
{
	return MeleeAnim.Pawn3P;
}

UAnimMontage* AOrionWeapon::GetReloadMontage()
{
	return ReloadAnim.Pawn3P;
}

UAnimMontage* AOrionWeapon::GetEquipMontage()
{
	return EquipAnim.Pawn3P;
}

UAnimMontage* AOrionWeapon::GetAimMontage()
{
	return nullptr;// AimAnim.Pawn3P;
}

UAnimMontage* AOrionWeapon::GetFireMontage()
{
	return FireAnim.Pawn3P;
}

UAnimMontage* AOrionWeapon::GetSprintMontage()
{
	return nullptr;// SprintAnim.Pawn3P;
}

AOrionCharacter* AOrionWeapon::GetOrionPawn()
{
	return MyPawn;
}

float AOrionWeapon::GetCurrentSpread() const
{
	return (InstantConfig.WeaponSpread + CurrentFiringSpread)*InstantConfig.NormalSpread;
}

FHitResult AOrionWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(WeaponFireTag, false, Instigator);
	TraceParams.bTraceAsyncScene = false;
	TraceParams.bReturnPhysicalMaterial = false;

	FHitResult Hit(ForceInit);
	if (GetWorld())
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	return Hit;
}

FHitResult AOrionWeapon::WeaponPenetrateTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	static FName WeaponFireTag = FName(TEXT("WeaponPenetrateTrace"));

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(WeaponFireTag, false, Instigator);
	TraceParams.bTraceAsyncScene = false;
	TraceParams.bReturnPhysicalMaterial = false;

	FVector vStart = StartTrace;

	FHitResult Hit(ForceInit);
	FHitResult RetHit(ForceInit);
	if (GetWorld())
	{
		while (true)
		{
			GetWorld()->LineTraceSingleByChannel(Hit, vStart, EndTrace, COLLISION_WEAPON, TraceParams);

			AOrionCharacter *P = Cast<AOrionCharacter>(Hit.GetActor());
			if (P)
			{
				RetHit = Hit;
				break;
			}

			if (Hit.ImpactPoint.Size() <= 1.0f)
				break;

			if ((Hit.ImpactPoint - EndTrace).Size() < 1.0f)
				break;

			FVector dir = (EndTrace - StartTrace).GetSafeNormal();

			if (FVector::DotProduct((EndTrace - Hit.ImpactPoint).GetSafeNormal(), dir) < 0.98f)
				break;

			vStart = Hit.ImpactPoint + (EndTrace - StartTrace).GetSafeNormal() * 10.0f;
		}
	}

	return RetHit;
}

FVector AOrionWeapon::GetAdjustedAim() const
{
	AController* const PlayerController = Instigator ? Instigator->Controller : NULL;
	FVector FinalAim = FVector::ZeroVector;

	if (Instigator)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Instigator->Controller);
		if (PC && PC->bThirdPersonCamera)
		{
			FVector CamLoc;
			FRotator CamRot;
			PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
			FinalAim = CamRot.Vector();

			return FinalAim;
		}
	}

	//top down aims in the general direction the pawn is facing
	if (MyPawn && MyPawn->IsTopDown())
	{
		//FinalAim = Instigator->GetActorRotation().Vector();
		FinalAim = MyPawn->AimPos - FVector(0, 0, MyPawn->bDowned ? -25.0f : 25.0f) - MyPawn->GetActorLocation();
	}
	// If we have a player controller use it for the aim
	else if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (Instigator)
	{
		// Now see if we have an AI controller - we will want to get the aim from there if we do
		/*ADHAIController* AIController = MyPawn ? Cast<ADHAIController>(MyPawn->Controller) : NULL;
		if (AIController != NULL)
		{
		FinalAim = AIController->GetControlRotation().Vector();
		}
		else
		{*/
		FinalAim = Instigator->GetBaseAimRotation().Vector();
		/*}*/
	}

	return FinalAim;
}

FVector AOrionWeapon::GetBarrelLocation(FName SocketName)
{
	//third person stuff
	FVector pos;
	FRotator rot;
	if (MyPawn && !MyPawn->IsFirstPerson()  && Mesh3P)
		Mesh3P->GetSocketWorldLocationAndRotation(SocketName, pos, rot);
	else if (MyPawn && MyPawn->IsFirstPerson() && Mesh1P)
		Mesh1P->GetSocketWorldLocationAndRotation(SocketName, pos, rot);

	return pos;
}

FVector AOrionWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const
{
	AController* PC = MyPawn ? MyPawn->Controller : NULL;
	AOrionPlayerController *MyPC = Cast<AOrionPlayerController>(PC);
	//ADHAIController* AIPC = MyPawn ? Cast<ADHAIController>(MyPawn->Controller) : NULL;
	FVector OutStartTrace = FVector::ZeroVector;

	if (MyPawn && MyPawn->IsTopDown() && (!MyPC || !MyPC->bThirdPersonCamera))
	{
		OutStartTrace = Instigator->GetActorLocation() + FVector(0, 0, MyPawn->bDowned ? -25 : 25.0f);
	}
	else if (PC)
	{
		// use player's camera
		FRotator UnusedRot;
		PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		////OutStartTrace = OutStartTrace + AimDir * ((Instigator->GetActorLocation() - OutStartTrace) | AimDir);
	}
	/*else if (AIPC)
	{
	OutStartTrace = GetMuzzleLocation();
	}*/

	return OutStartTrace;
}

void AOrionWeapon::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread, bool Success)
{
	if (MyPawn && MyPawn->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		// if we're a client and we've hit something that is being controlled by the server
		if (Impact.GetActor() && Impact.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			// notify the server of the hit
			ServerNotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);
		}
		else if (Impact.GetActor() == NULL)
		{
			if (Impact.bBlockingHit)
			{
				// notify the server of the hit
				ServerNotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);
			}
			else
			{
				// notify server of the miss
				ServerNotifyMiss(ShootDir, RandomSeed, ReticleSpread);
			}
		}
	}

	// process a confirmed hit
	ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread, Success);
}

bool AOrionWeapon::CanFire() const
{
	return AmmoInClip > 0 && WeaponState != WEAP_EQUIPPING && WeaponState != WEAP_PUTTINGDOWN && WeaponState != WEAP_MELEE;
}

void AOrionWeapon::UseAmmo()
{
	//make ai not use any ammo
	if (MyPawn && MyPawn->PlayerState && MyPawn->PlayerState->bIsABot)
		return;
	
	if (Role == ROLE_Authority)
	{
		if (MyPawn && InstantConfig.WeaponSlot == 1)
		{
			AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
			if (PC)
			{
				AOrionInventoryManager *Inv = PC->GetInventoryManager();
				if (Inv && Inv->HasStat(RARESTAT_NORELOAD))
					return;
			}
		}
		AmmoInClip--;
	}

	//infinite ammo for now
	////Ammo--;
}

bool AOrionWeapon::CanReload()
{
	if (MyPawn)
		return AmmoInClip < GetClipSize() && Ammo > 0 && WeaponState != WEAP_EQUIPPING && WeaponState != WEAP_PUTTINGDOWN && WeaponState != WEAP_MELEE && !MyPawn->IsRolling();
	else
		return false;
}

void AOrionWeapon::StartReload(bool bFromReplication)
{
	if (WeaponState == WEAP_RELOADING)
		return;

	BurstCounter = 0;

	if (GetWorldTimerManager().IsTimerActive(FireTimer))
		GetWorldTimerManager().ClearTimer(FireTimer);

	if (GetWorldTimerManager().IsTimerActive(BurstTimer))
		GetWorldTimerManager().ClearTimer(BurstTimer);

	if (!bFromReplication && Role < ROLE_Authority)
	{
		ServerStartReload();
	}

	if (bFromReplication || CanReload())
	{
		//bPendingReload = true;
		//DetermineWeaponState();

		GetWorldTimerManager().ClearTimer(MeleeTimer);// this, &AOrionWeapon::ResetMelee);

		WeaponState = WEAP_RELOADING;

		float AnimDuration = PlayWeaponAnimation(ReloadAnim, Role == ROLE_Authority);
		if (AnimDuration <= 0.0f)
		{
			AnimDuration = 1.0;// WeaponConfig.NoAnimReloadDuration;
		}

		StopAiming();

		if (InstantConfig.bSingleShellReload == true)
		{
			GetWorldTimerManager().SetTimer(ReloadTimer, this, &AOrionWeapon::StartShellReload, FMath::Max(0.1f, AnimDuration - 0.1f), false);
		}
		else
		{
			GetWorldTimerManager().SetTimer(ReloadStopTimer, this, &AOrionWeapon::StopReload, AnimDuration, false);
			//if (Role == ROLE_Authority)
			{
				GetWorldTimerManager().SetTimer(ReloadTimer, this, &AOrionWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);
			}

			/*if (MyPawn && MyPawn->IsLocallyControlled())
			{
				PlayWeaponSound(ReloadSound);
			}*/
		}
	}
}

void AOrionWeapon::StartShellReload()
{
	float AnimDuration = PlayWeaponAnimation(ReloadLoopAnim, Role == ROLE_Authority);
	if (AnimDuration <= 0.0f)
	{
		AnimDuration = 1.0;// WeaponConfig.NoAnimReloadDuration;
	}

	if (AmmoInClip < GetClipSize() && Ammo > AmmoInClip && CanReload())
		GetWorldTimerManager().SetTimer(ReloadTimer, this, &AOrionWeapon::ReloadNextShell, AnimDuration, false);
	else
		StopReload();
}

void AOrionWeapon::ReloadNextShell()
{
	if (MyPawn && MyPawn->GetWeapon() != this)
		return;

	AmmoInClip++;
	StartShellReload();
}

void AOrionWeapon::StopReload()
{
	WeaponState = WEAP_IDLE;

	if (InstantConfig.bSingleShellReload)
		PlayWeaponAnimation(ReloadEndAnim, Role == ROLE_Authority);

	if (GetWorldTimerManager().IsTimerActive(ReloadTimer))
		GetWorldTimerManager().ClearTimer(ReloadTimer);

	//if (CurrentState == EWeaponState::Reloading)
	//{
	//bPendingReload = false;
	//DetermineWeaponState();
		/*if (MyPawn)
		{
			if (!MyPawn->IsFirstPerson() && Mesh3P && ReloadAnim.Weapon3P)
			{
				Mesh3P->AnimScriptInstance->Montage_Stop(0.05, ReloadAnim.Weapon3P);
			}
		}*/
	//}

	if (MyPawn && MyPawn->bAim)
		StartAiming();
}

void AOrionWeapon::ReloadWeapon()
{
	if (MyPawn && MyPawn->GetWeapon() != this)
		return;

	int32 ClipDelta = FMath::Min(GetClipSize() - AmmoInClip, Ammo - AmmoInClip);

	if (ClipDelta > 0)
	{
		AmmoInClip += ClipDelta;
	}

	if (MyPawn && MyPawn->bAim)
		StartAiming();
}

void AOrionWeapon::StopWeaponAnimation(const FWeaponAnim& Animation)
{
	if (MyPawn)
	{
		UAnimMontage* UseAnim = MyPawn->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (UseAnim)
		{
			MyPawn->IsFirstPerson() ? (Mesh1P->AnimScriptInstance ? Mesh1P->AnimScriptInstance->Montage_Stop(0.1) : MyPawn = MyPawn) : MyPawn->StopAnimMontage(UseAnim);
		}
	}

	//PlayWeaponAnimation(IdleAnim);
}

bool AOrionWeapon::ServerStartReload_Validate()
{
	return true;
}

void AOrionWeapon::ServerStartReload_Implementation()
{
	StartReload(true);
}

bool AOrionWeapon::ServerStopReload_Validate()
{
	return true;
}

void AOrionWeapon::ServerStopReload_Implementation()
{
	StopReload();
}

void AOrionWeapon::ClientStartReload_Implementation()
{
	StartReload();
}

void AOrionWeapon::HandleFiring()
{
	if (AmmoInClip > 0 && CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn)// && MyPawn->IsLocallyControlled())
		{
			FireWeapon();

			// update firing FX on remote clients if function was called on server
			////BurstCounter++;
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	/*else if (MyPawn && MyPawn->IsLocallyControlled())
	{
	if (GetCurrentAmmo() == 0 && !bRefiring)
	{
	PlayWeaponSound(OutOfAmmoSound);
	ADHPlayerController* MyPC = Cast<ADHPlayerController>(MyPawn->Controller);
	ADHHUD* MyHUD = MyPC ? Cast<ADHHUD>(MyPC->GetHUD()) : NULL;
	if (MyHUD)
	{
	MyHUD->NotifyOutOfAmmo();
	}
	}

	// stop weapon fire FX, but stay in Firing state
	if (BurstCounter > 0)
	{
	OnBurstFinished();
	}
	}*/

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// local client will notify server
		if (Role < ROLE_Authority)
		{
			ServerHandleFiring();
		}

		// reload after firing last round
		/*if (CurrentAmmoInClip <= 0 && CanReload())
		{
		StartReload();
		}*/

		// setup refire timer
		/*bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
		GetWorldTimerManager().SetTimer(this, &AOrionWeapon::HandleFiring, WeaponConfig.TimeBetweenShots, false);
		}*/
	}

	//LastFireTime = GetWorld()->GetTimeSeconds();
}

bool AOrionWeapon::ServerHandleFiring_Validate()
{
	return true;
}

void AOrionWeapon::ServerHandleFiring_Implementation()
{
	//const bool bShouldUpdateAmmo = (AmmoInClip > 0 && CanFire());

	HandleFiring();

	//if (bShouldUpdateAmmo)
	//{
	// update ammo
	//UseAmmo();

	// update firing FX on remote clients
	////BurstCounter++;
	//}
}

void AOrionWeapon::ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread, bool Success)
{
	// play FX on remote clients
	if (Role == ROLE_Authority)
	{
		// handle damage
		if (Success && Impact.GetActor() != NULL)
			DealDamage(Impact, ShootDir);

		const FVector EndTrace = Origin + ShootDir * FMath::Max(6000.0f, InstantConfig.WeaponRange);
		const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

		HitNotify.Origin = EndPoint;// Origin;
		HitNotify.RandomSeed = RandomSeed;
		HitNotify.ReticleSpread = ReticleSpread;
		HitNotify.Socket = SpecialSocketName;

		SpecialSocketName = "";
	}

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		const FVector EndTrace = Origin + ShootDir * FMath::Max(6000.0f, InstantConfig.WeaponRange);
		const FVector EndPoint = Impact.bBlockingHit ? Impact.ImpactPoint : EndTrace;// Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

		SpawnTrailEffect(EndPoint);
		SpawnImpactEffects(Impact);

		FPointDamageEvent PointDmg;

		PointDmg.DamageTypeClass = InstantConfig.DamageType;
		PointDmg.HitInfo = Impact;
		PointDmg.ShotDirection = ShootDir;
		PointDmg.Damage = InstantConfig.HitDamage;

		//if (Impact.GetActor())
		//	Impact.GetActor()->TakeDamage(InstantConfig.HitDamage, PointDmg, MyPawn->Controller, this);
	}
}

bool AOrionWeapon::ServerNotifyHit_Validate(const FHitResult Impact, FVector ShootDir, int32 RandomSeed, float ReticleSpread)
{
	return true;
}

void AOrionWeapon::ServerNotifyHit_Implementation(const FHitResult Impact, FVector ShootDir, int32 RandomSeed, float ReticleSpread)
{
	const float WeaponAngleDot = FMath::Abs(FMath::Sin(ReticleSpread * PI / 180.f));

	// if we have an instigator, calculate dot between the view and the shot
	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit))
	{
		const FVector Origin = GetCameraDamageStartLocation(FVector(0.0f));// GetMuzzleLocation();
		const FVector ViewDir = (Impact.Location - Origin).GetSafeNormal();

		AOrionCharacter *Pawn = Cast<AOrionCharacter>(Instigator);
		AOrionCharacter *Victim = Cast<AOrionCharacter>(Impact.GetActor());

		// is the angle between the hit and the view within allowed limits (limit + weapon max angle)
		const float ViewDotHitDir = FVector::DotProduct((Pawn && !Cast<AOrionLobbyPawn>(Pawn) && !Pawn->bThirdPersonCamera)? (Pawn->AimPos - Pawn->GetActorLocation()).GetSafeNormal() : Instigator->GetViewRotation().Vector(), ViewDir);

		if (Victim && Victim->Health <= 0)
			ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
		else if (ViewDotHitDir > InstantConfig.AllowedViewDotHitDir - WeaponAngleDot)
		{
			//if (CurrentState != EWeaponState::Idle)
			//{
			if (Impact.GetActor() == NULL)
			{
				//if (Impact.bBlockingHit)
				//{
					ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
				//}
			}
			// assume it told the truth about static things because the don't move and the hit 
			// usually doesn't have significant gameplay implications
			else if (Impact.GetActor()->IsRootComponentStatic() || Impact.GetActor()->IsRootComponentStationary())
			{
				ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
			}
			else
			{
				// Get the component bounding box
				const FBox HitBox = Impact.GetActor()->GetComponentsBoundingBox();

				// calculate the box extent, and increase by a leeway
				FVector BoxExtent = 0.5 * (HitBox.Max - HitBox.Min);
				BoxExtent *= InstantConfig.ClientSideHitLeeway;

				// avoid precision errors with really thin objects
				BoxExtent.X = FMath::Max(150.0f, BoxExtent.X);
				BoxExtent.Y = FMath::Max(150.0f, BoxExtent.Y);
				BoxExtent.Z = FMath::Max(150.0f, BoxExtent.Z);

				// Get the box center
				const FVector BoxCenter = (HitBox.Min + HitBox.Max) * 0.5;

				// if we are within client tolerance
				if (FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z &&
					FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
					FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y)
				{
					ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
				}
				else
				{
					ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread, false);
					//UE_LOG(LogOrionWeapon, Log, TEXT("%s Rejected client side hit of %s (outside bounding box tolerance)"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
				}
			}
			//}
		}
		else if (ViewDotHitDir <= InstantConfig.AllowedViewDotHitDir)
		{
			ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread, false);
			//UE_LOG(LogOrionWeapon, Log, TEXT("%s Rejected client side hit of %s (facing too far from the hit direction)"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
		}
		else
		{
			ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread, false);
			//UE_LOG(LogOrionWeapon, Log, TEXT("%s Rejected client side hit of %s"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
		}
	}
}

FVector AOrionWeapon::GetMuzzleLocation() const
{
	if (MyPawn)
	{
		USkeletalMeshComponent* UseMesh = GetWeaponMesh(MyPawn->IsFirstPerson());
		if (UseMesh)
			return UseMesh->GetSocketLocation(MuzzleAttachPoint[0]);
	}

	return FVector(0.0f, 0.0f, 0.0f);
}

FVector AOrionWeapon::GetMuzzleDirection() const
{
	if (MyPawn)
	{
		USkeletalMeshComponent* UseMesh = GetWeaponMesh(MyPawn->IsFirstPerson());
		if (UseMesh)
			return UseMesh->GetSocketRotation(MuzzleAttachPoint[0]).Vector();
	}

	return FVector(0.0f, 0.0f, 0.0f);
}

void AOrionWeapon::OnRep_HitNotify()
{
	SimulateInstantHit(HitNotify.Origin, HitNotify.RandomSeed, HitNotify.ReticleSpread, HitNotify.Socket);
}

void AOrionWeapon::SimulateInstantHit(const FVector& ShotOrigin, int32 RandomSeed, float ReticleSpread, FName SocketName)
{
	FRandomStream WeaponRandomStream(RandomSeed);
	const float ConeHalfAngle = FMath::DegreesToRadians(ReticleSpread * 0.5f);

	FVector StartTrace;
	
	if (MyPawn && SocketName != TEXT(""))
	{
		USkeletalMeshComponent* UseMesh = GetWeaponMesh(MyPawn->IsFirstPerson());
		if (UseMesh)
			StartTrace = UseMesh->GetSocketLocation(SocketName);
	}
	else
		StartTrace = GetMuzzleLocation();// ShotOrigin;

	//const FVector AimDir = GetAdjustedAim();
	//const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
	const FVector EndTrace = ShotOrigin;// StartTrace + ShootDir * InstantConfig.WeaponRange;

	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	if (Impact.bBlockingHit)
	{
		SpawnImpactEffects(Impact);
		SpawnTrailEffect(Impact.ImpactPoint);
	}
	else
	{
		SpawnTrailEffect(EndTrace);
	}
}

void AOrionWeapon::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	float Variance = FMath::RandRange(-5, 5);

	//FRadialDamageEvent PointDmg;
	FPointDamageEvent PointDmg;

	PointDmg.DamageTypeClass = InstantConfig.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = InstantConfig.HitDamage + Variance;
	////PointDmg.Params.BaseDamage = InstantConfig.HitDamage;
	////PointDmg.Params.InnerRadius = 25.0f;
	////PointDmg.Params.OuterRadius = 25.0f;
	////PointDmg.Origin = Impact.Location;

	if (MyPawn)
		Impact.GetActor()->TakeDamage(InstantConfig.HitDamage + Variance, PointDmg, MyPawn->Controller, this);
}

bool AOrionWeapon::ServerNotifyMiss_Validate(FVector ShootDir, int32 RandomSeed, float ReticleSpread)
{
	return true;
}

void AOrionWeapon::ServerNotifyMiss_Implementation(FVector ShootDir, int32 RandomSeed, float ReticleSpread)
{
	const FVector Origin = GetMuzzleLocation();

	// play FX on remote clients
	HitNotify.Origin = Origin + ShootDir * InstantConfig.WeaponRange; //Origin;
	HitNotify.RandomSeed = RandomSeed;
	HitNotify.ReticleSpread = ReticleSpread;

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		const FVector EndTrace = Origin + ShootDir * FMath::Max(6000.0f, InstantConfig.WeaponRange);
		SpawnTrailEffect(EndTrace);
	}
}

void AOrionWeapon::SpawnImpactEffects(const FHitResult& Impact)
{
	if (GetNetMode() == NM_DedicatedServer)
		return;

	if (ImpactTemplate && Impact.bBlockingHit)
	{
		FHitResult UseImpact = Impact;

		// trace again to find component lost during replication
		if (!Impact.Component.IsValid())
		{
			const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
			const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
			FHitResult Hit = WeaponTrace(StartTrace, EndTrace);
			UseImpact = Hit;
		}

		FTransform Trans;
		Trans.SetLocation(Impact.ImpactPoint);
		Trans.SetRotation(Impact.ImpactNormal.Rotation().Quaternion());

		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GetGameState());
		float Scale = 1.0f;
		if (GRI && GRI->bIsLobby)
			Scale = 0.5f;

		AOrionImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<AOrionImpactEffect>(ImpactTemplate, Trans, Instigator, Instigator, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (EffectActor)
		{
			EffectActor->SurfaceHit = UseImpact;
			UGameplayStatics::FinishSpawningActor(EffectActor, FTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint));

			EffectActor->SetLifeSpan(5.0f);
			EffectActor->SetActorScale3D(FVector(Scale));
		}
	}
}

void AOrionWeapon::SpawnTrailEffect(const FVector& EndPoint)
{
	const FVector Origin = GetMuzzleLocation();
	//const FRotator Dir = GetMuzzleDirection().Rotation();
	FVector vDir = (EndPoint - Origin);
	vDir.Normalize();

	/*if (TrailFX)
	{
		UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, Origin, Dir);
		if (TrailPSC)
		{
			TrailPSC->SetVectorParameter(TrailTargetParam, EndPoint);
		}
	}*/

	float dist = (EndPoint - Origin).Size();

	if (TracerFX && dist > 150.0f)
	{
		//FActorSpawnParameters SpawnInfo;
		//SpawnInfo.Instigator = Instigator;
		//SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		/*AOrionProjectile *Proj = GetWorld()->SpawnActor<AOrionProjectile>(AOrionProjectile::StaticClass(), Origin, vDir.Rotation(), SpawnInfo);
		if (Proj)
		{
			Proj->SetReplicates(false);
			Proj->Init(TracerFX, Origin, EndPoint);
			Proj->ProjectileMovement->Velocity = (EndPoint - Origin).GetSafeNormal() * 10000.0f;
		}*/
		UParticleSystemComponent* TracerPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TracerFX, Origin, vDir.Rotation());

		//FVector Facing = MyPawn->GetActorRotation().Vector();

		//if (FVector::DotProduct(Facing, vDir) < 0.8f)
		//	vDir = Facing;

		if (TracerPSC)
		{
			TracerPSC->SetWorldScale3D((MyPawn && MyPawn->bThirdPersonCamera) ? FVector(1.0f) :FVector(1.0));
			TracerPSC->SetFloatParameter("BulletLife", FMath::Clamp(FMath::Clamp(((Origin - EndPoint).Size() / 11000.0f), 0.05f, 1.5f) - 0.05f, 0.05f, 1.5f));
		}
	}

	if (MuzzleFX.Num() > 0 && MyPawn)
	{
		UParticleSystemComponent* MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX[0], GetWeaponMesh(MyPawn->IsFirstPerson()), MuzzleAttachPoint[0]);
		if (MuzzlePSC > 0)
		{
			//MuzzlePSC->SetVectorParameter(FName("FlashScale"), FVector(InstantConfig.MuzzleScale));
			MuzzlePSC->SetWorldScale3D(FVector(InstantConfig.MuzzleScale * (MyPawn && MyPawn->IsTopDown() && !MyPawn->bThirdPersonCamera ? 2.5 : MyPawn && MyPawn->IsFirstPerson() ? 0.35 : 1.0)));
		}
	}

	if (GetWorld()->GetTimeSeconds() - LastFireSoundTime > 0.01f)
	{
		LastFireSoundTime = GetWorld()->GetTimeSeconds();

		if (MyPawn && !MyPawn->bDowned)
			PlayWeaponAnimation(MyPawn->bAim ? AimFireAnim : FireAnim, false);//Role == ROLE_Authority);
		PlayWeaponSound(FireSound);
	}
}

void AOrionWeapon::InitMaterials()
{
	//setup some materials
	//WeaponMats.Empty();
	//WeaponCloakMats.Empty();

	if (!CloakParent)
		return;

	if (WeaponMats.Num() == 0)
		WeaponMats.SetNumUninitialized(2);
	if (WeaponCloakMats.Num() == 0)
		WeaponCloakMats.SetNumUninitialized(2);

	//reset materials to default if needed
	if (Mesh1P) Mesh1P->OverrideMaterials.Empty();
	if (Mesh3P) Mesh3P->OverrideMaterials.Empty();

	if (Mesh1P) WeaponMats[0] = UMaterialInstanceDynamic::Create(Mesh1P->GetMaterial(0), this); if (Mesh1P && WeaponMats[0]) Mesh1P->SetMaterial(0, WeaponMats[0]);
	if (Mesh3P) WeaponMats[1] = UMaterialInstanceDynamic::Create(Mesh3P->GetMaterial(0), this); if (Mesh3P && WeaponMats[1]) Mesh3P->SetMaterial(0, WeaponMats[1]);

	if (Mesh1P) Mesh1P->SetTranslucentSortPriority(2);
	if (Mesh3P) Mesh3P->SetTranslucentSortPriority(2);

	WeaponCloakMats[0] = UMaterialInstanceDynamic::Create(CloakParent, this); if (Mesh1P && Mesh1P->SkeletalMesh) WeaponCloakMats[0]->CopyParameterOverrides(Cast<UMaterialInstance>(Mesh1P->SkeletalMesh->Materials[0].MaterialInterface));
	WeaponCloakMats[1] = UMaterialInstanceDynamic::Create(CloakParent, this); if (Mesh3P && Mesh3P->SkeletalMesh) WeaponCloakMats[1]->CopyParameterOverrides(Cast<UMaterialInstance>(Mesh3P->SkeletalMesh->Materials[0].MaterialInterface));
}

UAudioComponent* AOrionWeapon::PlayWeaponSound(USoundCue* Sound)
{
	UAudioComponent* AC = NULL;
	if (Sound && MyPawn)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, MyPawn->GetRootComponent());
	}

	return AC;
}

void AOrionWeapon::ClientStopFire_Implementation()
{
	StopFire();
}

void AOrionWeapon::StopFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
	}

	if (GetWorldTimerManager().IsTimerActive(FireTimer))// this, &AOrionWeapon::FireWeapon))
		GetWorldTimerManager().ClearTimer(FireTimer);// this, &AOrionWeapon::FireWeapon);

	/*if (bWantsToFire)
	{
	bWantsToFire = false;
	DetermineWeaponState();
	}*/
}

void AOrionWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOrionWeapon, MyPawn);
	DOREPLIFETIME_CONDITION(AOrionWeapon, HitNotify, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(AOrionWeapon, Ammo, COND_OwnerOnly);
	DOREPLIFETIME(AOrionWeapon, AmmoInClip);

	DOREPLIFETIME_CONDITION(AOrionWeapon, bKnifing, COND_SkipOwner);

	//DOREPLIFETIME_CONDITION(AOrionWeapon, BurstCounter, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(AOrionWeapon, bPendingReload, COND_SkipOwner);
}

void AOrionWeapon::OnEquip()
{
	AttachMeshToPawn();

	bPendingEquip = true;
	WeaponState = WEAP_EQUIPPING;

	BurstCounter = 0;

	float Duration = PlayWeaponAnimation(EquipAnim, false, 2.0f);// Role == ROLE_Authority);

	if (Duration <= 0.0f)
	{
		// failsafe
		Duration = 0.5f;
	}

	EquipStartedTime = GetWorld()->GetTimeSeconds();
	EquipDuration = Duration;

	GetWorldTimerManager().SetTimer(EquipTimer, this, &AOrionWeapon::OnEquipFinished, Duration, false);

	//if (MyPawn && MyPawn->IsLocallyControlled())
	//{
		PlayWeaponSound(DrawSound);
	//}
}

void AOrionWeapon::OnEquipFinished()
{
	//AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	WeaponState = WEAP_IDLE;

	if (MyPawn)
	{
		// try to reload empty clip
		if (MyPawn->IsLocallyControlled() && AmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}
	}
}

void AOrionWeapon::OnUnEquipFinished()
{
	DetachMeshFromPawn();
	bIsEquipped = false;
	WeaponState = WEAP_IDLE;
}

float AOrionWeapon::OnUnEquip()
{
	//DetachMeshFromPawn();
	StopFire();
	StopAiming();
	//bIsEquipped = false;

	if (bPendingReload || WeaponState == WEAP_RELOADING)
	{
		StopWeaponAnimation(ReloadAnim);
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(ReloadStopTimer);// this, &AOrionWeapon::StopReload);
		GetWorldTimerManager().ClearTimer(ReloadTimer);// this, &AOrionWeapon::ReloadWeapon);
	}

	if (LaserAimPSC)
	{
		LaserAimPSC->DeactivateSystem();
		LaserAimPSC = nullptr;
	}

	/*if (bPendingEquip)
	{
		StopWeaponAnimation(EquipAnim);
		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(this, &AOrionWeapon::OnEquipFinished);
	}*/

	float Duration = PlayWeaponAnimation(HolsterAnim, false, 2.0f);// Role == ROLE_Authority);

	if (Duration <= 0.0f)
	{
		// failsafe
		Duration = 0.5f;
	}

	if (WeaponState == WEAP_EQUIPPING && GetWorldTimerManager().IsTimerActive(EquipTimer))
	{
		GetWorldTimerManager().ClearTimer(EquipTimer);
		bIsEquipped = true;
		bPendingEquip = false;
	}

	//EquipStartedTime = GetWorld()->GetTimeSeconds();
	//EquipDuration = Duration;

	GetWorldTimerManager().SetTimer(EquipTimer, this, &AOrionWeapon::OnUnEquipFinished, Duration * 0.95f, false);

	WeaponState = WEAP_PUTTINGDOWN;

	PlayWeaponSound(HolsterSound);

	return Duration;
}

void AOrionWeapon::SetOwningPawn(AOrionCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);

		InitClip();
	}
}

bool AOrionWeapon::ServerMelee_Validate()
{
	return true;
}

void AOrionWeapon::ServerMelee_Implementation()
{
	DoMelee();
}

bool AOrionWeapon::ServerStartFire_Validate()
{
	return true;
}

void AOrionWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

bool AOrionWeapon::ServerStopFire_Validate()
{
	return true;
}

void AOrionWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

void AOrionWeapon::OnRep_Knifing()
{
	Mesh3P->SetHiddenInGame(bKnifing);
	KnifeMesh->SetHiddenInGame(!bKnifing);
}

float AOrionWeapon::PlayWeaponAnimation(const FWeaponAnim& Animation, bool bReplicate, float Scale)
{
	float Duration = 0.01f;

	float Rate = Scale;

	if (MyPawn && WeaponState == WEAP_RELOADING)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
		if (PC)
			Rate += float(PC->GetSkillValue(SKILL_RELOADSPEED)) / 100.0f;
	}

	if (MyPawn)
		Duration = MyPawn->OrionPlayAnimMontage(Animation, Rate, FName(""), bReplicate);
	return Duration;
}

void AOrionWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		//OnEnterInventory(MyPawn);
	}
	else
	{
		//OnLeaveInventory();
	}
}

/*void AOrionWeapon::OnRep_Reload()
{
	if (true)//bPendingReload)
	{
		StartReload(true);
	}
	else
	{
		StopReload();
	}
}*/

void AOrionWeapon::SimulateWeaponFire()
{
	if (Role == ROLE_Authority)// && CurrentState != EWeaponState::Firing)
	{
		return;
	}

	if (MuzzleFX.Num() > 0)
	{
		USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh(MyPawn->IsFirstPerson());
		if (true ||/*!bLoopedMuzzleFX || */MuzzlePSC.Num() <= 0)
		{
			// Split screen requires we create 2 effects. One that we see and one that the other player sees.
			if ((MyPawn != NULL) && (MyPawn->IsLocallyControlled() == true))
			{
				AController* PlayerCon = MyPawn->GetController();
				if (PlayerCon != NULL)
				{
					UseWeaponMesh->GetSocketLocation(MuzzleAttachPoint[0]);
					MuzzlePSC[0] = UGameplayStatics::SpawnEmitterAttached(MuzzleFX[0], UseWeaponMesh, MuzzleAttachPoint[0]);
					MuzzlePSC[0]->bOwnerNoSee = false;
					MuzzlePSC[0]->bOnlyOwnerSee = true;

					/*Mesh3P->GetSocketLocation(MuzzleAttachPoint);
					MuzzlePSCSecondary = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh3P, MuzzleAttachPoint);
					MuzzlePSCSecondary->bOwnerNoSee = true;
					MuzzlePSCSecondary->bOnlyOwnerSee = false;*/
				}
			}
			else
			{
				MuzzlePSC[0] = UGameplayStatics::SpawnEmitterAttached(MuzzleFX[0], UseWeaponMesh, MuzzleAttachPoint[0]);
			}
		}
	}

	//if (!bLoopedFireAnim || !bPlayingFireAnim)
	//{
	if (MyPawn && !MyPawn->bDowned)
		PlayWeaponAnimation(MyPawn->bAim ? AimFireAnim : FireAnim, false);//Role == ROLE_Authority);
	//	bPlayingFireAnim = true;
	//}

	/*if (bLoopedFireSound)
	{
	if (FireAC == NULL)
	{
	FireAC = PlayWeaponSound(FireLoopSound);
	}
	}
	else
	{
	PlayWeaponSound(FireSound);
	}

	ADHPlayerController* PC = (MyPawn != NULL) ? Cast<ADHPlayerController>(MyPawn->Controller) : NULL;
	if (PC != NULL && PC->IsLocalController())
	{
	if (FireCameraShake != NULL)
	{
	PC->ClientPlayCameraShake(FireCameraShake, 1);
	}
	if (FireForceFeedback != NULL)
	{
	PC->ClientPlayForceFeedback(FireForceFeedback, false, "Weapon");
	}
	}*/
}

void AOrionWeapon::StopSimulatingWeaponFire()
{
	/*if (bLoopedMuzzleFX)
	{
	if (MuzzlePSC != NULL)
	{
	MuzzlePSC->DeactivateSystem();
	MuzzlePSC = NULL;
	}
	if (MuzzlePSCSecondary != NULL)
	{
	MuzzlePSCSecondary->DeactivateSystem();
	MuzzlePSCSecondary = NULL;
	}
	}

	if (bLoopedFireAnim && bPlayingFireAnim)
	{*/
	StopWeaponAnimation(FireAnim);
	/*	bPlayingFireAnim = false;
	}

	if (FireAC)
	{
	FireAC->FadeOut(0.1f, 0.0f);
	FireAC = NULL;

	PlayWeaponSound(FireFinishSound);
	}*/
}