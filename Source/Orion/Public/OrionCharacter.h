// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Orion.h"
#include "OrionTypes.h"
#include "OrionArmor.h"
#include "BehaviorTree/BehaviorTree.h"
#include "OrionHealthBar.h"
#include "OrionGib.h"
#include "Perception/PawnSensingComponent.h"
//#include "AI/Navigation/NavigationInvokerComponent.h"
//#include "OrionHoverVehicle.h"
#include "OrionVoice.h"
#include "OrionCharacter.generated.h"

class AOrionHoverVehicle;
class AOrionFood;
class AOrionWeaponDroid;
class AOrionSquad;
class AOrionAIController;
class AOrionShipPawn;
class AOrionPlayerController;
class AOrionAbility;
class AOrionGrenade;
class AOrionBuff;

USTRUCT()
struct FVoiceReplication
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		bool bToggle;

	UPROPERTY()
		TEnumAsByte<EVoiceType> Type;
};

USTRUCT()
struct FBlinkHelper
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		FVector Start;

	UPROPERTY()
		FVector End;
};

USTRUCT()
struct FShipHelper
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		AOrionShipPawn *Ship;

	UPROPERTY()
		FString Socket;
};

USTRUCT()
struct FArmorHelper
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DELETEME)
		TSubclassOf<class AOrionArmor> HeadArmor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DELETEME)
		TSubclassOf<class AOrionArmor> BodyArmor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DELETEME)
		TSubclassOf<class AOrionArmor> LegsArmor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DELETEME)
		TSubclassOf<class AOrionArmor> ArmsArmor;
};

USTRUCT()
struct FGibReplicate
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		int32 Index;

	UPROPERTY()
		FName Socket;

	UPROPERTY()
		FVector Velocity;
};

USTRUCT()
struct FFootStepSound
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gibs)
		USoundCue *ConcreteSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gibs)
		USoundCue *GravelSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gibs)
		USoundCue *MetalSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gibs)
		USoundCue *MudSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gibs)
		USoundCue *WaterSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gibs)
		USoundCue *WoodSound;

	FFootStepSound()
	{
		ConcreteSound = nullptr;
		GravelSound = nullptr;
		MetalSound = nullptr;
		MudSound = nullptr;
		WaterSound = nullptr;
		WoodSound = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FGibHelper
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gibs)
		TSubclassOf<class AOrionGib> Gib;

	UPROPERTY()
		bool bActivated;

	float TimeTillBloodDecal;

	FGibHelper()
	{
		TimeTillBloodDecal = -1.0f;
		bActivated = false;
	}

	bool operator==(const FGibHelper Other) const { return Gib == Other.Gib; }
};

//this holds a player's character stats, it gets updated whenever gear changes or level up
USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		FPrimaryStats PrimaryStats;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		FSecondaryStats SecondaryStats;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		FBonusStats BonusStats;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		TArray<FRareStatsInfo> RareStats;
};

USTRUCT(BlueprintType)
struct FFatalityAnim
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* AttackerAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* VictimAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		AOrionCharacter *Victim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		bool bToggle;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		bool bRemove;
};

USTRUCT(BlueprintType)
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* Weapon1P;

	/** animation played on pawn (1st person view) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* Pawn1P;

	/** animation played on pawn (3rd person view) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* Pawn3P;

	/** animation played on the actual weapon model (mainly just reloads) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* Weapon3P;

	bool bHideWeapon;

	FWeaponAnim()
		: Weapon1P(NULL)
		, Weapon3P(NULL)
		, Pawn1P(NULL)
		, Pawn3P(NULL)
		, bHideWeapon(false)
	{}
};

USTRUCT()
struct FAnimInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		class UAnimMontage *Weapon1PMontage;
	UPROPERTY()
		class UAnimMontage *Weapon3PMontage;
	UPROPERTY()
		class UAnimMontage *Mesh1PMontage;
	UPROPERTY()
		class UAnimMontage *Mesh3PMontage;
	UPROPERTY()
		float Rate;
	UPROPERTY()
		FName SectionName;
	UPROPERTY()
		bool bToggle;
	UPROPERTY()
		bool bReplicatedToOwner;
	UPROPERTY()
		bool bStopAllOtherAnims;
	UPROPERTY()
		bool bHideWeapon;
};

USTRUCT()
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

	/** The amount of damage actually applied */
	UPROPERTY()
	float ActualDamage;

	/** The damage type we were hit with. */
	UPROPERTY()
		UClass* DamageTypeClass;

	/** Who hit us */
	UPROPERTY()
		class AOrionCharacter* PawnInstigator;

	/** Who actually caused the damage */
	UPROPERTY()
		class AActor* DamageCauser;

	/** Specifies which DamageEvent below describes the damage received. */
	UPROPERTY()
		int32 DamageEventClassID;

	/** Rather this was a kill */
	UPROPERTY()
		uint32 bKilled : 1;

private:

	/** A rolling counter used to ensure the struct is dirty and will replicate. */
	UPROPERTY()
		uint8 EnsureReplicationByte;

	/** Describes general damage. */
	UPROPERTY()
		FDamageEvent GeneralDamageEvent;

	/** Describes point damage, if that is what was received. */
	UPROPERTY()
		FPointDamageEvent PointDamageEvent;

	/** Describes radial damage, if that is what was received. */
	UPROPERTY()
		FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo()
		: ActualDamage(0)
		, DamageTypeClass(NULL)
		, PawnInstigator(NULL)
		, DamageCauser(NULL)
		, DamageEventClassID(0)
		, bKilled(false)
		, EnsureReplicationByte(0)
	{}

	FDamageEvent& GetDamageEvent()
	{
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			if (PointDamageEvent.DamageTypeClass == NULL)
			{
				PointDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return PointDamageEvent;

		case FRadialDamageEvent::ClassID:
			if (RadialDamageEvent.DamageTypeClass == NULL)
			{
				RadialDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return RadialDamageEvent;

		default:
			if (GeneralDamageEvent.DamageTypeClass == NULL)
			{
				GeneralDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return GeneralDamageEvent;
		}
	}

	void SetDamageEvent(const FDamageEvent& DamageEvent)
	{
		DamageEventClassID = DamageEvent.GetTypeID();
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			PointDamageEvent = *((FPointDamageEvent const*)(&DamageEvent));
			break;
		case FRadialDamageEvent::ClassID:
			RadialDamageEvent = *((FRadialDamageEvent const*)(&DamageEvent));
			break;
		default:
			GeneralDamageEvent = DamageEvent;
		}

		DamageTypeClass = DamageEvent.DamageTypeClass;
	}

	void EnsureReplication()
	{
		EnsureReplicationByte++;
	}
};

USTRUCT()
struct FDirectionalAnim
{
	GENERATED_USTRUCT_BODY()

	/** animation played on pawn (1st person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Forwards;

	/** animation played on pawn (3rd person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* Backwards;

	/** animation played on pawn (1st person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* Left;

	/** animation played on pawn (3rd person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* Right;
};

UCLASS(BlueprintType, Blueprintable, config = Game)
class ORION_API AOrionCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	AOrionCharacter(const FObjectInitializer& ObjectInitializer);

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class UOrionSkeletalMeshComponent* Arms1PMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class UOrionSkeletalMeshComponent* Arms1PArmorMesh;

	//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	//	UNavigationInvokerComponent* InvokerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<class UUserWidget>  DefaultHealthBarClass;

	UPROPERTY(BlueprintReadOnly, Category = Health)
		bool bIsHealableMachine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health)
		bool bCanAttackGenerator;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Generator)
		bool bPowered;
	
	UFUNCTION()
		void OnRep_Spawned();

	UPROPERTY(ReplicatedUsing = OnRep_Spawned)
		bool bNotSpawnedYet;

	UPROPERTY(Replicated)
		bool bSelfHealing;

	UPROPERTY(Replicated)
		float HealTarget;

	float HealAmount;
	float LastHealTime;

	int32 SpawnType;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = AI)
		int32 OutOfBoundsCounter;

	void HandleHealEffects(float DeltaSeconds);

	//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		//class USkeletalMeshComponent* Arms1PLegsMesh;

	/** spawn inventory, setup initial variables */
	virtual void PostInitializeComponents() override;

	/** Update the character. (Running, health etc). */
	virtual void Tick(float DeltaSeconds) override;

	/** Take damage, handle death */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	/** cleanup inventory */
	virtual void Destroyed() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
		uint32 bIsDying : 1;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		class AOrionWeapon* GetWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void Set1PArmorMesh(USkeletalMesh* newMesh) const;

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void Set1PLegsMesh(USkeletalMesh* newMesh) const;

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void SetHelmetMesh(USkeletalMesh* newMesh) const;

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void SetBodyMesh(USkeletalMesh* newMesh) const;

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void SetArmsMesh(USkeletalMesh* newMesh) const;

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void SetLegsMesh(USkeletalMesh* newMesh) const;

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void SetFlight1Mesh(USkeletalMesh* newMesh) const;

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void SetFlight2Mesh(USkeletalMesh* newMesh) const;

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void TakeOff();
	
	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void LandFromFlying();

	void EatFood(AOrionFood *Food);

	/** [server] perform PlayerState related setup */
	virtual void PossessedBy(class AController* C) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	/** handle mesh visibility and updates */
	virtual void UpdatePawnMeshes();

	virtual bool IsTopDown();

	int32 CameraIndex;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
		TEnumAsByte<EAIType> AIType;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
		bool bIsHiddenFromView;

	void SetAIType(EAIType type) { AIType = type; }

	bool bThrowingGrenade;

	virtual void DoGrenade();
	virtual void TossGrenade();
	virtual void ThrowGrenadeAtLocation();
	virtual void UpdateGrenadeTarget(float DeltaSeconds) {}
	virtual void CancelGrenadeTarget();

	FVector GrenadeTargetLocation;

	//press G to bring up target, and fire to throw at the target
	bool bTargetingGrenade;
	ADecalActor *GrenadeTarget;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class ADecalActor> GrenadeTargetClass;

	UFUNCTION(Reliable, server, WithValidation)
		void ServerTossGrenade(FVector dir);
		bool ServerTossGrenade_Validate(FVector dir);
		void ServerTossGrenade_Implementation(FVector dir);

	void ActuallyTossGrenade(FVector dir);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Grenade)
		FCooldownAbility GrenadeCooldown;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Grenade)
		FCooldownAbility BlinkCooldown;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Grenade)
		float RollCooldown;

	void UpdateCooldowns(float DeltaTime);

	void EquipArmor(AOrionArmor *Armor);
	void UnEquipArmor(EItemType Slot);

	//set the actual armor and not just the mesh
	void SetClassArmor(int32 Index);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DELETEME)
		TArray<FArmorHelper> ArmorList;

	UFUNCTION()
		void OnRep_HelmetArmor();
	UFUNCTION()
		void OnRep_BodyArmor();
	UFUNCTION()
		void OnRep_ArmsArmor();
	UFUNCTION()
		void OnRep_LegsArmor();

	UPROPERTY(ReplicatedUsing = OnRep_HelmetArmor)
		AOrionArmor *HelmetArmor;
	UPROPERTY(ReplicatedUsing = OnRep_BodyArmor)
		AOrionArmor *BodyArmor;
	UPROPERTY(ReplicatedUsing = OnRep_ArmsArmor)
		AOrionArmor *ArmsArmor;
	UPROPERTY(ReplicatedUsing = OnRep_LegsArmor)
		AOrionArmor *LegsArmor;

	//ring mesh under the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class UStaticMeshComponent* RingMesh;

	UPROPERTY()
		UMaterialInstanceDynamic *RingMat;

	void UpdatePlayerRingColor();

	//for recharging shields
	float LastTakeHitTime;

	UFUNCTION(BlueprintCallable, Category = AI)
		void PerformFatality(UAnimMontage *Anim, UAnimMontage *EnemyAnim, AOrionCharacter *TheVictim, bool bHideOnFatality);

	UFUNCTION()
		void OnRep_Fatality();

	UPROPERTY(ReplicatedUsing = OnRep_Fatality)
		FFatalityAnim FatalityAnim;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Fatality)
		bool bFatality;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Fatality)
		bool bFatalityRemove;

	bool bFinishingMove;

	//pointer to the enemy that is finishing us (mainly for camera work)
	AOrionCharacter *Finisher;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class UParticleSystemComponent* ShieldFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class UParticleSystemComponent* EliteFX;

	void ShowEliteFX(bool bShow);

	UFUNCTION()
		void OnRep_IsElite();

	void BeginPlay() override;

	//modular pieces
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* HelmetMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* ArmsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* LegsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* Flight1Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* Flight2Mesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* FirstPersonCameraComponent;

	/** Third person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* ThirdPersonCameraComponent;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		bool bFirstPerson;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		bool bDuck;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		bool bRun;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		bool bFly;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		bool bLanding;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		bool bAim;

	UFUNCTION(Reliable, server, WithValidation)
		void ServerDuck(bool bNewDuck);
		bool ServerDuck_Validate(bool bNewDuck);
		void ServerDuck_Implementation(bool bNewDuck);

	UFUNCTION(Reliable, server, WithValidation)
		void ServerDoRoll(ERollDir rDir, FRotator rRot);
		bool ServerDoRoll_Validate(ERollDir rDir, FRotator rRot);
		void ServerDoRoll_Implementation(ERollDir rDir, FRotator rRot);

	UFUNCTION(Reliable, server, WithValidation)
		void ServerRun(bool bNewRun);
		bool ServerRun_Validate(bool bNewRun);
		void ServerRun_Implementation(bool bNewRun);

	UFUNCTION(Reliable, server, WithValidation)
		void ServerAim(bool bNewAim);
		bool ServerAim_Validate(bool bNewAim);
		void ServerAim_Implementation(bool bNewAim);

	UFUNCTION()
		void OnRep_ArmorIndex();

	UFUNCTION()
		void SetArmor(int32 index);

	UPROPERTY(ReplicatedUsing = OnRep_ArmorIndex)
		int32 ArmorIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aim)
		FVector2D CurrentAim2D;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AOrionProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* FireSound;

	/** [server] spawns default inventory */
	void SpawnDefaultInventory();

	/** [server] remove all weapons from inventory and destroy them */
	void DestroyInventory();

	UFUNCTION(BlueprintCallable, Category = GamePlay)
		bool IsOnShip();

	/**
	* [server] add weapon to inventory
	*
	* @param Weapon	Weapon to add.
	*/
	void AddWeapon(class AOrionWeapon* Weapon);

	/**
	* [server] remove weapon from inventory
	*
	* @param Weapon	Weapon to remove.
	*/
	void RemoveWeapon(class AOrionWeapon* Weapon);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* DeathAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* GrenadeAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* ExitShipAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* LandAnim;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FDirectionalAnim RollAnimation;

	void SetDrivenVehicle(AOrionHoverVehicle *newVehicle);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
		float HealthMax;

	UFUNCTION(BlueprintCallable, Category = Health)
		void AddHealth(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = Health)
		void AddShield(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = RPG)
		void AddExp(int32 Amount);

	UPROPERTY(EditDefaultsOnly, Category = Spawn)
		TSubclassOf<class UNavigationQueryFilter> DefaultFilterClass;

	//how much base xp to award for killing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RPG)
		int32 ExpValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
		float Shield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
		float ShieldMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Camera)
		FVector CameraOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Camera)
		float CameraDist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Mesh)
		FName LeftFootSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Mesh)
		FName RightFootSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Grenade)
		FName GrenadeSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Mesh)
		FVector AimAjustment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Mesh)
		float MaxFootUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Mesh)
		float MaxFootDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Mesh)
		float MaxMeshAdjust;

	//every enemy type can have it's own loottable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Loot)
		TSubclassOf<class UOrionInventoryList> LootTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = RPG)
		int32 Level;

	//foot placement
	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual float GetFootOffset(FName Socket) const;

	virtual class UPawnMovementComponent* GetMovementComponent() const override;

	//weapon fire effect helper, mainly only used for special weapon types with multiple flashes going off
	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual void HandleSpecialWeaponFire(FName SocketName);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio)
		FFootStepSound FootStepSounds;

	UFUNCTION(BlueprintCallable, Category = Audio)
		void PlayFootStepSound(USoundCue *Cue, FVector pos);

	float LastFootStepSound;

	/** get camera view type */
	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual bool IsFirstPerson() const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual bool IsRolling() const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual bool IsDucking() const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual bool IsSprinting() const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual bool IsFlying() const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual bool IsAiming() const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual int32 GetWeaponIndex() const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual FRotator GetRootRotation() const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual bool ShouldPlayWeaponAnim() const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		USkeletalMeshComponent* GetSpecifcPawnMesh(bool WantFirstPerson) const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual FVector2D GetAim(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual float PlayOneShotAnimation(UAnimMontage *Anim);

	//animation helpers
	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual UBlendSpace* GetAnimationWalkBlend() const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual UBlendSpace* GetAnimationHoverBlend() const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual UBlendSpace* GetAnimationAimBlend() const;
	
	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual UAnimMontage* GetAnimationAimBase() const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual UAnimMontage* GetSprintAnimation() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UBlendSpace *WalkBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UBlendSpace *HoverBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UBlendSpace *AimBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *AimBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *SprintAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float RotationRate;

	/** check if pawn is still alive */
	bool IsAlive() const;

	void DoRoll();
	void EndRoll();

	void TryToBlink();
	void Blink(FVector dir);
	void EndBlink();
	void ActuallyTeleport();

	void DoBlinkEffect(bool bOn, FVector pos);

	UFUNCTION(reliable, server, WithValidation)
		void ServerBlink(FVector Pos);
		bool ServerBlink_Validate(FVector Pos);
		void ServerBlink_Implementation(FVector Pos);

	//UPROPERTY(Replicated)
		bool bBlinking;

	float LastTeleportTime;
	FVector TeleportStartPos;

	UFUNCTION()
		void OnRep_Teleport();

	UPROPERTY(ReplicatedUsing = OnRep_Teleport)
		FBlinkHelper BlinkPos;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
		UParticleSystem* BlinkFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
		USoundCue *BlinkStartSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
		USoundCue *BlinkStopSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
		USoundCue *KnifeHitSound;

	//rotate to face target even if standing still, good for flying enemies
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Rotation)
		bool bAlwaysRotate;

	/** get mesh component */
	USkeletalMeshComponent* GetPawnMesh() const;

	UFUNCTION(BlueprintCallable, Category = Health)
		void Heal(int32 Amount);

	UFUNCTION()
		void OnRep_Buffs();

	void UpdateBuffFX();

	UPROPERTY(EditDefaultsOnly, Category = Buff)
		class UParticleSystemComponent* BuffFX;

	TArray<AOrionPlayerController*> Assisters;

	UPROPERTY(ReplicatedUsing = OnRep_Buffs, BlueprintReadWrite, Category = Buffs)
		TArray<AOrionBuff*> Buffs;

	UFUNCTION(BlueprintCallable, Category = Buffs)
		void AddBuff(TSubclassOf<AOrionBuff> BuffClass, AController *cOwner, int32 TeamIndex);

	void HandleBuffs(float DeltaSeconds);

	void EquipWeapon(class AOrionWeapon* Weapon);
	AOrionWeapon *GetWeaponFromType(EItemType Type);
	void CheckWeaponEquipped();

	UFUNCTION(reliable, client, Category = Inventory)
		void ClientEquipWeapon(class AOrionWeapon* Weapon);

	UPROPERTY(BlueprintReadOnly, Category = Gibs)
		float LeftLegScale;

	UPROPERTY(BlueprintReadOnly, Category = Gibs)
		float RightLegScale;

	UPROPERTY(BlueprintReadOnly, Category = Gibs)
		float LeftArmScale;

	UPROPERTY(BlueprintReadOnly, Category = Gibs)
		float RightArmScale;

	UPROPERTY(BlueprintReadOnly, Category = Gibs)
		float HeadScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gibs)
		TArray<FGibHelper> Gibs;

	UFUNCTION()
		void OnRep_GibAll();

	void GibAll(FVector Center, AOrionPlayerController *PC);

	UPROPERTY(ReplicatedUsing = OnRep_GibAll)
		FVector GibCenter;

	void HandleGibs(float damage, FDamageEvent const& DamageEvent, AOrionPlayerController *PC);

	//UFUNCTION(unreliable, client, Category = Gibs)
	void SpawnGibs(int32 index, FVector pos, FRotator rot, FVector dir);

	UPROPERTY(EditDefaultsOnly, Category = Gibs)
		UParticleSystem* BloodSpurtFX;

	UPROPERTY(EditDefaultsOnly, Category = Gibs)
		UParticleSystem* OrbFX;

	UPROPERTY(EditDefaultsOnly, Category = Gibs)
		UMaterialInstanceConstant* BloodDecal;

	UFUNCTION()
		void OnRep_Gibs();

	UPROPERTY(ReplicatedUsing = OnRep_Gibs)
		FGibReplicate GibRep;

	TArray<FDecalHelper> BloodDecals;
	
	void UpdateBloodDecals(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = Skill)
		bool IsOvercharging() const;

	void AddAimKick(FRotator Kick);
	void ResetAimKick();
	void ProcessAimKick(FRotator& OutViewRotation, FRotator& OutDeltaRot);

	void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation, const float FOV);

	//UPROPERTY(EditAnywhere, Category = Behavior)
	//class UBehaviorTree* BotBehavior;

	float TotalDamageReceived;
	bool bDirectGrenadeHit;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "StartTrailFX"))
		void EventStartTrailFX();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "StopTrailFX"))
		void EventStopTrailFX();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Change 1P Armor"))
		void EventUpdate1PArmor(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Change Helmet"))
		void EventUpdateHelmet(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Change Chest"))
		void EventUpdateBody(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Change Arms"))
		void EventUpdateArms(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Change Legs"))
		void EventUpdateLegs(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Change Flight1"))
		void EventUpdateFlight1(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Change Flight2"))
		void EventUpdateFlight2(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Set Armor Color"))
		void EventUpdateArmorColor(int32 index);

	UFUNCTION(BlueprintCallable, Category = Mesh)
		USkeletalMeshComponent* GetMeshFromIndex(int32 index) const;

	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		UBehaviorTree *AITree;

	virtual void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) override;

	UFUNCTION(BlueprintCallable, Category = Controller)
		APlayerController *GetPlayerController();

	UFUNCTION(BlueprintCallable, Category = Animation)
		float OrionPlayAnimMontage(const FWeaponAnim Animation, float InPlayRate = 1.0f, FName StartSectionName = FName(""), bool bShouldReplicate = true, bool bReplicateToOwner = false, bool bStopOtherAnims = false);// override;

	void StopAnimMontage(class UAnimMontage* AnimMontage) override;

	UFUNCTION(server, reliable, WithValidation)
		void ServerPlayAnimMontage(const FWeaponAnim Animation, float InPlayRate = 1.0f, FName StartSectionName = FName(""), bool bShouldReplicate = true, bool bReplicateToOwner = false, bool bStopOtherAnims = false);
		bool ServerPlayAnimMontage_Validate(const FWeaponAnim Animation, float InPlayRate = 1.0f, FName StartSectionName = FName(""), bool bShouldReplicate = true, bool bReplicateToOwner = false, bool bStopOtherAnims = false);
		void ServerPlayAnimMontage_Implementation(const FWeaponAnim Animation, float InPlayRate = 1.0f, FName StartSectionName = FName(""), bool bShouldReplicate = true, bool bReplicateToOwner = false, bool bStopOtherAnims = false);

	UFUNCTION()
		void OnRep_ReplicatedAnimation();

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAnimation)
		FAnimInfo ReplicatedAnimation;

	UPROPERTY(Replicated)
		float AimYaw;

	UPROPERTY(Replicated)
		float AimPitch;

	UFUNCTION(BlueprintCallable, Category = Stats)
		FCharacterStats GetCharacterStats();

	UPROPERTY(BlueprintReadOnly, Category = AI)
		AActor *FocusActor;

	void EnterVehicle();
	void ExitVehicle();

private:
	UPROPERTY(Replicated)
		AOrionHoverVehicle *DrivenVehicle;

	void AttachDriver();
	void DetachDriver();

protected:
	FRotator AimKick;
	FRotator TargetAimKick;
	float LastTakeHitTimeTimeout;

	float LastAimKickTime;

	FVector GetPawnViewLocation() const override;

	void UpdateAimKick(float DeltaSeconds);

	void StopAllAnimMontages();

	void SetRagdollPhysics();

	void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	UFUNCTION()
		void OnRep_LastTakeHitInfo();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	void ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser, bool bKilled);

	/** current weapon rep handler */
	//UFUNCTION()
	//	void OnRep_CurrentWeapon(class AOrionWeapon* LastWeapon);

	/** currently equipped weapon */
	UPROPERTY(Transient)//, ReplicatedUsing = OnRep_CurrentWeapon)
	class AOrionWeapon* CurrentWeapon;

	UFUNCTION()
		void OnRep_Inventory();

	/** weapons in inventory */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Inventory)
		TArray<class AOrionWeapon*> Inventory;

public:
	/** Handler for a touch input beginning. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	void OnFire();
	void OnStopFire();

	void OnNextWeapon();
	void OnPrevWeapon();

	void StartAiming();
	void StopAiming();

	void DoMelee();

	void Duck();
	void UnDuck();
	void StopDucking();

	void GamepadSprint();
	void Sprint();
	void StopSprint();
	void ButtonStopSprint();

	//hax for when fatality breaks
	void MakeSureDead();

	void Reload();

	void BehindView();

	UPROPERTY(ReplicatedUsing = OnRep_IsElite, BlueprintReadOnly, Category = AI)
		bool bIsElite;

	//set this to true in order to stop a special finishing move, like the jeckyl tongue
	UPROPERTY(BlueprintReadWrite, Category = AI)
		bool bStopSpecialMove;

	void ResetStopSpecialMove();

	UPROPERTY(Replicated)
		FVector_NetQuantize/*FVector*/ AimPos;

	UFUNCTION(server, reliable, WithValidation)
		void ServerActivateSkill();
		bool ServerActivateSkill_Validate();
		void ServerActivateSkill_Implementation();

	//called by the client to try and use a skill
	void TryToActivateSkill();

	//called by authority to activate a skill
	void ActivateSkill();

	void SpawnDefaultAbilities();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Voice)
		TSubclassOf<AOrionVoice> VoiceClass;

	UFUNCTION(BlueprintCallable, Category = Voice)
		void PlayVoice(EVoiceType Type);

	UFUNCTION(BlueprintCallable, Category = Voice)
		void PlayHarvesterVoice(EVoiceType Type);

	UFUNCTION()
		void OnRep_VoiceType();

	UPROPERTY(ReplicatedUsing = OnRep_VoiceType)
		FVoiceReplication VoiceRep;

	float LastVoiceTime;

	//array for future use if we want multiple abilities on one character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skill)
		TArray<TSubclassOf<class AOrionAbility> > AbilityClasses;

	//our current ability
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Skill)
		AOrionAbility *CurrentSkill;

	//mainly for handling cloaking
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Skill)
		AOrionAbility *CurrentSecondarySkill;

	UFUNCTION(BlueprintCallable, Category = Skill)
		void SetAbility(AOrionAbility *NewSkill);

	UFUNCTION(BlueprintCallable, Category = Skill)
		void SetSecondaryAbility(AOrionAbility *NewSkill);

	UFUNCTION(BlueprintCallable, Category = Skill)
		void SetTeamCloaking();

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Skill)
		bool bLatchedOnto;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Skill)
		bool bKnockedDown;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Skill)
		AOrionCharacter *Knocker;

	void HandleKnockedDown();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "GetUp"))
		void EventGetUp();

	//timestamp of last time a rham tried to dive at us
	UPROPERTY(BlueprintReadWrite, Category = AI)
		float LastRhamAttackTime;

	UPROPERTY(BlueprintReadWrite, Category = AI)
		float LastRaptorJumpTime;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Skill)
		AOrionCharacter *Latcher;

	UPROPERTY(BlueprintReadOnly, Category = Skill)
		TArray<UMaterialInstanceDynamic*> CharacterMats;

	UPROPERTY(BlueprintReadOnly, Category = Skill)
		TArray<UMaterialInstanceDynamic*> CharacterCloakMats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skill)
		UMaterialInstance *CloakParent;

	virtual void InitMaterials();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Awareness)
		 UPawnSensingComponent *PawnSensor;

	UFUNCTION()
		void OnHearNoise(APawn *HeardPawn, const FVector &Location, float Volume);

	UFUNCTION()
		void OnSeePawn(APawn *SeenPawn);

	UFUNCTION(server, unreliable, WithValidation)
		void ServerSetAimPos(FVector pos);
		bool ServerSetAimPos_Validate(FVector pos);
		void ServerSetAimPos_Implementation(FVector pos);

	UFUNCTION(server, reliable, WithValidation)
		void ServerSetReviveTarget(AOrionCharacter *Target);
		bool ServerSetReviveTarget_Validate(AOrionCharacter *Target) { return true; }
		void ServerSetReviveTarget_Implementation(AOrionCharacter *Target);

	bool ShouldIgnoreControls();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = AI)
		bool bIsBigDino;

	UPROPERTY(BlueprintReadWrite, Category = Projectile)
		class AOrionProjectile *SpecialProjectile;

	UPROPERTY(BlueprintReadWrite, Category = AI)
		FVector RockTarget;

	// generic use keybind
	void Use();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	/** default inventory list */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class AOrionWeapon> > DefaultInventoryClasses;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class AOrionWeapon> > DefaultAssaultInventoryClasses;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class AOrionWeapon> > DefaultSupportInventoryClasses;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class AOrionWeapon> > DefaultReconInventoryClasses;

	void SpawnClassWeapons(int32 ClassIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Inventory)
		TSubclassOf<class AOrionGrenade> GrenadeClass;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		AOrionGrenade* Grenade;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI)
		float FlyingOffset;

	/** updates current weapon */
	void SetCurrentWeapon(class AOrionWeapon* NewWeapon, class AOrionWeapon* LastWeapon = NULL);

	/**
	* [server + local] equips weapon from inventory
	*
	* @param Weapon	Weapon to equip
	*/

	void UpdateRootYaw(float DeltaSeconds);

	void ResetRootRotation();

	virtual void FaceRotation(FRotator NewControlRotation, float DeltaTime = 0.f) override;

	/** equip weapon */
	UFUNCTION(reliable, server, WithValidation)
		void ServerEquipWeapon(class AOrionWeapon* NewWeapon);
		bool ServerEquipWeapon_Validate(class AOrionWeapon* NewWeapon);
		void ServerEquipWeapon_Implementation(class AOrionWeapon* NewWeapon);

	UFUNCTION(reliable, server, WithValidation)
		void ServerUse();
		bool ServerUse_Validate();
		void ServerUse_Implementation();

	float BobTime;
	FVector WalkBob;
	FRotator RootRotation;

	UPROPERTY(Replicated)
		float TargetYaw;

	UFUNCTION(reliable, server, WithValidation)
		void ServerSetAimYaw(float yaw, float pitch);
		bool ServerSetAimYaw_Validate(float yaw, float pitch);
		void ServerSetAimYaw_Implementation(float yaw, float pitch);

	UPROPERTY(Replicated)
		bool bRolling;

	//TODO : replicate this!
	TEnumAsByte<ERollDir> RollDir;

	void ReallyDoEquip();
	
	/** current weapon rep handler */
	UFUNCTION()
		void OnRep_NextWeapon();

	bool bController;

	UFUNCTION()
		void OnRep_Downed();

	//go into a crawling state when health reaches 0 for 30 seconds, or someone revives you to full health
	UPROPERTY(ReplicatedUsing = OnRep_Downed, BlueprintReadWrite, Category = Speed)
		bool bDowned;

	float LastRevivedTime;
	bool CheckForTeammatesToRevive();

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Speed)
		int32 DownedTime;

	FTimerHandle DownedTimer;
	void TickDownedTime();

	FDamageEvent DownedDamageEvent;
	AController* DownedEventInstigator;
	AActor* DownedDamageCauser;

	bool CanBeDowned(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);

	virtual void EquipWeaponFromSlot(int32 index) {}

	//speed related boosts
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Speed)
		float SprintRate;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Speed)
		float AimRate;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Speed)
		float CloakRate;

	bool HasOrbEffect(EOrbType Type);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Spawn)
		bool bShoulderCamera;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Spawn)
		bool bShipCamera;

	int32 TimesDowned;

	UPROPERTY(Replicated)
		AOrionCharacter *ReviveTarget;

	void HandleRevivingTeammates(float DeltaSeconds);
	void Revived();

	/** currently equipped weapon */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_NextWeapon)
	class AOrionWeapon* NextWeapon;

protected:
	UFUNCTION()
		void OnRep_ShipPawn();

	UPROPERTY(ReplicatedUsing = OnRep_ShipPawn)
		FShipHelper CurrentShip;

	UPROPERTY(Replicated)
		AOrionShipPawn *CameraShip;

	virtual void AttachToShip();
	virtual void DetachFromShip();

	FTimerHandle ExitShipTimer;

public:
	bool SetShip(AOrionPlayerController *PC, AOrionShipPawn *Ship);
	void FinishExitingShip();
	void EnableControl();

	FVector CameraLocation;

	AOrionSquad *Squad;

	bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

	UFUNCTION()
		void OnRep_Female();

	UPROPERTY(ReplicatedUsing = OnRep_Female, EditDefaultsOnly, BlueprintReadOnly, Category = RPG)
		bool bFemale;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Change Base Mesh"))
		void EventSetFemaleMesh();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

private:
	FTimerHandle EquipTimer;
	FTimerHandle UnEquipTimer;
	FTimerHandle RollTimer;
	FTimerHandle RollTimer2;
	FTimerHandle GrenadeTimer;
	FTimerHandle GrenadeHideTimer;

public:
	void UnhideWeapon();

	FTimerHandle TeleportTimer;

	FCharacterStats CharacterStats;

	//reference to health bar for us
	UPROPERTY(BlueprintReadWrite, Category = HUD)
		UUserWidget *MyHealthBar;

	void CreateHealthBar();
};

