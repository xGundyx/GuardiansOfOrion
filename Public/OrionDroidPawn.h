

#pragma once

#include "OrionCharacter.h"
#include "OrionDroidPawn.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionDroidPawn : public AOrionCharacter
{
	GENERATED_BODY()
public:
	AOrionDroidPawn(const FObjectInitializer& ObjectInitializer);

	void InitMaterials() override {}
	void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	float RandomVoiceInterval;

	UFUNCTION(BlueprintCallable, Category = Hover)
		virtual float GetFinRotation(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = Hover)
		virtual float GetFrontRotation(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = Hover)
		virtual float GetBackRotation(float DeltaTime);

	void HandleSpecialWeaponFire(FName SocketName);
	virtual FVector2D GetAim(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Eco)
		FName DroidName;
	
private:
	float FinsRotation;
	float FrontRotation;
	float BackRotation;
};
