// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionCharacter.h"
//#include "OrionTopDownPawn.h"
#include "OrionWeapon.generated.h"

//data to send to server/clients about our hits
USTRUCT()
struct FInstantHitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Origin;

	UPROPERTY()
		float ReticleSpread;

	UPROPERTY()
		int32 RandomSeed;

	UPROPERTY()
		FName Socket;
};

UENUM()
enum WeaponStates
{
	WEAP_IDLE,
	WEAP_RELOADING,
	WEAP_MELEE,
	WEAP_EQUIPPING,
	WEAP_PUTTINGDOWN
};

static inline bool NEQ(const FInstantHitInfo& A, const FInstantHitInfo& B, UPackageMap* Map, UActorChannel* Channel)
{
	return ((A.Origin - B.Origin).SizeSquared() > 0.4f) || (A.ReticleSpread != B.ReticleSpread) || (A.RandomSeed != B.RandomSeed);
}

USTRUCT()
struct FInstantWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
	float WeaponSpread;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float WeaponRange;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float NormalSpread;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float CrouchSpread;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float AimSpread;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float FiringSpreadIncrement;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float FiringSpreadMax;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float AllowedViewDotHitDir;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float ClientSideHitLeeway;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float HitDamage;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float MuzzleScale;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		int32 ClipSize;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		bool bAutomatic;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float WeaponScale;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float AimFOV;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float FOVTransitionTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Accuracy)
		int32 WeaponIndex;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		int32 WeaponSlot;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float RecoilScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Accuracy)
		FName AttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		bool bSingleShellReload;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		bool bBurst;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		int32 NumPellets;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		FVector AimAdjustment;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		FString WeaponName;

	//this is needed to make the left hand match up for third person animations
	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		FVector LeftHandOffset;
};

/**
*
*/
UCLASS()
class ORION_API AOrionWeapon : public AOrionInventory
{
	GENERATED_BODY()
public:
	AOrionWeapon(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
		FInstantWeaponData InstantConfig;

	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;

	virtual void Melee();

	UFUNCTION(BlueprintCallable, Category = AI)
		virtual float DoMelee();

	virtual void ResetMelee();
	virtual void CancelMelee();

	/** [local + server] start weapon fire */
	virtual void StartFire();

	/** [local + server] stop weapon fire */
	virtual void StopFire();

	/** [all] start weapon reload */
	virtual void StartReload(bool bFromReplication = false);

	UFUNCTION(BlueprintCallable, Category = Weapon)
		FString GetWeaponName() { return InstantConfig.WeaponName; }

	//mainly for shotties
	void ReloadNextShell();
	void StartShellReload();

	/** [local + server] interrupt weapon reload */
	virtual void StopReload();

	/** [server] performs actual reload */
	virtual void ReloadWeapon();

	/** trigger reload from server */
	UFUNCTION(reliable, client)
		void ClientStartReload();
		bool ClientStartReload_Validate();
		void ClientStartReload_Implementation();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
		bool bProjectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
		TSubclassOf<class AActor> ProjectileClass;

	/** play weapon animations */
	virtual float PlayWeaponAnimation(const FWeaponAnim& Animation, bool bReplicate, float Scale = 1.0f);

	/** stop playing weapon animations */
	virtual void StopWeaponAnimation(const FWeaponAnim& Animation);

	/** check if weapon can be reloaded */
	bool CanReload() const;

	FName SpecialSocketName;

	UPROPERTY(BlueprintReadOnly, Category = Skill)
		TArray<UMaterialInstanceDynamic*> WeaponMats;

	UPROPERTY(BlueprintReadOnly, Category = Skill)
		TArray<UMaterialInstanceDynamic*> WeaponCloakMats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skill)
		UMaterialInstance *CloakParent;

	void InitMaterials();

	/** reload animations */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim ReloadAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim ReloadEndAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim ReloadLoopAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim FireAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim AimFireAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim MeleeAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim MeleeKnockDownAnim;

	//UPROPERTY(EditDefaultsOnly, Category = Animation)
	//	FWeaponAnim SprintAnim;

	//UPROPERTY(EditDefaultsOnly, Category = Animation)
	//	FWeaponAnim IdleAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim EquipAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim HolsterAnim;

	UPROPERTY(EditDefaultsOnly, Category = Controller)
		UForceFeedbackEffect *FireRumbleEffect;

	//UPROPERTY(EditDefaultsOnly, Category = Animation)
	//	FWeaponAnim AimAnim;

	/** weapon is being equipped by owner pawn */
	virtual void OnEquip();

	/** weapon is now equipped by owner pawn */
	virtual void OnEquipFinished();

	/** weapon is holstered by owner pawn */
	virtual float OnUnEquip();

	virtual void OnUnEquipFinished();

	UFUNCTION()
		void OnRep_MyPawn();

	//UFUNCTION()
	//	void OnRep_Reload();

	/** set the weapon's owning pawn */
	void SetOwningPawn(AOrionCharacter* AOrionCharacter);

	/** Called in network play to do the cosmetic fx for firing */
	virtual void SimulateWeaponFire();

	float GetSpreadModifier();

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopSimulatingWeaponFire();

	/** attaches weapon mesh to pawn's mesh */
	virtual void AttachMeshToPawn();

	/** detaches weapon mesh from pawn */
	void DetachMeshFromPawn();

	/** get weapon mesh (needs pawn owner to determine variant) */
	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual USkeletalMeshComponent* GetWeaponMesh(bool bFirstPerson) const;

	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual FVector GetLeftHandOffset() const;

	UFUNCTION(reliable, server, WithValidation)
		void ServerMelee();
		bool ServerMelee_Validate();
		void ServerMelee_Implementation();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStartFire();
		bool ServerStartFire_Validate();
		void ServerStartFire_Implementation();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStopFire();
		bool ServerStopFire_Validate();
		void ServerStopFire_Implementation();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStartReload();
		bool ServerStartReload_Validate();
		void ServerStartReload_Implementation();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStopReload();
		bool ServerStopReload_Validate();
		void ServerStopReload_Implementation();

	/** instant hit notify for replication */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitNotify)
		FInstantHitInfo HitNotify;

	UFUNCTION()
		void OnRep_Knifing();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Knifing)
		bool bKnifing;

	/** server notified of hit from client to verify */
	UFUNCTION(reliable, server, WithValidation)
		void ServerNotifyHit(const FHitResult Impact, FVector ShootDir, int32 RandomSeed, float ReticleSpread);
		bool ServerNotifyHit_Validate(const FHitResult Impact, FVector ShootDir, int32 RandomSeed, float ReticleSpread);
		void ServerNotifyHit_Implementation(const FHitResult Impact, FVector ShootDir, int32 RandomSeed, float ReticleSpread);

	/** server notified of miss to show trail FX */
	UFUNCTION(unreliable, server, WithValidation)
		void ServerNotifyMiss(FVector ShootDir, int32 RandomSeed, float ReticleSpread);
		bool ServerNotifyMiss_Validate(FVector ShootDir, int32 RandomSeed, float ReticleSpread);
		void ServerNotifyMiss_Implementation(FVector ShootDir, int32 RandomSeed, float ReticleSpread);

	/** process the instant hit and notify the server if necessary */
	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	/** continue processing the instant hit, as if it has been confirmed by the server */
	void ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	/** check if weapon should deal damage to actor */
	//bool ShouldDealDamage(AActor* TestActor) const;

	/** handle damage */
	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

	void UseAmmo();

	bool CanReload();

	/** check if weapon can fire */
	bool CanFire() const;

	/** [server] fire & update ammo */
	UFUNCTION(reliable, server, WithValidation)
		void ServerHandleFiring();
		bool ServerHandleFiring_Validate();
		void ServerHandleFiring_Implementation();

	/** [local + server] handle weapon fire */
	void HandleFiring();

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon();
	virtual void FireBurst();

	void BreakTongue();

	UPROPERTY()
		float CurrentFiringSpread;

	UPROPERTY()//(Replicated)
		TEnumAsByte<WeaponStates> WeaponState;

	void CancelReload();

	UPROPERTY(Replicated)
		int32 AmmoInClip;

	UPROPERTY(Replicated)
		int32 Ammo;

	UPROPERTY()
		float LastFireTime;

	UPROPERTY()
		float LastFireSoundTime;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		TArray<FName> MuzzleAttachPoint;

	/** FX for muzzle flash */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		TArray<UParticleSystem*> MuzzleFX;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* LaserAimFX;

	/** spawned component for muzzle FX */
	UPROPERTY(Transient)
		TArray<UParticleSystemComponent*> MuzzlePSC;

	UPROPERTY(Transient)
		UParticleSystemComponent* LaserAimPSC;

	virtual void FireSpecial(FName SocketName, FVector Direction);

	virtual void FireProjectile(FName SocketName, FVector Direction);

	virtual FVector GetBarrelLocation(FName SocketName);

	FVector GetAdjustedAim() const;
	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const;
	float GetCurrentSpread() const;
	virtual FVector GetMuzzleLocation() const;
	virtual FVector GetMuzzleDirection() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
		int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
		int32 GetCurrentAmmoInClip() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
		int32 GetClipSize() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
		int32 GetMaxAmmo() const;

	UFUNCTION()
		void OnRep_HitNotify();

	/** called in network play to do the cosmetic fx  */
	void SimulateInstantHit(const FVector& Origin, int32 RandomSeed, float ReticleSpread, FName Socket);

	UPROPERTY(EditDefaultsOnly, Category = Aiming)
		bool bAiming;

	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual int32 GetWeaponIndex();

	UFUNCTION(BlueprintCallable, Category = Aiming)
		virtual bool IsAiming();

	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual AOrionCharacter* GetOrionPawn();

	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual UAnimMontage* GetIdleMontage();

	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual UAnimMontage* GetReloadMontage();

	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual UAnimMontage* GetMeleeMontage();

	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual UAnimMontage* GetEquipMontage();

	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual UAnimMontage* GetSprintMontage();

	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual UAnimMontage* GetAimMontage();

	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual UAnimMontage* GetFireMontage();

	virtual void StartAiming(bool bPlaySound = true);
	virtual void StopAiming();

	bool IsEquipped() const;

	uint32 bIsEquipped : 1;
	uint32 bPendingEquip : 1;
	uint32 bPendingReload : 1;
	float EquipStartedTime;
	float EquipDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Aiming)
		FVector ViewOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Aiming)
		FVector AimViewOffset;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* DrawSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* HolsterSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* ZoomInSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* ZoomOutSound;

	void InitClip();

	/** play weapon sounds */
	UAudioComponent* PlayWeaponSound(USoundCue* Sound);

	/** spawn effects for impact */
	void SpawnImpactEffects(const FHitResult& Impact);

	/** spawn trail effect */
	void SpawnTrailEffect(const FVector& EndPoint);

	/** smoke trail */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* TrailFX;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		TSubclassOf<class AOrionImpactEffect> ImpactTemplate;

	/** tracer */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* TracerFX;

	/** param name for beam target in smoke trail */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		FName TrailTargetParam;

	/** Update the character. (Running, health etc). */
	virtual void Tick(float DeltaSeconds) override;

	virtual void HandleZooms(float DeltaSeconds);
	virtual void HandleViewOffsets(float DeltaSeconds);

protected:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class AOrionCharacter* MyPawn;

	/** weapon mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		UOrionSkeletalMeshComponent* Mesh1P;

	/** weapon mesh: 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh3P;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* KnifeMesh;

	float TargetFOV;
	float TargetViewOffset;
	float LastAimTime;

protected:
	FTimerHandle ReloadTimer;
	FTimerHandle MeleeTimer;
	FTimerHandle ReloadStopTimer;
	FTimerHandle FireTimer;
	FTimerHandle EquipTimer;
	FTimerHandle BurstTimer;

	int32 BurstCounter;
};
