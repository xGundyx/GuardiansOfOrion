// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionWeapon.h"
#include "OrionCharacter.h"
#include "OrionGameMode.h"
#include "OrionPlayerController.h"
#include "OrionMovementComponent.h"
#include "OrionAbility.h"
#include "OrionBuff.h"
#include "OrionWeaponLink.h"
#include "OrionGameUserSettings.h"
//#include "OrionAIController.h"
#include "OrionProjectile.h"
#include "OrionShipPawn.h"
#include "OrionHoverVehicle.h"
#include "OrionPRI.h"
#include "Landscape.h"
#include "OrionGrenade.h"
#include "Kismet/KismetSystemLibrary.h"
#include "OrionWeaponDroid.h"
#include "OrionLobbyPawn.h"
#include "OrionSkeletalMeshComponent.h"

//////////////////////////////////////////////////////////////////////////
// AOrionCharacter

class AOrionWeaponLink;
class AOrionDinoPawn;
class AOrionAIController;
// class ALandscape;

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
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera

	ThirdPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->AttachParent = GetCapsuleComponent();
	ThirdPersonCameraComponent->RelativeLocation = FVector(0, 0, 0); // Position the camera

	//static ConstructorHelpers::FObjectFinder<UBlueprint> HealthBarObject(TEXT("/Game/UI/HUD/Widgets/HealthBar"));
	//if (HealthBarObject.Object != NULL)
	//{
	//	DefaultHealthBarClass = (UClass*)HealthBarObject.Object->GeneratedClass;
	//}

	static ConstructorHelpers::FObjectFinder<UBlueprint> FireBuffHelper(TEXT("/Game/Character/Debuffs/FireDamage"));
	if (FireBuffHelper.Object)
		FireBuff = (UClass*)FireBuffHelper.Object->GeneratedClass;

	static ConstructorHelpers::FObjectFinder<UBlueprint> IceBuffHelper(TEXT("/Game/Character/Debuffs/IceDamage"));
	if (IceBuffHelper.Object)
		IceBuff = (UClass*)IceBuffHelper.Object->GeneratedClass;

	static ConstructorHelpers::FObjectFinder<UBlueprint> LightningBuffHelper(TEXT("/Game/Character/Debuffs/LightningDamage"));
	if (LightningBuffHelper.Object)
		LightningBuff = (UClass*)LightningBuffHelper.Object->GeneratedClass;

	static ConstructorHelpers::FObjectFinder<UBlueprint> PoisonBuffHelper(TEXT("/Game/Character/Debuffs/PoisonDamage"));
	if (PoisonBuffHelper.Object)
		PoisonBuff = (UClass*)PoisonBuffHelper.Object->GeneratedClass;

	static ConstructorHelpers::FObjectFinder<UBlueprint> CorrosiveBuffHelper(TEXT("/Game/Character/Debuffs/CorrosiveDamage"));
	if (CorrosiveBuffHelper.Object)
		CorrosiveBuff = (UClass*)CorrosiveBuffHelper.Object->GeneratedClass;

	static ConstructorHelpers::FObjectFinder<USoundCue> KnifeHit(TEXT("SoundCue'/Game/Weapons/Knife/Sound/KnifeHitCue.KnifeHitCue'"));
	if (KnifeHit.Object)
		KnifeHitSound = Cast<USoundCue>(KnifeHit.Object);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> JetBurn(TEXT("ParticleSystem'/Game/KY_MagicEffects03/Particles/P_fireLv_1.P_fireLv_1'"));
	if (JetBurn.Object)
		JetpackBurnFX = Cast<UParticleSystem>(JetBurn.Object);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> Stun(TEXT("ParticleSystem'/Game/Dinosaurs/Particles/Stunned/StunnedStars.StunnedStars'"));
	if (Stun.Object)
		StunnedFX = Cast<UParticleSystem>(Stun.Object);

	static ConstructorHelpers::FObjectFinder<UMaterial> ThermalHelper(TEXT("Material'/Game/Character/Skills/ThermalVision/ThermalFresnel.ThermalFresnel'"));
	if (ThermalHelper.Object)
		ThermalMat = Cast<UMaterial>(ThermalHelper.Object);

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	/*Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;*/

	//effect gained from being shielded by an elite shielder
	ShieldFX = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("ShieldFX"));
	ShieldFX->AttachParent = GetMesh();
	ShieldFX->RelativeLocation = FVector(0, 0, 0);
	ShieldFX->bAutoActivate = false;

	//any effects given to this pawn when they are an elite
	EliteFX = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("EliteFX"));
	EliteFX->AttachParent = GetMesh();
	EliteFX->RelativeLocation = FVector(0, 0, 0);
	EliteFX->bAutoActivate = false;

	BuffFX = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("BuffFX"));
	BuffFX->AttachParent = GetMesh();
	BuffFX->RelativeLocation = FVector(0, 0, 0);
	BuffFX->bAutoActivate = false;

	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = false;// true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	GetMesh()->StreamingDistanceMultiplier = 1.0f;
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
	//Arms1PMesh->bChartDistanceFactor = true;
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
	//Arms1PArmorMesh->bChartDistanceFactor = true;
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
	//BodyMesh->bChartDistanceFactor = true;
	//BodyMesh->SetCollisionProfileName(CollisionProfileName);
	BodyMesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	BodyMesh->AttachParent = GetMesh();
	BodyMesh->SetMasterPoseComponent(GetMesh());
	BodyMesh->StreamingDistanceMultiplier = 1.0f;

	HelmetMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Helmet"));
	HelmetMesh->AlwaysLoadOnClient = true;
	HelmetMesh->AlwaysLoadOnServer = true;
	HelmetMesh->bOwnerNoSee = false;// true;
	HelmetMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	HelmetMesh->bCastDynamicShadow = true;
	HelmetMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	//HelmetMesh->bChartDistanceFactor = true;
	//HelmetMesh->SetCollisionProfileName(CollisionProfileName);
	HelmetMesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	HelmetMesh->AttachParent = GetMesh();
	HelmetMesh->SetMasterPoseComponent(GetMesh());
	HelmetMesh->StreamingDistanceMultiplier = 1.0f;

	ArmsMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Arms"));
	ArmsMesh->AlwaysLoadOnClient = true;
	ArmsMesh->AlwaysLoadOnServer = true;
	ArmsMesh->bOwnerNoSee = false;// true;
	ArmsMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	ArmsMesh->bCastDynamicShadow = true;
	ArmsMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	//ArmsMesh->bChartDistanceFactor = true;
	//ArmsMesh->SetCollisionProfileName(CollisionProfileName);
	ArmsMesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	ArmsMesh->AttachParent = GetMesh();
	ArmsMesh->SetMasterPoseComponent(GetMesh());
	ArmsMesh->StreamingDistanceMultiplier = 1.0f;

	LegsMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Legs"));
	LegsMesh->AlwaysLoadOnClient = true;
	LegsMesh->AlwaysLoadOnServer = true;
	LegsMesh->bOwnerNoSee = false;// true;
	LegsMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	LegsMesh->bCastDynamicShadow = true;
	LegsMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	//LegsMesh->bChartDistanceFactor = true;
	//LegsMesh->SetCollisionProfileName(CollisionProfileName);
	LegsMesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	LegsMesh->AttachParent = GetMesh();
	LegsMesh->SetMasterPoseComponent(GetMesh());
	LegsMesh->StreamingDistanceMultiplier = 1.0f;

	Flight1Mesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Flight1"));
	Flight1Mesh->AlwaysLoadOnClient = true;
	Flight1Mesh->AlwaysLoadOnServer = true;
	Flight1Mesh->bOwnerNoSee = false;// true;
	Flight1Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Flight1Mesh->bCastDynamicShadow = true;
	Flight1Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	//Flight1Mesh->bChartDistanceFactor = true;
	//Flight1Mesh->SetCollisionProfileName(CollisionProfileName);
	Flight1Mesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	Flight1Mesh->AttachParent = GetMesh();
	Flight1Mesh->SetMasterPoseComponent(GetMesh());
	Flight1Mesh->StreamingDistanceMultiplier = 1.0f;

	Flight2Mesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Flight2"));
	Flight2Mesh->AlwaysLoadOnClient = true;
	Flight2Mesh->AlwaysLoadOnServer = true;
	Flight2Mesh->bOwnerNoSee = false;// true;
	Flight2Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Flight2Mesh->bCastDynamicShadow = true;
	Flight2Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	//Flight2Mesh->bChartDistanceFactor = true;
	//Flight2Mesh->SetCollisionProfileName(CollisionProfileName);
	Flight2Mesh->bGenerateOverlapEvents = false;
	// Mesh acts as the head, as well as the parent for both animation and attachment.
	Flight2Mesh->AttachParent = GetMesh();
	Flight2Mesh->SetMasterPoseComponent(GetMesh());
	Flight2Mesh->StreamingDistanceMultiplier = 1.0f;

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

	GrenadeCooldown.SecondsToRecharge = 25.0f;
	GrenadeCooldown.Energy = GrenadeCooldown.SecondsToRecharge;

	BlinkCooldown.SecondsToRecharge = 5.0f;
	BlinkCooldown.Energy = BlinkCooldown.SecondsToRecharge;

	bIsHealableMachine = false;

	HeadScale = 1.0f;
	LeftLegScale = 1.0f;
	RightLegScale = 1.0f;
	LeftArmScale = 1.0f;
	RightArmScale = 1.0f;

	StunScale = 1.0f;

	bFirstPerson = true;
	//UpdatePawnMeshes();

	bCanAttackJetpackers = false;

	FlyingOffset = 100.0f;

	OutOfBoundsCounter = -1;

	bNotSpawnedYet = true;

	//mainly for minimap updates
	bAlwaysRelevant = true;

	bTempAlwaysRotate = false;

	BluntDamageReduction = 0.0f;
	PiercingDamageReduction = 0.0f;
	ExplosiveDamageReduction = 0.0f;
	ElementalDamageReduction = 0.0f;
	PoisonDamageReduction = 0.0f;
	DamageReduction = 0.0f;

	SpeedMultiplier = 1.0f;
}

void AOrionCharacter::OnRep_Spawned()
{
	if (!bNotSpawnedYet)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

		if (PC && !PC->DropPod)
		{
			PC->EventBlackFade();
			bShoulderCamera = true;
			PC->EventHUDSpawn(true);
		}
	}
}

void AOrionCharacter::BeginPlay()
{
	Super::BeginPlay();

	//FTimerHandle Handle;
	//GetWorldTimerManager().SetTimer(Handle, this, &AOrionCharacter::RapidFireGrenades, 0.1, true);

	bShoulderCamera = true;
	TimesDowned = 0;

	ThirdPersonAdjust = 1.75f;

	TotalDamageReceived = 0.0f;

	SprintRate = 1.0f;
	AimRate = 1.0f;
	CloakRate = 1.0f;

	if (bIsElite && EliteFX)
	{
		ShowEliteFX(true);
	}

	//spawn our health bar hud element
	//if (GetNetMode() != NM_DedicatedServer)
	//	CreateHealthBar();

	UOrionGameUserSettings *Settings = nullptr;

	if (GEngine && GEngine->GameUserSettings)
		Settings = Cast<UOrionGameUserSettings>(GEngine->GameUserSettings);

	if (Settings)
		bThirdPersonCamera = Settings->ThirdPersonEnabled;

	bBlinking = false;
	bFatality = false;
	bFinishingMove = false;
	bLatchedOnto = false;
	bKnockedDown = false;

	UpdatePawnMeshes();

#if !IS_SERVER
	//check if we need to be thermal visioned
	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	if (ThermalMat)
	{
		for (int32 i = 0; i < Controllers.Num(); i++)
		{
			AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);

			if (C)
			{
				AOrionCharacter *P = Cast<AOrionCharacter>(C->GetPawn());
				if (P && P->CurrentSkill && P->CurrentSkill->IsThermalVisioning())
				{
					//change our material to use thermal
					for (int32 j = 0; j < GetMesh()->GetMaterials().Num(); j++)
					{
						GetMesh()->SetMaterial(j, ThermalMat);
					}

					break;
				}
			}
		}
	}
#endif
}

void AOrionCharacter::OnRep_IsElite()
{
	ShowEliteFX(bIsElite);
}

void AOrionCharacter::ShowEliteFX(bool bShow)
{
	if (EliteFX)
	{
		if (bIsElite)
			EliteFX->ActivateSystem();// > SetHiddenInGame(!bShow);
		else
			EliteFX->DeactivateSystem();
	}
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
	if (Health >= HealthMax)
		Shield = FMath::Max(0.0f, FMath::Min(ShieldMax, Shield + Amount/2));
	else
		Health = FMath::Max(0.0f, FMath::Min(HealthMax, Health + Amount));
}

void AOrionCharacter::AddShield(int32 Amount)
{
	Shield = FMath::Min(ShieldMax, Shield + Amount);
}

void AOrionCharacter::AddExp(int32 Amount)
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
		PC->AddXP(Amount);
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

		//for spectating
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());
		if (PC)
			OutResult.Rotation = PC->GetControlRotation();

		FVector CamStart;
		FVector X, Y, Z;

		X = OutResult.Rotation.Vector();
		Z = FVector(0.0f, 0.0f, 1.0f);
		Y = FVector::CrossProduct(Z, X);
		Y.Normalize();
		Z = FVector::CrossProduct(X, Y);
		Z.Normalize();

		CamStart = GetMesh()->GetBoneLocation(FName("Chr01_Spine02"));

		FVector Offset = CamStart - GetActorLocation();

		if (bBlinking)
		{
			float TeleportTime = FMath::Max(0.0f, 0.2f - (GetWorld()->GetTimeSeconds() - LastTeleportTime));

			CamStart = BlinkPos.Start + (1.0f - (TeleportTime / 0.2f)) * (BlinkPos.End - BlinkPos.Start);
			CamStart += Offset;
		}

		ThirdPersonAdjust = FMath::Lerp(ThirdPersonAdjust, bFatality ? 4.5f : (bAim ? 1.0f : 2.5f), DeltaTime * 6.0f);

		OutResult.Location = CamStart - X*CameraOffset.X * ThirdPersonAdjust + CameraOffset.Y*Y + CameraOffset.Z*Z;

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
		//return;
	}
	else if (GetWorld()->TimeSeconds - LastAimKickTime>t / 3.0)
	{
		OutViewRotation = OutViewRotation + AimKick*(1.0 - ((GetWorld()->TimeSeconds - LastAimKickTime - (t / 3.0)) / (t - (t / 3.0))));
	}
	else
	{
		OutViewRotation = OutViewRotation + AimKick;
	}

	if (OutViewRotation.Pitch > 87.0f && OutViewRotation.Pitch < 180.0f)
		OutViewRotation.Pitch = 87.0f;

	if (OutViewRotation.Pitch < 90.0f)
		OutViewRotation.Pitch += 360.0f;

	OutViewRotation.Pitch = FMath::Clamp(OutViewRotation.Pitch, /*273.0f*/315.0f, 447.0f);

	//OutViewRotation.Pitch -= 180.0f;

	if (OutViewRotation.Pitch > 360.0f)
		OutViewRotation.Pitch -= 360.0f;
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

void AOrionCharacter::PlayHarvesterVoice(EVoiceType Type)
{
	if (Role != ROLE_Authority)
		return;

	if (!bIsHealableMachine)
		return;

	if (!VoiceClass)
		return;

	if (GetWorld()->TimeSeconds - LastVoiceTime < 4.0f)
		return;

	//convert from harv speak to normal speak
	if (Type == VOICE_TAKINGDAMAGE)
		Type = EVoiceType(0);
	else if (Type == VOICE_WOUNDED)
		Type = EVoiceType(1);
	else if (Type == VOICE_REVIVE)
		Type = EVoiceType(2);
	else if (Type == VOICE_REQUESTBACKUP)
		Type = EVoiceType(3);
	else if (Type == VOICE_BOSSKILL)
		Type = EVoiceType(4);

	VoiceRep.Type = Type;
	VoiceRep.bToggle = !VoiceRep.bToggle;

#if !IS_SERVER
	OnRep_VoiceType();
#else
	LastVoiceTime = GetWorld()->TimeSeconds;
#endif
}

void AOrionCharacter::PlayVoice(EVoiceType Type)
{
	if (Role != ROLE_Authority)
		return;

	if (Health <= 0 && !bDowned)
		return;
	
	if(bIsHealableMachine)
		return;

	if (!VoiceClass)
		return;

	if (GetWorld()->TimeSeconds - LastVoiceTime < 4.0f)
		return;

	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	AOrionInventoryManager *Inv = nullptr;
	if (PRI)
		Inv = PRI->InventoryManager;

	VoiceRep.Type = Type;
	VoiceRep.bToggle = !VoiceRep.bToggle;

	if (Inv && Inv->HasStat(RARESTAT_BONESVOICE)) VoiceRep.VoiceType = 1;
	else if (Inv && Inv->HasStat(RARESTAT_GRUMPSVOICE)) VoiceRep.VoiceType = 2;
	else VoiceRep.VoiceType = 0;

#if !IS_SERVER
	OnRep_VoiceType();
#else
	LastVoiceTime = GetWorld()->TimeSeconds;
#endif
}

void AOrionCharacter::OnRep_VoiceType()
{
	if (!VoiceClass)
		return;

	TArray<FVoiceHelper> Voices = VoiceClass.GetDefaultObject()->Voices;

	if (VoiceRep.VoiceType == 1 && BonesVoiceClass)
		Voices = BonesVoiceClass.GetDefaultObject()->Voices;
	else if (VoiceRep.VoiceType == 2 && GrumpsVoiceClass)
		Voices = GrumpsVoiceClass.GetDefaultObject()->Voices;

	/*for (int32 i = 0; i < Voices.Num(); i++)
	{
		for (int32 j = 0; j < Voices[i].Voice.Num(); j++)
		{
			if (VoiceRep.Type == Voices[i].Type)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), Voices[i].Voice[FMath::RandRange(0, Voices[i].Voice.Num() - 1)], GetActorLocation());
				LastVoiceTime = GetWorld()->TimeSeconds;
				return;
			}
		}
	}*/

	if (Voices.Num() > VoiceRep.Type)
	{
		if (Voices[VoiceRep.Type].Voice.Num() > 0)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), Voices[VoiceRep.Type].Voice[FMath::RandRange(0, Voices[VoiceRep.Type].Voice.Num() - 1)], GetActorLocation());
			LastVoiceTime = GetWorld()->TimeSeconds;
			return;
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

void AOrionCharacter::Stunned(float Duration)
{
	bStunned = true; 
	
	GetWorldTimerManager().SetTimer(StunTimer, this, &AOrionCharacter::EndStun, Duration, false);

	PlayStunEffect(true);
}

void AOrionCharacter::EndStun()
{
	bStunned = false;

	PlayStunEffect(false);
}

void AOrionCharacter::PlayStunEffect(bool bOn)
{
	if (bOn)
	{
		StunnedPSC = UGameplayStatics::SpawnEmitterAttached(StunnedFX, GetMesh(), "Stunned");

		if (StunnedPSC)
			StunnedPSC->SetWorldScale3D(FVector(StunScale));
	}
	else if (StunnedPSC)
	{
		StunnedPSC->DeactivateSystem();
		StunnedPSC = nullptr;
	}
}

void AOrionCharacter::OnRep_Stun()
{
	PlayStunEffect(bStunned);
}

void AOrionCharacter::SpawnClassWeapons(int32 ClassIndex)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.Owner = this;

	AOrionWeapon* NewWeapon = nullptr;
	TArray<TSubclassOf<class AOrionWeapon> > WeaponClasses;

	WeaponClasses = EventGetWeaponClasses(ClassIndex);

	/*switch(ClassIndex)
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
	case 3:
		WeaponClasses = DefaultTechInventoryClasses;
		break;
	}*/

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

void AOrionCharacter::EquipAdrenaline()
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i]->InstantConfig.WeaponSlot == 4)
		{
			EquipWeapon(Inventory[i]);
			return;
		}
	}
}

void AOrionCharacter::EquipWeapon(AOrionWeapon* Weapon)
{
	//keep us on adrenaline weapon when activated
	if (CurrentSkill && CurrentSkill->IsFlaming() && Weapon && Weapon->InstantConfig.WeaponSlot != 4)
		return;

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
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	if (PC && IsLocallyControlled() && NewWeapon)
	{
		PC->EventSelectWeapon(NewWeapon->InstantConfig.WeaponSlot, NewWeapon->GetWeaponName());
	}

	if (Cast<AOrionWeaponLink>(NewWeapon) && bAim)
		bAim = false;

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
	if (NextWeapon && NextWeapon->IsValidLowLevel())
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
		if (Weapon && Weapon->IsValidLowLevel())
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
	{
		DetachFromShip();
	}
}

//replicate the buffs to clients for effects
void AOrionCharacter::OnRep_Buffs()
{
	UpdateBuffFX();
}

void AOrionCharacter::UpdateBuffFX()
{
	if (BuffFX)
	{
		bool bFound = false;
		//for (int32 i = 0; i < Buffs.Num(); i++)
		//{
		float Index = Buffs.Num() - 1;
		if (Index >= 0)
		{
			if (Buffs[Index] && Buffs[Index]->IsValidLowLevel() && Buffs[Index]->Effect)
			{
				BuffFX->AttachTo(GetMesh(), "Aura");
				BuffFX->SetTemplate(Buffs[Index]->Effect);
				BuffFX->ActivateSystem();
				//BuffFX->SetWorldScale3D(FVector(0.55f));

				bFound = true;
				//break;
			}
		}
		//}

		if (!bFound)
		{
			BuffFX->DeactivateSystem();
		}
	}
}

void AOrionCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// everyone
	DOREPLIFETIME(AOrionCharacter, HealthMax);
	DOREPLIFETIME(AOrionCharacter, Health);
	DOREPLIFETIME(AOrionCharacter, ShieldMax);
	DOREPLIFETIME(AOrionCharacter, Shield);
	DOREPLIFETIME(AOrionCharacter, bIsElite);
	DOREPLIFETIME(AOrionCharacter, Buffs);
	DOREPLIFETIME(AOrionCharacter, bFemale);
	DOREPLIFETIME(AOrionCharacter, bLatchedOnto);
	DOREPLIFETIME(AOrionCharacter, Latcher);
	DOREPLIFETIME(AOrionCharacter, bKnockedDown);
	DOREPLIFETIME(AOrionCharacter, Knocker);
	DOREPLIFETIME(AOrionCharacter, ReviveTarget);
	DOREPLIFETIME(AOrionCharacter, bThirdPersonCamera);
	DOREPLIFETIME(AOrionCharacter, bStunned);
	DOREPLIFETIME_CONDITION(AOrionCharacter, FatalityVictim, COND_SkipOwner);
	DOREPLIFETIME(AOrionCharacter, bDoubleShield);
	DOREPLIFETIME(AOrionCharacter, SpeedMultiplier);

	DOREPLIFETIME(AOrionCharacter, bDowned);
	DOREPLIFETIME(AOrionCharacter, DownedTime);
	DOREPLIFETIME(AOrionCharacter, VoiceRep);
	DOREPLIFETIME(AOrionCharacter, VoiceClass);

	//speed 
	DOREPLIFETIME(AOrionCharacter, SprintRate);
	DOREPLIFETIME(AOrionCharacter, AimRate);
	DOREPLIFETIME(AOrionCharacter, CloakRate);

	DOREPLIFETIME_CONDITION(AOrionCharacter, bNotSpawnedYet, COND_OwnerOnly);

	//out of bounds
	DOREPLIFETIME(AOrionCharacter, OutOfBoundsCounter);

	//healing material stuff
	DOREPLIFETIME(AOrionCharacter, HealTarget);
	DOREPLIFETIME(AOrionCharacter, bSelfHealing);

	DOREPLIFETIME(AOrionCharacter, bPowered);

	DOREPLIFETIME(AOrionCharacter, GibRep);

	DOREPLIFETIME(AOrionCharacter, HelmetArmor);
	DOREPLIFETIME(AOrionCharacter, BodyArmor);
	DOREPLIFETIME(AOrionCharacter, ArmsArmor);
	DOREPLIFETIME(AOrionCharacter, LegsArmor);

	DOREPLIFETIME(AOrionCharacter, CurrentShip);

	DOREPLIFETIME_CONDITION(AOrionCharacter, AimPos, COND_SkipOwner);
	DOREPLIFETIME(AOrionCharacter, bRolling);
	DOREPLIFETIME(AOrionCharacter, BlinkPos);
	DOREPLIFETIME(AOrionCharacter, Level);

	DOREPLIFETIME(AOrionCharacter, bFatality);
	DOREPLIFETIME(AOrionCharacter, bFatalityRemove);
	DOREPLIFETIME(AOrionCharacter, FatalityAnim);
	DOREPLIFETIME(AOrionCharacter, bFinishingMove);
	DOREPLIFETIME(AOrionCharacter, GibCenter);

	DOREPLIFETIME_CONDITION(AOrionCharacter, CameraShip, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(AOrionCharacter, LastTakeHitInfo, COND_Custom);

	//DOREPLIFETIME(AOrionCharacter, CurrentWeapon);
	DOREPLIFETIME_CONDITION(AOrionCharacter, NextWeapon, COND_SkipOwner);

	DOREPLIFETIME(AOrionCharacter, Inventory);

	DOREPLIFETIME(AOrionCharacter, ReplicatedAnimation);

	DOREPLIFETIME_CONDITION(AOrionCharacter, TargetYaw, COND_SkipOwner);

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
	DOREPLIFETIME(AOrionCharacter, CurrentSecondarySkill);

	DOREPLIFETIME_CONDITION(AOrionCharacter, bShoulderCamera, COND_SkipOwner);
	//DOREPLIFETIME(AOrionCharacter, bShipCamera);

	//owner only rpg stuff
	DOREPLIFETIME_CONDITION(AOrionCharacter, MeleeDamageBoost, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, GrenadeRechargeRate, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, SkillRechargeRate, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, CriticalHitChance, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, CriticalHitMultiplier, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, DamageReduction, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, BluntDamageReduction, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, PiercingDamageReduction, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, ExplosiveDamageReduction, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, ElementalDamageReduction, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, PoisonDamageReduction, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, XPPerKill, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, FireDamage, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, LightningDamage, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, IceDamage, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, PoisonDamage, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, CorrosiveDamage, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, GoldFind, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, MagicFind, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, LargeDinoBoost, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AOrionCharacter, RobotBoost, COND_OwnerOnly);
}

void AOrionCharacter::Destroyed()
{
	////DestroyInventory();

	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
	if (GRI)
		GRI->RemoveRagdoll(this);

	if (MyHealthBar)
	{
		MyHealthBar->ConditionalBeginDestroy();
		MyHealthBar->RemoveFromParent();
		MyHealthBar = nullptr;
	}

	//remove ability and weapons
	if (CurrentSkill)
	{
		CurrentSkill->Destroy();
		CurrentSkill = nullptr;
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	Super::Destroyed();
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

	float length = 1.1f;

	EnableCustomDepth();
	
	//if (IsLocallyControlled() && Role < ROLE_Authority)
	//	ServerPlayAnimMontage(Info, 1.0f, TEXT(""), true, true, true);
	
	//if (IsLocallyControlled())
	if (Role == ROLE_Authority)
		length = OrionPlayAnimMontage(Info, 1.0f, TEXT(""), true, true, true);

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
	{
		CharacterMats.Empty();
		CharacterCloakMats.Empty();

		if (GetMesh()) GetMesh()->OverrideMaterials.Empty();

		int32 NumMaterials = 0;
		if (GetMesh() && GetMesh()->SkeletalMesh)NumMaterials += GetMesh()->SkeletalMesh->Materials.Num();

		CharacterMats.SetNumUninitialized(NumMaterials);

		int32 Counter = 0;

		if (GetMesh() && GetMesh()->SkeletalMesh)
		{
			for (int32 i = 0; i < GetMesh()->SkeletalMesh->Materials.Num(); i++)
			{
				CharacterMats[Counter] = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(i), this);
				if (CharacterMats[Counter])
					GetMesh()->SetMaterial(i, CharacterMats[Counter]);

				Counter++;
			}
		}

		return;
	}

	//setup some materials
	CharacterMats.Empty();
	CharacterCloakMats.Empty();

	//CharacterMats.SetNumUninitialized(7);
	//CharacterCloakMats.SetNumUninitialized(7);

	//reset materials to default if needed
	if (GetMesh()) GetMesh()->OverrideMaterials.Empty();
	if (HelmetMesh) HelmetMesh->OverrideMaterials.Empty();
	if (BodyMesh) BodyMesh->OverrideMaterials.Empty();
	if (LegsMesh) LegsMesh->OverrideMaterials.Empty();
	if (ArmsMesh) ArmsMesh->OverrideMaterials.Empty();
	if (Flight1Mesh) Flight1Mesh->OverrideMaterials.Empty();
	if (Flight2Mesh) Flight2Mesh->OverrideMaterials.Empty();

	int32 NumMaterials = 0;
	if (GetMesh() && GetMesh()->SkeletalMesh)NumMaterials += GetMesh()->SkeletalMesh->Materials.Num();
	if (HelmetMesh && HelmetMesh->SkeletalMesh)NumMaterials += HelmetMesh->SkeletalMesh->Materials.Num();
	if (BodyMesh && BodyMesh->SkeletalMesh)NumMaterials += BodyMesh->SkeletalMesh->Materials.Num();
	if (LegsMesh && LegsMesh->SkeletalMesh)NumMaterials += LegsMesh->SkeletalMesh->Materials.Num();
	if (ArmsMesh && ArmsMesh->SkeletalMesh)NumMaterials += ArmsMesh->SkeletalMesh->Materials.Num();
	if (Flight1Mesh && Flight1Mesh->SkeletalMesh)NumMaterials += Flight1Mesh->SkeletalMesh->Materials.Num();
	if (Flight2Mesh && Flight2Mesh->SkeletalMesh)NumMaterials += Flight2Mesh->SkeletalMesh->Materials.Num();

	CharacterMats.SetNumUninitialized(NumMaterials);
	CharacterCloakMats.SetNumUninitialized(NumMaterials);

	/*for (int32 i = 0; i < 7; i++)
	{
		CharacterMats[i].Mats.Empty();
		CharacterCloakMats[i].Mats.Empty();
	}*/

	int32 Counter = 0;

	if (GetMesh() && GetMesh()->SkeletalMesh)
	{
		for (int32 i = 0; i < GetMesh()->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterMats[Counter] = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(i), this);
			if (CharacterMats[Counter])
				GetMesh()->SetMaterial(i, CharacterMats[Counter]);

			Counter++;
		}
	}
	if (HelmetMesh && HelmetMesh->SkeletalMesh)
	{
		for (int32 i = 0; i < HelmetMesh->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterMats[Counter] = UMaterialInstanceDynamic::Create(HelmetMesh->GetMaterial(i), this);
			if (CharacterMats[Counter])
				HelmetMesh->SetMaterial(i, CharacterMats[Counter]);

			Counter++;
		}
	}
	if (BodyMesh && BodyMesh->SkeletalMesh)
	{
		for (int32 i = 0; i < BodyMesh->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterMats[Counter] = UMaterialInstanceDynamic::Create(BodyMesh->GetMaterial(i), this);
			if (CharacterMats[Counter])
				BodyMesh->SetMaterial(i, CharacterMats[Counter]);

			Counter++;
		}
	}
	if (LegsMesh && LegsMesh->SkeletalMesh)
	{
		for (int32 i = 0; i < LegsMesh->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterMats[Counter] = UMaterialInstanceDynamic::Create(LegsMesh->GetMaterial(i), this);
			if (CharacterMats[Counter])
				LegsMesh->SetMaterial(i, CharacterMats[Counter]);

			Counter++;
		}
	}
	if (ArmsMesh && ArmsMesh->SkeletalMesh)
	{
		for (int32 i = 0; i < ArmsMesh->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterMats[Counter] = UMaterialInstanceDynamic::Create(ArmsMesh->GetMaterial(i), this);
			if (CharacterMats[Counter])
				ArmsMesh->SetMaterial(i, CharacterMats[Counter]);

			Counter++;
		}
	}
	if (Flight1Mesh && Flight1Mesh->SkeletalMesh)
	{
		for (int32 i = 0; i < Flight1Mesh->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterMats[Counter] = UMaterialInstanceDynamic::Create(Flight1Mesh->GetMaterial(i), this);
			if (CharacterMats[Counter])
				Flight1Mesh->SetMaterial(i, CharacterMats[Counter]);

			Counter++;
		}
	}
	if (Flight2Mesh && Flight2Mesh->SkeletalMesh)
	{
		for (int32 i = 0; i < Flight2Mesh->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterMats[Counter] = UMaterialInstanceDynamic::Create(Flight2Mesh->GetMaterial(i), this);
			if (CharacterMats[Counter])
				Flight2Mesh->SetMaterial(i, CharacterMats[Counter]);

			Counter++;
		}
	}
	/*CharacterMats[1] = UMaterialInstanceDynamic::Create(HelmetMesh->GetMaterial(0), this); if (CharacterMats[1]) HelmetMesh->SetMaterial(0, CharacterMats[1]);
	CharacterMats[2] = UMaterialInstanceDynamic::Create(BodyMesh->GetMaterial(0), this); if (CharacterMats[2]) BodyMesh->SetMaterial(0, CharacterMats[2]);
	CharacterMats[3] = UMaterialInstanceDynamic::Create(LegsMesh->GetMaterial(0), this); if (CharacterMats[3]) LegsMesh->SetMaterial(0, CharacterMats[3]);
	CharacterMats[4] = UMaterialInstanceDynamic::Create(ArmsMesh->GetMaterial(0), this); if (CharacterMats[4]) ArmsMesh->SetMaterial(0, CharacterMats[4]);
	CharacterMats[5] = UMaterialInstanceDynamic::Create(Flight1Mesh->GetMaterial(0), this); if (CharacterMats[5]) Flight1Mesh->SetMaterial(0, CharacterMats[5]);
	CharacterMats[6] = UMaterialInstanceDynamic::Create(Flight2Mesh->GetMaterial(0), this); if (CharacterMats[6]) Flight2Mesh->SetMaterial(0, CharacterMats[6]);*/

	//some greasy sorting for going cloaked with armor on
	if (GetMesh()) GetMesh()->SetTranslucentSortPriority(1);
	if (HelmetMesh) HelmetMesh->SetTranslucentSortPriority(2);
	if (BodyMesh) BodyMesh->SetTranslucentSortPriority(2);
	if (LegsMesh) LegsMesh->SetTranslucentSortPriority(2);
	if (ArmsMesh) ArmsMesh->SetTranslucentSortPriority(2);
	if (Flight1Mesh) Flight1Mesh->SetTranslucentSortPriority(2);
	if (Flight2Mesh) Flight2Mesh->SetTranslucentSortPriority(2);

	Counter = 0;

	if (GetMesh() && GetMesh()->SkeletalMesh)
	{
		for (int32 i = 0; i < GetMesh()->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterCloakMats[Counter] = UMaterialInstanceDynamic::Create(CloakParent, this); if (GetMesh() && GetMesh()->SkeletalMesh) CharacterCloakMats[Counter]->CopyParameterOverrides(Cast<UMaterialInstance>(GetMesh()->SkeletalMesh->Materials[i].MaterialInterface)); Counter++;
		}
	}
	if (HelmetMesh && HelmetMesh->SkeletalMesh)
	{
		for (int32 i = 0; i < HelmetMesh->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterCloakMats[Counter] = UMaterialInstanceDynamic::Create(CloakParent, this); if (HelmetMesh && HelmetMesh->SkeletalMesh) CharacterCloakMats[Counter]->CopyParameterOverrides(Cast<UMaterialInstance>(HelmetMesh->SkeletalMesh->Materials[i].MaterialInterface)); Counter++;
		}
	}
	if (BodyMesh && BodyMesh->SkeletalMesh)
	{
		for (int32 i = 0; i < BodyMesh->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterCloakMats[Counter] = UMaterialInstanceDynamic::Create(CloakParent, this); if (BodyMesh && BodyMesh->SkeletalMesh) CharacterCloakMats[Counter]->CopyParameterOverrides(Cast<UMaterialInstance>(BodyMesh->SkeletalMesh->Materials[i].MaterialInterface)); Counter++;
		}
	}
	if (LegsMesh && LegsMesh->SkeletalMesh)
	{
		for (int32 i = 0; i < LegsMesh->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterCloakMats[Counter] = UMaterialInstanceDynamic::Create(CloakParent, this); if (LegsMesh && LegsMesh->SkeletalMesh) CharacterCloakMats[Counter]->CopyParameterOverrides(Cast<UMaterialInstance>(LegsMesh->SkeletalMesh->Materials[i].MaterialInterface)); Counter++;
		}
	}
	if (ArmsMesh && ArmsMesh->SkeletalMesh)
	{
		for (int32 i = 0; i < ArmsMesh->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterCloakMats[Counter] = UMaterialInstanceDynamic::Create(CloakParent, this); if (ArmsMesh && ArmsMesh->SkeletalMesh) CharacterCloakMats[Counter]->CopyParameterOverrides(Cast<UMaterialInstance>(ArmsMesh->SkeletalMesh->Materials[i].MaterialInterface)); Counter++;
		}
	}
	if (Flight1Mesh && Flight1Mesh->SkeletalMesh)
	{
		for (int32 i = 0; i < Flight1Mesh->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterCloakMats[Counter] = UMaterialInstanceDynamic::Create(CloakParent, this); if (Flight1Mesh && Flight1Mesh->SkeletalMesh) CharacterCloakMats[Counter]->CopyParameterOverrides(Cast<UMaterialInstance>(Flight1Mesh->SkeletalMesh->Materials[i].MaterialInterface)); Counter++;
		}
	}
	if (Flight2Mesh && Flight2Mesh->SkeletalMesh)
	{
		for (int32 i = 0; i < Flight2Mesh->SkeletalMesh->Materials.Num(); i++)
		{
			CharacterCloakMats[Counter] = UMaterialInstanceDynamic::Create(CloakParent, this); if (Flight2Mesh && Flight2Mesh->SkeletalMesh) CharacterCloakMats[Counter]->CopyParameterOverrides(Cast<UMaterialInstance>(Flight2Mesh->SkeletalMesh->Materials[i].MaterialInterface)); Counter++;
		}
	}


	/*CharacterCloakMats[0] = UMaterialInstanceDynamic::Create(CloakParent, this); if(GetMesh() && GetMesh()->SkeletalMesh) CharacterCloakMats[0]->CopyParameterOverrides(Cast<UMaterialInstance>(GetMesh()->SkeletalMesh->Materials[0].MaterialInterface));
	CharacterCloakMats[1] = UMaterialInstanceDynamic::Create(CloakParent, this); if (HelmetMesh && HelmetMesh->SkeletalMesh) CharacterCloakMats[1]->CopyParameterOverrides(Cast<UMaterialInstance>(HelmetMesh->SkeletalMesh->Materials[0].MaterialInterface));
	CharacterCloakMats[2] = UMaterialInstanceDynamic::Create(CloakParent, this); if (BodyMesh && BodyMesh->SkeletalMesh) CharacterCloakMats[2]->CopyParameterOverrides(Cast<UMaterialInstance>(BodyMesh->SkeletalMesh->Materials[0].MaterialInterface));
	CharacterCloakMats[3] = UMaterialInstanceDynamic::Create(CloakParent, this); if (LegsMesh && LegsMesh->SkeletalMesh) CharacterCloakMats[3]->CopyParameterOverrides(Cast<UMaterialInstance>(LegsMesh->SkeletalMesh->Materials[0].MaterialInterface));
	CharacterCloakMats[4] = UMaterialInstanceDynamic::Create(CloakParent, this); if (ArmsMesh && ArmsMesh->SkeletalMesh) CharacterCloakMats[4]->CopyParameterOverrides(Cast<UMaterialInstance>(ArmsMesh->SkeletalMesh->Materials[0].MaterialInterface));
	CharacterCloakMats[5] = UMaterialInstanceDynamic::Create(CloakParent, this); if (Flight1Mesh && Flight1Mesh->SkeletalMesh) CharacterCloakMats[5]->CopyParameterOverrides(Cast<UMaterialInstance>(Flight1Mesh->SkeletalMesh->Materials[0].MaterialInterface));
	CharacterCloakMats[6] = UMaterialInstanceDynamic::Create(CloakParent, this); if (Flight2Mesh && Flight2Mesh->SkeletalMesh) CharacterCloakMats[6]->CopyParameterOverrides(Cast<UMaterialInstance>(Flight2Mesh->SkeletalMesh->Materials[0].MaterialInterface));*/
}

void AOrionCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/*if (Role == ROLE_Authority)
	{
		bShoulderCamera = true;// FMath::RandRange(0, 1) == 1;
		bShipCamera = !bShoulderCamera;
	}*/

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

	UpdatePlayerRingColor();
}

void AOrionCharacter::OnRep_GibAll()
{
	GibAll(GibCenter, nullptr);
}

void AOrionCharacter::GibAll(FVector Center, AOrionPlayerController *PC)
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

			if (PC && PC->GetStats())
				PC->GetStats()->AddStatValue(Gib->DoBloodSpurt ? STAT_HEADSBLOWNOFF : STAT_LIMBSBLOWNOFF, 1);
		}
	}

	if (Role == ROLE_Authority)
		GibCenter = Center;
}

void AOrionCharacter::HandleGibs(float damage, FDamageEvent const& DamageEvent, AOrionPlayerController *Damager)
{
	if (!Damager || !Damager->IsValidLowLevel())
		return;

#if !IS_SERVER
	UOrionGameUserSettings *Settings = nullptr;

	if (GEngine && GEngine->GameUserSettings)
		Settings = Cast<UOrionGameUserSettings>(GEngine->GameUserSettings);

	if (Settings->GoreEnabled == false)
		return;
#endif

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

						if (Damager && Damager->GetStats())
							Damager->GetStats()->AddStatValue(Gib->DoBloodSpurt ? STAT_HEADSBLOWNOFF : STAT_LIMBSBLOWNOFF, 1);

						GibRep.Index = i;
						GibRep.Socket = Gib->SocketName;
						GibRep.Velocity = vel;

						AOrionCharacter *KillerPawn = Cast<AOrionCharacter>(Damager->GetPawn());
						if (KillerPawn)
							KillerPawn->PlayVoice(VOICE_BOSSKILL);

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

void AOrionCharacter::CutOffLimb(EGibType Limb)
{
	if (Gibs.Num() <= Limb)
		return;

	AOrionGib *Gib = Cast<AOrionGib>(Gibs[Limb].Gib.GetDefaultObject());

	if (Gib && Gib->Mesh)
	{
		FVector pos;
		FRotator rot;

		FVector vel = FVector(0.0f, 0.0f, 50.0f);

		GetMesh()->GetSocketWorldLocationAndRotation(Gib->SocketName, pos, rot);
		SpawnGibs(Limb, pos, rot, vel);

		//GibRep.Index = Limb;
		//GibRep.Socket = Gib->SocketName;
		//GibRep.Velocity = vel;

		//SpawnGibs(Limb, pos, rot, vel);
	}
}

void AOrionCharacter::OnRep_Gibs()
{
#if !IS_SERVER
	UOrionGameUserSettings *Settings = nullptr;

	if (GEngine && GEngine->GameUserSettings)
		Settings = Cast<UOrionGameUserSettings>(GEngine->GameUserSettings);

	if (Settings->GoreEnabled == false)
		return;
#endif

	FVector pos;
	FRotator rot;

	GetMesh()->GetSocketWorldLocationAndRotation(GibRep.Socket, pos, rot);

	SpawnGibs(GibRep.Index, pos, rot, GibRep.Velocity);
}

void AOrionCharacter::SpawnGibs(int32 index, FVector pos, FRotator rot, FVector dir)
{
	if (index >= Gibs.Num() || !Gibs[index].Gib)
		return;

	//spawn the actual gib into the world
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.Owner = this;

	AOrionGib *NewGib = GetWorld()->SpawnActor<AOrionGib>(Gibs[index].Gib, pos + FVector(0.0f, 0.0f, 20.0f), rot, SpawnInfo);

	if (NewGib)
	{
		if (NewGib->Mesh == nullptr)
			return;

		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GetGameState());

		if (GRI && Role == ROLE_Authority)
			GRI->TotalLimbsBlownOff++;

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

void AOrionCharacter::SetSecondaryAbility(AOrionAbility *NewAbility)
{
	if (CurrentSecondarySkill)
		CurrentSecondarySkill->Destroy();

	CurrentSecondarySkill = NewAbility;
}

void AOrionCharacter::SetTeamCloaking()
{
	if (CurrentSkill && CurrentSkill->IsCloaking())
		return;

	if (CurrentSecondarySkill)
	{
		if (!CurrentSecondarySkill->IsSkillActive())
		{
			if (CurrentSecondarySkill->ActivateSkill())
			{

			}
		}
		else
		{
			CurrentSecondarySkill->DeactivateSkill();
		}
	}
}

bool AOrionCharacter::IsOvercharging() const
{
	if (CurrentSkill && CurrentSkill->IsOvercharging())
		return true;

	return false;
}

void AOrionCharacter::Explode(AOrionPlayerController *EventInstigator)
{
	if (EventInstigator && EventInstigator->FragGrenadeClass)
	{
		//UGameplayStatics::ApplyRadialDamage(GetWorld(), 500.0f * Level, GetActorLocation(), 750.0f, ExplosionDamageType, TArray<AActor*>(), EventInstigator->GetPawn(), EventInstigator, false);

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.Owner = EventInstigator->GetPawn();// EventInstigator;

		AOrionGrenade *Grenade = GetWorld()->SpawnActor<AOrionGrenade>(EventInstigator->FragGrenadeClass, GetActorLocation(), FRotator::ZeroRotator, SpawnInfo);
		if (Grenade)
		{
			Grenade->GrenadeLife = 0.5f;
			Grenade->GrenadeScale = Grenade->ExplosionScale;
			Grenade->bIsMiniGrenade = true;

			Grenade->Init(FVector(0.0f));
			Grenade->SetFuseTime(0.5f);
		}
	}
}

bool AOrionCharacter::IsOnFire()
{
	for (auto Itr(Buffs.CreateIterator()); Itr; ++Itr)
	{
		AOrionBuff *Buff = *Itr;

		if (!Itr)
			continue;

		if (Buff->ElementalDamageType == ELEMENTAL_FIRE)
			return true;
	}

	return false;
}

float AOrionCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	UOrionDamageType *DamageType = Cast<UOrionDamageType>(DamageEvent.DamageTypeClass.GetDefaultObject());

	//this is needed for damage that has no controller or self inflicted damage so it doesn't get applied multiple times
	bool bSelfDamage = (EventInstigator == Controller);

	if (!DamageType)
		return 0.0f;

	//take no damage while blinking
	if (bBlinking)
		return 0.0f;

	//take no damage if waiting for revive
	if (bDowned)
		return 0.0f;

	if (GetWorld()->TimeSeconds - LastRevivedTime < 2.0f)
		return 0.0f;

	for (auto Itr(Buffs.CreateIterator()); Itr; ++Itr)
	{
		AOrionBuff *Buff = *Itr;

		if (!Itr)
			continue;

		if (Buff->DamageReduction > 0.0f)
			Damage *= (1.0f - Buff->DamageReduction);
	}

	//if damagecauser is a weapon, get the weapon's instigator instead
	AOrionWeapon *WeaponDamageCauser = Cast<AOrionWeapon>(DamageCauser);

	if (WeaponDamageCauser)
	{
		//apply gear and skill based damage adjustments
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(EventInstigator);
		if (PC)
		{
			if (WeaponDamageCauser->InstantConfig.WeaponSlot == 2)
				Damage *= 1.0f + float(PC->GetSkillValue(SKILL_SECONDARYDAMAGE)) / 100.0f;
			else if (WeaponDamageCauser->InstantConfig.WeaponSlot == 1)
				Damage *= 1.0f + float(PC->GetSkillValue(SKILL_PRIMARYDAMAGE)) / 100.0f;
		}

		DamageCauser = WeaponDamageCauser->Instigator;
	}

	AOrionGrenade *GrenadeDamageCauser = Cast<AOrionGrenade>(DamageCauser);

	if (GrenadeDamageCauser)
	{
		DamageCauser = GrenadeDamageCauser->GetOwner();
	}

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	if (PC)
	{
		if (GetWorld()->TimeSeconds - LastHealTime < 1.0f)
		{
			Damage *= 1.0f - float(PC->GetSkillValue(SKILL_HEALINGDR)) / 100.0f;
		}

		Damage *= 1.0f - float(PC->GetSkillValue(SKILL_DR)) / 100.0f;

		Damage *= 1.0f - float(PC->GetSkillValue(SKILL_THERMALDR)) / 100.0f;

		if (CurrentSkill && CurrentSkill->IsCloaking() && PC->GetSkillValue(SKILL_CLOAKTEAMMATES) > 0)
		{
			Damage *= 0.5f;
		}

		if (PC->GetAchievements() && (bLatchedOnto || DamageType->WeaponName.ToUpper() == TEXT("JECKYL")))
			PC->GetAchievements()->UnlockAchievement(ACH_JECKYLFATALITY, PC);

		if (Cast<AOrionDroidPawn>(DamageCauser))
			Damage *= 1.0f - float(PC->GetSkillValue(SKILL_ROBOTDAMAGEREDUCTION)) / 100.0f;
		if (CurrentSkill && CurrentSkill->IsJetpacking())
			Damage *= 1.0f - float(PC->GetSkillValue(SKILL_JETPACKDAMAGEREDUCTION)) / 100.0f;

		if (PC->HasOrbEffect(ORB_STRENGTH))
			Damage *= 0.5f;
	}

	AOrionPlayerController *AttackerPC = Cast<AOrionPlayerController>(EventInstigator);

	bool bCrit = false;
	bool bKnockBack = true;

	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

	AOrionCharacter *AttackerPawn = nullptr;
	if (EventInstigator)
		AttackerPawn = Cast<AOrionCharacter>(EventInstigator->GetPawn());

	//apply any gear related stats to this damage
	if (PC)
	{
		//dino damage is based on dino ilvl and expected health/shield of a player of equal level
		int32 EnemyLevel = 1;

		/*if (EventInstigator)
		{
			AOrionCharacter *EnemyPawn = Cast<AOrionCharacter>(EventInstigator->GetPawn());

			if (EnemyPawn)
			{
				EnemyLevel = EnemyPawn->Level;
			}
		}*/

		//int32 iLevel = 1;
		//AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
		//if (GRI)
		//	iLevel = GRI->ItemLevel;
		if (Game)
			EnemyLevel = Game->GetEnemyItemLevel(true);

		Damage = (/*Damage + */FMath::Pow(LEVELPOWER, EnemyLevel / LEVELINTERVAL) * Damage) / 4.0f;//((1000.0f + EnemyLevel * 12.0f) / 400.0f) * Damage * (1.0f + (EnemyLevel / 200.0f));

		//do we have the dodge bullet legendary?
		AOrionInventoryManager *Inv = PC->GetInventoryManager();
		if (Inv)
		{
			if (Inv->HasStat(RARESTAT_INVULNFAT) && bFinishingMove) return 0.0f;
			if (Inv->HasStat(RARESTAT_JETPACKIMMUNE) && CurrentSkill && CurrentSkill->IsJetpacking()) return 0.0f;
			if (Inv->HasStat(RARESTAT_CLOAKIMMUNE) && CurrentSkill && CurrentSkill->IsCloaking()) return 0.0f;
			if (Inv->HasStat(RARESTAT_PYROINVULNERABLE) && CurrentSkill && CurrentSkill->IsFlaming()) Damage /= 2.0f;
			if (Inv->HasStat(RARESTAT_MARKDR) && CurrentSkill && CurrentSkill->IsThermalVisioning()) Damage /= 2.0f;


			if (Inv->HasStat(RARESTAT_BULLETDODGER) && DamageType->bIsBullet && FMath::FRand() <= 0.5f) return 0.0f;

			//legendary bonuses
			if (AttackerPawn)
			{
				if (Inv->HasStat(RARESTAT_LARGEDINODR) && AttackerPawn->bIsBigDino) Damage *= 0.75f;
				if (Inv->HasStat(RARESTAT_ROBOTDR) && Cast<AOrionDroidPawn>(AttackerPawn)) Damage *= 0.75f;
			}

			if (Inv->HasStat(RARESTAT_KNOCKBACKIMMUNE))
			{
				bKnockBack = false;
				if (DamageType->bKnockBack) Damage *= 0.5f;
			}
		}
	}

	if (!bSelfDamage && AttackerPC && AttackerPawn && AttackerPawn->GetWeapon())
	{
		if (AttackerPawn->GetWeapon()->InstantConfig.WeaponName == "SNIPER RIFLE")
		{
			Damage *= 1.0f + AttackerPC->GetSkillValue(SKILL_THERMALDAMAGE) / 100.0f;
		}

		if (AttackerPawn->GetWeapon()->InstantConfig.WeaponName.ToUpper() == "FLAMETHROWER" && AttackerPC->GetSkillValue(SKILL_FLAMERDAMAGE))// && DamageType->WeaponName == "Flamethrower")
			Damage *= 1.0f + (AttackerPC->GetSkillValue(SKILL_FLAMERDAMAGE) / 100.0f);
	}

	if (Game && AttackerPawn && AttackerPawn->bIsHealableMachine)
		Damage *= 0.1f * FMath::Pow(LEVELPOWER, FMath::Max(1, Game->GetEnemyItemLevel(true) - LEVELSYNC) / LEVELINTERVAL);

	if (!bSelfDamage && AttackerPC)
	{
		//AOrionCharacter *AttackerPawn = Cast<AOrionCharacter>(AttackerPC->GetPawn());

		Damage *= 1.0f + float(AttackerPC->GetSkillValue(SKILL_DAMAGEBOOST)) / 100.0f;

		if (AttackerPawn)
		{
			Damage *= 1.0f + AttackerPawn->DamageBonus;

			int32 tIndex = 0;

			AOrionPRI *aPRI = Cast<AOrionPRI>(AttackerPC->PlayerState);
			if (aPRI)
				tIndex = aPRI->GetTeamIndex();

			AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

			//adjust weapon damage
			if (AttackerPC->GetInventoryManager())
			{
				if (DamageType->WeaponSlot == 1)// && DamageType->bIsKnife)
					Damage = Damage * 0.1f * AttackerPC->GetInventoryManager()->GetPrimaryWeaponDamage();  //Damage * AttackerPC->GetInventoryManager()->GetPrimaryWeaponDamage() * 0.05f;
				else if (DamageType->WeaponSlot == 2)
					Damage = Damage * 0.1f * AttackerPC->GetInventoryManager()->GetSecondaryWeaponDamage();  //Damage * AttackerPC->GetInventoryManager()->GetSecondaryWeaponDamage() * 0.05f;
				else //for grenades and regen gun and abilities and etc:p
					Damage = Damage * 0.1f * 
					(AttackerPC->GetInventoryManager()->GetPrimaryWeaponDamage() +
					AttackerPC->GetInventoryManager()->GetSecondaryWeaponDamage())
									/ 2.0f;
			}

			if (GRI && !GRI->OnSameTeam(aPRI, PRI) && (DamageType->WeaponSlot == 1 || DamageType->WeaponSlot == 2))
			{
				if (AttackerPawn->FireDamage > 0.0f) { /*Damage += AttackerPC->GetInventoryManager()->GetLevelScaledValue(AttackerPawn->FireDamage, AttackerPC->ILevel);*/ AddBuff(FireBuff, AttackerPC, tIndex); }
				if (AttackerPawn->IceDamage > 0.0f) { /*Damage += AttackerPC->GetInventoryManager()->GetLevelScaledValue(AttackerPawn->IceDamage, AttackerPC->ILevel);*/ AddBuff(IceBuff, AttackerPC, tIndex); }
				if (AttackerPawn->LightningDamage > 0.0f) { /*Damage += AttackerPC->GetInventoryManager()->GetLevelScaledValue(AttackerPawn->LightningDamage, AttackerPC->ILevel);*/ AddBuff(LightningBuff, AttackerPC, tIndex); }
				if (AttackerPawn->PoisonDamage > 0.0f) { /*Damage += AttackerPC->GetInventoryManager()->GetLevelScaledValue(AttackerPawn->PoisonDamage, AttackerPC->ILevel);*/ AddBuff(PoisonBuff, AttackerPC, tIndex); }
				if (AttackerPawn->CorrosiveDamage > 0.0f) { /*Damage += AttackerPC->GetInventoryManager()->GetLevelScaledValue(AttackerPawn->CorrosiveDamage, AttackerPC->ILevel);*/ AddBuff(CorrosiveBuff, AttackerPC, tIndex); }
			}

			//apply crit damage bonuses
			if (FMath::FRandRange(0.0f, 100.0f) <= AttackerPawn->CriticalHitChance || (AttackerPC->GetSkillValue(SKILL_FLAMEREXTRADAMAGETOBURNING) > 0 && IsOnFire()))
			{
				Damage *= 1.0f + AttackerPawn->CriticalHitMultiplier / 100.0f;
				bCrit = true;
			}

			//damage type bonuses, blunt, piercing, elemental, etc
			//apply these as a dot
			/*Pawn->FireDamage = EquippedStats.Secondary[SECONDARYSTAT_FIREDAMAGE];
			Pawn->LightningDamage = EquippedStats.Secondary[SECONDARYSTAT_LIGHTNINGDAMAGE];
			Pawn->IceDamage = EquippedStats.Secondary[SECONDARYSTAT_ICEDAMAGE];
			Pawn->PoisonDamage = EquippedStats.Secondary[SECONDARYSTAT_POISONDAMAGE];
			Pawn->CorrosiveDamage = EquippedStats.Secondary[SECONDARYSTAT_CORROSIVEDAMAGE];*/

			//bonus damage to enemy type
			if (bIsBigDino) Damage *= 1.0f + (AttackerPawn->LargeDinoBoost / 100.0f);
			if (Cast<AOrionDroidPawn>(this)) Damage *= 1.0f + (AttackerPawn->RobotBoost / 100.0f);

			//elemental skills
			if (DamageType->DamageType == DAMAGE_ELEMENTAL) Damage *= 1.0f + float(AttackerPC->GetSkillValue(SKILL_ELEMENTALDAMAGE)) / 100.0f;

			//legendary weapon type damage boosts
			AOrionInventoryManager *Inv = AttackerPC->GetInventoryManager();
			if (Inv && !DamageType->bIsKnife && DamageType->DamageType != DAMAGE_ELEMENTAL && (DamageType->WeaponSlot == 1 || DamageType->WeaponSlot == 2))
			{
				if (Inv->HasStat(RARESTAT_PISTOLBONUSDAMAGE) && DamageType->WeaponType == WEAPON_PISTOL) Damage *= 1.5f;
				if (Inv->HasStat(RARESTAT_PROJECTILEDAMAGEBONUS) && DamageType->WeaponType == WEAPON_PROJECTILE) Damage *= 1.5f;
				if (Inv->HasStat(RARESTAT_AUTORIFLEBONUSDAMAGE) && DamageType->WeaponType == WEAPON_AUTORIFLE) Damage *= 1.5f;
				if (Inv->HasStat(RARESTAT_LMGBONUSDAMAGE) && DamageType->WeaponType == WEAPON_LMG) Damage *= 1.5f;
				if (Inv->HasStat(RARESTAT_SHOTGUNBONUSDAMAGE) && DamageType->WeaponType == WEAPON_SHOTGUN) Damage *= 1.5f;
				if (Inv->HasStat(RARESTAT_BONUSMELEE) && DamageType->WeaponType == WEAPON_MELEE) Damage *= 1.5f;
				if (Inv->HasStat(RARESTAT_BONUSSNIPER) && DamageType->WeaponType == WEAPON_SNIPER) Damage *= 1.5f;
			}

			if (Inv && Inv->HasStat(RARESTAT_DOUBLEDAMAGENOSHIELD) && AttackerPawn->Shield <= 0.0f) Damage *= 2.0f;
		}
	}

	//rpg damage reduction
	Damage *= 1.0f - (DamageReduction / 100.0f);
	if (DamageType->DamageType == DAMAGE_BLUNT) Damage *= 1.0f - (BluntDamageReduction / 100.0f);
	if (DamageType->DamageType == DAMAGE_PIERCING) Damage *= 1.0f - (PiercingDamageReduction / 100.0f);
	if (DamageType->DamageType == DAMAGE_EXPLOSIVE) Damage *= 1.0f - (ExplosiveDamageReduction / 100.0f);
	if (DamageType->DamageType == DAMAGE_ELEMENTAL) Damage *= 1.0f - (ElementalDamageReduction / 100.0f);
	if (DamageType->DamageType == DAMAGE_POISON) Damage *= 1.0f - (PoisonDamageReduction / 100.0f);

	/*if (AttackerPC)
	{
		AOrionCharacter *AttackerPawn = Cast<AOrionCharacter>(AttackerPC->GetPawn());

		if (AttackerPawn && AttackerPawn->bDowned)
			Damage *= 0.75f;
	}*/

	if (DamageType && DamageType->bIsKnife)
	{
		if (AttackerPC)
		{
			Damage *= 1.0f + (AttackerPC->GetSkillValue(SKILL_MELEEDAMAGE) / 100.0f);
			Damage *= 1.0f + (MeleeDamageBoost / 100.0f);

			AOrionInventoryManager *Inv = AttackerPC->GetInventoryManager();
			if (AttackerPawn && Inv && Inv->HasStat(RARESTAT_CLOAKMELEE) && AttackerPawn->CurrentSkill && AttackerPawn->CurrentSkill->IsCloaking()) Damage *= 5.0f;
		}
	}

	if (bIsBigDino)
	{
		if (AttackerPC)
			Damage *= 1.0f + float(AttackerPC->GetSkillValue(SKILL_DAMAGETOLARGEDINOS)) / 100.0f;
	}

	if (AttackerPC && AttackerPC->HasOrbEffect(ORB_STOPPING))
		Damage *= 2.0f;

	if (Health <= 0.f)
	{
		/*if (DamageType && DamageType->bGibAll)
		{
			const FPointDamageEvent *RealEvent = (FPointDamageEvent*)&DamageEvent;
			GibAll(RealEvent->HitInfo.ImpactPoint);
		}
		//see if we want to blow some body pieces off
		else
			HandleGibs(Damage, DamageEvent);*/

		return 0.f;
	}

	if (IsOvercharging())
		Damage /= 2.0f;

	//this is the number that damage numbers will see
	float OriginalDamage = Damage;

	// Modify based on game rules.
	//AOrionGameMode* const Game = GetWorld()->GetAuthGameMode<AOrionGameMode>();

	if (!DamageType || !DamageType->bIgnoreModify)
		Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	if (bKnockBack && !bIsBigDino && !bIsHealableMachine && !bLatchedOnto && !bKnockedDown && DamageType && !bFatality && !bFinishingMove && DamageType->bKnockBack && (!CurrentSkill || !CurrentSkill->IsJetpacking()))
	{
		//if this is a big dino doing the knockback, kill any small dinos caught in the area, and do no knockbacks to other dinos
		AOrionDinoPawn *EnemyDino = Cast<AOrionDinoPawn>(DamageCauser);
		AOrionDinoPawn *Dino = Cast<AOrionDinoPawn>(this);
		bool bDoKnockback = true;

		if (EnemyDino && EnemyDino->bIsBigDino)
		{
			if (PlayerState && PlayerState->bIsABot)
			{
				bDoKnockback = false;

				if (!Dino || !Dino->bIsBigDino)
				{
					Damage = 10000000.0f;
				}
			}
		}

		//ignore damage to self
		if (bDoKnockback && EventInstigator != Controller && GetMovementComponent())
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
		if (DamageType && DamageType->WeaponName == "Jetpack")
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), JetpackBurnFX, GetActorLocation());

		//add this player to our assist tracker
		if (AttackerPC)
			Assisters.AddUnique(AttackerPC);

		//if we're an ai, tell us about this bastard
		if (Cast<AOrionAIController>(Controller))
			Cast<AOrionAIController>(Controller)->OnSeePawn(Cast<APawn>(DamageCauser));

		LastTakeHitTime = GetWorld()->TimeSeconds;

		//take off shield damage first
		float TotalDamage = ActualDamage;

		bool bShield = Shield > 0.0;

		float S = 1.0f;

		if (bDoubleShield)
			S = 2.0f;

		TotalDamage = FMath::Max(0.0f, TotalDamage - Shield / S);
		Shield = FMath::Max(0.0f, Shield - ActualDamage / S);

		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

		if (bShield && Shield <= 0.0f)
		{
			if (FMath::RandRange(0, 1) == 1)
				PlayVoice(VOICE_INEEDBACKUP);
			else
				PlayVoice(VOICE_WOUNDED);

			if (PC)
			{
				PC->PlayShieldEffect(false);
			}
		}

		if (PC)
			PC->PlayHUDHit();

		Health = FMath::Max(0.0f, Health - TotalDamage);

		if (FMath::RandRange(0, 15) == 2)
			PlayVoice(VOICE_TAKINGDAMAGE);

		AOrionPlayerController *Damager = Cast<AOrionPlayerController>(EventInstigator);
		if (Damager)
		{
			const FPointDamageEvent *RealEvent = (FPointDamageEvent*)&DamageEvent;
			if (RealEvent)
			{
				if (DamageType && DamageType->bIsKnife)
					Damager->AddDamageNumber(OriginalDamage, GetActorLocation(), bCrit);
				else if (DamageType && DamageType->bGibAll)
					Damager->AddDamageNumber(OriginalDamage, GetActorLocation(), bCrit);
				else if (DamageType && DamageType->WeaponName == "Turret")
					Damager->AddDamageNumber(OriginalDamage, GetActorLocation(), bCrit);
				else if (DamageType && DamageType->WeaponName == "Flamethrower")
					Damager->AddDamageNumber(OriginalDamage, GetActorLocation(), bCrit);
				else
					Damager->AddDamageNumber(OriginalDamage, RealEvent->HitInfo.ImpactPoint, bCrit);
			}
		}

		//don't let dinos die while doing a fatality
		if (bFinishingMove)
			Health = FMath::Max(1.0f, Health);

		if (Health <= 0)
		{
			if (CanBeDowned(Damage, DamageEvent, EventInstigator, DamageCauser))
			{
				TimesDowned++;

				PC->SendTutorialMessage(TEXT("DOWNED"), TEXT("YOU HAVE BEEN DOWNED, WAIT FOR A TEAMMATE TO REVIVE YOU, OR KILL ENEMIES TO REGAIN HEALTH!"));

				PC->SendTutorialMessage(TEXT("DOWNED AGAIN"), TEXT("EVERY TIME YOU GO DOWN, THE TIMER GETS SHORTER!"));

				bDowned = true;

				AOrionAIController *AI = Cast<AOrionAIController>(EventInstigator);
				if (AI)
				{
					AOrionCharacter *AIPawn = Cast<AOrionCharacter>(AI->GetPawn());
					if (AIPawn)
					{
						AIPawn->PlayVoice(VOICE_BOSSKILL);
					}
				}

				//can only use pistol when downed
				if(CurrentWeapon)
					CurrentWeapon->ClientStopFire();

				EquipWeaponFromSlot(2);
				bRun = false;
				StopAiming();

				if (CurrentSkill)
					CurrentSkill->DeactivateSkill();

				int32 BaseDowns = 40;

				if (PC)
				{
					AOrionInventoryManager *Inv = PC->GetInventoryManager();
					if (Inv && Inv->HasStat(RARESTAT_EXTRADOWN))
						BaseDowns = 50;
				}

				DownedTime = BaseDowns - (TimesDowned * 10);

				AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
				GetWorld()->GetAuthGameMode<AOrionGameMode>()->HandleStats(EventInstigator, KilledPlayer, this, DamageType);

				DownedDamageEvent = DamageEvent;
				DownedEventInstigator = nullptr;// EventInstigator;
				DownedDamageCauser = nullptr;// DamageCauser;

				GetWorldTimerManager().SetTimer(DownedTimer, this, &AOrionCharacter::TickDownedTime, 1.0f, false);
			}
			else
			{
				if (AttackerPC)
				{
					AOrionInventoryManager *Inv = AttackerPC->GetInventoryManager();
					if (Inv)
					{
						if (AttackerPawn && AttackerPawn->CurrentWeapon && Inv->HasStat(RARESTAT_KILLRELOAD) && DamageType->WeaponSlot == 2) AttackerPawn->CurrentWeapon->AmmoInClip = AttackerPawn->CurrentWeapon->GetClipSize();

						if (Inv->HasStat(RARESTAT_EXPLODEKILLS) && DamageType->WeaponSlot == 1) Explode(AttackerPC);
						if (Inv->HasStat(RARESTAT_KNIFEREGEN) && DamageType->bIsKnife && bFinishingMove && AttackerPawn) AttackerPawn->Health = FMath::Min(AttackerPawn->HealthMax, AttackerPawn->Health + 0.5f * AttackerPawn->HealthMax);
						if (Inv->HasStat(RARESTAT_KNIFERELOAD) && DamageType->bIsKnife && AttackerPawn && AttackerPawn->GetWeapon()) AttackerPawn->GetWeapon()->ReloadWeapon();// ->AmmoInClip = AttackerPawn->GetWeapon()->InstantConfig.ClipSize;
					}
				}
				Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);

				if (DamageType && DamageType->bGibAll)
				{
					const FPointDamageEvent *RealEvent = (FPointDamageEvent*)&DamageEvent;
					GibAll(RealEvent->HitInfo.ImpactPoint, Damager);

					AOrionDinoPawn *DeadDino = Cast<AOrionDinoPawn>(this);
					if (DeadDino && DeadDino->DinoName.ToString().ToUpper() == TEXT("TRIKE"))
					{
						if (Damager && Damager->GetAchievements())
							Damager->GetAchievements()->UnlockAchievement(ACH_INHUMANE, Damager);
					}
				}
				//see if we want to blow some body pieces off
				else
					HandleGibs(Damage, DamageEvent, Damager);
			}
		}
		else
		{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);

		if (Health > 0)
		{
			TotalDamageReceived += ActualDamage;
			if (PC && PC->GetAchievements() && TotalDamageReceived >= 25000.0f)
				PC->GetAchievements()->UnlockAchievement(ACH_UNKILLABLE, PC);

			AOrionDroidPawn *Droid = Cast<AOrionDroidPawn>(DamageCauser);
			if (Game && Game->Difficulty >= DIFF_INSANE && PC && PC->GetAchievements() && Droid && Droid->DroidName.ToString().ToUpper() == TEXT("ORB") && bDirectGrenadeHit)
				PC->GetAchievements()->UnlockAchievement(ACH_IRONBALLS, PC);
		}
	}

	bDirectGrenadeHit = false;

	return ActualDamage;
}

void AOrionCharacter::TickDownedTime()
{
	DownedTime = FMath::Max(0, DownedTime - 1);

	if (DownedTime <= 0 && GetWorld()->TimeSeconds - LastHealTime > 1.5f)
	{
		bDowned = false;
		Die(50.0f, DownedDamageEvent, DownedEventInstigator, DownedDamageCauser);
	}
	else
		GetWorldTimerManager().SetTimer(DownedTimer, this, &AOrionCharacter::TickDownedTime, 1.0f, false);
}

void AOrionCharacter::OnRep_Downed()
{
	if (bDowned)
	{
		//ignore dino collision
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Ignore);

		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
		if (PC)
		{
			PC->SendTutorialMessage(TEXT("DOWNED"), TEXT("YOU HAVE BEEN DOWNED, WAIT FOR A TEAMMATE TO REVIVE YOU!"));

			//pistol only
			EquipWeaponFromSlot(2);
			bRun = false;
			StopAiming();

			if (CurrentWeapon)
				CurrentWeapon->StopFire();

			if (CurrentSkill)
				CurrentSkill->DeactivateSkill();
		}
		//get local player and tell them how to revive players
		else
		{
			PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());

			if (PC)
				PC->SendTutorialMessage(TEXT("TEAMMATE DOWN"), TEXT("A TEAMMATE HAS DIED, HEAL THEM WITH YOUR REGEN GUN OR HOLD x8xRELOADx8x NEAR THEIR BODY"));
		}
	}
	else
	{
		//reset dino collision
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);
	}
}

bool AOrionCharacter::CanBeDowned(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	//limit to only happen on dedis for now
	////if (GetNetMode() != NM_DedicatedServer)
	////	return false;

	//3 revives per life
	int32 Revives = 3;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	if (PC)
	{
		AOrionInventoryManager *Inv = PC->GetInventoryManager();
		if (Inv && Inv->HasStat(RARESTAT_EXTRADOWN))
			Revives = 4;
	}

	if (TimesDowned >= Revives)
		return false;

	UOrionDamageType *DamageType = Cast<UOrionDamageType>(DamageEvent.DamageTypeClass.GetDefaultObject());
	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	if (DamageType && PRI)
	{
		if (bFatality || bLatchedOnto || bKnockedDown || PRI->bIsABot)
			return false;

		return true;
	}

	return false;
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

	if (Killer && Killer->GetPawn())
	{
		AOrionCharacter *KillerPawn = Cast<AOrionCharacter>(Killer->GetPawn());

		if (KillerPawn && KillerPawn->bIsHealableMachine)
		{
			AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

			if (GRI)
				GRI->HarvKills++;
		}
	}

	// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	UOrionDamageType *dType = Cast<UOrionDamageType>(DamageEvent.DamageTypeClass.GetDefaultObject());

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Killer);

	if (dType && dType->WeaponName != TEXT(""))
	{
		if (PC && PC->GetStats())
		{
			if (dType->WeaponName.ToUpper() == TEXT("AUTORIFLE"))
				PC->GetStats()->AddStatValue(STAT_AUTORIFLEKILLS, 1);
			else if (dType->WeaponName.ToUpper() == TEXT("PISTOL"))
				PC->GetStats()->AddStatValue(STAT_PISTOLKILLS, 1);
			else if (dType->WeaponName.ToUpper() == TEXT("COMBATSHOTGUN"))
				PC->GetStats()->AddStatValue(STAT_COMBATSHOTGUNKILLS, 1);
			else if (dType->WeaponName.ToUpper() == TEXT("MAGNUM"))
				PC->GetStats()->AddStatValue(STAT_MAGNUMKILLS, 1);
			else if (dType->WeaponName.ToUpper() == TEXT("SILENCEDSMG"))
				PC->GetStats()->AddStatValue(STAT_SILENCEDSMGKILLS, 1);
			else if (dType->WeaponName.ToUpper() == TEXT("AUTOPISTOL"))
				PC->GetStats()->AddStatValue(STAT_AUTOPISTOLKILLS, 1);
			else if (dType->WeaponName.ToUpper() == TEXT("SMG"))
				PC->GetStats()->AddStatValue(STAT_SMGKILLS, 1);
			else if (dType->WeaponName.ToUpper() == TEXT("AUTOSHOTGUN"))
				PC->GetStats()->AddStatValue(STAT_AUTOSHOTGUNKILLS, 1);
			else if (dType->WeaponName.ToUpper() == TEXT("SNIPER"))
				PC->GetStats()->AddStatValue(STAT_SNIPERKILLS, 1);

			if (dType->WeaponName.ToUpper() == TEXT("FRAGGRENADE"))
			{
				if (GetWorld()->GetTimeSeconds() - PC->LastGrenadeKillTime > 1.0f)
					PC->GrenadeKills = 0;

				PC->GrenadeKills++;

				if (PC->GrenadeKills >= 10 && PC->GetAchievements())
					PC->GetAchievements()->UnlockAchievement(ACH_MONSTERFRAG, PC);

				PC->LastGrenadeKillTime = GetWorld()->GetTimeSeconds();
			}
		}
	}

	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	GetWorld()->GetAuthGameMode<AOrionGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	if (Killer && Killer->IsValidLowLevel())
		GetWorld()->GetAuthGameMode<AOrionGameMode>()->HandleStats(Killer, KilledPlayer, this, DamageType);

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

		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

		FHitResult HitInfo;
		FVector ImpulseDir;
		DamageEvent.GetBestHitInfo(this, PawnInstigator, HitInfo, ImpulseDir);

		//play some kind of hit effect from bites and stuff, so the player can see when they're hit

		UOrionDamageType *DamageType = Cast<UOrionDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());

		if (DamageType && DamageType->bIsKnife)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), KnifeHitSound, HitInfo.ImpactPoint);
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
	//if (ArmorList.Num() > index)
	//{
		UnEquipArmor(ITEM_HELMET);
		UnEquipArmor(ITEM_CHEST);
		UnEquipArmor(ITEM_LEGS);
		UnEquipArmor(ITEM_HANDS);

		EquipArmor(EventGetArmor(ITEM_HELMET, index).GetDefaultObject());
		EquipArmor(EventGetArmor(ITEM_CHEST, index).GetDefaultObject());
		EquipArmor(EventGetArmor(ITEM_LEGS, index).GetDefaultObject());
		EquipArmor(EventGetArmor(ITEM_HANDS, index).GetDefaultObject());

		if (index == 2)
		{
			bFemale = true;
			//EventSetFemaleMesh();
		}
	//}
}

void AOrionCharacter::OnRep_Female()
{
	EventSetFemaleMesh();
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

	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	if (!PRI)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AOrionCharacter::CreateHealthBar, 0.1f, false);
		return;
	}

	//EventCreateHealthBar();

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());

	AOrionPRI *PRI2 = Cast<AOrionPRI>(PC->PlayerState);

	if (!PRI2)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AOrionCharacter::CreateHealthBar, 0.1f, false);
		return;
	}

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
	bDowned = false;

	if (bIsDying)
	{
		return;
	}

	if (bLatchedOnto)
		bLatchedOnto = false;

	if (bKnockedDown)
		bKnockedDown = false;

	PlayStunEffect(false);

	GetMesh()->DetachFromParent(true);

	UOrionDamageType *DamageType = Cast<UOrionDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	if (DamageType && DamageType->bIsKnife)
	{
		FHitResult HitInfo;
		FVector ImpulseDir;
		DamageEvent.GetBestHitInfo(this, PawnInstigator, HitInfo, ImpulseDir);

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), KnifeHitSound, HitInfo.ImpactPoint);
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
	{
		CurrentSkill->DeactivateSkill();
		CurrentSkill->Destroy();
		CurrentSkill = nullptr;
	}

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

	Buffs.Empty();
	UpdateBuffFX();

	if (Role == ROLE_Authority)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
		if (PRI)
			PRI->OrbEffects.Empty();

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

	//stop any fx we have going on
	EventStopTrailFX();

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
			PC->ClientSetDeathSpectate(bFatality && bFatalityRemove? nullptr : this);
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
	if (bFatality && bFatalityRemove)
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

void AOrionCharacter::TornOff()
{
	Super::TornOff();

	SetLifeSpan(45.f);
	SetRagdollPhysics();
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

	//if (ReplicatedAnimation.bStopAllOtherAnims)
	//	StopAllAnimMontages();

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
				CurrentWeapon->GetWeaponMesh(true)->AnimScriptInstance->Montage_Play(Animation.Weapon1P, InPlayRate);// / Animation.Weapon1P->RateScale;
			if (CurrentWeapon->GetWeaponMesh(false) && Animation.Weapon3P && !IsFirstPerson())
				CurrentWeapon->GetWeaponMesh(false)->AnimScriptInstance->Montage_Play(Animation.Weapon3P, InPlayRate);// / Animation.Weapon3P->RateScale;
		}

		//play 1p arms animation
		if (Arms1PMesh && Arms1PMesh->AnimScriptInstance && Animation.Pawn1P)
			Duration = Arms1PMesh->AnimScriptInstance->Montage_Play(Animation.Pawn1P, InPlayRate) / Animation.Pawn1P->RateScale;// , 1.0);
		//play 3p char animation
		if (Animation.Pawn3P)
		{
			if (Duration < 0.05f)
				Duration = FMath::Max(Duration, AnimInstance->Montage_Play(Animation.Pawn3P, InPlayRate) / Animation.Pawn3P->RateScale);
			else
				AnimInstance->Montage_Play(Animation.Pawn3P, InPlayRate);
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
				CurrentWeapon->GetWeaponMesh(false)->SetHiddenInGame(true);
				GetWorldTimerManager().SetTimer(GrenadeHideTimer, this, &AOrionCharacter::UnhideWeapon, Duration * 0.9f, false);
			}

			return Duration / InPlayRate;
		}
	}

	return 0.f;
}

void AOrionCharacter::UnhideWeapon()
{
	if (CurrentWeapon)
		CurrentWeapon->GetWeaponMesh(false)->SetHiddenInGame(false);
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
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
		if (GRI)
			GRI->AddRagdoll(this);

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
			if (GRI->bIsLocalCoop)
			{
				AOrionPlayerController *MyPC = Cast<AOrionPlayerController>(Controller);
				if (MyPC)
				{
					if (MyPC->ControllerIndex == 0)
					{
						col.R = 0.0f;
						col.G = 5.0f;
						col.B = 0.0f;
						col.A = 1.0f;
					}
					else if (MyPC->ControllerIndex == 1)
					{
						col.R = 0.0f;
						col.G = 0.0f;
						col.B = 5.0f;
						col.A = 1.0f;
					}
					else if (MyPC->ControllerIndex == 2)
					{
						col.R = 5.0f;
						col.G = 0.0f;
						col.B = 5.0f;
						col.A = 1.0f;
					}
					else if (MyPC->ControllerIndex == 3)
					{
						col.R = 5.0f;
						col.G = 5.0f;
						col.B = 0.0f;
						col.A = 1.0f;
					}
				}
			}
			else if (PC == Controller)
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
			bThirdPersonCamera = PC->bThirdPersonCamera;
			/*AOrionPRI *PRI = Cast<AOrionPRI>(PC->PlayerState);
			if (PRI && PRI->InventoryManager)
			{
				if (!PRI->PlayFabID.IsEmpty())
					PRI->InventoryManager->EquipItems(this, ITEM_ANY);
			}*/

			//set our stencil index, 0 means off
			if (PC->ControllerIndex > -1)
			{
				GetMesh()->CustomDepthStencilValue = PC->ControllerIndex + 1;
				BodyMesh->CustomDepthStencilValue = PC->ControllerIndex + 1;
				HelmetMesh->CustomDepthStencilValue = PC->ControllerIndex + 1;
				ArmsMesh->CustomDepthStencilValue = PC->ControllerIndex + 1;
				LegsMesh->CustomDepthStencilValue = PC->ControllerIndex + 1;
				Flight1Mesh->CustomDepthStencilValue = PC->ControllerIndex + 1;
				Flight2Mesh->CustomDepthStencilValue = PC->ControllerIndex + 1;
			}
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

	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
	AOrionLobbyPawn *LPawn = Cast<AOrionLobbyPawn>(this);
	bool bLobby = false;
	if (GRI)
		bLobby = GRI->bIsLobby || LPawn != nullptr;

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

	if (Arms1PArmorMesh)
		Arms1PArmorMesh->SetHiddenInGame(!bFirst || bBlinking || bLobby); //SetOwnerNoSee(!bFirst);

	//hide first person stuff for now
	////if (Arms1PArmorMesh) Arms1PArmorMesh->SetHiddenInGame(true);
	////if (Arms1PMesh) Arms1PMesh->SetHiddenInGame(true);

	if (CurrentWeapon)
	{
		CurrentWeapon->AttachMeshToPawn();
		if (bAim)
			CurrentWeapon->StartAiming(false);
	}
	//	CurrentWeapon->OnEquip();

	if (GRI && GRI->bIsLocalCoop && Controller && Controller->IsLocalPlayerController())
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

		if (Cast<AOrionLobbyPawn>(this) == nullptr && PC && (PC->DropPod == nullptr || PC->DropPod->bHasLanded))
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
}

void AOrionCharacter::EnableCustomDepth_Implementation()
{
	if (Cast<AOrionLobbyPawn>(this))
		return;

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

		if(CurrentWeapon && PlayerState && !PlayerState->bIsABot)
		{
			CurrentWeapon->GetWeaponMesh(false)->SetRenderCustomDepth(true);
			if (Cast<AOrionPlayerController>(Controller))
				CurrentWeapon->GetWeaponMesh(false)->CustomDepthStencilValue = Cast<AOrionPlayerController>(Controller)->ControllerIndex + 1;
		}
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

	int32 HiddenLevel = 0;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	AOrionInventoryManager *Inv = nullptr;

	if (PC)
		Inv = PC->GetInventoryManager();

	DamageBonus = 0.0f;
	SpeedMultiplier = 1.0f;

	bDoubleShield = false;

	for (auto Itr(Buffs.CreateIterator()); Itr; ++Itr)
	{
		AOrionBuff *Buff = *Itr;

		if (!Itr)
		{
			Buffs.Remove(Buff);
			continue;
		}

		//see if we're hidden from enemy AI view
		if (Buff->bBlockSight)
			HiddenLevel = Buff->UpgradeLevel;

		AOrionPlayerController *OwnerPC = Cast<AOrionPlayerController>(Buff->ControllerOwner);
		AOrionInventoryManager *OwnerInv = nullptr;

		if (OwnerPC)
			OwnerInv = OwnerPC->GetInventoryManager();

		if (Buff->DamageBonus > 0.0f && OwnerInv && OwnerInv->HasStat(Buff->RareStat))
			DamageBonus += Buff->DamageBonus;

		if (Buff->BuffName == "DoubleShield")
			bDoubleShield = true;

		//check if we need to tick damage or anything
		Buff->TickTimer -= DeltaSeconds;

		SpeedMultiplier = FMath::Min(SpeedMultiplier, Buff->SpeedMultiplier);

		if (Buff->TickTimer <= 0.0f)
		{
			//apply damage/heal or whatever
			if (Buff->Damage > 0.0f)
			{
				if (Inv && Inv->HasStat(Buff->RareStat) && !bDowned && Buff->DamageBonus == 0.0f)
				{
					AddHealth(int32(HealthMax / 3));
				}
				else
				{
					FPointDamageEvent PointDmg;

					PointDmg.DamageTypeClass = Buff->DamageType;
					PointDmg.Damage = Buff->Damage;
					PointDmg.ShotDirection = FVector(0.0f);

					AController *cOwner = Buff->ControllerOwner;

					if (!cOwner || !cOwner->IsValidLowLevel())
						cOwner = nullptr;

					if (cOwner && !GRI->OnSameTeam(Cast<AOrionPRI>(cOwner->PlayerState), Cast<AOrionPRI>(PlayerState)))
						TakeDamage(Buff->Damage, PointDmg, cOwner, cOwner ? cOwner->GetPawn() : nullptr);
					else if (!cOwner && PRI->GetTeamIndex() != Buff->TeamIndex)
						TakeDamage(Buff->Damage, PointDmg, Controller, this);
				}
			}
			else if (Buff->Damage < 0.0f && !bDowned)
				AddHealth(int32(-Buff->Damage));

			if (Buff->HealPercent > 0.0f && !bDowned)
			{
				Health = FMath::Min(HealthMax, Health + (Buff->HealPercent * HealthMax));

				LastHealTime = GetWorld()->GetTimeSeconds();
				HealTarget = 5.0f;
			}

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

			UpdateBuffFX();

			continue;
		}
	}

	HiddenFromViewLevel = HiddenLevel;
}

void AOrionCharacter::AddBuff(TSubclassOf<AOrionBuff> BuffClass, AController *cOwner, int32 TeamIndex)
{
	if (Role < ROLE_Authority)
		return;

	//only living players can be buffed/debuffed
	if (Health <= 0)
		return;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
	{
		AOrionInventoryManager *Inv = PC->GetInventoryManager();
		if (Inv)
		{
			if (Inv->HasStat(RARESTAT_INVULNFAT) && bFinishingMove) return;
			if (Inv->HasStat(RARESTAT_JETPACKIMMUNE) && CurrentSkill && CurrentSkill->IsJetpacking()) return;
			if (Inv->HasStat(RARESTAT_CLOAKIMMUNE) && CurrentSkill && CurrentSkill->IsCloaking()) return;
		}
	}

	//first check if we already have this buff, if we do, add any stacks and refresh the timer
	for (int32 i = 0; i < Buffs.Num(); i++)
	{
		if (Buffs[i] && Buffs[i]->GetClass() == BuffClass && TeamIndex == Buffs[i]->TeamIndex)
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

		UOrionDamageType *dType = Cast<UOrionDamageType>(Buff->DamageType);
		if (PC && dType && dType->DamageType == DAMAGE_ELEMENTAL)
			Buff->Duration += PC->GetSkillValue(SKILL_ELEMENTALLENGTH);

		Buffs.Add(Buff);

		UpdateBuffFX();
	}
}

void AOrionCharacter::MakeSureDead()
{
	if (Health >= 0)
	{
		FDamageEvent dEvent = FDamageEvent::FDamageEvent();
		dEvent.DamageTypeClass = UOrionDamageType::StaticClass();

		Die(10000000.0f, dEvent, nullptr, this);
	}
}

void AOrionCharacter::PerformFatality(UAnimMontage *Anim, UAnimMontage *EnemyAnim, AOrionCharacter *TheVictim, bool bHideOnFatality)
{
	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	if (!TheVictim || !TheVictim->IsValidLowLevel())
		return;

	//don't perform if we are in a state that won't allow it
	if (bDowned || bLatchedOnto || bKnockedDown)
		return;

	if (Role == ROLE_Authority)
	{
		FatalityAnim.AttackerAnim = Anim;
		FatalityAnim.VictimAnim = EnemyAnim;
		FatalityAnim.Victim = TheVictim;
		FatalityAnim.bRemove = bHideOnFatality;
		FatalityAnim.bToggle = !FatalityAnim.bToggle;

		AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

		if (PRI && PRI->bIsABot && Game)
			Game->PlaySlowMotion(7.0f);

		//make the victim lose control over themselves
		if (TheVictim)
		{
			TheVictim->bFatality = true;
			TheVictim->bFatalityRemove = bHideOnFatality;

			FTimerHandle Handle;
			GetWorldTimerManager().SetTimer(Handle, TheVictim, &AOrionCharacter::MakeSureDead, 3.5f, false);

			AOrionPlayerController *PC = Cast<AOrionPlayerController>(TheVictim->Controller);

			if (PC && PC->GetAchievements())
			{
				if (Cast<AOrionDinoPawn>(this))
				{
					FString DinoName = Cast<AOrionDinoPawn>(this)->DinoName.ToString().ToUpper();
					if (DinoName == TEXT("TREX"))
					{
						PC->GetAchievements()->UnlockAchievement(ACH_TREXFATALITY, PC);

						//trex gets some hp back on eats
						Health = FMath::Min(HealthMax, Health + 0.15f * HealthMax);
					}
					else if (DinoName == TEXT("KRUGER"))
						PC->GetAchievements()->UnlockAchievement(ACH_KRUGERFATALITY, PC);
					else if (DinoName == TEXT("JECKYL"))
						PC->GetAchievements()->UnlockAchievement(ACH_JECKYLFATALITY, PC);
				}
				else if (Cast<AOrionDroidPawn>(this))
				{
					FString DinoName = Cast<AOrionDroidPawn>(this)->DroidName.ToString().ToUpper();
				}
			}

			AOrionPRI *VictimPRI = Cast<AOrionPRI>(TheVictim->PlayerState);

			if (VictimPRI)
				VictimPRI->Deaths++;
		}
	}

	bFinishingMove = true;

	//attach the victim to the attacker for now
	if (TheVictim)
	{
		FWeaponAnim vAnim;
		vAnim.Pawn3P = EnemyAnim;

		if (TheVictim->CurrentWeapon)
		{
			if (TheVictim->CurrentWeapon->InstantConfig.bSingleShellReload)
				TheVictim->CurrentWeapon->CancelReload();
			TheVictim->CurrentWeapon->ClientStopFire();
		}

		TheVictim->StopAllAnimMontages();

		TheVictim->OrionPlayAnimMontage(vAnim, 1.0f, TEXT(""), false, false, true);

		if (PRI && PRI->bIsABot)
			TheVictim->GetMesh()->AttachTo(GetMesh(), "Swallow");
		else
			TheVictim->GetMesh()->AttachTo(GetMesh(), "CompySlice");

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

	float Len = FMath::Max(0.1f, OrionPlayAnimMontage(aAnim, 1.0f, TEXT(""), false, false, true));

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AOrionCharacter::ResetFatality, Len * 1.0f, false);
}

void AOrionCharacter::ResetFatality()
{
	bFinishingMove = false;
	FatalityVictim = nullptr;
}

void AOrionCharacter::OnRep_Fatality()
{
	//play the animations on both us and the victim
	PerformFatality(FatalityAnim.AttackerAnim, FatalityAnim.VictimAnim, FatalityAnim.Victim, FatalityAnim.bRemove);
}

void AOrionCharacter::HandleKnockedDown()
{
	if (Role == ROLE_Authority)
	{
		if (bLatchedOnto && (!Latcher || !Latcher->IsValidLowLevel() || Latcher->Health <= 0.0f))
		{
			bLatchedOnto = false;
			Latcher = nullptr;
		}

		if (bKnockedDown && (!Knocker || !Knocker->IsValidLowLevel()))
		{
			if (Health > 0)
				EventGetUp();

			//bKnockedDown = false;
			Knocker = nullptr;

			//reset dino collision
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);
		}
		else if (Knocker && Knocker->IsValidLowLevel())
		{
			AOrionAIController *C = Cast<AOrionAIController>(Knocker->Controller);
			if (Knocker->Health <= 0.0f)
			{
				if (Health > 0)
					EventGetUp();

				//bKnockedDown = false;
				Knocker = nullptr;

				//reset dino collision
				GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);
			}
			else if (C && C->GetEnemy() != this)
			{
				if (Health > 0)
					EventGetUp();

				//bKnockedDown = false;
				Knocker = nullptr;

				//reset dino collision
				GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);
			}
		}
	}
}

void AOrionCharacter::OnRep_Knocker()
{
	if (Knocker)
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Ignore);
	else if (Health > 0)
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);
}

void AOrionCharacter::HandleRevivingTeammates(float DeltaSeconds)
{
	if (Role == ROLE_Authority && ReviveTarget)
	{
		float Rate = 1.0f;

		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
		if (PC)
		{
			Rate = 1.0f + float(PC->GetSkillValue(SKILL_REVIVESPEED)) / 100.0f;

			AOrionInventoryManager *Inv = PC->GetInventoryManager();

			if (Inv)
			{
				if (Inv->HasStat(RARESTAT_SUPERFASTREVIVE))
					Rate *= 2.0f;
			}
		}

		ReviveTarget->Health = FMath::Min(ReviveTarget->HealthMax, ReviveTarget->Health + DeltaSeconds * 0.5f * Rate * ReviveTarget->HealthMax);
		ReviveTarget->LastHealTime = GetWorld()->TimeSeconds;

		if (ReviveTarget->Health >= ReviveTarget->HealthMax)
		{
			ReviveTarget->Revived();
			ReviveTarget = nullptr;
			return;
		}

		//check to see if we need to break revive
		float dist = (GetActorLocation() - ReviveTarget->GetActorLocation()).Size();

		bool bBreak = false;

		//50 units of leeway
		if (dist > 250.0f)
			bBreak = true;
		else if (Health <= 0.0)
			bBreak = true;
		else if (bFatality || bKnockedDown || bLatchedOnto)
			bBreak = true;

		if (bBreak)
		{
			ReviveTarget = nullptr;
		}
	}
}

void AOrionCharacter::Revived()
{
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (GRI)
	{
		AOrionCharacter *Best = nullptr;
		float Dist = 1000000.0f;

		for (int32 i = 0; i < GRI->PlayerList.Num(); i++)
		{
			if (!GRI->PlayerList[i] || !GRI->PlayerList[i]->IsValidLowLevel())
				continue;

			if (GRI->PlayerList[i]->ControlledPawn == this)
				continue;

			if (GRI->PlayerList[i]->ControlledPawn == nullptr)
				continue;

			float d = (GetActorLocation() - GRI->PlayerList[i]->ControlledPawn->GetActorLocation()).SizeSquared();

			if (d < Dist)
			{
				Best = GRI->PlayerList[i]->ControlledPawn;
				Dist = d;
			}
		}

		if (Best)
		{
			Best->PlayVoice(VOICE_REVIVE);
		}
	}

	if (GetWorldTimerManager().IsTimerActive(DownedTimer))
		GetWorldTimerManager().ClearTimer(DownedTimer);

	LastRevivedTime = GetWorld()->TimeSeconds;

	bDowned = false;

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AOrionCharacter::PlayRevivedVoice, 1.0f, false);
}

void AOrionCharacter::PlayRevivedVoice()
{
	PlayVoice(VOICE_AFFIRMATIVE);
}

void AOrionCharacter::UnequipAdrenaline()
{
	EquipWeapon(Inventory[0]);
	ClientEquipWeapon(Inventory[0]);
}

void AOrionCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateGrenadeTarget(DeltaSeconds);

	UpdateRootYaw(DeltaSeconds);

	UpdateAimKick(DeltaSeconds);

	UpdateBloodDecals(DeltaSeconds);

	UpdateCooldowns(DeltaSeconds);

	HandleBuffs(DeltaSeconds);

	HandleHealEffects(DeltaSeconds);

	//HandleThermalVision(DeltaSeconds);

	HandleKnockedDown();

	HandleRevivingTeammates(DeltaSeconds);

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

			PC->EventHUDSpawn(false);
		}
	}

	//hax for health bars
	if (MyHealthBar == nullptr && GetNetMode() != NM_DedicatedServer && Health > 0 && PlayerState != NULL)
		CreateHealthBar();

	//recharge shields
	if (Role == ROLE_Authority && !bDowned)
	{
		float Rate = 1.0f;

		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
		if (PC)
			Rate = 1.0f + float(PC->GetSkillValue(SKILL_SHIELDREGENSPEED)) / 100.0f;

		if (Health >= HealthMax && Shield < ShieldMax && GetWorld()->TimeSeconds - LastTakeHitTime >= 10.0f)
		{
			Shield = FMath::Min(ShieldMax, Shield + DeltaSeconds * Rate * (ShieldMax / 10.0f));

			if (PC && Shield >= ShieldMax)
				PC->PlayShieldEffect(true);
		}

		if (PC && PC->GetSkillValue(SKILL_HEALTHREGEN) > 0 && Health < HealthMax && GetWorld()->TimeSeconds - LastTakeHitTime >= 10.0f)
			Health = FMath::Min(HealthMax, Health + DeltaSeconds * Rate * (HealthMax / 10.0f));

		if (PC && PC->HasOrbEffect(ORB_HEALTH))
			Health = FMath::Min(HealthMax, Health + DeltaSeconds * (HealthMax / 20.0f));

		if (CurrentSkill && CurrentSkill->IsCloaking())
		{
			Health = FMath::Min(HealthMax, Health + DeltaSeconds * (float(PC->GetSkillValue(SKILL_CLOAKREGEN)) / 100.0f)  * HealthMax);
		}
	}

	if (Role == ROLE_Authority && bDowned && Health >= HealthMax)
		Revived();

	//hax
	if (bBlinking && GetWorld()->TimeSeconds - LastTeleportTime > 1.0f)
		bBlinking = false;

	if (BuffFX)
		BuffFX->SetWorldRotation(FRotator::ZeroRotator);

	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
	if (PRI && PRI->OrbEffects.Num() > 0)
	{
		if (!bDowned)
			LastHealTime = GetWorld()->GetTimeSeconds();
		HealTarget = 5.0f;
	}

	if (IsLocallyControlled() && CurrentSkill && !CurrentSkill->IsFlaming() && CurrentWeapon && CurrentWeapon->InstantConfig.WeaponSlot == 4)
		EquipWeaponFromSlot(1);
	else if (IsLocallyControlled() && CurrentSkill && CurrentSkill->IsFlaming() && CurrentWeapon && CurrentWeapon->InstantConfig.WeaponSlot != 4)
		EquipWeaponFromSlot(4);
	else if (IsLocallyControlled() && Cast<AOrionLobbyPawn>(this) && CurrentWeapon && CurrentWeapon->InstantConfig.WeaponSlot != 2)
		EquipWeaponFromSlot(2);

	//hax for now
	//if (CurrentWeapon == NULL && Inventory.Num() > 0)
	//	EquipWeapon(Inventory[0]);
}

void AOrionCharacter::EquipWeaponFromSlot(int32 index)
{
	if (bDowned)
		index = 2;

	const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);

	int32 nIndex = CurrentWeaponIdx + 1;

	if (nIndex >= Inventory.Num())
		nIndex = 0;

	while (nIndex != CurrentWeaponIdx)
	{
		if (Inventory[nIndex] && Inventory[nIndex]->InstantConfig.WeaponSlot == index)
			break;

		nIndex++;

		if (nIndex >= Inventory.Num())
			nIndex = 0;
	}

	AOrionWeapon* NWeapon = Inventory[nIndex % Inventory.Num()];

	if (NWeapon == CurrentWeapon)
		return;

	EquipWeapon(NWeapon);
}

bool AOrionCharacter::HasOrbEffect(EOrbType Type)
{
	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	if (PRI)
	{
		for (int32 i = 0; i < PRI->OrbEffects.Num(); i++)
		{
			if (PRI->OrbEffects[i].Type == Type)
				return true;
		}
	}

	return false;
}

void AOrionCharacter::HandleHealEffects(float DeltaTime)
{
	if (Role == ROLE_Authority)
	{
		if (GetWorld()->GetTimeSeconds() - LastHealTime > 1.0f)
			HealTarget = 0.0f;
	}

	if (HealTarget != HealAmount)
	{
		if (HealTarget > HealAmount)
			HealAmount = FMath::Min(HealTarget, HealAmount + 5.0f * DeltaTime);
		else if (HealTarget < HealAmount)
			HealAmount = FMath::Max(HealTarget, HealAmount - 5.0f * DeltaTime);

		//update our materials
		for (int32 i = 0; i < CharacterMats.Num(); i++)
		{
			if (CharacterMats[i])
			{
				CharacterMats[i]->SetScalarParameterValue("HealAmount", HealAmount);
			}
		}
	}
}

void AOrionCharacter::HandleThermalVision(float DeltaTime)
{
	//update our thermal vision
	bool bThermal = false;
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		AOrionCharacter *Pawn = Cast<AOrionCharacter>(PC->GetPawn());
		if (Pawn)
		{
			if (Pawn->CurrentSkill && Pawn->CurrentSkill->IsThermalVisioning())
				bThermal = true;
		}
	}

	for (int32 i = 0; i < CharacterMats.Num(); i++)
	{
		if (CharacterMats[i])
		{
			CharacterMats[i]->SetScalarParameterValue("ThermalVision", bThermal ? 1.0f : 0.0f);
		}
	}
}

void AOrionCharacter::UpdateCooldowns(float DeltaTime)
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (!PC)
		return;

	if (Role == ROLE_Authority)
	{
		//don't recharge grenade energy while smoke is active
		if (!bIsGrenadeActive)
		{
			float Rate = 1.0f + (1.0f + GrenadeRechargeRate / 100.0f) * float(PC->GetSkillValue(SKILL_GRENADECOOLDOWN)) / 25.0f;
			GrenadeCooldown.Energy = FMath::Min(GrenadeCooldown.Energy + DeltaTime * Rate, float(GrenadeCooldown.SecondsToRecharge * (PC->GetSkillValue(SKILL_EXTRAGRENADE) > 0 ? 2 : 1)));
		}

		int32 blinkCharges = PC->GetSkillValue(SKILL_EXTRABLINK) > 0 ? 2 : 1;

		if (PC)
		{
			AOrionInventoryManager *Inv = PC->GetInventoryManager();

			if (Inv)
			{
				if (Inv->HasStat(RARESTAT_FREEBLINK))
					blinkCharges++;
			}
		}

		float BlinkRate = 1.0f;
		if(PC)
			BlinkRate += float(PC->GetSkillValue(SKILL_BLINKRECHARGE)) / 100.0f;
		BlinkCooldown.Energy = FMath::Min(BlinkCooldown.Energy + BlinkRate * DeltaTime, float(BlinkCooldown.SecondsToRecharge * blinkCharges));

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
			RootRotation.Yaw = FMath::Min(TargetYaw, RootRotation.Yaw + DeltaSeconds*360.0f);
		}
		else
		{
			RootRotation.Yaw = FMath::Max(TargetYaw, RootRotation.Yaw - DeltaSeconds*360.0f);
		}
	}
	else
	{
		if (TargetYaw - RootRotation.Yaw > 180.0f)
		{
			RootRotation.Yaw += 360.0f;
			RootRotation.Yaw = FMath::Max(TargetYaw, RootRotation.Yaw - DeltaSeconds*360.0f);
		}
		else
		{
			RootRotation.Yaw = FMath::Min(TargetYaw, RootRotation.Yaw + DeltaSeconds*360.0f);
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

	if (bDowned)
		return;

	if (ShouldIgnoreControls())
		return;

	if (GrenadeCooldown.Energy < GrenadeCooldown.SecondsToRecharge)
		return;

	if (false)
	{
		//spawn the grenade target
		if (!GrenadeTarget && GrenadeTargetClass)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.Owner = this;

			GrenadeTarget = GetWorld()->SpawnActor<ADecalActor>(GrenadeTargetClass, AimPos, FRotator::ZeroRotator, SpawnInfo);
			if (GrenadeTarget)
				GrenadeTarget->SetActorHiddenInGame(false);
		}

		if (!GrenadeTarget)
		{
			return;
		}

		if (bTargetingGrenade)
		{
			bTargetingGrenade = false;
			if (GrenadeTarget)
				GrenadeTarget->SetActorHiddenInGame(true);
		}
		else
		{
			bTargetingGrenade = true;
			if (GrenadeTarget)
				GrenadeTarget->SetActorHiddenInGame(false);
		}
	}
	else
	{
		if (CurrentWeapon)
			CurrentWeapon->StopFire();

		GrenadeTargetLocation = AimPos;
		ThrowGrenadeAtLocation();
	}
}

void AOrionCharacter::ThrowGrenadeAtLocation()
{
	bTargetingGrenade = false;

	FWeaponAnim GrenAnim;
	GrenAnim.Pawn3P = GrenadeAnim;
	GrenAnim.bHideWeapon = true;

	bThrowingGrenade = true;

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

void AOrionCharacter::SetGrenadeActive(float ActiveTime)
{
	bIsGrenadeActive = true;
	GetWorldTimerManager().SetTimer(GrenadeActiveTimer, this, &AOrionCharacter::ResetGrenadeActive, ActiveTime, false);
}

void AOrionCharacter::ResetGrenadeActive()
{
	bIsGrenadeActive = false;
}

void AOrionCharacter::RapidFireGrenades()
{
	//FVector dir = FMath::VRand();
	//GrenadeCooldown.Energy = 10000;
	//ActuallyTossGrenade(dir);
}

//throw this bitch
void AOrionCharacter::ActuallyTossGrenade(FVector dir)
{
	if (GrenadeCooldown.Energy < GrenadeCooldown.SecondsToRecharge)
		return;

	if (Role == ROLE_Authority)
	{
		GrenadeCooldown.Energy -= GrenadeCooldown.SecondsToRecharge;
		
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
			{
				if (Grenade->GrenadeName.ToUpper() == TEXT("SMOKE"))
				{
					float Life = Grenade->FXTime + Grenade->LifeTime;
					AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
					if (PC)
						Life += float(PC->GetSkillValue(SKILL_GRENADECOOLDOWN));

					SetGrenadeActive(Life);
				}

				AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

				Grenade->GrenadeLife = Grenade->FXTime;
				Grenade->GrenadeScale = Grenade->ExplosionScale;

				Grenade->bCanSpawnExplosionGrenade = true;

				if (PC)
				{
					Grenade->GrenadeLife += float(PC->GetSkillValue(SKILL_GRENADECOOLDOWN));
					Grenade->GrenadeScale *= 1.0f + (float(PC->GetSkillValue(SKILL_GRENADERADIUS) / 100.0f));

					//legendary grenade bonuses
					AOrionInventoryManager *Inv = PC->GetInventoryManager();
					if (Inv)
					{
						if (Inv->HasStat(RARESTAT_FRAGGRENADE)) Grenade->SecondaryGrenadeType = GRENADE_FRAG;
						if (Inv->HasStat(RARESTAT_EMPGRENADE)) Grenade->SecondaryGrenadeType = GRENADE_EMP;
						if (Inv->HasStat(RARESTAT_SMOKEGRENADE)) Grenade->SecondaryGrenadeType = GRENADE_SMOKE;
						if (Inv->HasStat(RARESTAT_STUNGRENADE)) Grenade->SecondaryGrenadeType = GRENADE_STUN;
						if (Inv->HasStat(RARESTAT_NAPALMGRENADE)) Grenade->SecondaryGrenadeType = GRENADE_NAPALM;
						if (Inv->HasStat(RARESTAT_MULTIGRENADE)) Grenade->bMultiGrenade = true;
					}
				}

				PlayVoice(Grenade->VoiceType);

				Grenade->Init(dir);
				Grenade->SetFuseTime(Grenade->LifeTime);

				if (PC && PC->GetStats())
				{
					if (Grenade->GrenadeName.ToUpper() == TEXT("FRAG"))
						PC->GetStats()->AddStatValue(STAT_FRAGGRENADESTHROWN, 1);
					else if (Grenade->GrenadeName.ToUpper() == TEXT("SMOKE"))
						PC->GetStats()->AddStatValue(STAT_SMOKEGRENADESTHROWN, 1);
					else if (Grenade->GrenadeName.ToUpper() == TEXT("EMP"))
						PC->GetStats()->AddStatValue(STAT_EMPGRENADESTHROWN, 1);
					else if (Grenade->GrenadeName.ToUpper() == TEXT("STUN"))
						PC->GetStats()->AddStatValue(STAT_STUNGRENADESTHROWN, 1);
					else if (Grenade->GrenadeName.ToUpper() == TEXT("NAPALM"))
						PC->GetStats()->AddStatValue(STAT_NAPALMGRENADESTHROWN, 1);
				}
			}
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

	bThrowingGrenade = false;
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
	if (GetWorldTimerManager().IsTimerActive(MeleeHoldTimer))
		return;

	if (ShouldIgnoreControls())
		return;

	if (bDowned)
		return;

	if (CurrentSkill && CurrentSkill->bTarget)
	{
		ActivateSkill();
		return;
	}

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
	if (bDowned)
		return;

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
				if (Cast<AOrionWeaponLink>(CurrentWeapon))
					CurrentWeapon->StopFire();

				AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
				AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
				/*if (Role == ROLE_Authority && PRI && PRI->ClassType == "RECON" && PC && PC->GetSkillValue(SKILL_CLOAKTEAMMATES) > 0)
				{
					TArray<AActor*> Controllers;

					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

					for (int32 i = 0; i < Controllers.Num(); i++)
					{
						AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);

						if (C == PC)
							continue;

						if (C && C->GetPawn())
						{
							AOrionCharacter *Pawn = Cast<AOrionCharacter>(C->GetPawn());
							if (Pawn)
							{
								Pawn->SetTeamCloaking();
							}
						}
					}
				}*/
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

	if (bDowned)
		return;

	//no interupting grenade animation
	if (GetWorldTimerManager().IsTimerActive(GrenadeTimer))
		return;

	AOrionPlayerController* MyPC = Cast<AOrionPlayerController>(Controller);
	if (MyPC)// && MyPC->IsGameInputAllowed())
	{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || true))//CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);

			int32 nIndex = CurrentWeaponIdx + 1;

			if (nIndex >= Inventory.Num())
				nIndex = 0;

			while (nIndex != CurrentWeaponIdx)
			{
				if (!Cast<AOrionWeaponLink>(Inventory[nIndex]) && Inventory[nIndex]->InstantConfig.WeaponSlot < 4)
					break;

				nIndex++;

				if (nIndex >= Inventory.Num())
					nIndex = 0;
			}

			AOrionWeapon* NWeapon = Inventory[nIndex % Inventory.Num()];

			if (NWeapon == CurrentWeapon)
				return;

			EquipWeapon(NWeapon);
		}
	}
}

void AOrionCharacter::OnPrevWeapon()
{
	if (ShouldIgnoreControls())
		return;

	if (bDowned)
		return;

	//no interupting grenade animation
	if (GetWorldTimerManager().IsTimerActive(GrenadeTimer))
		return;

	AOrionPlayerController* MyPC = Cast<AOrionPlayerController>(Controller);
	if (MyPC)// && MyPC->IsGameInputAllowed())
	{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || true))//CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);

			int32 nIndex = CurrentWeaponIdx - 1;

			if (nIndex < 0)
				nIndex = Inventory.Num() - 1;

			while (nIndex != CurrentWeaponIdx)
			{
				if (!Cast<AOrionWeaponLink>(Inventory[nIndex]) && Inventory[nIndex]->InstantConfig.WeaponSlot < 4)
					break;

				nIndex--;

				if (nIndex < 0)
					nIndex = Inventory.Num() - 1;
			}

			AOrionWeapon* PrevWeapon = Inventory[nIndex % Inventory.Num()];

			if (PrevWeapon == CurrentWeapon)
				return;

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

/*void AOrionCharacter::Use()
{
	if (ShouldIgnoreControls())
		return;

	if (Role < ROLE_Authority)
	{
		ServerUse();
		return;
	}
}*/

void AOrionCharacter::Use()
{
	if (ShouldIgnoreControls())
		return;

	//stuff that only happens client side
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	if (PC && IsLocallyControlled())
	{
		PC->EventCheckUse();
	}

	/*if (Role < ROLE_Authority)
	{
	ServerUse();
	return;
	}*/
}

void AOrionCharacter::BehindView()
{
	bFirstPerson = !bFirstPerson;

	UpdatePawnMeshes();
}

bool AOrionCharacter::ShouldIgnoreControls()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	return IsRolling() || bBlinking || bShoulderCamera || bShipCamera || bFatality || bFinishingMove || bLatchedOnto || bKnockedDown || (PC && PC->bMenuOpen);
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
	BlinkCooldown.Energy -= 5.0f;
	if (Role < ROLE_Authority)
	{
		ServerBlink(dir);
		return;
	}

	if (!DefaultFilterClass)
		return;

	if (bFatality || bKnockedDown || bLatchedOnto)
	{
		BlinkCooldown.Energy += 4.5f;

		EndBlink();
		return;
	}

	if (GetWorld() && GetWorld()->GetNavigationSystem())
	{
		FNavLocation loc;
		FNavAgentProperties props;
		props.AgentHeight = 150.0f;
		props.AgentRadius = 150.0f;

		BlinkPos.Start = GetActorLocation();

		float Dist = 250.0f;

		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
		if (PC)
		{
			Dist *= 1.0f + float(PC->GetSkillValue(SKILL_BLINKDISTANCE)) / 100.0f;
		}

		for (int32 i = 0; i < 4; i++)
		{
			//find our blink end direction
			FVector EndPos = GetActorLocation() + dir * (4 - i) * 250.0f;
			if (GetWorld()->GetNavigationSystem()->ProjectPointToNavigation(EndPos, loc, FVector(125.0f, 125.0f, 1000.0f), GetWorld()->GetNavigationSystem()->GetNavDataForProps(props)))
			//if (GetWorld()->GetNavigationSystem()->->GetRandomReachablePointInRadius(GetWorld(), EndPos, 100.0f))
			{
				//make sure this point is reachable
				//FPathFindingQuery Query;
				//Query.StartLocation = GetActorLocation();
				//Query.EndLocation = loc.Location;

				TSharedPtr<const FNavigationQueryFilter> QueryFilter = GetWorld()->GetNavigationSystem()->MainNavData->GetQueryFilter(DefaultFilterClass);// UNavigationQueryFilter::GetQueryFilter(GetWorld()->GetNavigationSystem()->MainNavData, DefaultFilterClass);

				if (!GetWorld()->GetNavigationSystem()->TestPathSync(FPathFindingQuery(nullptr, *GetWorld()->GetNavigationSystem()->MainNavData, GetActorLocation(), loc.Location, QueryFilter)))
					continue;

				FCollisionQueryParams TraceParams(FName(TEXT("ExtraBlinkTrace")), true, this);

				TraceParams.AddIgnoredActor(this);
				TraceParams.bTraceAsyncScene = true;
				TraceParams.bReturnPhysicalMaterial = true;

				TArray<FHitResult> Results;

				ALandscape *StartLandscape = nullptr;
				ALandscape *EndLandscape = nullptr;

				if (GetWorld()->SweepMultiByChannel(Results, GetActorLocation(), GetActorLocation() - FVector(0, 0, 500.0f), FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(5.0f), TraceParams))
				{
					int32 Counter = 0;

					for (int32 j = 0; j < Results.Num(); j++)
					{
						if (Cast<ALandscape>(Results[j].GetActor()))
							StartLandscape = Cast<ALandscape>(Results[j].GetActor());
					}
				}
				else
					continue;

				Results.Empty();

				if (GetWorld()->SweepMultiByChannel(Results, loc.Location + FVector(0, 0, 2000.0f), loc.Location - FVector(0,0,1000.0f), FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(5.0f), TraceParams))
				{
					int32 Counter = 0;

					for (int32 j = 0; j < Results.Num(); j++)
					{
						if (Cast<ALandscape>(Results[j].GetActor()))
							EndLandscape = Cast<ALandscape>(Results[j].GetActor());
					}
				}
				else
					continue;

				if (StartLandscape != EndLandscape || (EndLandscape && !EndLandscape->bUsedForNavigation))
					continue;

				FVector StartPos = GetActorLocation();
				EndPos = loc.Location + GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				if (TeleportTo(EndPos, GetActorRotation(), true))
				{
					if (PC && PC->GetSkillValue(SKILL_BLINKDISTANCE) == PC->CharacterSkills[SKILL_BLINKDISTANCE].MaxPoints * PC->CharacterSkills[SKILL_BLINKDISTANCE].Modifier)
					{
						//drop a grenade at their old spot
						FActorSpawnParameters SpawnInfo;
						SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
						SpawnInfo.Owner = this;

						AOrionGrenade *Grenade = GetWorld()->SpawnActor<AOrionGrenade>(GrenadeClass, StartPos + FVector(0.0f, 0.0f, 25.0f) + FVector(0.0f, 0.0f, 1.0f) * 75.0f, FRotator::ZeroRotator, SpawnInfo);
						if (Grenade)
						{
							Grenade->Init(FVector(0.0f, 0.0f, 1.0f));
							Grenade->bIsMiniGrenade = true;
							Grenade->SetFuseTime(0.5f);

							Grenade->GrenadeLife = Grenade->FXTime * 0.25f;
							Grenade->GrenadeScale = Grenade->ExplosionScale * 0.25f;

							Grenade->GrenadeLife += float(PC->GetSkillValue(SKILL_GRENADECOOLDOWN)) * 0.25f;
							Grenade->GrenadeScale *= 1.0f + float(PC->GetSkillValue(SKILL_GRENADERADIUS)) / 100.0f;
						}
					}

					if (GetNetMode() != NM_DedicatedServer)
						DoBlinkEffect(true, StartPos);

					if (PC && PC->GetStats())
						PC->GetStats()->AddStatValue(STAT_TIMESBLINKED, 1);

					BlinkPos.End = EndPos;
				}
				else // teleport failed, need a better way to handle this
				{
					//refund our blink energy
					BlinkCooldown.Energy += 4.5f;

					EndBlink();
					return;
				}
				break;
			}
		}

		if (BlinkPos.End == FVector(0, 0, 0))
		{
			//refund our blink energy
			BlinkCooldown.Energy += 4.5f;

			EndBlink();
			return;
		}

		bBlinking = true;
		LastTeleportTime = GetWorld()->GetTimeSeconds();
	}
	else
	{
		//refund our blink energy
		BlinkCooldown.Energy += 4.5f;

		EndBlink();
		return;
	}
	
	GetWorldTimerManager().SetTimer(TeleportTimer, this, &AOrionCharacter::ActuallyTeleport, 0.2f, false);
}

void AOrionCharacter::ActuallyTeleport()
{
	//TeleportTo(BlinkPos, GetActorRotation());

	TeleportTo(BlinkPos.End, GetActorRotation(), false, true);
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

	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	if (PRI && !PRI->bIsABot)
		SetActorHiddenInGame(bOn);

	if (CurrentWeapon)
		CurrentWeapon->SetActorHiddenInGame(bOn);

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	if (PC)
	{
		//PC->SetIgnoreMoveInput(bOn);
		//PC->SetIgnoreLookInput(bOn);
	}

	if (BlinkFX)
	{
		UParticleSystemComponent* BlinkPSC = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlinkFX, pos);
		if (BlinkPSC)
		{
			BlinkPSC->SetWorldScale3D(FVector(2.0f));
		}
	}

	if (BlinkStartSound && bOn)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BlinkStartSound, GetActorLocation(), 1.0f, 1.0f);
	else if (BlinkStopSound && !bOn)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BlinkStopSound, GetActorLocation(), 1.0f, 1.0f);
}

bool AOrionCharacter::ServerDoRoll_Validate(ERollDir rDir, FRotator rRot)
{
	return true;
}

void AOrionCharacter::ResetInput()
{
	DestroyPlayerInputComponent();
	PawnClientRestart();
	/*APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC && PC->IsLocalController())
	{
		// Set up player input component, if there isn't one already.
		if (InputComponent)
		{
			SetupPlayerInputComponent(InputComponent);
		}
	}*/
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
		if (CurrentWeapon->InstantConfig.bSingleShellReload)
			CurrentWeapon->CancelReload();
		CurrentWeapon->StopFire();
	}

	StopAllAnimMontages();

	float Rate = 1.0f;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	if (PC)
		Rate += float(PC->GetSkillValue(SKILL_ROLLSPEED)) / 100.0f;

	float Length = OrionPlayAnimMontage(Info, Rate, FName(""), true, false, true);

	//set roll after anim
	bRolling = true;

	if (PC && PC->GetStats())
		PC->GetStats()->AddStatValue(STAT_TIMESROLLED, 1);

	GetWorldTimerManager().SetTimer(RollTimer2, this, &AOrionCharacter::EndRoll, 0.9f*Length, false);
}

void AOrionCharacter::DoRoll()
{
	if (GetWorldTimerManager().IsTimerActive(RollTimer2))
		return;

	//no interupting grenade animation
	if (GetWorldTimerManager().IsTimerActive(GrenadeTimer))
		return;

	if (RollCooldown > 0.0f)
		return;

	float Length = 1.0f;

	//don't roll if we're not moving
	if (GetVelocity().Size2D() < 1.0f)
		return;

	FRotator newRot = GetActorRotation();

	float Rate = 1.0f;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	if (PC)
		Rate += float(PC->GetSkillValue(SKILL_ROLLSPEED)) / 100.0f;

	if (RollAnimation.Backwards != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			if (CurrentWeapon && CurrentWeapon->InstantConfig.bSingleShellReload && CurrentWeapon->WeaponState == WEAP_RELOADING)
				CurrentWeapon->CancelReload();

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
				//TargetYaw = 180.0f + AimRotLS.Yaw;
				Length = OrionPlayAnimMontage(Info, Rate);// AnimInstance->Montage_Play(RollAnimation.Backwards, 1.f) / RollAnimation.Backwards->RateScale;
				RollDir = ROLL_BACKWARDS;
				newRot = (-AimDirWS).Rotation();
				SetActorRotation((-AimDirWS).Rotation());
			}
			else if (AimRotLS.Yaw < -75.0f)
			{
				Info.Pawn3P = RollAnimation.Left;
				//TargetYaw = 0.0f;
				Length = OrionPlayAnimMontage(Info, Rate);// AnimInstance->Montage_Play(RollAnimation.Left, 1.f) / RollAnimation.Left->RateScale;
				RollDir = ROLL_LEFT;
			}
			else if (AimRotLS.Yaw < 75.0f)
			{
				Info.Pawn3P = RollAnimation.Forwards;
				//TargetYaw = AimRotLS.Yaw;
				Length = OrionPlayAnimMontage(Info, Rate);// AnimInstance->Montage_Play(RollAnimation.Forwards, 1.f) / RollAnimation.Forwards->RateScale;
				RollDir = ROLL_FORWARDS;
				newRot = AimDirWS.Rotation();
				SetActorRotation(AimDirWS.Rotation());
			}
			else
			{
				Info.Pawn3P = RollAnimation.Right;
				//TargetYaw = 0.0f;
				Length = OrionPlayAnimMontage(Info, Rate);// AnimInstance->Montage_Play(RollAnimation.Right , 1.f) / RollAnimation.Right->RateScale;
				RollDir = ROLL_RIGHT;
			}
		}
	}

	bRolling = true;

	if (PC && PC->GetStats())
		PC->GetStats()->AddStatValue(STAT_TIMESROLLED, 1);

	RollCooldown = 1.0f;

	if (Role < ROLE_Authority)
		ServerDoRoll(RollDir, newRot);

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

	if (!bTempAlwaysRotate && (IsRolling() || IsPlayingRootMotion() || bStunned))
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

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && PC->bThirdPersonCamera)
	{

	}
	else
	{
		if (!bTempAlwaysRotate && !bUseControllerRotationYaw)
		{
			NewControlRotation.Yaw = CurrentRotation.Yaw;
		}
	}

	if (!bUseControllerRotationRoll)
	{
		NewControlRotation.Roll = CurrentRotation.Roll;
	}

	//no rolls for now
	if (PC)
	{
		NewControlRotation.Roll = 0.0f;
		NewControlRotation.Pitch = 0.0f;
	}

	if (bIsHealableMachine)
	{
		NewControlRotation.Pitch = 0.0f;
		NewControlRotation.Roll = 0.0f;
	}

	/*if(GetCharacterMovement()->MovementMode != MOVE_Flying && IsSprinting())
		SetActorRotation(GetMovementComponent()->Velocity.Rotation());
	else */if (RotationRate > 0.1f)
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
	/*UOrionGameUserSettings *Settings = nullptr;

	if (GEngine && GEngine->GameUserSettings)
		Settings = Cast<UOrionGameUserSettings>(GEngine->GameUserSettings);

	bool bToggle = false;

	if (Settings->ToggleSprintEnabled == true)
		bToggle = true;*/

	if (ShouldIgnoreControls())
		return;

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
	if (ShouldIgnoreControls() && CurrentWeapon)
	{
		if (bKnockedDown)
		{
			CancelGrenadeTarget();
			CurrentWeapon->Melee();
		}
		else if (bLatchedOnto)
		{
			CurrentWeapon->Melee();
		}
		return;
	}

	if (bDowned)
		return;

	//no interupting grenade animation
	if (GetWorldTimerManager().IsTimerActive(GrenadeTimer))
		return;

	if (CurrentWeapon)
	{
		CancelGrenadeTarget();

		//if (!CheckForCompySlice())
		//	CurrentWeapon->Melee();
		GetWorldTimerManager().SetTimer(MeleeHoldTimer, this, &AOrionCharacter::UnMelee, 0.35f, false);
	}
}

//called when the melee button has been released
void AOrionCharacter::EndMelee()
{
	if (GetWorldTimerManager().IsTimerActive(MeleeHoldTimer))
	{
		if (CurrentWeapon)
			CurrentWeapon->Melee();

		GetWorldTimerManager().ClearTimer(MeleeHoldTimer);
	}
}

//called 0.35 seconds after pressing melee to see if the button is being held
//if it is being held, check for a valid fatality
void AOrionCharacter::UnMelee()
{
	if (CurrentWeapon && !CheckForCompySlice())
		CurrentWeapon->Melee();
}

void AOrionCharacter::ServerCompySlice_Implementation(AOrionCharacter *Target)
{
	PerformFatality(PlayerSliceAnim, CompySliceAnim, Target, false);
	FatalityVictim = Target;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && PC->GetStats())
		PC->GetStats()->AddStatValue(STAT_COMPYSLICE, 1);
}

bool AOrionCharacter::CheckForCompySlice()
{
	if (ShouldIgnoreControls())
		return false;

	//see if there are any compies in our slice range, and if there are, start a slicin
	FVector pos;
	pos = GetMesh()->GetSocketLocation("CompySlice");

	float radius = 100.0f;

	TArray<AActor*> Ignore;

	Ignore.AddUnique(this);

	TArray<AActor*> Result;

	TArray<TEnumAsByte<EObjectTypeQuery> > Obj;

	StopSprint();

	if (UKismetSystemLibrary::SphereOverlapActors_NEW(GetWorld(), pos, radius, Obj, AOrionDinoPawn::StaticClass(), Ignore, Result))
	{
		AOrionDinoPawn *Dino = Cast<AOrionDinoPawn>(Result[0]);

		if (Dino && Dino->Health > 0.0f && Dino->DinoName == "COMPY" && !Dino->bFatality)
		{
			if (Role < ROLE_Authority)
				ServerCompySlice(Dino);
			else
			{
				AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

				if (PC && PC->GetStats())
					PC->GetStats()->AddStatValue(STAT_COMPYSLICE, 1);

				PerformFatality(PlayerSliceAnim, CompySliceAnim, Dino, false);
			}

			FatalityVictim = Dino;

			return true;
		}
	}

	return false;
}

void AOrionCharacter::ResetStopSpecialMove()
{
	bStopSpecialMove = false;// true;
}

void AOrionCharacter::ServerSetReviveTarget_Implementation(AOrionCharacter *Target)
{
	//make sure it's valid
	if (!Target || !Target->IsValidLowLevel())
	{
		ReviveTarget = nullptr;
		return;
	}

	float dist = (GetActorLocation() - Target->GetActorLocation()).Size();

	if (dist < 200.0f)
		ReviveTarget = Target;
}

bool AOrionCharacter::CheckForTeammatesToRevive()
{
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (bDowned)
		return false;

	if (GRI)
	{
		for (int32 i = 0; i < GRI->PlayerList.Num(); i++)
		{
			if (!GRI->PlayerList[i] || !GRI->PlayerList[i]->IsValidLowLevel())
				continue;

			AOrionCharacter *Pawn = GRI->PlayerList[i]->ControlledPawn;

			if (Pawn && Pawn->bDowned)
			{
				float dist = (GetActorLocation() - Pawn->GetActorLocation()).Size();

				if (dist < 200.0f)
				{
					if (Role < ROLE_Authority)
						ServerSetReviveTarget(Pawn);
					else
						ReviveTarget = Pawn;
					return true;
				}
			}
		}
	}

	return false;
}

void AOrionCharacter::Reload()
{
	if (ShouldIgnoreControls())
		return;

	if (bThrowingGrenade)
		return;

	//no interupting grenade animation
	if (GetWorldTimerManager().IsTimerActive(GrenadeTimer))
		return;

	//see if there is anyone around us that we can revive
	if (CheckForTeammatesToRevive())
	{
		return;
	}

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
		if (Cast<AOrionWeaponLink>(CurrentWeapon) == nullptr)
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

	if (CurrentWeapon)
		Kick.Pitch *= CurrentWeapon->GetSpreadModifier();

	TargetAimKick.Pitch = FMath::Min(TargetAimKick.Pitch + (TargetAimKick.Pitch>2.0f ? Kick.Pitch - 1.0f : Kick.Pitch), 4.0f);
	TargetAimKick.Yaw = FMath::Clamp(TargetAimKick.Yaw + Kick.Yaw, -2.0f, 2.0f);
}

bool AOrionCharacter::IsTopDown()
{
	return false;
}

void AOrionCharacter::CancelGrenadeTarget()
{
	if (bTargetingGrenade)
	{
		bTargetingGrenade = false;
		if (GrenadeTarget)
			GrenadeTarget->SetActorHiddenInGame(true);
	}
}

void AOrionCharacter::StartAiming()
{
	if (ShouldIgnoreControls())
		return;

	if (bThrowingGrenade)
		return;

	if (bDowned)
		return;

	CancelGrenadeTarget();

	//no interupting grenade animation
	if (GetWorldTimerManager().IsTimerActive(GrenadeTimer))
		return;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && PC->bCinematicMode)
		return;

	if (Role < ROLE_Authority)
		ServerAim(true);

	StopSprint();

	if (CurrentWeapon)
	{
		if (Cast<AOrionWeaponLink>(CurrentWeapon) == nullptr)
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

	if (bThrowingGrenade)
		return;

	//no interupting grenade animation
	if (GetWorldTimerManager().IsTimerActive(GrenadeTimer))
		return;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && PC->bCinematicMode)
		return;

	if (CurrentSkill && CurrentSkill->bTargetting)
	{
		CurrentSkill->TriggerTarget(this);
		return;
	}

	StopSprint();

	if (bTargetingGrenade && GrenadeTarget)
	{
		GrenadeTarget->SetActorHiddenInGame(true);
		GrenadeTargetLocation = GrenadeTarget->GetActorLocation();
		ThrowGrenadeAtLocation();
		return;
	}

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
	if (ShouldIgnoreControls())
		return;

	if (Value != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// add movement in that direction
		AddMovementInput(Direction, Value);

		if (Value <= 0.1f && bRun)
			StopSprint();
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
	if (ShouldIgnoreControls())
		return;

	if (Value != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement in that direction
		AddMovementInput(Direction, Value);// (IsSprinting() ? 0.5f : 1.0f) * Value);
	}

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (IsSprinting() && PC && PC->bThirdPersonCamera)
	{
		FVector AimDirWS = GetVelocity();
		AimDirWS.Normalize();

		const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
		const FRotator AimRotLS = AimDirLS.Rotation();

		TargetYaw = FMath::Clamp(AimRotLS.Yaw, -45.0f, 45.0f);
	}
	else
		TargetYaw = 0.0f;

	ServerSetTargetYaw(TargetYaw);
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
		SetFlight1Mesh(nullptr);
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