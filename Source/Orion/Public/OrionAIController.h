

#pragma once

#include "AIController.h"
#include "OrionFlyableArea.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "OrionAIController.generated.h"

class AOrionSquad;

UENUM(BlueprintType)
enum AIMoveType
{
	AIMOVE_IDLE,
	AIMOVE_WALKING,
	AIMOVE_FLYING,
	AIMOVE_LANDING,
	AIMOVE_SWIMMING
};

/**
 * 
 */
UCLASS()
class ORION_API AOrionAIController : public AAIController
{
	GENERATED_BODY()
public:
	AOrionAIController(const FObjectInitializer& ObjectInitializer);

	virtual void PawnPendingDestroy(APawn* inPawn);

	virtual void Possess(APawn* aPawn) override;
	
	virtual void Tick(float DeltaTime) override;

	void GetPlayerViewPoint(FVector& OutCamLoc, FRotator& OutCamRot) const override;

	/** Update direction AI is looking based on FocalPoint */
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;

	UFUNCTION()
		void OnHearNoise(APawn *HeardPawn, const FVector &Location, float Volume);

	UFUNCTION()
		void OnSeePawn(APawn *SeenPawn);

	//set this to trigger a roar/taunt in the anim tree
	UPROPERTY(BlueprintReadWrite, Category = AI)
		bool bShouldRoar;

	UPROPERTY(BlueprintReadWrite, Category = AI)
		bool bIsStuck;

	void HandleStuck();
	void ResetStuck();

	int32 TimesStuck;

	FTimerHandle StuckTimer;
	int32 StuckCounter;
	FVector LastStuckPos;

	UBlackboardComponent *GetBlackboard();

	UFUNCTION(BlueprintCallable, Category = Target)
		virtual void SetEnemy(APawn *pEnemy);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Set Blackboard Enemy"))
		void EventSetBlackboardEnemy(APawn *pEnemy);

	UFUNCTION(BlueprintCallable, Category = Target)
		APawn *GetEnemy();

	UFUNCTION(BlueprintCallable, Category = Target)
		void CheckEnemyStatus();

	UFUNCTION(BlueprintCallable, Category = Target)
		void RemoveEnemy();

	UFUNCTION(BlueprintCallable, Category = Target)
		void StartFiringWeapon(FName SocketName, FVector Direction);

	UFUNCTION(BlueprintCallable, Category = Target)
		void StopFiringWeapon();

	UPROPERTY(BlueprintReadWrite, Category = Squad)
		AOrionSquad *Squad;

	AOrionFlyableArea *GetFlyableNavMesh();

	UFUNCTION(BlueprintCallable, Category = Navigation)
		void FindFlightPath(FVector Destination);

	UFUNCTION(BlueprintCallable, Category = Navigation)
		void FlyToPoint(FVector Destination);

	UFUNCTION(BlueprintCallable, Category = Navigation)
		FVector GetNextDirectFlyLocation();

	UFUNCTION(BlueprintCallable, Category = Navigation)
		FVector GetRandomFlightPoint(bool bIsLanding);

	UFUNCTION(BlueprintCallable, Category = Navigation)
		void SetLanding(bool IsLanding);

	UPROPERTY(BlueprintReadWrite, Category = Navigation)
		bool bLanding;

	UPROPERTY(BlueprintReadWrite, Category = Navigation)
		FVector LandingLocation;

	UPROPERTY(BlueprintReadWrite, Category = Navigation)
		TEnumAsByte<AIMoveType> MoveType;

	UPROPERTY(BlueprintReadOnly, Category = AI)
		FName AIName;

	UPROPERTY(BlueprintReadOnly, Category = AI)
		bool bIsElite;

	EPathFollowingRequestResult::Type MoveFlyToLocation(const FVector& Dest, float AcceptanceRadius, bool bStopOnOverlap, bool bUsePathfinding, bool bProjectDestinationToNavigation, bool bCanStrafe, TSubclassOf<UNavigationQueryFilter> FilterClass);
	EPathFollowingRequestResult::Type MoveDirectFlyToLocation(const FVector& Dest, float AcceptanceRadius, TSubclassOf<UNavigationQueryFilter> FilterClass);

private:
	APawn *myEnemy;

	//flying path variables
	AOrionFlyableArea *FlyArea;

public:
	TArray<FVector> FlightPath;
	int32 FlightIndex;

	//timestamp of last time we got a new enemy, after 5 seconds of chasing someone, we can switch to a better target if desired
	float LastEnemyTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Navigation)
		bool bFinishedPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Navigation)
		bool bFinishedLanding;
};
