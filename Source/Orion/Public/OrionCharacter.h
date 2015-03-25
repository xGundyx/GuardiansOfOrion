// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Orion.h"
#include "BehaviorTree/BehaviorTree.h"
//#include "OrionHoverVehicle.h"
#include "OrionCharacter.generated.h"

class AOrionHoverVehicle;
class AOrionFood;
class AOrionWeaponDroid;

USTRUCT()
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* Weapon1P;

	/** animation played on pawn (1st person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* Pawn1P;

	/** animation played on pawn (3rd person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* Pawn3P;

	/** animation played on the actual weapon model (mainly just reloads) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* Weapon3P;

	FWeaponAnim()
		: Weapon1P(NULL)
		, Weapon3P(NULL)
		, Pawn1P(NULL)
		, Pawn3P(NULL)
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
class AOrionCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	AOrionCharacter(const FObjectInitializer& ObjectInitializer);

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Arms1PMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Arms1PArmorMesh;

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

	void EatFood(AOrionFood *Food);

	/** [server] perform PlayerState related setup */
	virtual void PossessedBy(class AController* C) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	/** handle mesh visibility and updates */
	void UpdatePawnMeshes();

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
		bool bAim;

	UFUNCTION(Reliable, server, WithValidation)
		void ServerDuck(bool bNewDuck);
		bool ServerDuck_Validate(bool bNewDuck);
		void ServerDuck_Implementation(bool bNewDuck);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Camera)
		FVector CameraOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Camera)
		float CameraDist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Mesh)
		FName LeftFootSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Mesh)
		FName RightFootSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Mesh)
		FVector AimAjustment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Mesh)
		float MaxFootUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Mesh)
		float MaxFootDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Mesh)
		float MaxMeshAdjust;

	//foot placement
	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual float GetFootOffset(FName Socket) const;

	virtual class UPawnMovementComponent* GetMovementComponent() const override;

	//weapon fire effect helper, mainly only used for special weapon types with multiple flashes going off
	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual void HandleSpecialWeaponFire(FName SocketName);

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

	/** check if pawn is still alive */
	bool IsAlive() const;

	void DoRoll();
	void EndRoll();

	/** get mesh component */
	USkeletalMeshComponent* GetPawnMesh() const;

	void AddAimKick(FRotator Kick);
	void ResetAimKick();
	void ProcessAimKick(FRotator& OutViewRotation, FRotator& OutDeltaRot);

	void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation, const float FOV);

	//UPROPERTY(EditAnywhere, Category = Behavior)
	//class UBehaviorTree* BotBehavior;

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Change 1P Armor"))
		void EventUpdate1PArmor(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Change Helmet"))
		void EventUpdateHelmet(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Change Chest"))
		void EventUpdateBody(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Change Arms"))
		void EventUpdateArms(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Change Legs"))
		void EventUpdateLegs(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Change Flight1"))
		void EventUpdateFlight1(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Change Flight2"))
		void EventUpdateFlight2(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Set Armor Color"))
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

	float OrionPlayAnimMontage(const FWeaponAnim Animation, float InPlayRate = 1.0f, FName StartSectionName = FName(""));// override;
	void StopAnimMontage(class UAnimMontage* AnimMontage) override;

	UFUNCTION()
		void OnRep_ReplicatedAnimation();

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAnimation)
		FAnimInfo ReplicatedAnimation;

	UPROPERTY(Replicated)
		float AimYaw;

	UPROPERTY(Replicated)
		float AimPitch;

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
	UFUNCTION()
		void OnRep_CurrentWeapon(class AOrionWeapon* LastWeapon);

	/** currently equipped weapon */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class AOrionWeapon* CurrentWeapon;

	/** weapons in inventory */
	UPROPERTY(Transient, Replicated)
		TArray<class AOrionWeapon*> Inventory;

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

	void Sprint();
	void StopSprint();

	void Reload();

	void BehindView();

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

	/** updates current weapon */
	void SetCurrentWeapon(class AOrionWeapon* NewWeapon, class AOrionWeapon* LastWeapon = NULL);

	/**
	* [server + local] equips weapon from inventory
	*
	* @param Weapon	Weapon to equip
	*/
	void EquipWeapon(class AOrionWeapon* Weapon);

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


	bool bRolling;

	void ReallyDoEquip();
	AOrionWeapon *NextWeapon;
public:
	FVector CameraLocation;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

private:
	FTimerHandle EquipTimer;
	FTimerHandle UnEquipTimer;
	FTimerHandle RollTimer;
};

