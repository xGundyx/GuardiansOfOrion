#include "Orion.h"
#include "OrionGib.h"

AOrionGib::AOrionGib(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Mesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("GibMesh"));
	Mesh->AlwaysLoadOnClient = true;
	Mesh->AlwaysLoadOnServer = true;

	static FName CollisionProfileName(TEXT("Ragdoll"));
	Mesh->SetCollisionProfileName(CollisionProfileName);

	Mesh->bReceivesDecals = false;
	Mesh->SetSimulatePhysics(true);

	RootComponent = Mesh;

	DoBloodSpurt = false;

	PrimaryActorTick.bCanEverTick = true;
}

void AOrionGib::Tick(float DeltaSeconds)
{
	if (BloodDecal.Mat)
	{
		//update our decal if needed
		BloodDecal.TimeLeft = BloodDecal.TimeLeft - DeltaSeconds * 0.1f;
		BloodDecal.Mat->SetScalarParameterValue("Time_M", FMath::Min(0.99f, 1.2f - BloodDecal.TimeLeft));

		if (BloodDecal.TimeLeft <= -1.0f)
		{
			BloodDecal.Mat = NULL;
		}
	}
}

void AOrionGib::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle THandle;
	GetWorldTimerManager().SetTimer(THandle, this, &AOrionGib::SpawnBlood, 2.0f, false);

	SetLifeSpan(32.0f);
}

void AOrionGib::SpawnBlood()
{
	//if we aren't at a stop yet, wait a bit
	//if (Mesh->ComponentVelocity.Size() > 10.0f)
	if (Mesh->GetPhysicsLinearVelocity(Mesh->GetSocketBoneName("Blood")).Size() > 1.0f)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AOrionGib::SpawnBlood, 0.25f, false);
		return;
	}

	if (BloodMat)
	{
		FVector pos;
		FRotator rot;
		Mesh->GetSocketWorldLocationAndRotation("Blood", pos, rot);

		FHitResult Hit;

		FCollisionQueryParams TraceParams(FName(TEXT("BloodGibTrace")), true, this);

		TraceParams.AddIgnoredActor(this);
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = true;

		if (GetWorld()->LineTraceSingleByObjectType(Hit, pos, pos - FVector(0.0f, 0.0f, 150.0f), FCollisionQueryFlag::Get().GetAllStaticObjectsQueryFlag(), TraceParams))
		{
			pos = Hit.ImpactPoint;

			UDecalComponent *Dec = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BloodMat, FVector(FMath::RandRange(64, 128), FMath::RandRange(64, 128), 32.0f), pos, FRotator(-90, 0, 0), 30.0f);

			if (Dec)
			{
				BloodDecal.Mat = UMaterialInstanceDynamic::Create(Dec->GetMaterial(0), this);
				BloodDecal.TimeLeft = 1.2f;

				Dec->SetMaterial(0, BloodDecal.Mat);
			}
		}
	}
}