// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionWeapon.h"
#include "OrionCharacter.h"
#include "OrionGameMode.h"
#include "OrionPlayerController.h"
#include "OrionMovementComponent.h"
#include "OrionAbility.h"
#include "OrionBuff.h"
//#include "OrionAIController.h"
#include "OrionProjectile.h"
#include "OrionShipPawn.h"
#include "OrionHoverVehicle.h"
#include "OrionPRI.h"
#include "OrionGrenade.h"
#include "OrionSkeletalMeshComponent.h"

//////////////////////////////////////////////////////////////////////////
// AOrionCharacter

AOrionCharacter::AOrionCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UOrionMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	RotationRate = 0.0f;

	RingMat = nullptr;

	//AIControllerClass = AOrionAIController::StaticClass();

	// Create a CameraComponent	
	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera

	ThirdPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->AttachParent = GetCapsuleComponent();
	ThirdPersonCameraComponent->RelativeLocation = FVector(0, 0, 0); // Position the camera

	//static ConstructorHelpers::FObjectFinder<UBlueprint> HealthBarObject(TEXT("/Game/UI/HUD/Widgets/HealthBar"));
	//if (HealthBarObject.Object != NULL)
	//{
	//	DefaultHealthBarClass = (UClass*)HealthBarObject.Object->GeneratedClass;
	//}

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	/*Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;*/

	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = false;// true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	GetMesh()->StreamingDistanceMultiplier = 10.0f;
	GetMesh()->LDMaxDrawDistance = 12500.0f;

	//InvokerComponent = ObjectInitializer.CreateOptionalDefaultSubobject<UNavigationInvokerComponent>(this, TEXT("NavInvoker"));
	//InvokerComponent->bAutoActivate = true;

	Arms1PMesh = ObjectInitializer.CreateOptionalDefaultSubobject<UOrionSkeletalMeshComponent>(this, TEXT("Arms1PMesh1P"));
	Arms1PMesh->AlwaysLoadOnClient = true;
	Arms1PMesh->AlwaysLoadOnServer = false;
	Arms1PMesh->bOwnerNoSee = false;
	Arms1PMesh->bOnlyOwnerSee = true;
	Arms1PMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	Arms1PMesh->bCastDynamicShadow = true;
	Arms1PMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Arms1PMesh->bChartDistanceFactor = true;
	Arms1PMesh->bGenerateOverlapEvents = false;
	Arms1PMesh->AttachParent = FirstPersonCameraComponent;
	Arms1PMesh->CastShadow = false;
	Arms1PMesh->bPerBoneMotionBlur = false;
	Arms1PMesh->WeaponFOV = 60.0f;
	//Arms1PMesh->SetMasterPoseComponent(GetMesh());

	Arms1PArmorMesh = ObjectInitializer.CreateOptionalDefaultSubobject<UOrionSkeletalMeshComponent>(this, TEXT("Arms1PArmorMesh1P"));
	Arms1PArmorMesh->AlwaysLoadOnClient = true;
	Arms1PArmorMesh->AlwaysLoadOnServer = false;
	Arms1PArmorMesh->bOwnerNoSee = false;
	Arms1PArmorMesh->bOnlyOwnerSee = true;
	Arms1PArmorMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Arms1PArmorMesh->bCastDynamicShadow = true;
	Arms1PArmorMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Arms1PArmorMesh->bChartDistanceFactor = true;
	Arms1PArmorMesh->bGenerateOverlapEvents = false;
	Arms1PArmorMesh->AttachParent = Arms1PMesh;
	Arms1PArmorMesh->CastShadow = false;
	Arms1PArmorMesh->bPerBoneMotionBlur = false;
	Arms1PArmorMesh->SetMasterPoseComponent(Arms1PMesh);
	Arms1PArmorMesh->WeaponFOV = 60.0f;

	/*Arms1PLegsMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("LegsMesh1P"));
	Arms1PLegsMesh->AlwaysLoadOnClient = true;
	Arms1PLegsMesh->AlwaysLoadOnServer = false;
	Arms1PLegsMesh->bOwnerNoSee = false;
	Arms1PLegsMesh->bOnlyOwnerSee = true;
	Arms1PLegsMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Arms1PLegsMesh->bCastDynamicShadow = true;
	Arms1PLegsMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Arms1PLegsMesh->bChartDistanceFactor = true;
	Arms1PLegsMesh->bGenerateOverlapEvents = false;
	Arms1PLegsMesh->AttachParent = FirstPersonCameraComponent;
	Arms1PLegsMesh->CastShadow = false;
	Arms1PLegsMesh->bPerBoneMotionBlur = false;*/

	BodyMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Body"));
	BodyMesh->AlwaysLoadOnClient = true;
	BodyMesh->AlwaysLoadOnServer = true;
	BodyMesh->bOwnerNoSee = false;// true;
	BodyMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	BodyMesh->bCastDynamicShadow = true;
	BodyMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	BodyMesh->bChartDistanceFactor = true;
	//BodyMesh->SetCollisionProfileName(CollisionProfileName);
	BodyMesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	BodyMesh->AttachParent = GetMesh();
	BodyMesh->SetMasterPoseComponent(GetMesh());
	BodyMesh->StreamingDistanceMultiplier = 10.0f;

	HelmetMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Helmet"));
	HelmetMesh->AlwaysLoadOnClient = true;
	HelmetMesh->AlwaysLoadOnServer = true;
	HelmetMesh->bOwnerNoSee = false;// true;
	HelmetMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	HelmetMesh->bCastDynamicShadow = true;
	HelmetMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	HelmetMesh->bChartDistanceFactor = true;
	//HelmetMesh->SetCollisionProfileName(CollisionProfileName);
	HelmetMesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	HelmetMesh->AttachParent = GetMesh();
	HelmetMesh->SetMasterPoseComponent(GetMesh());
	HelmetMesh->StreamingDistanceMultiplier = 10.0f;

	ArmsMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Arms"));
	ArmsMesh->AlwaysLoadOnClient = true;
	ArmsMesh->AlwaysLoadOnServer = true;
	ArmsMesh->bOwnerNoSee = false;// true;
	ArmsMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	ArmsMesh->bCastDynamicShadow = true;
	ArmsMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	ArmsMesh->bChartDistanceFactor = true;
	//ArmsMesh->SetCollisionProfileName(CollisionProfileName);
	ArmsMesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	ArmsMesh->AttachParent = GetMesh();
	ArmsMesh->SetMasterPoseComponent(GetMesh());
	ArmsMesh->StreamingDistanceMultiplier = 10.0f;

	LegsMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Legs"));
	LegsMesh->AlwaysLoadOnClient = true;
	LegsMesh->AlwaysLoadOnServer = true;
	LegsMesh->bOwnerNoSee = false;// true;
	LegsMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	LegsMesh->bCastDynamicShadow = true;
	LegsMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	LegsMesh->bChartDistanceFactor = true;
	//LegsMesh->SetCollisionProfileName(CollisionProfileName);
	LegsMesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	LegsMesh->AttachParent = GetMesh();
	LegsMesh->SetMasterPoseComponent(GetMesh());
	LegsMesh->StreamingDistanceMultiplier = 10.0f;

	Flight1Mesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Flight1"));
	Flight1Mesh->AlwaysLoadOnClient = true;
	Flight1Mesh->AlwaysLoadOnServer = true;
	Flight1Mesh->bOwnerNoSee = false;// true;
	Flight1Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Flight1Mesh->bCastDynamicShadow = true;
	Flight1Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Flight1Mesh->bChartDistanceFactor = true;
	//Flight1Mesh->SetCollisionProfileName(CollisionProfileName);
	Flight1Mesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	Flight1Mesh->AttachParent = GetMesh();
	Flight1Mesh->SetMasterPoseComponent(GetMesh());
	Flight1Mesh->StreamingDistanceMultiplier = 10.0f;

	Flight2Mesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Flight2"));
	Flight2Mesh->AlwaysLoadOnClient = true;
	Flight2Mesh->AlwaysLoadOnServer = true;
	Flight2Mesh->bOwnerNoSee = false;// true;
	Flight2Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Flight2Mesh->bCastDynamicShadow = true;
	Flight2Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Flight2Mesh->bChartDistanceFactor = true;
	//Flight2Mesh->SetCollisionProfileName(CollisionProfileName);
	Flight2Mesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	Flight2Mesh->AttachParent = GetMesh();
	Flight2Mesh->SetMasterPoseComponent(GetMesh());
	Flight2Mesh->StreamingDistanceMultiplier = 10.0f;

	PawnSensor = ObjectInitializer.CreateOptionalDefaultSubobject<UPawnSensingComponent>(this, TEXT("Pawn Sensor"));
	PawnSensor->SensingInterval = .25f; // 4 times per second
	PawnSensor->bOnlySensePlayers = false;
	PawnSensor->SetPeripheralVisionAngle(85.f);

	RingMesh = ObjectInitializer.CreateOptionalDefaultSubobject<UStaticMeshComponent>(this, TEXT("Ring"));
	RingMesh->AddLocalOffset(FVector(0.0f, 0.0f, -85.0f));
	RingMesh->AttachParent = GetCapsuleComponent();

	Health = 100.0;
	HealthMax = 100.0;

	Shield = 500.0;
	ShieldMax = 500.0;

	GrenadeCooldown = 0.0f;

	HeadScale = 1.0f;
	LeftLegScale = 1.0f;
	RightLegScale = 1.0f;
	LeftArmScale = 1.0f;
	RightArmScale = 1.0f;

	bFirstPerson = true;
	UpdatePawnMeshes();

	FlyingOffset = 100.0f;
}

void AOrionCharacter::OnHearNoise(APawn *HeardPawn, const FVector &Location, float Volume)
{
	//this is only needed for aicontrollers
	AOrionAIController *AI = Cast<AOrionAIController>(Controller);
	if (AI)
		AI->OnHearNoise(HeardPawn, Location, Volume);
}

void AOrionCharacter::OnSeePawn(APawn *SeenPawn)
{
	//this is only needed for aicontrollers
	AOrionAIController *AI = Cast<AOrionAIController>(Controller);
	if (AI)
		AI->OnSeePawn(SeenPawn);
}

void AOrionCharacter::SetDrivenVehicle(AOrionHoverVehicle *newVehicle)
{
	DrivenVehicle = newVehicle;

	AttachDriver();
}

void AOrionCharacter::AttachDriver()
{
	//tell our controller to possess the vehicle and unpossess us
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetController());
	if (PC)
	{
		//set us to hidden
		GetMesh()->SetHiddenInGame(true);
		GetMesh()->AttachTo(DrivenVehicle->VehicleMesh);

		PC->UnPossess();
		PC->Possess(DrivenVehicle);

		SetActorEnableCollision(false);
	}
}

void AOrionCharacter::DetachDriver()
{
	//tell our controller to possess the vehicle and unpossess us
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(DrivenVehicle->GetController());
	if (PC)
	{
		//set us to not hidden
		GetMesh()->SetHiddenInGame(false);
		GetMesh()->DetachFromParent(true);
		GetMesh()->AttachTo(GetCapsuleComponent());
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

		PC->UnPossess();
		PC->Possess(this);

		SetActorEnableCollision(true);
	}
}

FVector2D AOrionCharacter::GetAim(float DeltaTime)
{
	FVector pos;
	FRotator rot;

	if (CurrentWeapon == NULL)
		return FVector2D(0.0f, 0.0f);

	if (IsSprinting())
		return FVector2D(0.0f, 0.0f);

	////if (!Controller || !IsLocallyControlled())
	////	return FVector2D(AimYaw, AimPitch);

	//GetPawnMesh()->GetSocketWorldLocationAndRotation(FName("Aim"), pos, rot);

	//GetWeapon()->GetWeaponMesh(false)->GetSocketWorldLocationAndRotation(FName("MuzzleFlashSocket"), pos, rot);
	FVector AimDirWS = /*(CameraLocation + Controller->GetControlRotation().Vector()*5000.0) - pos;*/ GetBaseAimRotation().Vector();
	//DrawDebugBox(GWorld, CameraLocation + Controller->GetControlRotation().Vector()*5000.0, FVector(10, 10, 10), FColor(255, 0, 0, 255), false);
	AimDirWS.Normalize();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	//we need to offset the aim a bit based on player third person camera offsets
	/////AimYaw = (2.5f + AimRotLS.Yaw) / 90.0f;
	////AimPitch = AimRotLS.Pitch / 90.0;

	////if (Role < ROLE_Authority)
	////	ServerSetAimYaw(AimYaw, AimPitch);

	////return FVector2D(AimYaw, AimPitch);

	return FVector2D((2.5f + AimRotLS.Yaw) / 90.0f, AimRotLS.Pitch / 90.0);
}

bool AOrionCharacter::ServerSetAimYaw_Validate(float yaw, float pitch)
{
	return true;
}

void AOrionCharacter::ServerSetAimYaw_Implementation(float yaw, float pitch)
{
	AimYaw = yaw;
	AimPitch = pitch;
}

void AOrionCharacter::AddHealth(int32 Amount)
{
	Health = FMath::Min(HealthMax, Health + Amount);
}

void AOrionCharacter::AddShield(int32 Amount)
{
	Shield = FMath::Min(ShieldMax, Shield + Amount);
}

void AOrionCharacter::AddExp(int32 Amount)
{
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
		Y = FVector::CrossProduct(Z, GetActorRotation().Vector());
		Y.Normalize();
		Z = FVector::CrossProduct(X, Y);
		Z.Normalize();

		X *= CameraDist;

		if (Health > 0)
		{
			CamStart = GetMesh()->GetBoneLocation(FName("Chr01_Spine02"));
			CamStart.Z += GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - 18.0;
			if (bDuck)
				CamStart.Z += 25.0;
		}

		OutResult.Location = CamStart - X*CameraOffset.X + CameraOffset.Y*Y + CameraOffset.Z*Z;

		//FRotator rot;
		//GetMesh()->GetSocketWorldLocationAndRotation(FName("Camera"), OutResult.Location, rot);

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
	if (GetMovementComponent() && GetMovementComponent()->IsFalling())
		Speed2D = 0.0f;

	if (IsRolling() || IsSprinting() || (CurrentSkill && CurrentSkill->IsJetpacking()))
		Speed2D = 0.0f;

	if (Speed2D < 10)
		BobTime += 0.2 * FMath::Min(0.1f,DeltaTime);
	else
		BobTime += FMath::Min(0.1f, DeltaTime) * FMath::Min(1.0f, 0.3f + 0.7f * Speed2D / GetMovementComponent()->GetMaxSpeed());

	//Bob = Lerp(Bob, (bIsWalking ? default.Bob : default.Bob*BobScaleWhenRunning), DeltaSeconds * 10);

	float TimeDilation = 1.0f;

	if (IsAiming())
		TimeDilation = 0.4f;

	WalkBob = Y * 0.01 * Speed2D * FMath::Sin(8.0f * BobTime * TimeDilation);
	WalkBob.Z = 0.01 * Speed2D * FMath::Sin(16.0f * BobTime * TimeDilation);

	//FirstPersonCameraComponent->SetRelativeLocation(FMath::Lerp(FirstPersonCameraComponent->RelativeLocation, FVector(0, 0, 64.0) + WalkBob, FMath::Min(0.5f,DeltaTime*10.0f)));
	OutResult.Location += WalkBob * (IsAiming() ? 0.02f : 0.05f);
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
	if (IsFirstPerson() || (PlayerState && PlayerState->bIsABot))
		return GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight);
	//else if (bIsABot && GetMesh())
	//	return GetMesh()->GetSocketLocation(FName("Bite"));
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

void AOrionCharacter::SpawnDefaultAbilities()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	int32 NumAbilities = AbilityClasses.Num();
	for (int32 i = 0; i < NumAbilities; i++)
	{
		if (AbilityClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.Owner = this;

			CurrentSkill = GetWorld()->SpawnActor<AOrionAbility>(AbilityClasses[i], SpawnInfo);
			//only do one ability for now
			break;
		}
	}
}

void AOrionCharacter::SpawnDefaultInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	int32 NumWeaponClasses = DefaultInventoryClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.Owner = this;
			AOrionWeapon* NewWeapon = GetWorld()->SpawnActor<AOrionWeapon>(DefaultInventoryClasses[i], SpawnInfo);
			AddWeapon(NewWeapon);
		}
	}

	// equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
		ClientEquipWeapon(Inventory[0]);
	}
}

void AOrionCharacter::SpawnClassWeapons(int32 ClassIndex)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.Owner = this;

	AOrionWeapon* NewWeapon = nullptr;
	TArray<TSubclassOf<class AOrionWeapon> > WeaponClasses;

	switch(ClassIndex)
	{
	case 0:
		WeaponClasses = DefaultAssaultInventoryClasses;
		break;
	case 1:
		WeaponClasses = DefaultSupportInventoryClasses;
		break;
	case 2:
		WeaponClasses = DefaultReconInventoryClasses;
		break;
	}

	if (WeaponClasses.Num() <= 0)
		return;

	for (int32 i = 0; i < WeaponClasses.Num(); i++)
	{
		if (WeaponClasses[i])
		{
			NewWeapon = GetWorld()->SpawnActor<AOrionWeapon>(WeaponClasses[i], SpawnInfo);
			if (NewWeapon)
				AddWeapon(NewWeapon);
		}
	}

	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
		ClientEquipWeapon(Inventory[0]);
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
			SetCurrentWeapon(Weapon);
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

//void AOrionCharacter::OnRep_CurrentWeapon(AOrionWeapon* LastWeapon)
//{
	//SetCurrentWeapon(CurrentWeapon, LastWeapon);
//}

void AOrionCharacter::OnRep_Inventory()
{
	UpdatePawnMeshes();

	//todo: make this check for best weapon to use when getting a new gun!
	if (IsLocallyControlled() && Inventory.Num() > 0)
		EquipWeapon(Inventory[0]);

	//check to make sure everything is still valid, clean up old guns and whatnot

}

void AOrionCharacter::OnRep_NextWeapon()
{
	if (!IsLocallyControlled() && NextWeapon != nullptr)
		SetCurrentWeapon(NextWeapon, CurrentWeapon);
}

void AOrionCharacter::HandleSpecialWeaponFire(FName SocketName)
{

}

void AOrionCharacter::SetCurrentWeapon(class AOrionWeapon* NewWeapon, class AOrionWeapon* LastWeapon)
{
	//if we're already putting our weapon down, just set the next weapon and don't trigger any anims
	if (GetWorldTimerManager().IsTimerActive(UnEquipTimer))
	{
		NextWeapon = NewWeapon;
		return;
	}

	AOrionWeapon* LocalLastWeapon = NULL;
	float Duration = 0.01f;

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
		Duration = LocalLastWeapon->OnUnEquip();
	}

	if (Duration < 0.1f)
		Duration = 0.1f;

	NextWeapon = NewWeapon;
	GetWorldTimerManager().SetTimer(UnEquipTimer, this, &AOrionCharacter::ReallyDoEquip, 0.9*Duration, false);
}

void AOrionCharacter::ReallyDoEquip()
{
	CurrentWeapon = NextWeapon;

	// equip new one
	if (NextWeapon)
	{
		if (Role == ROLE_Authority)
			NextWeapon->SetOwningPawn(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!
		NextWeapon->OnEquip();
	}

	//if (Role == ROLE_Authority)
		//NextWeapon = nullptr;
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
			//don't destroy it, it will still exist inside the inventory manager!
			//remove this once playfab is fully setup
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
		Weapon->SetOwningPawn(this);
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

AOrionWeapon *AOrionCharacter::GetWeaponFromType(EItemType Type)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Type == Inventory[i]->InventoryType)
			return Inventory[i];
	}

	return nullptr;
}

//make sure our equipped weapon is legit, otherwise equip one that is
void AOrionCharacter::CheckWeaponEquipped()
{
	AOrionWeapon *Weap = GetWeapon();

	if (!Weap || Inventory.Num() < 1)
		return;

	bool bLegit = true;

	for (int32 i = 0; i < Inventory.Num() && !bLegit; i++)
	{
		if (GetWeapon() == Inventory[i])
			bLegit = true;
	}

	if (!bLegit)
		EquipWeapon(Inventory[0]);
}

USkeletalMeshComponent* AOrionCharacter::GetPawnMesh() const
{
	return IsFirstPerson() && CurrentWeapon ? Arms1PMesh : GetMesh();
}

bool AOrionCharacter::IsAlive() const
{
	return Health > 0;
}

USkeletalMeshComponent* AOrionCharacter::GetSpecifcPawnMesh(bool WantFirstPerson) const
{
	return WantFirstPerson && CurrentWeapon ? Arms1PMesh : GetMesh();
}

void AOrionCharacter::OnRep_ArmorIndex()
{
	SetArmor(ArmorIndex);
}

void AOrionCharacter::OnRep_ShipPawn()
{
	if (CurrentShip.Ship)
	{
		CameraShip = CurrentShip.Ship;
		AttachToShip();
	}
	else
		DetachFromShip();
}

void AOrionCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// everyone
	DOREPLIFETIME(AOrionCharacter, HealthMax);
	DOREPLIFETIME(AOrionCharacter, Health);
	DOREPLIFETIME(AOrionCharacter, ShieldMax);
	DOREPLIFETIME(AOrionCharacter, Shield);

	DOREPLIFETIME(AOrionCharacter, GibRep);

	DOREPLIFETIME(AOrionCharacter, HelmetArmor);
	DOREPLIFETIME(AOrionCharacter, BodyArmor);
	DOREPLIFETIME(AOrionCharacter, ArmsArmor);
	DOREPLIFETIME(AOrionCharacter, LegsArmor);

	DOREPLIFETIME(AOrionCharacter, CurrentShip);

	DOREPLIFETIME(AOrionCharacter, AimPos);
	DOREPLIFETIME(AOrionCharacter, bRolling);
	DOREPLIFETIME(AOrionCharacter, BlinkPos);
	DOREPLIFETIME(AOrionCharacter, Level);

	DOREPLIFETIME(AOrionCharacter, bFatality);
	DOREPLIFETIME(AOrionCharacter, FatalityAnim);
	DOREPLIFETIME(AOrionCharacter, GibCenter);

	DOREPLIFETIME_CONDITION(AOrionCharacter, CameraShip, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(AOrionCharacter, LastTakeHitInfo, COND_Custom);

	//DOREPLIFETIME(AOrionCharacter, CurrentWeapon);
	DOREPLIFETIME_CONDITION(AOrionCharacter, NextWeapon, COND_SkipOwner);

	DOREPLIFETIME(AOrionCharacter, Inventory);

	DOREPLIFETIME(AOrionCharacter, ReplicatedAnimation);

	DOREPLIFETIME(AOrionCharacter, TargetYaw);

	DOREPLIFETIME(AOrionCharacter, ArmorIndex);

	//skip owner
	DOREPLIFETIME_CONDITION(AOrionCharacter, AimYaw, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AOrionCharacter, AimPitch, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AOrionCharacter, bRun, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AOrionCharacter, bFly, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AOrionCharacter, bAim, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AOrionCharacter, bDuck, COND_SkipOwner);

	DOREPLIFETIME(AOrionCharacter, GrenadeCooldown);
	DOREPLIFETIME(AOrionCharacter, BlinkCooldown);
	DOREPLIFETIME(AOrionCharacter, RollCooldown);

	DOREPLIFETIME(AOrionCharacter, DrivenVehicle);
	DOREPLIFETIME(AOrionCharacter, CurrentSkill);

	DOREPLIFETIME(AOrionCharacter, bShoulderCamera);
	DOREPLIFETIME(AOrionCharacter, bShipCamera);
}

void AOrionCharacter::Destroyed()
{
	Super::Destroyed();
	////DestroyInventory();
}

void AOrionCharacter::FinishExitingShip()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->bEnablePhysicsInteraction = true;

	//DetachRootComponentFromParent(true);
	SetActorEnableCollision(true);

	//disable player control for a bit
	GetWorldTimerManager().SetTimer(ExitShipTimer, this, &AOrionCharacter::EnableControl, 0.5f, false);
}

void AOrionCharacter::EnableControl()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetController());

	if (PC)
	{
		PC->SetCinematicMode(false, true, true);
		PC->ClientSetCinematicMode(false, true, true, true);
	}
}

bool AOrionCharacter::ServerSetAimPos_Validate(FVector pos)
{
	return true;
}

void AOrionCharacter::ServerSetAimPos_Implementation(FVector pos)
{
	AimPos = pos;
}

void AOrionCharacter::AttachToShip()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->bEnablePhysicsInteraction = false;
	GetCharacterMovement()->SetComponentTickEnabled(false);

	//FString Socket = FString::Printf(TEXT("Spawn%i"), CurrentShip->GetNumPassengers() + 1);
	AttachRootComponentTo(CurrentShip.Ship->GetMesh(), FName(*CurrentShip.Socket), EAttachLocation::SnapToTarget, true);
	SetActorEnableCollision(false);
}

void AOrionCharacter::DetachFromShip()
{
	FWeaponAnim Info;
	Info.Pawn3P = ExitShipAnim;
	float length = OrionPlayAnimMontage(Info, 1.0f, TEXT(""), true, true, false);

	DetachRootComponentFromParent(true);
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->SetComponentTickEnabled(true);

	if (length < 0.1f)
		length = 0.1f;

	GetWorldTimerManager().SetTimer(ExitShipTimer, this, &AOrionCharacter::FinishExitingShip, length * 1.1f, false);
}

//this is a server only function
bool AOrionCharacter::SetShip(AOrionPlayerController *PC, AOrionShipPawn *Ship)
{
	CurrentShip.Ship = Ship;
	if (Ship != nullptr)
		CameraShip = Ship;

	AOrionPRI *PRI = nullptr;

	if (PC)
		PRI = Cast<AOrionPRI>(PC->PlayerState);

	if (!PC || !PRI)
		return false;

	if (CurrentShip.Ship == nullptr)
	{
		if (PRI)
			PRI->bOnShip = false;

		DetachFromShip();

		return true;
	}

	if (PRI)
		PRI->bOnShip = true;

	CurrentShip.Socket = FString::Printf(TEXT("Spawn%i"), CurrentShip.Ship->GetNumPassengers() + 1);
	AttachToShip();

	if (PC)
	{
		PC->SetCinematicMode(true, true, true);
		PC->ClientSetCinematicMode(true, true, true, true);
	}

	return true;
}

bool AOrionCharacter::IsOnShip()
{
	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	if (PRI)
	{
		return PRI->IsOnShip();
	}

	return false;
}

void AOrionCharacter::InitMaterials()
{
	if (!CloakParent)
		return;

	//setup some materials
	CharacterMats.Empty();
	CharacterCloakMats.Empty();

	CharacterMats.SetNumUninitialized(5);
	CharacterCloakMats.SetNumUninitialized(5);

	//reset materials to default if needed
	if (GetMesh()) GetMesh()->OverrideMaterials.Empty();
	if (HelmetMesh) HelmetMesh->OverrideMaterials.Empty();
	if (BodyMesh) BodyMesh->OverrideMaterials.Empty();
	if (LegsMesh) LegsMesh->OverrideMaterials.Empty();
	if (ArmsMesh) ArmsMesh->OverrideMaterials.Empty();

	CharacterMats[0] = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this); if (CharacterMats[0]) GetMesh()->SetMaterial(0, CharacterMats[0]);
	CharacterMats[1] = UMaterialInstanceDynamic::Create(HelmetMesh->GetMaterial(0), this); if (CharacterMats[1]) HelmetMesh->SetMaterial(0, CharacterMats[1]);//head
	CharacterMats[2] = UMaterialInstanceDynamic::Create(BodyMesh->GetMaterial(0), this); if (CharacterMats[2]) BodyMesh->SetMaterial(0, CharacterMats[2]);//body
	CharacterMats[3] = UMaterialInstanceDynamic::Create(LegsMesh->GetMaterial(0), this); if (CharacterMats[3]) LegsMesh->SetMaterial(0, CharacterMats[3]);//legs
	CharacterMats[4] = UMaterialInstanceDynamic::Create(ArmsMesh->GetMaterial(0), this); if (CharacterMats[4]) ArmsMesh->SetMaterial(0, CharacterMats[4]);//hands

	//some greasy sorting for going cloaked with armor on
	if (GetMesh()) GetMesh()->SetTranslucentSortPriority(1);
	if (HelmetMesh) HelmetMesh->SetTranslucentSortPriority(2);
	if (BodyMesh) BodyMesh->SetTranslucentSortPriority(2);
	if (LegsMesh) LegsMesh->SetTranslucentSortPriority(2);
	if (ArmsMesh) ArmsMesh->SetTranslucentSortPriority(2);

	CharacterCloakMats[0] = UMaterialInstanceDynamic::Create(CloakParent, this); if(GetMesh() && GetMesh()->SkeletalMesh) CharacterCloakMats[0]->CopyParameterOverrides(Cast<UMaterialInstance>(GetMesh()->SkeletalMesh->Materials[0].MaterialInterface));
	CharacterCloakMats[1] = UMaterialInstanceDynamic::Create(CloakParent, this); if (HelmetMesh && HelmetMesh->SkeletalMesh) CharacterCloakMats[1]->CopyParameterOverrides(Cast<UMaterialInstance>(HelmetMesh->SkeletalMesh->Materials[0].MaterialInterface));
	CharacterCloakMats[2] = UMaterialInstanceDynamic::Create(CloakParent, this); if (BodyMesh && BodyMesh->SkeletalMesh) CharacterCloakMats[2]->CopyParameterOverrides(Cast<UMaterialInstance>(BodyMesh->SkeletalMesh->Materials[0].MaterialInterface));
	CharacterCloakMats[3] = UMaterialInstanceDynamic::Create(CloakParent, this); if (LegsMesh && LegsMesh->SkeletalMesh) CharacterCloakMats[3]->CopyParameterOverrides(Cast<UMaterialInstance>(LegsMesh->SkeletalMesh->Materials[0].MaterialInterface));
	CharacterCloakMats[4] = UMaterialInstanceDynamic::Create(CloakParent, this); if (ArmsMesh && ArmsMesh->SkeletalMesh) CharacterCloakMats[4]->CopyParameterOverrides(Cast<UMaterialInstance>(ArmsMesh->SkeletalMesh->Materials[0].MaterialInterface));
}

void AOrionCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority)
	{
		bShoulderCamera = FMath::RandRange(0, 1) == 1;
		bShipCamera = !bShoulderCamera;
	}

	if (PawnSensor)
	{
		PawnSensor->OnSeePawn.AddDynamic(this, &AOrionCharacter::OnSeePawn);
		PawnSensor->OnHearNoise.AddDynamic(this, &AOrionCharacter::OnHearNoise);
	}
	
	InitMaterials();

	if (Role == ROLE_Authority)
	{
		Health = HealthMax;
		SpawnDefaultInventory();
		SpawnDefaultAbilities();
	}

	// set initial mesh visibility (3rd person view)
	UpdatePawnMeshes();

	//spawn our health bar hud element
	if (GetNetMode() != NM_DedicatedServer)
		CreateHealthBar();

	UpdatePlayerRingColor();
}

void AOrionCharacter::OnRep_GibAll()
{
	GibAll(GibCenter);
}

void AOrionCharacter::GibAll(FVector Center)
{
	for (int32 i = 0; i < Gibs.Num(); i++)
	{
		if (Gibs[i].bActivated)
			continue;

		AOrionGib *Gib = Cast<AOrionGib>(Gibs[i].Gib.GetDefaultObject());

		if (Gib && Gib->Mesh)
		{
			//TargetBone = GetMesh()->GetSocketBoneName(Gib->SocketName);
			FVector loc = GetMesh()->GetSocketLocation(Gib->SocketName);

			FVector vel = (loc - Center).GetSafeNormal() * FMath::FRandRange(500.0, 1500.0f);
			//do gib action
			FVector pos;
			FRotator rot;
			GetMesh()->GetSocketWorldLocationAndRotation(Gib->SocketName, pos, rot);
			SpawnGibs(i, pos, rot, vel);
		}
	}

	if (Role == ROLE_Authority)
		GibCenter = Center;
}

void AOrionCharacter::HandleGibs(float damage, FDamageEvent const& DamageEvent)
{
	const FPointDamageEvent *RealEvent = (FPointDamageEvent*)&DamageEvent;
	FName CurrentBone, TargetBone;

	if (RealEvent)
	{
		for (int32 i = 0; i < Gibs.Num(); i++)
		{
			if (Gibs[i].bActivated)
				continue;

			AOrionGib *Gib = Cast<AOrionGib>(Gibs[i].Gib.GetDefaultObject());

			if (Gib && Gib->Mesh)
			{
				CurrentBone = RealEvent->HitInfo.BoneName;
				TargetBone = GetMesh()->GetSocketBoneName(Gib->SocketName);

				while (CurrentBone != NAME_None)
				{
					if (TargetBone == CurrentBone)
					{
						FVector vel = RealEvent->ShotDirection * FMath::Max(500.0f, FMath::Min(1500.0f, damage * 2.5f));
						//do gib action
						FVector pos;
						FRotator rot;
						GetMesh()->GetSocketWorldLocationAndRotation(Gib->SocketName, pos, rot);
						SpawnGibs(i, pos, rot, vel);

						GibRep.Index = i;
						GibRep.Socket = Gib->SocketName;
						GibRep.Velocity = vel;

						return;
					}

					CurrentBone = GetMesh()->GetParentBone(CurrentBone);
				}
			}
		}
	}
}

void AOrionCharacter::PlayFootStepSound(USoundCue *Cue, FVector pos)
{
	if (GetWorld()->GetTimeSeconds() - LastFootStepSound < 0.25f)
		return;

	LastFootStepSound = GetWorld()->GetTimeSeconds();

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Cue, pos, bRun ? 0.5f : 0.25f, 1.0f);
}

void AOrionCharacter::OnRep_Gibs()
{
	FVector pos;
	FRotator rot;

	GetMesh()->GetSocketWorldLocationAndRotation(GibRep.Socket, pos, rot);

	SpawnGibs(GibRep.Index, pos, rot, GibRep.Velocity);
}

void AOrionCharacter::SpawnGibs(int32 index, FVector pos, FRotator rot, FVector dir)
{
	if (index >= Gibs.Num())
		return;

	//spawn the actual gib into the world
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.Owner = this;

	AOrionGib *NewGib = GetWorld()->SpawnActor<AOrionGib>(Gibs[index].Gib, pos + FVector(0.0f, 0.0f, 20.0f), rot, SpawnInfo);

	if (NewGib)
	{
		//hide the bone on the mesh
		////GetMesh()->HideBoneByName(GetMesh()->GetSocketBoneName(NewGib->SocketName), PBO_None);
		NewGib->Mesh->AddImpulse(dir + FVector(0, 0, 250.0f), NAME_None, true);
		NewGib->BloodMat = BloodDecal;

		Gibs[index].bActivated = true;			//make sure it only happens once
		Gibs[index].TimeTillBloodDecal = 2.0f;	//delay the blood pool to give the mesh time to stop moving

		switch (NewGib->GibType)
		{
		case GIB_HEAD:
			HeadScale = 0.0f;
			break;
		case GIB_LEFTLEG:
			LeftLegScale = 0.0f;
			break;
		case GIB_RIGHTLEG:
			RightLegScale = 0.0f;
			break;
		case GIB_LEFTARM:
			LeftArmScale = 0.0f;
			break;
		case GIB_RIGHTARM:
			RightArmScale = 0.0f;
			break;
		}

		if (NewGib->DoBloodSpurt)
		{
			FName BloodSpurt = FName(*(NewGib->SocketName.ToString() + TEXT("Spurt")));
			if (GetMesh()->GetSocketByName(BloodSpurt))
				UParticleSystemComponent* BloodPSC = UGameplayStatics::SpawnEmitterAttached(BloodSpurtFX, GetMesh(), BloodSpurt);
		}
	}
}

void AOrionCharacter::UpdateBloodDecals(float DeltaSeconds)
{
	//first update any timers relating to pending blood decals
	for (auto giter(Gibs.CreateIterator()); giter; giter++)
	{
		if (giter && giter->bActivated && giter->TimeTillBloodDecal >= 0.0f)
		{
			giter->TimeTillBloodDecal -= DeltaSeconds;

			if (giter->TimeTillBloodDecal <= 0.0f)
			{
				giter->TimeTillBloodDecal = -1.0f;

				AOrionGib *Gib = Cast<AOrionGib>(giter->Gib.GetDefaultObject());

				if (BloodDecal && Gib && Gib->Mesh)
				{
					FVector pos;
					FRotator rot;
					GetMesh()->GetSocketWorldLocationAndRotation(Gib->SocketName, pos, rot);

					FHitResult Hit;

					FCollisionQueryParams TraceParams(FName(TEXT("FootTrace")), true, this);

					TraceParams.AddIgnoredActor(this);
					TraceParams.bTraceAsyncScene = true;
					TraceParams.bReturnPhysicalMaterial = true;

					if (GetWorld()->LineTraceSingleByObjectType(Hit, pos, pos - FVector(0.0f, 0.0f, 150.0f), FCollisionObjectQueryParams::DefaultObjectQueryParam, TraceParams))
					{
						pos = Hit.ImpactPoint;

						UDecalComponent *Dec = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BloodDecal, FVector(FMath::RandRange(64, 128), FMath::RandRange(64, 128), 32.0f), pos, FRotator(-90, 0, 0), 30.0f);
						if (Dec)
						{
							FDecalHelper NewDecal;
							NewDecal.Mat = UMaterialInstanceDynamic::Create(Dec->GetMaterial(0), this);
							NewDecal.TimeLeft = 1.2f;

							Dec->SetMaterial(0, NewDecal.Mat);

							//Dec->SetRelativeScale3D(FVector(0.0f, 0.0f, 1.0f));

							//Gibs.Remove(*giter);

							BloodDecals.Add(NewDecal);
						}
					}
					//else
					//	Gibs.Remove(*giter);
				}
			}
		}
	}

	for (auto iter(BloodDecals.CreateIterator()); iter; iter++)
	{
		if (iter)// && iter->Mat->IsValidLowLevel())
		{
			iter->TimeLeft = iter->TimeLeft - DeltaSeconds * 0.1f;
			iter->Mat->SetScalarParameterValue("Time_M", FMath::Min(0.99f, 1.2f - iter->TimeLeft));

			if (iter->TimeLeft <= -1.0f)
			{
				FDecalHelper tDecal;
				tDecal.Mat = iter->Mat;
				BloodDecals.Remove(tDecal);
				//iter->Mat = NULL;
			}
		}
	}
}

void AOrionCharacter::SetAbility(AOrionAbility *NewAbility)
{
	if (CurrentSkill)
		CurrentSkill->Destroy();

	CurrentSkill = NewAbility;
}

bool AOrionCharacter::IsOvercharging() const
{
	if (CurrentSkill && CurrentSkill->IsOvercharging())
		return true;

	return false;
}

float AOrionCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	UOrionDamageType *DamageType = Cast<UOrionDamageType>(DamageEvent.DamageTypeClass.GetDefaultObject());

	if (Health <= 0.f)
	{
		if (DamageType && DamageType->bGibAll)
		{
			const FPointDamageEvent *RealEvent = (FPointDamageEvent*)&DamageEvent;
			GibAll(RealEvent->HitInfo.ImpactPoint);
		}
		//see if we want to blow some body pieces off
		else
			HandleGibs(Damage, DamageEvent);

		return 0.f;
	}

	if (IsOvercharging())
		Damage /= 2.0f;

	//this is the number that damage numbers will see
	float OriginalDamage = Damage;

	// Modify based on game rules.
	AOrionGameMode* const Game = GetWorld()->GetAuthGameMode<AOrionGameMode>();

	if (!DamageType || !DamageType->bIgnoreModify)
		Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	if (DamageType && DamageType->bKnockBack && (!CurrentSkill || !CurrentSkill->IsJetpacking()))
	{
		//ignore damage to self
		if (EventInstigator != Controller && GetMovementComponent())
		{
			AOrionCharacter *Damager = Cast<AOrionCharacter>(DamageCauser);
			if (Damager && Damager->GetMesh())
			{
				FVector pos;
				pos = Damager->GetMesh()->GetSocketLocation(DamageType->KnockbackSocket);

				FRotator dir;
				dir = Damager->GetMesh()->GetSocketRotation(DamageType->KnockbackSocket);

				UOrionMovementComponent *Comp = Cast<UOrionMovementComponent>(GetMovementComponent());
				if (Comp)
				{
					FVector force = dir.Vector();// GetActorLocation() - pos;
					//force.Z = DamageType->KnockbackUpForce;
					force.Z = 0.0f;
					force.Normalize();
					Comp->AddImpulse(force * DamageType->KnockbackForce + FVector(0.0f, 0.0f, DamageType->KnockbackUpForce), true);
				}
				//GetMovementComponent()->AddRadialImpulse(pos, 500.0, DamageType->KnockbackForce, ERadialImpulseFalloff::RIF_Constant, true);
			}
		}
	}

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.f)
	{
		//take off shield damage first
		float TotalDamage = ActualDamage;

		TotalDamage = FMath::Max(0.0f, TotalDamage - Shield);
		Shield = FMath::Max(0.0f, Shield - ActualDamage);

		Health -= TotalDamage;

		AOrionPlayerController *Damager = Cast<AOrionPlayerController>(EventInstigator);
		if (Damager)
		{
			const FPointDamageEvent *RealEvent = (FPointDamageEvent*)&DamageEvent;
			if (RealEvent)
			{
				Damager->AddDamageNumber(OriginalDamage, RealEvent->HitInfo.ImpactPoint);
			}
		}

		//don't let dinos die while doing a fatality
		if (bFinishingMove)
			Health = FMath::Max(1.0f, Health);

		if (Health <= 0)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);

			if (DamageType && DamageType->bGibAll)
			{
				const FPointDamageEvent *RealEvent = (FPointDamageEvent*)&DamageEvent;
				GibAll(RealEvent->HitInfo.ImpactPoint);
			}
			//see if we want to blow some body pieces off
			else
				HandleGibs(Damage, DamageEvent);
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

	//if (Squad && Cast<AOrionAIController>(Controller))
	//	Squad->RemoveSquadMember(Cast<AOrionAIController>(Controller));

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

void AOrionCharacter::SetClassArmor(int32 index)
{
	if (ArmorList.Num() > index)
	{
		EquipArmor(ArmorList[index].HeadArmor.GetDefaultObject());
		EquipArmor(ArmorList[index].BodyArmor.GetDefaultObject());
		EquipArmor(ArmorList[index].LegsArmor.GetDefaultObject());
		EquipArmor(ArmorList[index].ArmsArmor.GetDefaultObject());
	}
}

void AOrionCharacter::SetArmor(int32 index)
{
	ArmorIndex = index;

	EventUpdateHelmet(index);
	EventUpdateBody(index);
	EventUpdateArms(index);
	EventUpdateLegs(index);
	EventUpdateFlight1(index);
	EventUpdateFlight2(index);
	EventUpdate1PArmor(index);
}

void AOrionCharacter::StopAllAnimMontages()
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (UseMesh && UseMesh->AnimScriptInstance)
	{
		UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
	}

	if (CurrentWeapon)
	{
		USkeletalMeshComponent* WeaponMesh = CurrentWeapon->GetWeaponMesh(bFirstPerson);
		if (WeaponMesh && WeaponMesh->AnimScriptInstance)
			WeaponMesh->AnimScriptInstance->Montage_Stop(0.0f);
	}
}

void AOrionCharacter::CreateHealthBar()
{
	if (!DefaultHealthBarClass || MyHealthBar || Health <= 0)
		return;

	//EventCreateHealthBar();

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());

	if (PC)
		PC->EventCreateHealthBar(this);

	/*MyHealthBar = CreateWidget<UOrionHealthBar>(GetWorld(), DefaultHealthBarClass);

	if (MyHealthBar)
	{
		MyHealthBar->SetFlags(RF_RootSet);
		MyHealthBar->AddToViewport(10);
		MyHealthBar->OwningPawn = this;
	}*/
}

void AOrionCharacter::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}

	//remove outline
	GetMesh()->SetRenderCustomDepth(false);
	BodyMesh->SetRenderCustomDepth(false);
	HelmetMesh->SetRenderCustomDepth(false);
	ArmsMesh->SetRenderCustomDepth(false);
	LegsMesh->SetRenderCustomDepth(false);
	Flight1Mesh->SetRenderCustomDepth(false);
	Flight2Mesh->SetRenderCustomDepth(false);

	if (CurrentSkill)
		CurrentSkill->DeactivateSkill();

	if (CurrentWeapon)
		CurrentWeapon->StopFire();

	if (MyHealthBar)
	{
		MyHealthBar->ConditionalBeginDestroy();
		MyHealthBar->RemoveFromParent();
		MyHealthBar = nullptr;
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

	// hide ring
	if (RingMesh)
		RingMesh->SetHiddenInGame(true);

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)// && PC->IsLocalPlayerController())
	{
		PC->PlayerState->bIsSpectator = true;
		PC->bPlayerIsWaiting = true;
		PC->ChangeState(NAME_Spectating);

		if (!PC->IsLocalPlayerController())
			PC->ClientSetDeathSpectate(bFatality? nullptr : this);
	}

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

	//remove us completely if we got eaten
	if (bFatality)
	{
		Destroy();
		return;
	}

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
	FWeaponAnim Info;
	Info.Pawn3P = DeathAnim;
	float DeathAnimDuration = OrionPlayAnimMontage(Info);

	// Ragdoll
	if (DeathAnimDuration > 0.f)
	{
		FTimerHandle THandle;
		GetWorldTimerManager().SetTimer(THandle, this, &AOrionCharacter::SetRagdollPhysics, FMath::Min(0.1f, DeathAnimDuration), false);
	}
	else
	{
		SetRagdollPhysics();
	}
}

void AOrionCharacter::ClientEquipWeapon_Implementation(AOrionWeapon *Weapon)
{
	SetCurrentWeapon(Weapon);
}

void AOrionCharacter::OnRep_ReplicatedAnimation()
{
	//skip if this animation is only played locally
	if (IsLocallyControlled() && ReplicatedAnimation.bReplicatedToOwner == false)
		return;

	FWeaponAnim Info;
	Info.Pawn1P = ReplicatedAnimation.Mesh1PMontage;
	Info.Pawn3P = ReplicatedAnimation.Mesh3PMontage;
	Info.Weapon1P = ReplicatedAnimation.Weapon1PMontage;
	Info.Weapon3P = ReplicatedAnimation.Weapon3PMontage;
	Info.bHideWeapon = ReplicatedAnimation.bHideWeapon;

	if (ReplicatedAnimation.bStopAllOtherAnims)
		StopAllAnimMontages();

	OrionPlayAnimMontage(Info, ReplicatedAnimation.Rate, ReplicatedAnimation.SectionName);
}

//override this so we can add some replication to it
float AOrionCharacter::OrionPlayAnimMontage(const FWeaponAnim Animation, float InPlayRate, FName StartSectionName, bool bShouldReplicate, bool bReplicateToOwner, bool bStopOtherAnims)
{
	//don't play any animations while rolling, looks funkeh
	if (bRolling && Role == ROLE_Authority)
		return 0.0f;

	float Duration = 0.0f;

	if (Role == ROLE_Authority && bShouldReplicate)
	{
		//if (ReplicatedAnimation.Montage == AnimMontage)
			ReplicatedAnimation.bToggle = !ReplicatedAnimation.bToggle;

		ReplicatedAnimation.Weapon1PMontage = Animation.Weapon1P;
		ReplicatedAnimation.Weapon3PMontage = Animation.Weapon3P;
		ReplicatedAnimation.Mesh1PMontage = Animation.Pawn1P;
		ReplicatedAnimation.Mesh3PMontage = Animation.Pawn3P;
		ReplicatedAnimation.Rate = InPlayRate;
		ReplicatedAnimation.SectionName = StartSectionName;
		ReplicatedAnimation.bReplicatedToOwner = bReplicateToOwner;
		ReplicatedAnimation.bStopAllOtherAnims = bStopOtherAnims;
		ReplicatedAnimation.bHideWeapon = Animation.bHideWeapon;
	}

	UAnimInstance * AnimInstance = (GetMesh()) ? GetMesh()->GetAnimInstance() : NULL;
	if (AnimInstance)
	{
		//float const Duration = AnimInstance->Montage_Play(AnimMontage, InPlayRate);
		//play weapon animations if needed
		if (CurrentWeapon && Cast<AOrionWeaponDroid>(CurrentWeapon) == nullptr)
		{
			//stop old animations if needed
			if (CurrentWeapon->GetWeaponMesh(true) && !Animation.Weapon1P)
				CurrentWeapon->GetWeaponMesh(true)->AnimScriptInstance->Montage_Stop(0.05);
			if (CurrentWeapon->GetWeaponMesh(false) && !Animation.Weapon3P)
				CurrentWeapon->GetWeaponMesh(false)->AnimScriptInstance->Montage_Stop(0.05);

			if (CurrentWeapon->GetWeaponMesh(true) && Animation.Weapon1P && IsFirstPerson())
				CurrentWeapon->GetWeaponMesh(true)->AnimScriptInstance->Montage_Play(Animation.Weapon1P, 1.0f);// / Animation.Weapon1P->RateScale;
			if (CurrentWeapon->GetWeaponMesh(false) && Animation.Weapon3P && !IsFirstPerson())
				CurrentWeapon->GetWeaponMesh(false)->AnimScriptInstance->Montage_Play(Animation.Weapon3P, 1.0f);// / Animation.Weapon3P->RateScale;
		}

		//play 1p arms animation
		if (Arms1PMesh && Arms1PMesh->AnimScriptInstance && Animation.Pawn1P)
			Duration = Arms1PMesh->AnimScriptInstance->Montage_Play(Animation.Pawn1P, 1.0f) / Animation.Pawn1P->RateScale;// , 1.0);
		//play 3p char animation
		if (Animation.Pawn3P)
		{
			if (Duration < 0.05f)
				Duration = FMath::Max(Duration, AnimInstance->Montage_Play(Animation.Pawn3P, 1.0f) / Animation.Pawn3P->RateScale);
			else
				AnimInstance->Montage_Play(Animation.Pawn3P, 1.0f);
		}
		if (Duration > 0.f)
		{
			// Start at a given Section.
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName);
			}

			/*if (CurrentWeapon && !IsFirstPerson() && CurrentWeapon->ReloadAnim.Weapon3P && CurrentWeapon->GetWeaponMesh(false) && AnimMontage == CurrentWeapon->ReloadAnim.Pawn3P)
			{
				CurrentWeapon->GetWeaponMesh(false)->AnimScriptInstance->Montage_Play(CurrentWeapon->ReloadAnim.Weapon3P, 1.0);
			}
			else if (CurrentWeapon && CurrentWeapon->ReloadAnim.Weapon3P && !IsFirstPerson() && CurrentWeapon->GetWeaponMesh(false))
			{
				CurrentWeapon->GetWeaponMesh(false)->AnimScriptInstance->Montage_Stop(0.05, CurrentWeapon->ReloadAnim.Weapon3P);
			}*/

			if (CurrentWeapon && Animation.bHideWeapon)
			{
				CurrentWeapon->SetActorHiddenInGame(true);
				GetWorldTimerManager().SetTimer(GrenadeHideTimer, this, &AOrionCharacter::UnhideWeapon, Duration * 0.9f, false);
			}

			return Duration;
		}
	}

	return 0.f;
}

void AOrionCharacter::UnhideWeapon()
{
	if (CurrentWeapon)
		CurrentWeapon->SetActorHiddenInGame(false);
}

void AOrionCharacter::EatFood(AOrionFood *Food)
{

}

void AOrionCharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
	UAnimInstance * AnimInstance = (GetMesh()) ? GetMesh()->GetAnimInstance() : NULL;
	UAnimMontage * MontageToStop = (AnimMontage) ? AnimMontage : GetCurrentMontage();
	bool bShouldStopMontage = AnimInstance && MontageToStop && !AnimInstance->Montage_GetIsStopped(MontageToStop);

	if (bShouldStopMontage)
	{
		AnimInstance->Montage_Stop(MontageToStop->BlendOutTime);

		if (!IsFirstPerson() && CurrentWeapon && CurrentWeapon->GetWeaponMesh(false) && AnimMontage == CurrentWeapon->ReloadAnim.Pawn3P)
		{
			CurrentWeapon->GetWeaponMesh(false)->AnimScriptInstance->Montage_Stop(0.05, CurrentWeapon->ReloadAnim.Weapon3P);
		}
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

void AOrionCharacter::UpdatePlayerRingColor()
{
	if (PlayerState == nullptr)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AOrionCharacter::UpdatePlayerRingColor, 1.0, false);
		return;
	}

	if (PlayerState && PlayerState->bIsABot)
		return;

	if (RingMesh && !RingMesh->StaticMesh)
		return;

	if (!RingMat)
	{
		RingMat = UMaterialInstanceDynamic::Create(RingMesh->GetMaterial(0), this);
		RingMesh->SetMaterial(0, RingMat);
	}

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());

	FLinearColor col;
	col.R = 5.0f;
	col.G = 0.0f;
	col.B = 0.0f;
	col.A = 1.0f;

	if (PC)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(PC->PlayerState);
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GetGameState());
		if (PRI && GRI)
		{
			if (PC == Controller)
			{
				col.R = 0.0f;
				col.G = 5.0f;
				col.B = 0.0f;
				col.A = 1.0f;
			}
			else if (GRI->OnSameTeam(PRI, Cast<AOrionPRI>(PlayerState)))
			{
				col.R = 0.0f;
				col.G = 2.0f;
				col.B = 5.0f;
				col.A = 1.0f;
			}
			else
			{
				col.R = 5.0f;
				col.G = 0.0f;
				col.B = 0.0f;
				col.A = 1.0f;
			}
		}
	}

	RingMat->SetVectorParameterValue("CircleColor", col);
}

void AOrionCharacter::PossessedBy(class AController* InController)
{
	Super::PossessedBy(InController);

	// [server] as soon as PlayerState is assigned, set team colors of this pawn for local player
	////UpdateTeamColorsAllMIDs();
	//UpdatePlayerRingColor();

	UpdatePawnMeshes();

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
		PC->Ragdoll = this;

	if (Role == ROLE_Authority)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
		if (PC)
		{
			/*AOrionPRI *PRI = Cast<AOrionPRI>(PC->PlayerState);
			if (PRI && PRI->InventoryManager)
			{
				if (!PRI->PlayFabID.IsEmpty())
					PRI->InventoryManager->EquipItems(this, ITEM_ANY);
			}*/
		}
	}
}

bool AOrionCharacter::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	if (bAlwaysRelevant || RealViewer == Controller || IsOwnedBy(ViewTarget) || IsOwnedBy(RealViewer) || this == ViewTarget || ViewTarget == Instigator
		|| IsBasedOnActor(ViewTarget) || (ViewTarget && ViewTarget->IsBasedOnActor(this)))
	{
		return true;
	}
	else if ((bHidden || bOnlyRelevantToOwner) && (!GetRootComponent() || !GetRootComponent()->IsCollisionEnabled()))
	{
		return false;
	}
	else
	{
		UPrimitiveComponent* MovementBase = GetMovementBase();
		AActor* BaseActor = MovementBase ? MovementBase->GetOwner() : NULL;
		if (MovementBase && BaseActor && GetMovementComponent() && ((Cast<const USkeletalMeshComponent>(MovementBase)) || (BaseActor == GetOwner())))
		{
			return BaseActor->IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
		}
	}

	const AOrionPlayerController *PC = Cast<AOrionPlayerController>(RealViewer);
	FRotator Rot;
	FVector Pos;// = SrcLocation;
	if (PC)
		PC->GetPlayerViewPoint(Pos, Rot);
	else
		Pos = SrcLocation;

	return ((Pos - GetActorLocation()).SizeSquared() < NetCullDistanceSquared);
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
	case 6:
		return Arms1PArmorMesh;
		break;
	}

	return NULL;
}

void AOrionCharacter::UpdatePawnMeshes()
{
	bool const bFirst = IsFirstPerson();

	if (!GetMesh())
		return;

	//GetMesh()->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetHiddenInGame(bFirst || bBlinking); //SetOwnerNoSee(bFirst);

	//BodyMesh->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	BodyMesh->SetHiddenInGame(bFirst || bBlinking); //SetOwnerNoSee(bFirst);

	//HelmetMesh->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	HelmetMesh->SetHiddenInGame(bFirst || bBlinking); //SetOwnerNoSee(bFirst);

	//ArmsMesh->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	ArmsMesh->SetHiddenInGame(bFirst || bBlinking); //SetOwnerNoSee(bFirst);

	//LegsMesh->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	LegsMesh->SetHiddenInGame(bFirst || bBlinking); //>SetOwnerNoSee(bFirst);

	//Flight1Mesh->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Flight1Mesh->SetHiddenInGame(bFirst || bBlinking); //SetOwnerNoSee(bFirst);

	//Flight2Mesh->MeshComponentUpdateFlag = bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Flight2Mesh->SetHiddenInGame(bFirst || bBlinking); //SetOwnerNoSee(bFirst);

	//Arms1PArmorMesh->MeshComponentUpdateFlag = !bFirst ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	if(Arms1PArmorMesh)
		Arms1PArmorMesh->SetHiddenInGame(!bFirst || bBlinking); //SetOwnerNoSee(!bFirst);

	if (CurrentWeapon)
		CurrentWeapon->AttachMeshToPawn();
	//	CurrentWeapon->OnEquip();

	//if we're a local player controller, set us up to render outlines behind walls
	if (Controller && Controller->IsLocalPlayerController())
	{
		GetMesh()->SetRenderCustomDepth(true);
		BodyMesh->SetRenderCustomDepth(true);
		HelmetMesh->SetRenderCustomDepth(true);
		ArmsMesh->SetRenderCustomDepth(true);
		LegsMesh->SetRenderCustomDepth(true);
		Flight1Mesh->SetRenderCustomDepth(true);
		Flight2Mesh->SetRenderCustomDepth(true);
	}
}

class UPawnMovementComponent* AOrionCharacter::GetMovementComponent() const
{
	return Super::GetMovementComponent();
}

void AOrionCharacter::Heal(int32 HealAmount)
{

}

void AOrionCharacter::HandleBuffs(float DeltaSeconds)
{
	//only server does this
	if (Role != ROLE_Authority)
		return;

	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GetGameState());
	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	if (!GRI || !PRI)
		return;

	bool bHiddenFromView = false;

	for (auto Itr(Buffs.CreateIterator()); Itr; ++Itr)
	{
		AOrionBuff *Buff = *Itr;

		if (!Itr)
			continue;

		//see if we're hidden from enemy AI view
		if (Buff->bBlockSight)
			bHiddenFromView = true;

		//check if we need to tick damage or anything
		Buff->TickTimer -= DeltaSeconds;

		if (Buff->TickTimer <= 0.0f)
		{
			//apply damage/heal or whatever
			if (Buff->Damage > 0.0f)
			{
				FPointDamageEvent PointDmg;

				PointDmg.DamageTypeClass = Buff->DamageType;
				PointDmg.Damage = Buff->Damage;

				AController *cOwner = Buff->ControllerOwner;

				if (!cOwner || !cOwner->IsValidLowLevel())
					cOwner = nullptr;

				if (cOwner && !GRI->OnSameTeam(Cast<AOrionPRI>(cOwner->PlayerState), Cast<AOrionPRI>(PlayerState)))
					TakeDamage(Buff->Damage, PointDmg, cOwner, cOwner ? cOwner->GetPawn() : nullptr);
				else if (!cOwner && PRI->GetTeamIndex() != Buff->TeamIndex)
					TakeDamage(Buff->Damage, PointDmg, Controller, this);
			}
			else if (Buff->Damage < 0.0f)
				Heal(int32(-Buff->Damage));

			//reset the ticker
			Buff->TickTimer += Buff->TickInterval;
		}

		//check if the effect should wear off
		if (GetWorld()->GetTimeSeconds() - Buff->LastRefreshedTime >= Buff->Duration)
		{
			bool bBuff = Buff->bIsBuff;

			//remove any effects and status ailments
			Buff->Destroy();

			Buffs.Remove(Buff);

			continue;
		}
	}

	bIsHiddenFromView = bHiddenFromView;
}

void AOrionCharacter::AddBuff(TSubclassOf<AOrionBuff> BuffClass, AController *cOwner, int32 TeamIndex)
{
	//only living players can be buffed/debuffed
	if (Health <= 0)
		return;

	//first check if we already have this buff, if we do, add any stacks and refresh the timer
	for (int32 i = 0; i < Buffs.Num(); i++)
	{
		if (Buffs[i]->GetClass() == BuffClass && TeamIndex == Buffs[i]->TeamIndex)
		{
			Buffs[i]->LastRefreshedTime = GetWorld()->GetTimeSeconds();
			if (Buffs[i]->bStackable)
				Buffs[i]->NumStacks = FMath::Min(Buffs[i]->StackLimit, Buffs[i]->NumStacks + 1);

			return;
		}
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.Owner = this;

	AOrionBuff *Buff = GetWorld()->SpawnActor<AOrionBuff>(BuffClass, FVector(0.0f), FRotator(0), SpawnInfo);
	if (Buff)
	{
		Buff->NumStacks = 1;
		Buff->LastRefreshedTime = GetWorld()->GetTimeSeconds();
		Buff->ControllerOwner = cOwner;
		Buff->TeamIndex = TeamIndex;
		Buffs.Add(Buff);
	}
}

void AOrionCharacter::PerformFatality(UAnimMontage *Anim, UAnimMontage *EnemyAnim, AOrionCharacter *TheVictim)
{
	if (Role == ROLE_Authority)
	{
		FatalityAnim.AttackerAnim = Anim;
		FatalityAnim.VictimAnim = EnemyAnim;
		FatalityAnim.Victim = TheVictim;
		FatalityAnim.bToggle = !FatalityAnim.bToggle;

		//make the victim lose control over themselves
		if (TheVictim)
			TheVictim->bFatality = true;
	}

	bFinishingMove = true;

	//attach the victim to the attacker for now
	if (TheVictim)
	{
		FWeaponAnim vAnim;
		vAnim.Pawn3P = EnemyAnim;

		TheVictim->OrionPlayAnimMontage(vAnim, 1.0f, TEXT(""), false, false, true);

		TheVictim->GetMesh()->AttachTo(GetMesh(), "Swallow");

		//hide weapon
		if (TheVictim->CurrentWeapon)
			TheVictim->CurrentWeapon->SetActorHiddenInGame(true);

		//remove outline if needed
		TheVictim->GetMesh()->SetRenderCustomDepth(false);
		TheVictim->BodyMesh->SetRenderCustomDepth(false);
		TheVictim->HelmetMesh->SetRenderCustomDepth(false);
		TheVictim->ArmsMesh->SetRenderCustomDepth(false);
		TheVictim->LegsMesh->SetRenderCustomDepth(false);
		TheVictim->Flight1Mesh->SetRenderCustomDepth(false);
		TheVictim->Flight2Mesh->SetRenderCustomDepth(false);

		TheVictim->Finisher = this;
	}

	FWeaponAnim aAnim;
	aAnim.Pawn3P = Anim;

	OrionPlayAnimMontage(aAnim, 1.0f, TEXT(""), false, false, true);
}

void AOrionCharacter::OnRep_Fatality()
{
	//play the animations on both us and the victim
	PerformFatality(FatalityAnim.AttackerAnim, FatalityAnim.VictimAnim, FatalityAnim.Victim);
}

void AOrionCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateRootYaw(DeltaSeconds);

	UpdateAimKick(DeltaSeconds);

	UpdateBloodDecals(DeltaSeconds);

	UpdateCooldowns(DeltaSeconds);

	HandleBuffs(DeltaSeconds);

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && !PC->bCinematicMode && !IsOnShip() && (bShoulderCamera || bShipCamera) && !GetWorldTimerManager().IsTimerActive(ExitShipTimer))
	{
		if (GetMovementComponent()->IsFalling() == false && GetMovementComponent()->IsFlying() == false)
		{
			bShoulderCamera = false;
			bShipCamera = false;

			FWeaponAnim Anim;
			Anim.Pawn3P = LandAnim;

			OrionPlayAnimMontage(Anim, 1.0f, TEXT(""), true, false, true);
		}
	}

	//hax for health bars
	if (MyHealthBar == nullptr && GetNetMode() != NM_DedicatedServer)
		CreateHealthBar();

	//hax for now
	//if (CurrentWeapon == NULL && Inventory.Num() > 0)
	//	EquipWeapon(Inventory[0]);
}

void AOrionCharacter::UpdateCooldowns(float DeltaTime)
{
	if (Role == ROLE_Authority)
	{
		GrenadeCooldown = FMath::Max(0.0f, GrenadeCooldown - DeltaTime);
		BlinkCooldown = FMath::Max(0.0f, BlinkCooldown - DeltaTime);
		RollCooldown = FMath::Max(0.0f, RollCooldown - DeltaTime);
	}
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

	InputComponent->BindAction("Use", IE_Pressed, this, &AOrionCharacter::Use);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AOrionCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AOrionCharacter::LookUpAtRate);

	InputComponent->BindAction("BehindView", IE_Pressed, this, &AOrionCharacter::BehindView);

	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &AOrionCharacter::OnNextWeapon);
	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &AOrionCharacter::OnPrevWeapon);

	InputComponent->BindAction("ActivateSkill", IE_Pressed, this, &AOrionCharacter::TryToActivateSkill);

	InputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &AOrionCharacter::TossGrenade);
}

void AOrionCharacter::TossGrenade()
{
	//ignore if the anim is already playing
	if (GetWorldTimerManager().IsTimerActive(GrenadeTimer))
		return;

	if (ShouldIgnoreControls())
		return;

	if (GrenadeCooldown > 0.0f)
		return;

	FWeaponAnim GrenAnim;
	GrenAnim.Pawn3P = GrenadeAnim;
	GrenAnim.bHideWeapon = true;

	float Len = OrionPlayAnimMontage(GrenAnim, 1.0f, TEXT(""), true, false, true);

	if (Role < ROLE_Authority)
		ServerPlayAnimMontage(GrenAnim, 1.0f, TEXT(""), true, false, true);

	GetWorldTimerManager().SetTimer(GrenadeTimer, this, &AOrionCharacter::DoGrenade, Len*0.6f, false);
}

bool AOrionCharacter::ServerTossGrenade_Validate(FVector dir)
{
	return true;
}

void AOrionCharacter::ServerTossGrenade_Implementation(FVector dir)
{
	ActuallyTossGrenade(dir);
}

//throw this bitch
void AOrionCharacter::ActuallyTossGrenade(FVector dir)
{
	if (GrenadeCooldown > 0.0f)
		return;

	if (Role == ROLE_Authority)
	{
		GrenadeCooldown = 20.0f;

		if (GrenadeClass)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.Owner = this;

			FVector pos;
			FRotator rot;

			GetMesh()->GetSocketWorldLocationAndRotation(GrenadeSocket, pos, rot);

			AOrionGrenade *Grenade = GetWorld()->SpawnActor<AOrionGrenade>(GrenadeClass, pos + FVector(0.0f, 0.0f, 25.0f) + dir.GetSafeNormal() * 75.0f, dir.Rotation(), SpawnInfo);
			if (Grenade)
				Grenade->Init(dir);
		}
	}
}

void AOrionCharacter::DoGrenade()
{
	FVector dir;

	dir = GetViewRotation().Vector();

	if (Role < ROLE_Authority)
		ServerTossGrenade(dir * 1000.0f);
	else
		ActuallyTossGrenade(dir * 1000.0f);
}

bool AOrionCharacter::ServerPlayAnimMontage_Validate(const FWeaponAnim Animation, float InPlayRate, FName StartSectionName, bool bShouldReplicate, bool bReplicateToOwner, bool bStopOtherAnims)
{
	return true;
}

void AOrionCharacter::ServerPlayAnimMontage_Implementation(const FWeaponAnim Animation, float InPlayRate, FName StartSectionName, bool bShouldReplicate, bool bReplicateToOwner, bool bStopOtherAnims)
{
	OrionPlayAnimMontage(Animation, InPlayRate, StartSectionName, bShouldReplicate, bReplicateToOwner, bStopOtherAnims);
}

void AOrionCharacter::TryToActivateSkill()
{
	if (ShouldIgnoreControls())
		return;

	if (Role < ROLE_Authority)
		ServerActivateSkill();
	else
		ActivateSkill();
}

bool AOrionCharacter::ServerActivateSkill_Validate()
{
	return true;
}

void AOrionCharacter::ServerActivateSkill_Implementation()
{
	//try to activate our current skill
	ActivateSkill();
}

void AOrionCharacter::ActivateSkill()
{
	UOrionMovementComponent *comp = Cast<UOrionMovementComponent>(GetMovementComponent());
	if (!comp || comp->MovementMode == MOVE_Flying)
		return;

	if (CurrentSkill)
	{
		if (!CurrentSkill->IsSkillActive())
		{
			if (CurrentSkill->ActivateSkill())
			{

			}
		}
		else
		{
			CurrentSkill->DeactivateSkill();
		}
	}
}

void AOrionCharacter::OnNextWeapon()
{
	if (ShouldIgnoreControls())
		return;

	AOrionPlayerController* MyPC = Cast<AOrionPlayerController>(Controller);
	if (MyPC)// && MyPC->IsGameInputAllowed())
	{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || true))//CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			AOrionWeapon* NWeapon = Inventory[(CurrentWeaponIdx + 1) % Inventory.Num()];
			EquipWeapon(NWeapon);
		}
	}
}

void AOrionCharacter::OnPrevWeapon()
{
	if (ShouldIgnoreControls())
		return;

	AOrionPlayerController* MyPC = Cast<AOrionPlayerController>(Controller);
	if (MyPC)// && MyPC->IsGameInputAllowed())
	{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || true))//CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			AOrionWeapon* PrevWeapon = Inventory[(CurrentWeaponIdx - 1 + Inventory.Num()) % Inventory.Num()];
			EquipWeapon(PrevWeapon);
		}
	}
}

bool AOrionCharacter::ServerUse_Validate()
{
	return true;
}

void AOrionCharacter::ServerUse_Implementation()
{
	EnterVehicle();
}

void AOrionCharacter::EnterVehicle()
{
	//check to see if there's any vehicle for us to try to get into
	for (FConstPawnIterator it = GetWorld()->GetPawnIterator(); it; ++it)
	{
		AOrionHoverVehicle *V = Cast<AOrionHoverVehicle>(*it);
		if (V)
		{
			//is it close to us?
			if ((V->GetActorLocation() - GetActorLocation()).Size() < 350.0f)
			{
				V->TryToGetIn(this);
			}
		}
	}
}

void AOrionCharacter::ExitVehicle()
{
	DetachDriver();
	DrivenVehicle = nullptr;
}

void AOrionCharacter::Use()
{
	if (ShouldIgnoreControls())
		return;

	ServerUse();
}

void AOrionCharacter::BehindView()
{
	bFirstPerson = !bFirstPerson;

	UpdatePawnMeshes();
}

bool AOrionCharacter::ShouldIgnoreControls()
{
	return IsRolling() || bBlinking || bShoulderCamera || bShipCamera || bFatality || bFinishingMove;
}

void AOrionCharacter::Duck()
{
	if (ShouldIgnoreControls())
		return;

	GetWorldTimerManager().SetTimer(RollTimer, this, &AOrionCharacter::DoRoll, 0.35, false);
}

void AOrionCharacter::StopDucking()
{
	bDuck = false;
	ServerDuck(bDuck);
}

void AOrionCharacter::UnDuck()
{
	if (GetWorldTimerManager().IsTimerActive(RollTimer))
	{
		GetWorldTimerManager().ClearTimer(RollTimer);
		bDuck = !bDuck;
		ServerDuck(bDuck);
	}
}

bool AOrionCharacter::ServerDuck_Validate(bool bNewDuck)
{
	return true;
}

void AOrionCharacter::ServerDuck_Implementation(bool bNewDuck)
{
	bDuck = bNewDuck;
}

bool AOrionCharacter::ServerRun_Validate(bool bNewRun)
{
	return true;
}

void AOrionCharacter::ServerRun_Implementation(bool bNewRun)
{
	if (bNewRun)
		Sprint();
	else
		StopSprint();
}

bool AOrionCharacter::ServerAim_Validate(bool bNewAim)
{
	return true;
}

void AOrionCharacter::ServerAim_Implementation(bool bNewAim)
{
	if (bNewAim)
		StartAiming();
	else
		StopAiming();
}

FRotator AOrionCharacter::GetRootRotation() const
{
	return RootRotation;
}

void AOrionCharacter::TryToBlink()
{
	//find the direction we want to try and blink in
	FVector dir;
	FRotator ncRot;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	float X, Y;
	if (false)//PC && PC->GetMousePosition(X, Y) && GEngine)
	{
		float X1 = GEngine->GameViewport->Viewport->GetSizeXY().X;
		float Y1 = GEngine->GameViewport->Viewport->GetSizeXY().Y;

		//convert this to a direction for us
		ncRot.Pitch = 0.0f;
		ncRot.Roll = 0.0f;
		AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
		int32 cIndex = CameraIndex;

		if (PRI && CameraIndex == 0)
			cIndex = PRI->GetTeamIndex();
		else
			cIndex = FMath::Max(0, CameraIndex - 1);

		ncRot.Yaw = (FVector(X, Y, 0) - FVector(X1, Y1, 0) / 2.0f).GetSafeNormal().Rotation().Yaw + (cIndex == 0 ? 0 : -180) + 45.0f + 90.0f;

		dir = ncRot.Vector();
	}
	else
	{
		//don't blink unless we're moving
		if (GetVelocity().SizeSquared2D() < 10.0f)
			return;

		dir = GetVelocity().GetSafeNormal();
	}

	/*if (Role < ROLE_Authority)
	{
		bBlinking = true;
		DoBlinkEffect(true);
		ServerBlink(dir);
	}
	else*/
	//this is for client side camera movement
	TeleportStartPos = GetActorLocation();
	Blink(dir);
}

bool AOrionCharacter::ServerBlink_Validate(FVector Pos)
{
	return true;
}

void AOrionCharacter::ServerBlink_Implementation(FVector Dir)
{
	Blink(Dir);
}

void AOrionCharacter::OnRep_Teleport()
{
	if (BlinkPos.End == FVector(0, 0, 0))
	{
		bBlinking = false;
		DoBlinkEffect(false, BlinkPos.Start);
	}
	else
	{
		bBlinking = true;
		DoBlinkEffect(true, BlinkPos.Start);
		LastTeleportTime = GetWorld()->GetTimeSeconds();
	}

	//GetWorldTimerManager().SetTimer(TeleportTimer, this, &AOrionCharacter::EndBlink, 0.2, false);
}

void AOrionCharacter::Blink(FVector dir)
{
	BlinkCooldown = 5.0f;
	if (Role < ROLE_Authority)
	{
		ServerBlink(dir);
		return;
	}

	if (GetWorld() && GetWorld()->GetNavigationSystem())
	{
		FNavLocation loc;
		FNavAgentProperties props;
		props.AgentHeight = 150.0f;
		props.AgentRadius = 150.0f;

		BlinkPos.Start = GetActorLocation();

		for (int32 i = 0; i < 4; i++)
		{
			//find our blink end direction
			FVector EndPos = GetActorLocation() + dir * (4 - i) * 250.0f;
			if (GetWorld()->GetNavigationSystem()->ProjectPointToNavigation(EndPos, loc, FVector(125.0f, 125.0f, 1000.0f), GetWorld()->GetNavigationSystem()->GetNavDataForProps(props)))
			{
				FVector StartPos = GetActorLocation();
				EndPos = loc.Location + GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				if (TeleportTo(EndPos, GetActorRotation()))
				{
					if (GetNetMode() != NM_DedicatedServer)
						DoBlinkEffect(true, StartPos);

					BlinkPos.End = EndPos;
				}
				else // teleport failed, need a better way to handle this
				{
					EndBlink();
					return;
				}
				break;
			}
		}

		if (BlinkPos.End == FVector(0, 0, 0))
		{
			EndBlink();
			return;
		}

		bBlinking = true;
		LastTeleportTime = GetWorld()->GetTimeSeconds();
	}
	else
	{
		EndBlink();
		return;
	}
	
	GetWorldTimerManager().SetTimer(TeleportTimer, this, &AOrionCharacter::ActuallyTeleport, 0.2f, false);
}

void AOrionCharacter::ActuallyTeleport()
{
	//TeleportTo(BlinkPos, GetActorRotation());

	EndBlink();
}

void AOrionCharacter::EndBlink()
{
	bBlinking = false;

	if (Role == ROLE_Authority)
	{
		BlinkPos.Start = GetActorLocation();
		BlinkPos.End = FVector(0, 0, 0);
	}

	//become visible again
	if (GetNetMode() != NM_DedicatedServer)
		DoBlinkEffect(false, GetActorLocation());
}

void AOrionCharacter::DoBlinkEffect(bool bOn, FVector pos)
{
	UpdatePawnMeshes();
	//GetMesh()->SetHiddenInGame(bOn);

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	if (PC)
	{
		PC->SetIgnoreMoveInput(bOn);
		PC->SetIgnoreLookInput(bOn);
	}

	if (BlinkFX)
	{
		UParticleSystemComponent* BlinkPSC = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlinkFX, pos);
		if (BlinkPSC)
		{
			BlinkPSC->SetWorldScale3D(FVector(2.0f));
		}
	}

	if (BlinkSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BlinkSound, GetActorLocation(), 1.0f, 1.0f);
}

bool AOrionCharacter::ServerDoRoll_Validate(ERollDir rDir, FRotator rRot)
{
	return true;
}

void AOrionCharacter::ServerDoRoll_Implementation(ERollDir rDir, FRotator rRot)
{
	FWeaponAnim Info;
	switch (rDir)
	{
	case ROLL_LEFT:
		Info.Pawn3P = RollAnimation.Left;
		break;
	case ROLL_RIGHT:
		Info.Pawn3P = RollAnimation.Right;
		break;
	case ROLL_FORWARDS:
		Info.Pawn3P = RollAnimation.Forwards;
		break;
	case ROLL_BACKWARDS:
		Info.Pawn3P = RollAnimation.Backwards;
		break;
	default:
		break;
	};

	SetActorRotation(rRot);

	RollCooldown = 1.0f;

	if (CurrentWeapon)
	{
		CurrentWeapon->StopReload();
		CurrentWeapon->StopFire();
	}

	StopAllAnimMontages();

	float Length = OrionPlayAnimMontage(Info, 1.0f, FName(""), true, false, true);

	//set roll after anim
	bRolling = true;

	GetWorldTimerManager().SetTimer(RollTimer2, this, &AOrionCharacter::EndRoll, 0.9f*Length, false);
}

void AOrionCharacter::DoRoll()
{
	if (GetWorldTimerManager().IsTimerActive(RollTimer2))
		return;

	if (RollCooldown > 0.0f)
		return;

	float Length = 1.0f;

	//don't roll if we're not moving
	if (GetVelocity().Size2D() < 1.0f)
		return;

	FRotator newRot = GetActorRotation();

	if (RollAnimation.Backwards != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			if (CurrentWeapon && CurrentWeapon->WeaponState == WEAP_RELOADING)
			{
				//CurrentWeapon->StopReload();
			}

			//prevent upper body from doing wacky things
			OnStopFire();
			StopAllAnimMontages();

			FVector AimDirWS = GetVelocity();
			AimDirWS.Normalize();
			const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
			const FRotator AimRotLS = AimDirLS.Rotation();

			FWeaponAnim Info;

			if (AimRotLS.Yaw<-105.0f || AimRotLS.Yaw>105.0f)
			{
				Info.Pawn3P = RollAnimation.Backwards;
				TargetYaw = 180.0f + AimRotLS.Yaw;
				Length = OrionPlayAnimMontage(Info);// AnimInstance->Montage_Play(RollAnimation.Backwards, 1.f) / RollAnimation.Backwards->RateScale;
				RollDir = ROLL_BACKWARDS;
				newRot = (-AimDirWS).Rotation();
				SetActorRotation((-AimDirWS).Rotation());
			}
			else if (AimRotLS.Yaw < -75.0f)
			{
				Info.Pawn3P = RollAnimation.Left;
				TargetYaw = 0.0f;
				Length = OrionPlayAnimMontage(Info);// AnimInstance->Montage_Play(RollAnimation.Left, 1.f) / RollAnimation.Left->RateScale;
				RollDir = ROLL_LEFT;
			}
			else if (AimRotLS.Yaw < 75.0f)
			{
				Info.Pawn3P = RollAnimation.Forwards;
				TargetYaw = AimRotLS.Yaw;
				Length = OrionPlayAnimMontage(Info);// AnimInstance->Montage_Play(RollAnimation.Forwards, 1.f) / RollAnimation.Forwards->RateScale;
				RollDir = ROLL_FORWARDS;
				newRot = AimDirWS.Rotation();
				SetActorRotation(AimDirWS.Rotation());
			}
			else
			{
				Info.Pawn3P = RollAnimation.Right;
				TargetYaw = 0.0f;
				Length = OrionPlayAnimMontage(Info);// AnimInstance->Montage_Play(RollAnimation.Right , 1.f) / RollAnimation.Right->RateScale;
				RollDir = ROLL_RIGHT;
			}
		}
	}

	bRolling = true;

	RollCooldown = 1.0f;

	if (Role < ROLE_Authority)
		ServerDoRoll(RollDir, newRot);

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	if (PC)
	{
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
	}

	GetWorldTimerManager().SetTimer(RollTimer2, this, &AOrionCharacter::EndRoll, 0.9f*Length, false);
	//GetWorldTimerManager().SetTimer(this, &AOrionCharacter::ResetRootRotation, Length*0.8f, false);
}

APlayerController *AOrionCharacter::GetPlayerController()
{
	return Cast<APlayerController>(Controller);
}

void AOrionCharacter::FaceRotation(FRotator NewControlRotation, float DeltaTime)
{
	FRotator ncRot = NewControlRotation;

	if (IsRolling() || IsPlayingRootMotion())
		return;

	/*if (IsLocallyControlled() && Cast<AOrionPlayerController>(Controller) != NULL)
	{
		Super::FaceRotation(NewControlRotation, DeltaTime);
		return;
	}

	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewControlRotation, DeltaTime, 2.5f);

	Super::FaceRotation(CurrentRotation, DeltaTime);*/

	const FRotator CurrentRotation = GetActorRotation();

	if (!bUseControllerRotationPitch && !bFly)
	{
		NewControlRotation.Pitch = CurrentRotation.Pitch;
	}

	if (!bUseControllerRotationYaw)
	{
		NewControlRotation.Yaw = CurrentRotation.Yaw;
	}

	if (!bUseControllerRotationRoll)
	{
		NewControlRotation.Roll = CurrentRotation.Roll;
	}

	if (RotationRate > 0.1f)
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), NewControlRotation, DeltaTime, RotationRate));
	else
		SetActorRotation(NewControlRotation);
}


void AOrionCharacter::ResetRootRotation()
{
	bRolling = false;
	RootRotation.Yaw = 0.0f;
}

void AOrionCharacter::EndRoll()
{
	RollDir = ROLL_NONE;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	if (PC)
	{
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);
	}

	//stop ducking if we roll
	bDuck = false;
	ServerDuck(bDuck);
	
	bRolling = false;
	TargetYaw = 0.0f;
}

void AOrionCharacter::GamepadSprint()
{
	bRun = !bRun;

	if (bRun)
		Sprint();
	else
		StopSprint();
}

void AOrionCharacter::Sprint()
{
	if (Role < ROLE_Authority)
		ServerRun(true);

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
	FWeaponAnim Info;
	Info.Pawn3P = Anim;
	return OrionPlayAnimMontage(Info);
}

bool AOrionCharacter::IsSprinting() const
{
	return bRun && GetMovementComponent()->IsMovingOnGround() && GetMovementComponent()->Velocity.Size2D() > 1.0f;
}

bool AOrionCharacter::IsFlying() const
{
	return bFly;
}

void AOrionCharacter::TakeOff()
{
	bFly = true;

	UCharacterMovementComponent *MoveComp = Cast<UCharacterMovementComponent>(GetMovementComponent());

	if (MoveComp)
		MoveComp->SetMovementMode(MOVE_Flying);
}

void AOrionCharacter::LandFromFlying()
{
	bFly = false;

	UCharacterMovementComponent *MoveComp = Cast<UCharacterMovementComponent>(GetMovementComponent());

	if (MoveComp)
		MoveComp->SetMovementMode(MOVE_Walking);
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
	if (Role < ROLE_Authority)
		ServerRun(false);

	bRun = false;
}

void AOrionCharacter::DoMelee()
{
	if (ShouldIgnoreControls())
		return;

	if (CurrentWeapon)
	{
		CurrentWeapon->Melee();
	}
}

void AOrionCharacter::Reload()
{
	if (ShouldIgnoreControls())
		return;

	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
}

void AOrionCharacter::StopAiming()
{
	if (Role < ROLE_Authority)
		ServerAim(false);

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
		&& !GetMesh()->AnimScriptInstance->Montage_IsActive(CurrentWeapon->EquipAnim.Pawn3P)
		&& !GetMesh()->AnimScriptInstance->Montage_IsActive(CurrentWeapon->HolsterAnim.Pawn3P)
		&& !GetMesh()->AnimScriptInstance->Montage_IsActive(ExitShipAnim);
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

bool AOrionCharacter::IsTopDown()
{
	return false;
}

void AOrionCharacter::StartAiming()
{
	if (ShouldIgnoreControls())
		return;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && PC->bCinematicMode)
		return;

	if (Role < ROLE_Authority)
		ServerAim(true);

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
	if (ShouldIgnoreControls())
		return;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && PC->bCinematicMode)
		return;

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
	if (GWorld->SweepSingleByChannel(Hit, vStart, vEnd, FQuat::Identity, ECollisionChannel::ECC_Pawn, FCollisionShape::MakeSphere(5), TraceParams))
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

FCharacterStats AOrionCharacter::GetCharacterStats()
{
	return CharacterStats;
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

void AOrionCharacter::EquipArmor(AOrionArmor *Armor)
{
	if (Armor == nullptr)
		return;

	switch (Armor->InventoryType)
	{
	case ITEM_HELMET:
		HelmetArmor = Armor;
		SetHelmetMesh(Armor->Mesh);
		break;
	case ITEM_CHEST:
		BodyArmor = Armor;
		SetBodyMesh(Armor->Mesh);
		SetFlight1Mesh(Armor->ExtraMesh);
		break;
	case ITEM_HANDS:
		ArmsArmor = Armor;
		SetArmsMesh(Armor->Mesh);
		Set1PArmorMesh(Armor->Mesh1P);
		break;
	case ITEM_LEGS:
		LegsArmor = Armor;
		SetLegsMesh(Armor->Mesh);
		break;
	};

	InitMaterials();
}

void AOrionCharacter::UnEquipArmor(EItemType Slot)
{
	switch (Slot)
	{
	case ITEM_HELMET:
		HelmetArmor = nullptr;
		SetHelmetMesh(nullptr);
		break;
	case ITEM_CHEST:
		BodyArmor = nullptr;
		SetBodyMesh(nullptr);
		break;
	case ITEM_HANDS:
		ArmsArmor = nullptr;
		SetArmsMesh(nullptr);
		Set1PArmorMesh(nullptr);
		break;
	case ITEM_LEGS:
		LegsArmor = nullptr;
		SetLegsMesh(nullptr);
		break;
	}
}

void AOrionCharacter::OnRep_HelmetArmor(){ if (!HelmetArmor) UnEquipArmor(ITEM_HELMET); else EquipArmor(HelmetArmor); }
void AOrionCharacter::OnRep_BodyArmor(){ if (!BodyArmor) UnEquipArmor(ITEM_CHEST); else EquipArmor(BodyArmor); }
void AOrionCharacter::OnRep_ArmsArmor(){ if (!ArmsArmor) UnEquipArmor(ITEM_HANDS); else EquipArmor(ArmsArmor); }
void AOrionCharacter::OnRep_LegsArmor(){ if (!LegsArmor) UnEquipArmor(ITEM_LEGS); else EquipArmor(LegsArmor); }

void AOrionCharacter::Set1PArmorMesh(USkeletalMesh* newMesh) const
{
	if (Arms1PArmorMesh && GetNetMode() != NM_DedicatedServer)
		Arms1PArmorMesh->SetSkeletalMesh(newMesh);
}

void AOrionCharacter::Set1PLegsMesh(USkeletalMesh* newMesh) const
{
	//Arms1PLegsMesh->SetSkeletalMesh(newMesh);
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