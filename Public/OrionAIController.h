

#pragma once

#include "AIController.h"
#include "OrionFlyableArea.h"
#include "OrionAIController.generated.h"

class UOrionSquad;

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

	/** Update direction AI is looking based on FocalPoint */
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;

	UFUNCTION(BlueprintCallable, Category = Target)
		void SetEnemy(APawn *pEnemy);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Set Blackboard Enemy"))
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
		UOrionSquad *Squad;

	AOrionFlyableArea *GetFlyableNavMesh();

	UFUNCTION(BlueprintCallable, Category = Navigation)
		void FindFlightPath(FVector Destination);

	UFUNCTION(BlueprintCallable, Category = Navigation)
		FVector GetRandomFlightPoint();

	UFUNCTION(BlueprintCallable, Category = Navigation)
		void SetLanding(bool IsLanding);

	UPROPERTY(BlueprintReadWrite, Category = Navigation)
		bool bLanding;

	UPROPERTY(BlueprintReadWrite, Category = Navigation)
		FVector LandingLocation;

	EPathFollowingRequestResult::Type MoveFlyToLocation(const FVector& Dest, float AcceptanceRadius, bool bStopOnOverlap, bool bUsePathfinding, bool bProjectDestinationToNavigation, bool bCanStrafe, TSubclassOf<UNavigationQueryFilter> FilterClass);

private:
	APawn *myEnemy;

	//flying path variables
	AOrionFlyableArea *FlyArea;

public:
	TArray<FVector> FlightPath;
	int32 FlightIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Navigation)
		bool bFinishedPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Navigation)
		bool bFinishedLanding;
};
