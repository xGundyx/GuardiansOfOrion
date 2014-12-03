

#pragma once

#include "OrionCharacter.h"
#include "OrionDroidPawn.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionDroidPawn : public AOrionCharacter
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = Hover)
		virtual float GetFinRotation(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = Hover)
		virtual float GetFrontRotation(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = Hover)
		virtual float GetBackRotation(float DeltaTime);

	void HandleSpecialWeaponFire(FName SocketName);
	
private:
	float FinsRotation;
	float FrontRotation;
	float BackRotation;
};