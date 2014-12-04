// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionCharacter.h"
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
};

UENUM()
enum WeaponStates
{
	WEAP_IDLE,
	WEAP_RELOADING,
	WEAP_MELEE,
	WEAP_EQUIPPING
};

static inline bool NEQ(const FInstantHitInfo& A, const FInstantHitInfo& B, UPackageMap* Map, UActorChannel* Channel)
{
	return ((A.Origin - B.Origin).SizeSquared() > 0.4f) || (A.ReticleSpread != B.ReticleSpread) || (A.RandomSeed != B.RandomSeed);
}

USTRUCT()
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()

	/** animation played on pawn (1st person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Pawn1P;

	/** animation played on pawn (3rd person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* Pawn3P;
};

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

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		int32 WeaponIndex;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float RecoilScale;
};

/**
*
*/
UCLASS()
class AOrionWeapon : public AOrionInventory
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FInstantWeaponData InstantConfig;

	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;

	virtual void Melee();
	virtual void DoMelee();
	virtual void ResetMelee();
	virtual void CancelMelee();

	int32 GetWeaponIndex();

	/** [local + server] start weapon fire */
	virtual void StartFire();

	/** [local + server] stop weapon fire */
	virtual void StopFire();

	/** [all] start weapon reload */
	virtual void StartReload(bool bFromReplication = false);

	/** [local + server] interrupt weapon reload */
	virtual void StopReload();

	/** [server] performs actual reload */
	virtual void ReloadWeapon();

	/** trigger reload from server */
	UFUNCTION(reliable, client)
		void ClientStartReload();

	/** play weapon animations */
	virtual float PlayWeaponAnimation(const FWeaponAnim& Animation);

	/** stop playing weapon animations */
	virtual void StopWeaponAnimation(const FWeaponAnim& Animation);

	/** check if weapon can be reloaded */
	bool CanReload() const;

	/** reload animations */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim ReloadAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim FireAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim AimFireAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim MeleeAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim SprintAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim IdleAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim EquipAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim AimAnim;


	/** weapon is being equipped by owner pawn */
	virtual void OnEquip();

	/** weapon is now equipped by owner pawn */
	virtual void OnEquipFinished();

	/** weapon is holstered by owner pawn */
	virtual void OnUnEquip();

	UFUNCTION()
		void OnRep_MyPawn();

	UFUNCTION()
		void OnRep_Reload();

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		TSubobjectPtr<class USkeletalMeshComponent> ArmsMesh;

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

	UFUNCTION(reliable, server, WithValidation)
		void ServerMelee();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStartFire();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStopFire();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStartReload();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStopReload();

	/** instant hit notify for replication */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitNotify)
		FInstantHitInfo HitNotify;

	/** server notified of hit from client to verify */
	UFUNCTION(reliable, server, WithValidation)
		void ServerNotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);

	/** server notified of miss to show trail FX */
	UFUNCTION(unreliable, server, WithValidation)
		void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);

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

	/** [local + server] handle weapon fire */
	void HandleFiring();

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon();

	UPROPERTY()
		float CurrentFiringSpread;

	WeaponStates WeaponState;

	void CancelReload();

	UPROPERTY()
		int32 AmmoInClip;

	UPROPERTY()
		int32 Ammo;

	UPROPERTY()
		float LastFireTime;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		TArray<FName> MuzzleAttachPoint;

	/** FX for muzzle flash */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		TArray<UParticleSystem*> MuzzleFX;

	/** spawned component for muzzle FX */
	UPROPERTY(Transient)
		TArray<UParticleSystemComponent*> MuzzlePSC;

	void FireSpecial(FName SocketName, FVector Direction);

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
		int32 GetMaxAmmo() const;

	UFUNCTION()
		void OnRep_HitNotify();

	/** called in network play to do the cosmetic fx  */
	void SimulateInstantHit(const FVector& Origin, int32 RandomSeed, float ReticleSpread);

	UPROPERTY(EditDefaultsOnly, Category = Aiming)
		bool bAiming;

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

	void StartAiming();
	void StopAiming();

	UPROPERTY(EditDefaultsOnly, Category = Aiming)
		FVector ViewOffset;

	UPROPERTY(EditDefaultsOnly, Category = Aiming)
		FVector AimViewOffset;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* ReloadSound;

	/** play weapon sounds */
	UAudioComponent* PlayWeaponSound(USoundCue* Sound);

	/** spawn effects for impact */
	void SpawnImpactEffects(const FHitResult& Impact);

	/** spawn trail effect */
	void SpawnTrailEffect(const FVector& EndPoint);

	/** smoke trail */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* TrailFX;

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
		TSubobjectPtr<USkeletalMeshComponent> Mesh1P;

	/** weapon mesh: 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		TSubobjectPtr<USkeletalMeshComponent> Mesh3P;

	float TargetFOV;
	float TargetViewOffset;
	float LastAimTime;
};