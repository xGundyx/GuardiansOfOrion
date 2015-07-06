

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

	void InitMaterials() override {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *Bite1Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *Bite2Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *NoseLiftAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *BlinkAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *Tail1Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *Tail2Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *StompAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *FeedAnimation;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *LookLeftAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage *LookRightAnimation;

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

private:
	FVector GroundNormal;
};
