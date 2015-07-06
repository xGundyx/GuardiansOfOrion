#pragma once

#include "GameFramework/Actor.h"
#include "OrionGib.generated.h"

UCLASS()
class AOrionGib : public AActor
{
	GENERATED_BODY()

public:
	AOrionGib(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gibs)
		class USkeletalMeshComponent *Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gibs)
		FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gibs)
		bool DoBloodSpurt;

	//UDecalComponent* Decal;
	FDecalHelper BloodDecal;

	void SpawnBlood();

	UMaterialInstanceConstant* BloodMat;

	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;
};