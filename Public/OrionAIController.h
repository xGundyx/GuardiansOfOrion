

#pragma once

#include "AIController.h"
#include "OrionAIController.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionAIController : public AAIController
{
	GENERATED_BODY()
public:
	AOrionAIController(const FObjectInitializer& ObejctInitializer);
	
	virtual void Tick(float DeltaTime) override;

	/** Update direction AI is looking based on FocalPoint */
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;

	UFUNCTION(BlueprintCallable, Category = Target)
		void SetEnemy(APawn *pEnemy);

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

private:
	APawn *myEnemy;
};
