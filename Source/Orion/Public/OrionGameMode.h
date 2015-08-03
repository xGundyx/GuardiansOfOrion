// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "OrionPickup.h"
#include "OrionShipPawn.h"
#include "OrionGameMode.generated.h"

UCLASS()
class ORION_API AOrionGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AOrionGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	//virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole RemoteRole, const FString& Portal, const FString& Options, const TSharedPtr<FUniqueNetId>& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage) override;
	virtual void Logout(AController* Exiting) override;
	virtual void SetInitialTeam(APlayerController *PC);

	void SpawnItems(AActor *Spawner);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<class AOrionPickup>  DefaultPickupClass;

	UPROPERTY(BlueprintReadOnly, Category = Classes)
		bool bAlwaysShowCursor;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetShip(AOrionShipPawn *Ship, int32 Index){}

	UPROPERTY(BlueprintReadOnly, Category = Game)
		bool bTeamGame;

	UPROPERTY(BlueprintReadOnly, Category = Game)
		bool bTopDown;

	float ModifyDamage(float Damage, AOrionCharacter *PawnToDamage, struct FDamageEvent const& DamageEvent, class AController *EventInstigator, class AActor *DamageCauser);

	void InitGRI();

	virtual void SpawnWave() {}

private:
	FTimerHandle GRITimer;
};



