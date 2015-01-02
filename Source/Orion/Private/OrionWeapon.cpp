// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionWeapon.h"


AOrionWeapon::AOrionWeapon(const FObjectInitializer& ObejctInitializer) : Super(ObejctInitializer)
{
	Mesh1P = ObejctInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->bChartDistanceFactor = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh1P->bPerBoneMotionBlur = false;
	RootComponent = Mesh1P;

	ArmsMesh = ObejctInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("ArmsMesh1P"));
	ArmsMesh->AlwaysLoadOnClient = true;
	ArmsMesh->AlwaysLoadOnServer = true;
	ArmsMesh->bOwnerNoSee = false;
	ArmsMesh->bOnlyOwnerSee = true;
	ArmsMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	ArmsMesh->bCastDynamicShadow = true;
	ArmsMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	ArmsMesh->bChartDistanceFactor = true;
	ArmsMesh->bGenerateOverlapEvents = false;
	ArmsMesh->AttachParent = Mesh1P;
	ArmsMesh->CastShadow = false;
	ArmsMesh->SetMasterPoseComponent(Mesh1P);

	Mesh3P = ObejctInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));
	Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh3P->bChartDistanceFactor = true;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
	//Mesh3P->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	Mesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mesh3P->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	Mesh3P->AttachParent = Mesh1P;

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
	bReplicateInstigator = true;
	bNetUseOwnerRelevancy = true;
}

void AOrionWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AmmoInClip = InstantConfig.ClipSize;
	Ammo = InstantConfig.MaxAmmo;

	TargetFOV = 90.0;

	Mesh1P->SetWorldScale3D(FVector(InstantConfig.WeaponScale));
	Mesh1P->UpdateBounds();

	ArmsMesh->SetWorldScale3D(FVector(InstantConfig.WeaponScale));
	ArmsMesh->UpdateBounds();

	PlayWeaponAnimation(IdleAnim);

	DetachMeshFromPawn();
}

void AOrionWeapon::AttachMeshToPawn()
{
	if (MyPawn)
	{
		// Remove and hide both first and third person meshes
		DetachMeshFromPawn();

		PlayWeaponAnimation(IdleAnim);

		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		FName AttachPoint = TEXT("WeaponPoint");// MyPawn->GetWeaponAttachPoint();
		if (MyPawn->IsLocallyControlled() == true)
		{
			USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecifcPawnMesh(true);
			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecifcPawnMesh(false);
			Mesh1P->SetHiddenInGame(!MyPawn->IsFirstPerson());
			ArmsMesh->SetHiddenInGame(!MyPawn->IsFirstPerson());
			Mesh3P->SetHiddenInGame(MyPawn->IsFirstPerson());
			Mesh1P->AttachTo(MyPawn->FirstPersonCameraComponent);
			Mesh3P->AttachTo(PawnMesh3p, AttachPoint);

			//Mesh3P->SetRelativeLocation(FVector(0, 49.9, 1.0));
			//Mesh3P->SetRelativeRotation(FRotator(0, 0, -5));

			StopAiming();
		}
		else
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh(false);
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			UseWeaponMesh->AttachTo(UsePawnMesh, AttachPoint);
			UseWeaponMesh->SetHiddenInGame(false);
		}
	}
}

void AOrionWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		//HandleZooms(DeltaSeconds);
		HandleViewOffsets(DeltaSeconds);

		if (CurrentFiringSpread > 0.0f && GetWorld()->TimeSeconds - LastFireTime > 0.5f)
			CurrentFiringSpread = FMath::Max(0.0f, CurrentFiringSpread - DeltaSeconds*2.0f);
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

void AOrionWeapon::StartAiming()
{
	bAiming = true;
	LastAimTime = GetWorld()->TimeSeconds;
	//Mesh1P->SetRelativeLocation(AimViewOffset);

	//MyPawn->FirstPersonCameraComponent->FieldOfView = InstantConfig.AimFOV;
	TargetFOV = InstantConfig.AimFOV;

	PlayWeaponAnimation(AimAnim);
}

void AOrionWeapon::StopAiming()
{
	bAiming = false;
	LastAimTime = GetWorld()->TimeSeconds;
	//Mesh1P->SetRelativeLocation(ViewOffset);

	//MyPawn->FirstPersonCameraComponent->FieldOfView = 90;
	TargetFOV = 90.0f;

	StopWeaponAnimation(AimAnim);
}

USkeletalMeshComponent* AOrionWeapon::GetWeaponMesh(bool bFirstPerson) const
{
	return bFirstPerson ? Mesh1P : Mesh3P;
}

void AOrionWeapon::DetachMeshFromPawn()
{
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

	if (WeaponState == WEAP_RELOADING)
		StopReload();

	if (Role < ROLE_Authority)
	{
		ServerMelee();
	}

	DoMelee();
}

void AOrionWeapon::DoMelee()
{
	WeaponState = WEAP_MELEE;
	float Len = PlayWeaponAnimation(MeleeAnim);

	GetWorldTimerManager().SetTimer(this, &AOrionWeapon::ResetMelee, Len, false);
}

void AOrionWeapon::ResetMelee()
{
	WeaponState = WEAP_IDLE;
}

void AOrionWeapon::StartFire()
{
	//can't fire when punching stuff
	if (WeaponState == WEAP_MELEE)
		return;

	if (CanFire() == false)
		return;

	if (Role < ROLE_Authority)
	{
		ServerStartFire();
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

	if (InstantConfig.TimeBetweenShots > 0 && LastFireTime + InstantConfig.TimeBetweenShots > GetWorld()->GetTimeSeconds())
		GetWorldTimerManager().SetTimer(this, &AOrionWeapon::FireWeapon, LastFireTime + InstantConfig.TimeBetweenShots - GetWorld()->GetTimeSeconds(), false);
	else
		FireWeapon();
}

void AOrionWeapon::CancelReload()
{
	GetWorldTimerManager().ClearTimer(this, &AOrionWeapon::StopReload);
	GetWorldTimerManager().ClearTimer(this, &AOrionWeapon::ReloadWeapon);

	WeaponState = WEAP_IDLE;
}

void AOrionWeapon::CancelMelee()
{
	GetWorldTimerManager().ClearTimer(this, &AOrionWeapon::ResetMelee);

	WeaponState = WEAP_IDLE;
}

void AOrionWeapon::FireSpecial(FName SocketName, FVector Direction)
{
	LastFireTime = GetWorld()->GetTimeSeconds();

	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = GetCurrentSpread();
	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

	const FVector AimDir = GetAdjustedAim();
	const FVector StartTrace = GetBarrelLocation(SocketName);
	const FVector ShootDir = Direction;
	const FVector EndTrace = StartTrace + ShootDir * InstantConfig.WeaponRange;

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);

	CurrentFiringSpread = FMath::Min(InstantConfig.FiringSpreadMax, CurrentFiringSpread + InstantConfig.FiringSpreadIncrement) * GetSpreadModifier();
}

void AOrionWeapon::FireWeapon()
{
	if (!CanFire())
		return;

	if (InstantConfig.bAutomatic)
		GetWorldTimerManager().SetTimer(this, &AOrionWeapon::FireWeapon, InstantConfig.TimeBetweenShots, false);

	LastFireTime = GetWorld()->GetTimeSeconds();

	UseAmmo();

	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = GetCurrentSpread();
	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

	const FVector AimDir = GetAdjustedAim();
	const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
	const FVector EndTrace = StartTrace + ShootDir * InstantConfig.WeaponRange;

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);

	CurrentFiringSpread = FMath::Min(InstantConfig.FiringSpreadMax, CurrentFiringSpread + InstantConfig.FiringSpreadIncrement) * GetSpreadModifier();

	if (MyPawn)
		MyPawn->AddAimKick(FRotator(FMath::FRandRange(1.0f, 2.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f));

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
	return IdleAnim.Pawn3P;
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
	return AimAnim.Pawn3P;
}

UAnimMontage* AOrionWeapon::GetFireMontage()
{
	return FireAnim.Pawn3P;
}

UAnimMontage* AOrionWeapon::GetSprintMontage()
{
	return SprintAnim.Pawn3P;
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
		GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	return Hit;
}

FVector AOrionWeapon::GetAdjustedAim() const
{
	AController* const PlayerController = Instigator ? Instigator->Controller : NULL;
	FVector FinalAim = FVector::ZeroVector;
	// If we have a player controller use it for the aim
	if (PlayerController)
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
	if (Mesh3P)
		Mesh3P->GetSocketWorldLocationAndRotation(SocketName, pos, rot);

	return pos;
}

FVector AOrionWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const
{
	AController* PC = MyPawn ? MyPawn->Controller : NULL;
	//ADHAIController* AIPC = MyPawn ? Cast<ADHAIController>(MyPawn->Controller) : NULL;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC)
	{
		// use player's camera
		FRotator UnusedRot;
		PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * ((Instigator->GetActorLocation() - OutStartTrace) | AimDir);
	}
	/*else if (AIPC)
	{
	OutStartTrace = GetMuzzleLocation();
	}*/

	return OutStartTrace;
}

void AOrionWeapon::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
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
	ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
}

bool AOrionWeapon::CanFire() const
{
	return AmmoInClip>0;
}

void AOrionWeapon::UseAmmo()
{
	AmmoInClip--;

	Ammo--;
}

bool AOrionWeapon::CanReload()
{
	return AmmoInClip<InstantConfig.ClipSize && Ammo > 0;
}

void AOrionWeapon::StartReload(bool bFromReplication)
{
	if (!bFromReplication && Role < ROLE_Authority)
	{
		ServerStartReload();
	}

	if (bFromReplication || CanReload())
	{
		//bPendingReload = true;
		//DetermineWeaponState();

		GetWorldTimerManager().ClearTimer(this, &AOrionWeapon::ResetMelee);

		WeaponState = WEAP_RELOADING;

		float AnimDuration = PlayWeaponAnimation(ReloadAnim);
		if (AnimDuration <= 0.0f)
		{
			AnimDuration = 1.0;// WeaponConfig.NoAnimReloadDuration;
		}

		StopAiming();

		GetWorldTimerManager().SetTimer(this, &AOrionWeapon::StopReload, AnimDuration, false);
		if (Role == ROLE_Authority)
		{
			GetWorldTimerManager().SetTimer(this, &AOrionWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			PlayWeaponSound(ReloadSound);
		}
	}
}

void AOrionWeapon::StopReload()
{
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
}

void AOrionWeapon::ReloadWeapon()
{
	int32 ClipDelta = FMath::Min(InstantConfig.ClipSize - AmmoInClip, Ammo - AmmoInClip);

	if (ClipDelta > 0)
	{
		AmmoInClip += ClipDelta;
	}
}

void AOrionWeapon::StopWeaponAnimation(const FWeaponAnim& Animation)
{
	if (MyPawn)
	{
		UAnimMontage* UseAnim = MyPawn->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (UseAnim)
		{
			MyPawn->IsFirstPerson() ? Mesh1P->AnimScriptInstance->Montage_Stop(0.1) : MyPawn->StopAnimMontage(UseAnim);
		}
	}

	PlayWeaponAnimation(IdleAnim);
}

bool AOrionWeapon::ServerStartReload_Validate()
{
	return true;
}

void AOrionWeapon::ServerStartReload_Implementation()
{
	StartReload();
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

void AOrionWeapon::ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	// play FX on remote clients
	if (Role == ROLE_Authority)
	{
		// handle damage
		if (Impact.GetActor() != NULL)
			DealDamage(Impact, ShootDir);

		const FVector EndTrace = Origin + ShootDir * InstantConfig.WeaponRange;
		const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

		HitNotify.Origin = EndPoint;// Origin;
		HitNotify.RandomSeed = RandomSeed;
		HitNotify.ReticleSpread = ReticleSpread;
	}

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		const FVector EndTrace = Origin + ShootDir * InstantConfig.WeaponRange;
		const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

		SpawnTrailEffect(EndPoint);
		SpawnImpactEffects(Impact);
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
		const FVector Origin = GetMuzzleLocation();
		const FVector ViewDir = (Impact.Location - Origin).SafeNormal();

		// is the angle between the hit and the view within allowed limits (limit + weapon max angle)
		const float ViewDotHitDir = FVector::DotProduct(Instigator->GetViewRotation().Vector(), ViewDir);
		if (ViewDotHitDir > InstantConfig.AllowedViewDotHitDir - WeaponAngleDot)
		{
			//if (CurrentState != EWeaponState::Idle)
			//{
			if (Impact.GetActor() == NULL)
			{
				if (Impact.bBlockingHit)
				{
					ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
				}
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
				BoxExtent.X = FMath::Max(20.0f, BoxExtent.X);
				BoxExtent.Y = FMath::Max(20.0f, BoxExtent.Y);
				BoxExtent.Z = FMath::Max(20.0f, BoxExtent.Z);

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
					//UE_LOG(LogOrionWeapon, Log, TEXT("%s Rejected client side hit of %s (outside bounding box tolerance)"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
				}
			}
			//}
		}
		else if (ViewDotHitDir <= InstantConfig.AllowedViewDotHitDir)
		{
			//UE_LOG(LogOrionWeapon, Log, TEXT("%s Rejected client side hit of %s (facing too far from the hit direction)"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
		}
		else
		{
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
	SimulateInstantHit(HitNotify.Origin, HitNotify.RandomSeed, HitNotify.ReticleSpread);
}

void AOrionWeapon::SimulateInstantHit(const FVector& ShotOrigin, int32 RandomSeed, float ReticleSpread)
{
	FRandomStream WeaponRandomStream(RandomSeed);
	const float ConeHalfAngle = FMath::DegreesToRadians(ReticleSpread * 0.5f);

	const FVector StartTrace = GetMuzzleLocation();// ShotOrigin;
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
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = InstantConfig.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = InstantConfig.HitDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, MyPawn->Controller, this);
}

bool AOrionWeapon::ServerNotifyMiss_Validate(FVector ShootDir, int32 RandomSeed, float ReticleSpread)
{
	return true;
}

void AOrionWeapon::ServerNotifyMiss_Implementation(FVector ShootDir, int32 RandomSeed, float ReticleSpread)
{
	const FVector Origin = GetMuzzleLocation();

	// play FX on remote clients
	////HitNotify.Origin = Origin + ShootDir * InstantConfig.WeaponRange; //Origin;
	////HitNotify.RandomSeed = RandomSeed;
	////HitNotify.ReticleSpread = ReticleSpread;

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		const FVector EndTrace = Origin + ShootDir * InstantConfig.WeaponRange;
		SpawnTrailEffect(EndTrace);
	}
}

void AOrionWeapon::SpawnImpactEffects(const FHitResult& Impact)
{
	/*if (ImpactTemplate && Impact.bBlockingHit)
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

	ADHImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<ADHImpactEffect>(ImpactTemplate, Impact.ImpactPoint, Impact.ImpactNormal.Rotation());
	if (EffectActor)
	{
	EffectActor->SurfaceHit = UseImpact;
	UGameplayStatics::FinishSpawningActor(EffectActor, FTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint));
	}
	}*/
}

void AOrionWeapon::SpawnTrailEffect(const FVector& EndPoint)
{
	const FVector Origin = GetMuzzleLocation();
	const FRotator Dir = GetMuzzleDirection().Rotation();
	FVector vDir = (EndPoint - Origin);
	vDir.Normalize();

	if (TrailFX)
	{
		UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, Origin, Dir);
		if (TrailPSC)
		{
			TrailPSC->SetVectorParameter(TrailTargetParam, EndPoint);
		}
	}

	if (TracerFX)
	{
		UParticleSystemComponent* TracerPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TracerFX, Origin, vDir.Rotation());
		if (TracerPSC)
		{
			TracerPSC->SetWorldScale3D(FVector(1.0));
		}
	}

	if (MuzzleFX.Num() > 0)
	{
		UParticleSystemComponent* MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX[0], GetWeaponMesh(MyPawn->IsFirstPerson()), MuzzleAttachPoint[0]);
		if (MuzzlePSC > 0)
		{
			MuzzlePSC->SetVectorParameter(FName("FlashScale"), FVector(InstantConfig.MuzzleScale));
		}
	}

	PlayWeaponAnimation(bAiming ? AimFireAnim : FireAnim);

	PlayWeaponSound(FireSound);
}

UAudioComponent* AOrionWeapon::PlayWeaponSound(USoundCue* Sound)
{
	UAudioComponent* AC = NULL;
	if (Sound && MyPawn)
	{
		AC = UGameplayStatics::PlaySoundAttached(Sound, MyPawn->GetRootComponent());
	}

	return AC;
}

void AOrionWeapon::StopFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
	}

	if (GetWorldTimerManager().IsTimerActive(this, &AOrionWeapon::FireWeapon))
		GetWorldTimerManager().ClearTimer(this, &AOrionWeapon::FireWeapon);

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
	DOREPLIFETIME_CONDITION(AOrionWeapon, AmmoInClip, COND_OwnerOnly);

	//DOREPLIFETIME_CONDITION(AOrionWeapon, BurstCounter, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(AOrionWeapon, bPendingReload, COND_SkipOwner);
}

void AOrionWeapon::OnEquip()
{
	AttachMeshToPawn();

	/*bPendingEquip = true;
	DetermineWeaponState();

	float Duration = PlayWeaponAnimation(EquipAnim);
	if (Duration <= 0.0f)
	{
	// failsafe
	Duration = 0.5f;
	}
	EquipStartedTime = GetWorld()->GetTimeSeconds();
	EquipDuration = Duration;

	GetWorldTimerManager().SetTimer(this, &AOrionWeapon::OnEquipFinished, Duration, false);

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
	PlayWeaponSound(EquipSound);
	}*/
}

void AOrionWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	/*bIsEquipped = true;
	bPendingEquip = false;

	if (MyPawn)
	{
	// try to reload empty clip
	if (MyPawn->IsLocallyControlled() &&
	CurrentAmmoInClip <= 0 &&
	CanReload())
	{
	StartReload();
	}
	}

	DetermineWeaponState();*/
}

void AOrionWeapon::OnUnEquip()
{
	DetachMeshFromPawn();
	StopFire();
	/*bIsEquipped = false;

	if (bPendingReload)
	{
	StopWeaponAnimation(ReloadAnim);
	bPendingReload = false;

	GetWorldTimerManager().ClearTimer(this, &AOrionWeapon::StopReload);
	GetWorldTimerManager().ClearTimer(this, &AOrionWeapon::ReloadWeapon);
	}

	if (bPendingEquip)
	{
	StopWeaponAnimation(EquipAnim);
	bPendingEquip = false;

	GetWorldTimerManager().ClearTimer(this, &AOrionWeapon::OnEquipFinished);
	}

	DetermineWeaponState();*/
}

void AOrionWeapon::SetOwningPawn(AOrionCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
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

float AOrionWeapon::PlayWeaponAnimation(const FWeaponAnim& Animation)
{
	float Duration = 0.0f;
	if (MyPawn)
	{
		UAnimMontage* UseAnim = MyPawn->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (UseAnim)
		{
			Duration = MyPawn->IsFirstPerson() ? Mesh1P->AnimScriptInstance->Montage_Play(UseAnim, 1.0) : MyPawn->PlayAnimMontage(UseAnim);
		}
	}

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

void AOrionWeapon::OnRep_Reload()
{
	if (true)//bPendingReload)
	{
		StartReload(true);
	}
	else
	{
		StopReload();
	}
}

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
	PlayWeaponAnimation(bAiming ? AimFireAnim : FireAnim);
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