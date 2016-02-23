

#pragma once

#include "OrionCharacter.h"
#include "OrionDinoPawn.generated.h"

/**
 * 
 */

UENUM()
enum EDinoType
{
	DINOTYPE_CARNIVORE,
	DINOTYPE_HERBIVORE,
	DINOTYPE_OMNIVORE
};

UCLASS()
class ORION_API AOrionDinoPawn : public AOrionCharacter
{
	GENERATED_BODY()
public:
	AOrionDinoPawn(const FObjectInitializer& ObjectInitializer);

	//void InitMaterials() override {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *BlinkAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *ShortRoarAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *MediumRoarAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *LongRoarAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *TurnLeft180Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *TurnLeft90Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *TurnRight180Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *TurnRight90Animation;

	virtual FVector2D GetAim(float DeltaTime) override;
	virtual void Tick(float DeltaTime) override;
	void OrientToGround(float DeltaTime);
	void HandleFootPlacement(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Eco)
		TEnumAsByte<EDinoType> DinoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Eco)
		FName DinoName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GamePlay)
		float RunningModifier;

	//charging a big move
	UPROPERTY(BlueprintReadWrite, Category = AI)
		bool bChargingAttack;

	//running at an enemy doing a charge attack
	UPROPERTY(Replicated, BlueprintReadWrite, Category = AI)
		bool bChargeAttack;

	UPROPERTY(BlueprintReadWrite, Category = AI)
		FVector ChargeTarget;

	UPROPERTY(BlueprintReadWrite, Category = AI)
		bool bTongueAttack;

	UPROPERTY(BlueprintReadWrite, Category = AI)
		FVector TongueTarget;

	UPROPERTY(BlueprintReadWrite, Category = AI)
		bool bShouldStopMovement;

private:
	FVector GroundNormal;
};
