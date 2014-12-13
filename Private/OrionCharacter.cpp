// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionWeapon.h"
#include "OrionCharacter.h"
#include "OrionGameMode.h"
#include "OrionPlayerController.h"
#include "OrionMovementComponent.h"
//#include "OrionAIController.h"
#include "OrionProjectile.h"

//////////////////////////////////////////////////////////////////////////
// AOrionCharacter

AOrionCharacter::AOrionCharacter(const FObjectInitializer& ObejctInitializer)
: Super(ObejctInitializer.SetDefaultSubobjectClass<UOrionMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//AIControllerClass = AOrionAIController::StaticClass();

	// Create a CameraComponent	
	FirstPersonCameraComponent = ObejctInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera

	ThirdPersonCameraComponent = ObejctInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->AttachParent = GetCapsuleComponent();
	ThirdPersonCameraComponent->RelativeLocation = FVector(0, 0, 0); // Position the camera

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	/*Mesh1P = ObejctInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;*/

	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	BodyMesh = ObejctInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Body"));
	BodyMesh->AlwaysLoadOnClient = true;
	BodyMesh->AlwaysLoadOnServer = true;
	BodyMesh->bOwnerNoSee = true;
	BodyMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	BodyMesh->bCastDynamicShadow = true;
	BodyMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	BodyMesh->bChartDistanceFactor = true;
	//BodyMesh->SetCollisionProfileName(CollisionProfileName);
	BodyMesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	BodyMesh->AttachParent = GetMesh();
	BodyMesh->SetMasterPoseComponent(GetMesh());

	HelmetMesh = ObejctInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Helmet"));
	HelmetMesh->AlwaysLoadOnClient = true;
	HelmetMesh->AlwaysLoadOnServer = true;
	HelmetMesh->bOwnerNoSee = true;
	HelmetMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	HelmetMesh->bCastDynamicShadow = true;
	HelmetMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	HelmetMesh->bChartDistanceFactor = true;
	//HelmetMesh->SetCollisionProfileName(CollisionProfileName);
	HelmetMesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	HelmetMesh->AttachParent = GetMesh();
	HelmetMesh->SetMasterPoseComponent(GetMesh());

	ArmsMesh = ObejctInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Arms"));
	ArmsMesh->AlwaysLoadOnClient = true;
	ArmsMesh->AlwaysLoadOnServer = true;
	ArmsMesh->bOwnerNoSee = true;
	ArmsMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	ArmsMesh->bCastDynamicShadow = true;
	ArmsMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	ArmsMesh->bChartDistanceFactor = true;
	//ArmsMesh->SetCollisionProfileName(CollisionProfileName);
	ArmsMesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	ArmsMesh->AttachParent = GetMesh();
	ArmsMesh->SetMasterPoseComponent(GetMesh());

	LegsMesh = ObejctInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Legs"));
	LegsMesh->AlwaysLoadOnClient = true;
	LegsMesh->AlwaysLoadOnServer = true;
	LegsMesh->bOwnerNoSee = true;
	LegsMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	LegsMesh->bCastDynamicShadow = true;
	LegsMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	LegsMesh->bChartDistanceFactor = true;
	//LegsMesh->SetCollisionProfileName(CollisionProfileName);
	LegsMesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	LegsMesh->AttachParent = GetMesh();
	LegsMesh->SetMasterPoseComponent(GetMesh());

	Flight1Mesh = ObejctInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Flight1"));
	Flight1Mesh->AlwaysLoadOnClient = true;
	Flight1Mesh->AlwaysLoadOnServer = true;
	Flight1Mesh->bOwnerNoSee = true;
	Flight1Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	Flight1Mesh->bCastDynamicShadow = true;
	Flight1Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Flight1Mesh->bChartDistanceFactor = true;
	//Flight1Mesh->SetCollisionProfileName(CollisionProfileName);
	Flight1Mesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	Flight1Mesh->AttachParent = GetMesh();
	Flight1Mesh->SetMasterPoseComponent(GetMesh());

	Flight2Mesh = ObejctInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Flight2"));
	Flight2Mesh->AlwaysLoadOnClient = true;
	Flight2Mesh->AlwaysLoadOnServer = true;
	Flight2Mesh->bOwnerNoSee = true;
	Flight2Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	Flight2Mesh->bCastDynamicShadow = true;
	Flight2Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Flight2Mesh->bChartDistanceFactor = true;
	//Flight2Mesh->SetCollisionProfileName(CollisionProfileName);
	Flight2Mesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	Flight2Mesh->AttachParent = GetMesh();
	Flight2Mesh->SetMasterPoseComponent(GetMesh());

	Health = 100.0;
	HealthMax = 100.0;

	bFirstPerson = true;
	UpdatePawnMeshes();

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

FVector2D AOrionCharacter::GetAim(float DeltaTime)
{
	FVector pos;
	FRotator rot;

	if (CurrentWeapon == NULL)
		return FVector2D(0.0f, 0.0f);

	if (IsSprinting())
		return FVector2D(0.0f, 0.0f);

	if (!Controller)
		return FVector2D(0.0f, 0.0f);

	GetPawnMesh()->GetSocketWorldLocationAndRotation(FName("Aim"), pos, rot);

	//GetWeapon()->GetWeaponMesh(false)->GetSocketWorldLocationAndRotation(FName("MuzzleFlashSocket"), pos, rot);
	FVector AimDirWS = (CameraLocation + Controller->GetControlRotation().Vector()/*ThirdPersonCameraComponent->GetComponentRotation().Vector()*/*5000.0) - pos;// GetBaseAimRotation().Vector();
	//DrawDebugBox(GWorld, CameraLocation + Controller->GetControlRotation().Vector()*5000.0, FVector(10, 10, 10), FColor(255, 0, 0, 255), false);
	AimDirWS.Normalize();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	//we need to offset the aim a bit based on player third person camera offsets
	return FVector2D((2.5 + AimRotLS.Yaw) / 90.0, AimRotLS.Pitch / 90.0);
}

void AOrionCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	if (IsFirstPerson())
	{
		FirstPersonCameraComponent->GetCameraView(DeltaTime, OutResult);
		if (Controller)
			OutResult.Rotation = Controller->GetControlRotation();
	}
	else
	{
		ThirdPersonCameraComponent->GetCameraView(DeltaTime, OutResult);

		if (Controller)
			OutResult.Rotation = Controller->GetControlRotation();

		//adjust the camera to always point towards the character
		//OutResult.Location = CameraOffset*CameraDist;

		FVector CamStart;
		FVector X, Y, Z;

		X = OutResult.Rotation.Vector();
		Z = FVector(0.0f, 0.0f, 1.0f);
		Y = FVector::CrossProduct(Z, X);

		X *= CameraDist;

		if (Health > 0)
		{
			CamStart = GetMesh()->GetBoneLocation(FName("Trek_Spine02"));
			CamStart.Z += GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - 18.0;
			if (bDuck)
				CamStart.Z += 25.0;
		}

		OutResult.Location = CamStart - X*CameraOffset.X + CameraOffset.Y*Y;

		CameraLocation = OutResult.Location;
	}

	//add some weapon bob and view sway
	//default eye height is 0,0,64 for firstperson
	float OldBobTime = BobTime;
	FRotator rot = GetActorRotation();
	FVector X = rot.Vector();
	FVector Z = FVector(0, 0, 1);
	FVector Y = FVector::CrossProduct(Z, X);

	float Speed2D = GetVelocity().Size2D();
	if (Speed2D < 10)
		BobTime += 0.2 * FMath::Min(0.1f,DeltaTime);
	else
		BobTime += FMath::Min(0.1f, DeltaTime) * (0.3 + 0.7 * Speed2D / GetMovementComponent()->GetMaxSpeed());

	//Bob = Lerp(Bob, (bIsWalking ? default.Bob : default.Bob*BobScaleWhenRunning), DeltaSeconds * 10);

	WalkBob = Y * 0.01 * Speed2D * FMath::Sin(8.0f * BobTime);
	WalkBob.Z = 0.01 * Speed2D * FMath::Sin(16.0f * BobTime);

	FirstPersonCameraComponent->SetRelativeLocation(FMath::Lerp(FirstPersonCameraComponent->RelativeLocation, FVector(0, 0, 64.0) + WalkBob, FMath::Min(0.5f,DeltaTime*10.0f)));

	//add some aim kick
	/*if (IsFirstPerson())
	FirstPersonCameraComponent->SetRelativeRotation(AimKick);
	else
	ThirdPersonCameraComponent->SetRelativeRotation(AimKick);
	*/
	//OutResult.Rotation += AimKick;
}

void AOrionCharacter::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation, const float FOV)
{
	/*USkeletalMeshComponent* DefMesh1P = Cast<USkeletalMeshComponent>(AOrionWeapon::StaticClass()->GetDefaultSubobjectByName(TEXT("WeaponMesh1P")));
	const FMatrix DefMeshLS = FRotationTranslationMatrix(DefMesh1P->RelativeRotation, DefMesh1P->RelativeLocation);
	const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

	// Mesh rotating code expect uniform scale in LocalToWorld matrix

	const FRotator RotCameraPitch(CameraRotation.Pitch, 0.0f, 0.0f);
	const FRotator RotCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);

	const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RotCameraYaw, CameraLocation) * LocalToWorld.InverseSafe();
	const FMatrix PitchedCameraLS = FRotationMatrix(RotCameraPitch) * LeveledCameraLS;
	const FMatrix MeshRelativeToCamera = DefMeshLS * LeveledCameraLS.InverseSafe();
	const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;

	GetWeapon()->GetWeaponMesh(true)->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(), PitchedMesh.Rotator());*/

	FirstPersonCameraComponent->FieldOfView = FOV;
	ThirdPersonCameraComponent->FieldOfView = FOV;
}

void AOrionCharacter::ResetAimKick()
{
	AimKick = FRotator(0, 0, 0);
}

FVector AOrionCharacter::GetPawnViewLocation() const
{
	if (IsFirstPerson())
		return GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight);
	else
		return CameraLocation; 
}

void AOrionCharacter::ProcessAimKick(FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	FRotator rot;
	float t;

	if (CurrentWeapon == NULL)
		return;

	t = CurrentWeapon->InstantConfig.RecoilScale;

	if (GetWorld()->TimeSeconds - LastAimKickTime>t)
	{
		TargetAimKick = FRotator(0, 0, 0);
		return;
	}
	else if (GetWorld()->TimeSeconds - LastAimKickTime>t / 3.0)
	{
		OutViewRotation = OutViewRotation + AimKick*(1.0 - ((GetWorld()->TimeSeconds - LastAimKickTime - (t / 3.0)) / (t - (t / 3.0))));
	}
	else
	{
		OutViewRotation = OutViewRotation + AimKick;
	}
}

bool AOrionCharacter::IsFirstPerson() const
{
	return IsAlive() && bFirstPerson && Controller && Controller->IsLocalPlayerController();
}

bool AOrionCharacter::IsRolling() const
{
	return bRolling;
}

void AOrionCharacter::SpawnDefaultInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	// #ttp 321024 - GDC: OrionGAME: Disable secondary weapon (temp)
	// Temporary hack for GDC - add only the first weapon (rifle) thereby disabling the rocket launcher
	int32 NumWeaponClasses = DefaultInventoryClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;
			AOrionWeapon* NewWeapon = GetWorld()->SpawnActor<AOrionWeapon>(DefaultInventoryClasses[i], SpawnInfo);
			AddWeapon(NewWeapon);
		}
	}

	// equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
	}
}

AOrionWeapon* AOrionCharacter::GetWeapon() const
{
	return CurrentWeapon;
}

void AOrionCharacter::EquipWeapon(AOrionWeapon* Weapon)
{
	if (Weapon)
	{
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}

bool AOrionCharacter::ServerEquipWeapon_Validate(AOrionWeapon* Weapon)
{
	return true;
}

void AOrionCharacter::ServerEquipWeapon_Implementation(AOrionWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

void AOrionCharacter::OnRep_CurrentWeapon(AOrionWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void AOrionCharacter::HandleSpecialWeaponFire(FName SocketName)
{

}

void AOrionCharacter::SetCurrentWeapon(class AOrionWeapon* NewWeapon, class AOrionWeapon* LastWeapon)
{
	AOrionWeapon* LocalLastWeapon = NULL;

	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// unequip previous
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	// equip new one
	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!
		NewWeapon->OnEquip();
	}
}

void AOrionCharacter::DestroyInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	// remove all weapons from inventory and destroy them
	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		AOrionWeapon* Weapon = Inventory[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			Weapon->Destroy();
		}
	}
}

void AOrionCharacter::AddWeapon(AOrionWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		////Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
	}
}

void AOrionCharacter::RemoveWeapon(AOrionWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		////Weapon->OnLeaveInventory();
		Inventory.RemoveSingle(Weapon);
	}
}

USkeletalMeshComponent* AOrionCharacter::GetPawnMesh() const
{
	return IsFirstPerson() && CurrentWeapon ? CurrentWeapon->ArmsMesh : GetMesh();
}

bool AOrionCharacter::IsAlive() const
{
	return Health > 0;
}

USkeletalMeshComponent* AOrionCharacter::GetSpecifcPawnMesh(bool WantFirstPerson) const
{
	return WantFirstPerson && CurrentWeapon ? CurrentWeapon->ArmsMesh : GetMesh();
}

void AOrionCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// everyone
	DOREPLIFETIME(AOrionCharacter, HealthMax);
	DOREPLIFETIME(AOrionCharacter, Health);

	DOREPLIFETIME_CONDITION(AOrionCharacter, LastTakeHitInfo, COND_Custom);
}

void AOrionCharacter::Destroyed()
{
	Super::Destroyed();
	////DestroyInventory();
}

void AOrionCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority)
	{
		Health = HealthMax;// GetMaxHealth();
		SpawnDefaultInventory();
	}

	// set initial mesh visibility (3rd person view)
	UpdatePawnMeshes();

	// create material instance for setting team colors (3rd person view)
	/*for (int32 iMat = 0; iMat < Mesh->GetNumMaterials(); iMat++)
	{
	MeshMIDs.Add(Mesh->CreateAndSetMaterialInstanceDynamic(iMat));
	}*/
}

float AOrionCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	//AOrionPlayerController* MyPC = Cast<AOrionPlayerController>(Controller);

	if (Health <= 0.f)
	{
		return 0.f;
	}

	// Modify based on game rules.
	AOrionGameMode* const Game = GetWorld()->GetAuthGameMode<AOrionGameMode>();
	////Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}

	return ActualDamage;
}

bool AOrionCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	/*if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
	return false;
	}*/

	Health = FMath::Min(0.0f, Health);

	// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	GetWorld()->GetAuthGameMode<AOrionGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	NetUpdateFrequency = GetDefault<AOrionCharacter>()->NetUpdateFrequency;
	GetCharacterMovement()->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	return true;
}

void AOrionCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (Role == ROLE_Authority)
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, false);

		// play the force feedback effect on the client player controller
		APlayerController* PC = Cast<APlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass)
		{
			UOrionDamageType *DamageType = Cast<UOrionDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageType && DamageType->HitForceFeedback)
			{
				PC->ClientPlayForceFeedback(DamageType->HitForceFeedback, false, "Damage");
			}
		}
	}

	if (DamageTaken > 0.f)
	{
		ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
	}

	/*AOrionPlayerController* MyPC = Cast<AOrionPlayerController>(Controller);
	AOrionHUD* MyHUD = MyPC ? Cast<AOrionHUD>(MyPC->GetHUD()) : NULL;
	if (MyHUD)
	{
	MyHUD->NotifyHit(DamageTaken, DamageEvent, PawnInstigator);
	}

	if (PawnInstigator && PawnInstigator != this && PawnInstigator->IsLocallyControlled())
	{
	AOrionPlayerController* InstigatorPC = Cast<AOrionPlayerController>(PawnInstigator->Controller);
	AOrionHUD* InstigatorHUD = InstigatorPC ? Cast<AShooterHUD>(InstigatorPC->GetHUD()) : NULL;
	if (InstigatorHUD)
	{
	InstigatorHUD->NotifyEnemyHit();
	}
	}*/
}

void AOrionCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator, LastTakeHitInfo.DamageCauser);
	}
	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator, LastTakeHitInfo.DamageCauser);
	}
}

void AOrionCharacter::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled)
{
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if ((PawnInstigator == LastTakeHitInfo.PawnInstigator) && (LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass) && (LastTakeHitTimeTimeout == TimeoutTime))
	{
		// same frame damage
		if (bKilled && LastTakeHitInfo.bKilled)
		{
			// Redundant death take hit, just ignore it
			return;
		}

		// otherwise, accumulate damage done this frame
		Damage += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = Damage;
	LastTakeHitInfo.PawnInstigator = Cast<AOrionCharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();

	LastTakeHitTimeTimeout = TimeoutTime;
}

void AOrionCharacter::StopAllAnimMontages()
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (UseMesh && UseMesh->AnimScriptInstance)
	{
		UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
	}
}

void AOrionCharacter::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}

	bReplicateMovement = false;
	bTearOff = true;
	bIsDying = true;

	if (Role == ROLE_Authority)
	{
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

		// play the force feedback effect on the client player controller
		APlayerController* PC = Cast<APlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass)
		{
			UOrionDamageType *DamageType = Cast<UOrionDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageType && DamageType->KilledForceFeedback)
			{
				PC->ClientPlayForceFeedback(DamageType->KilledForceFeedback, false, "Damage");
			}
		}
	}

	// cannot use IsLocallyControlled here, because even local client's controller may be NULL here
	if (GetNetMode() != NM_DedicatedServer && DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	// remove all weapons
	DestroyInventory();

	// switch back to 3rd person view
	UpdatePawnMeshes();

	DetachFromControllerPendingDestroy();
	StopAllAnimMontages();

	/*if (LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
	{
	LowHealthWarningPlayer->Stop();
	}

	if (RunLoopAC)
	{
	RunLoopAC->Stop();
	}*/

	// disable collisions on capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);

	// Death anim
	float DeathAnimDuration = PlayAnimMontage(DeathAnim);

	// Ragdoll
	if (DeathAnimDuration > 0.f)
	{
		GetWorldTimerManager().SetTimer(this, &AOrionCharacter::SetRagdollPhysics, FMath::Min(0.1f, DeathAnimDuration), false);
	}
	else
	{
		SetRagdollPhysics();
	}
}

void AOrionCharacter::SetRagdollPhysics()
{
	bool bInRagdoll = false;

	if (IsPendingKill())
	{
		bInRagdoll = false;
	}
	else if (!GetMesh() || !GetMesh()->GetPhysicsAsset())
	{
		bInRagdoll = false;
	}
	else
	{
		// initialize physics/etc
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;

		bInRagdoll = true;
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	if (!bInRagdoll)
	{
		// hide and set short lifespan
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}
	else
	{
		SetLifeSpan(45.0f);
	}
}

void AOrionCharacter::PossessedBy(class AController* InController)
{
	Super::PossessedBy(InController);

	// [server] as soon as PlayerState is assigned, set team colors of this pawn for local player
	////UpdateTeamColorsAllMIDs();

	UpdatePawnMeshes();
}

USkeletalMeshComponent* AOrionCharacter::GetMeshFromIndex(int32 index) const
{
	switch (index)
	{
	case 0:
		return HelmetMesh;
		break;
	case 1:
		return BodyMesh;
		break;
	case 2:
		return ArmsMesh;
		break;
	case 3:
		return LegsMesh;
		break;
	case 4:
		return Flight1Mesh;
		break;
	case 5:
		return Flight2Mesh;
		break;
	}

	return NULL;
}

void AOrionCharacter::UpdatePawnMeshes()
{
	bool const bFirst = IsFirstPerson();

	GetMesh()->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetOwnerNoSee(bFirst);

	BodyMesh->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	BodyMesh->SetOwnerNoSee(bFirst);

	HelmetMesh->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	HelmetMesh->SetOwnerNoSee(bFirst);

	ArmsMesh->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	ArmsMesh->SetOwnerNoSee(bFirst);

	LegsMesh->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	LegsMesh->SetOwnerNoSee(bFirst);

	Flight1Mesh->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Flight1Mesh->SetOwnerNoSee(bFirst);

	Flight2Mesh->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Flight2Mesh->SetOwnerNoSee(bFirst);

	if (CurrentWeapon)
		CurrentWeapon->OnEquip();
}

class UPawnMovementComponent* AOrionCharacter::GetMovementComponent() const
{
	return Super::GetMovementComponent();
}

void AOrionCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateRootYaw(DeltaSeconds);

	UpdateAimKick(DeltaSeconds);
}

void AOrionCharacter::UpdateAimKick(float DeltaSeconds)
{
	AimKick = FMath::RInterpTo(AimKick, TargetAimKick, DeltaSeconds, 20.0f);
}

void AOrionCharacter::UpdateRootYaw(float DeltaSeconds)
{
	if (RootRotation.Yaw > TargetYaw)
	{
		if (RootRotation.Yaw - TargetYaw > 180.0f)
		{
			RootRotation.Yaw -= 360.0f;
			RootRotation.Yaw = FMath::Min(TargetYaw, RootRotation.Yaw + DeltaSeconds*180.0f);
		}
		else
		{
			RootRotation.Yaw = FMath::Max(TargetYaw, RootRotation.Yaw - DeltaSeconds*180.0f);
		}
	}
	else
	{
		if (TargetYaw - RootRotation.Yaw > 180.0f)
		{
			RootRotation.Yaw += 360.0f;
			RootRotation.Yaw = FMath::Max(TargetYaw, RootRotation.Yaw - DeltaSeconds*180.0f);
		}
		else
		{
			RootRotation.Yaw = FMath::Min(TargetYaw, RootRotation.Yaw + DeltaSeconds*180.0f);
		}
	}
}

void AOrionCharacter::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	// Only replicate this property for a short duration after it changes so join in progress players don't get spammed with fx when joining late
	DOREPLIFETIME_ACTIVE_OVERRIDE(AOrionCharacter, LastTakeHitInfo, GetWorld() && GetWorld()->GetTimeSeconds() < LastTakeHitTimeTimeout);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AOrionCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	InputComponent->BindAction("Reload", IE_Pressed, this, &AOrionCharacter::Reload);

	InputComponent->BindAction("Melee", IE_Pressed, this, &AOrionCharacter::DoMelee);

	InputComponent->BindAction("Duck", IE_Pressed, this, &AOrionCharacter::Duck);
	InputComponent->BindAction("Duck", IE_Released, this, &AOrionCharacter::UnDuck);

	InputComponent->BindAction("Run", IE_Pressed, this, &AOrionCharacter::Sprint);
	InputComponent->BindAction("Run", IE_Released, this, &AOrionCharacter::StopSprint);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AOrionCharacter::OnFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AOrionCharacter::OnStopFire);
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AOrionCharacter::TouchStarted);

	InputComponent->BindAction("Aim", IE_Pressed, this, &AOrionCharacter::StartAiming);
	InputComponent->BindAction("Aim", IE_Released, this, &AOrionCharacter::StopAiming);

	InputComponent->BindAxis("MoveForward", this, &AOrionCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AOrionCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AOrionCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AOrionCharacter::LookUpAtRate);

	InputComponent->BindAction("BehindView", IE_Pressed, this, &AOrionCharacter::BehindView);
}

void AOrionCharacter::BehindView()
{
	bFirstPerson = !bFirstPerson;

	UpdatePawnMeshes();
}

void AOrionCharacter::Duck()
{
	GetWorldTimerManager().SetTimer(this, &AOrionCharacter::DoRoll, 0.35, false);
}

void AOrionCharacter::StopDucking()
{
	bDuck = false;
}

void AOrionCharacter::UnDuck()
{
	if (GetWorldTimerManager().IsTimerActive(this, &AOrionCharacter::DoRoll))
	{
		GetWorldTimerManager().ClearTimer(this, &AOrionCharacter::DoRoll);
		bDuck = !bDuck;
	}
}

FRotator AOrionCharacter::GetRootRotation() const
{
	return RootRotation;
}

void AOrionCharacter::DoRoll()
{
	float Length = 1.0f;

	if (RollAnimation.Backwards != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			FVector AimDirWS = GetVelocity();
			AimDirWS.Normalize();
			const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
			const FRotator AimRotLS = AimDirLS.Rotation();

			if (AimRotLS.Yaw<-110.0f || AimRotLS.Yaw>110.0f)
			{
				TargetYaw = 180.0f + AimRotLS.Yaw;
				Length = AnimInstance->Montage_Play(RollAnimation.Backwards, 1.f);
			}
			else if (AimRotLS.Yaw < -80.0f)
			{
				TargetYaw = 0.0f;
				Length = AnimInstance->Montage_Play(RollAnimation.Left, 1.f);
			}
			else if (AimRotLS.Yaw < 80.0f)
			{
				TargetYaw = AimRotLS.Yaw;
				Length = AnimInstance->Montage_Play(RollAnimation.Forwards, 1.f);
			}
			else
			{
				TargetYaw = 0.0f;
				Length = AnimInstance->Montage_Play(RollAnimation.Right, 1.f);
			}

			bRolling = true;

			GetWorldTimerManager().SetTimer(this, &AOrionCharacter::EndRoll, Length, false);
			GetWorldTimerManager().SetTimer(this, &AOrionCharacter::ResetRootRotation, Length*0.8f, false);
		}
	}
}

APlayerController *AOrionCharacter::GetPlayerController()
{
	return Cast<APlayerController>(Controller);
}

void AOrionCharacter::FaceRotation(FRotator NewControlRotation, float DeltaTime)
{
	if (IsRolling())
		return;

	if (IsLocallyControlled() && Cast<AOrionPlayerController>(Controller) != NULL)
	{
		Super::FaceRotation(NewControlRotation, DeltaTime);
		return;
	}

	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewControlRotation, DeltaTime, 2.5f);

	Super::FaceRotation(CurrentRotation, DeltaTime);
}

void AOrionCharacter::ResetRootRotation()
{
	bRolling = false;
	RootRotation.Yaw = 0.0f;
}

void AOrionCharacter::EndRoll()
{
	bRolling = false;
	TargetYaw = 0.0f;
}

void AOrionCharacter::Sprint()
{
	if (bDuck)
	{
		StopDucking();
	}

	OnStopFire();
	StopAiming();

	bRun = true;
}

float AOrionCharacter::PlayOneShotAnimation(UAnimMontage *Anim)
{
	return PlayAnimMontage(Anim);
}

bool AOrionCharacter::IsSprinting() const
{
	return bRun;
}

bool AOrionCharacter::IsDucking() const
{
	return bDuck;
}

bool AOrionCharacter::IsAiming() const
{
	return bAim;
}

void AOrionCharacter::StopSprint()
{
	bRun = false;
}

void AOrionCharacter::DoMelee()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Melee();
	}
}

void AOrionCharacter::Reload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
}

void AOrionCharacter::StopAiming()
{
	if (CurrentWeapon)
	{
		bAim = false;
		CurrentWeapon->StopAiming();
	}
}

bool AOrionCharacter::ShouldPlayWeaponAnim() const
{
	return !bRun && CurrentWeapon && !bRolling
		&& !GetMesh()->AnimScriptInstance->Montage_IsActive(CurrentWeapon->ReloadAnim.Pawn3P)
		&& !GetMesh()->AnimScriptInstance->Montage_IsActive(CurrentWeapon->MeleeAnim.Pawn3P)
		&& !GetMesh()->AnimScriptInstance->Montage_IsActive(CurrentWeapon->EquipAnim.Pawn3P);
}

int32 AOrionCharacter::GetWeaponIndex() const
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetWeaponIndex();
	}

	return 0;
}

void AOrionCharacter::AddAimKick(FRotator Kick)
{
	LastAimKickTime = GetWorld()->TimeSeconds;

	Kick.Pitch *= CurrentWeapon->GetSpreadModifier();

	TargetAimKick.Pitch = FMath::Min(TargetAimKick.Pitch + (TargetAimKick.Pitch>2.0f ? Kick.Pitch - 1.0f : Kick.Pitch), 4.0f);
	TargetAimKick.Yaw = FMath::Clamp(TargetAimKick.Yaw + Kick.Yaw, -2.0f, 2.0f);
}

void AOrionCharacter::StartAiming()
{
	StopSprint();

	if (CurrentWeapon)
	{
		bAim = true;
		CurrentWeapon->StartAiming();
	}
}

void AOrionCharacter::OnStopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void AOrionCharacter::OnFire()
{
	StopSprint();

	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}

	// try and play a firing animation if specified
	/*if(FireAnimation != NULL)
	{
	// Get the animation object for the arms mesh
	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	if(AnimInstance != NULL)
	{
	AnimInstance->Montage_Play(FireAnimation, 1.f);
	}
	}*/

}

void AOrionCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// only fire for first finger down
	if (FingerIndex == 0)
	{
		OnFire();
	}
}

void AOrionCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

float AOrionCharacter::GetFootOffset(FName Socket) const
{
	//make sure we have a mesh
	if (GetMesh() == NULL)
		return 0.0;

	FVector pos;
	FRotator rot;

	GetMesh()->GetSocketWorldLocationAndRotation(Socket, pos, rot);

	FHitResult Hit;

	FCollisionQueryParams TraceParams(FName(TEXT("FootTrace")), true, this);

	TraceParams.AddIgnoredActor(this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FVector vStart = pos;
	vStart.Z = GetActorLocation().Z;// +Mesh->RelativeLocation.Z;

	FVector vEnd = vStart - FVector(0,0,GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*2.0);

	//if (GWorld->LineTraceSingle(Hit, vStart, vEnd, COLLISION_WEAPON,TraceParams))
	if (GWorld->SweepSingle(Hit, vStart, vEnd, FQuat::Identity, ECollisionChannel::ECC_Pawn, FCollisionShape::MakeSphere(5), TraceParams))
	{
		return FMath::Clamp(vStart.Z - Hit.Location.Z, 40.0f, 165.0f);
	}

	return 96.0;
}

void AOrionCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AOrionCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AOrionCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AOrionCharacter::SetHelmetMesh(USkeletalMesh* newMesh) const
{
	HelmetMesh->SetSkeletalMesh(newMesh);
}

void AOrionCharacter::SetBodyMesh(USkeletalMesh* newMesh) const
{
	BodyMesh->SetSkeletalMesh(newMesh);
}

void AOrionCharacter::SetArmsMesh(USkeletalMesh* newMesh) const
{
	ArmsMesh->SetSkeletalMesh(newMesh);
}

void AOrionCharacter::SetLegsMesh(USkeletalMesh* newMesh) const
{
	LegsMesh->SetSkeletalMesh(newMesh);
}

void AOrionCharacter::SetFlight1Mesh(USkeletalMesh* newMesh) const
{
	Flight1Mesh->SetSkeletalMesh(newMesh);
}

void AOrionCharacter::SetFlight2Mesh(USkeletalMesh* newMesh) const
{
	Flight2Mesh->SetSkeletalMesh(newMesh);
}

UBlendSpace* AOrionCharacter::GetAnimationWalkBlend() const
{
	return WalkBlend;
}

UBlendSpace* AOrionCharacter::GetAnimationHoverBlend() const
{
	return HoverBlend;
}

UBlendSpace* AOrionCharacter::GetAnimationAimBlend() const
{
	return AimBlend;
}

UAnimMontage* AOrionCharacter::GetAnimationAimBase() const
{
	return AimBase;
}

UAnimMontage* AOrionCharacter::GetSprintAnimation() const
{
	return SprintAnimation;
}