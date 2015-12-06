#include "Orion.h"
#include "OrionProjectile.h"
#include "OrionAbility.h"
#include "OrionGrenade.h"

AOrionGrenade::AOrionGrenade(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GrenadeMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GrenadeMesh"));

	//GrenadeMesh->bChartDistanceFactor = false;
	GrenadeMesh->bReceivesDecals = false;
	GrenadeMesh->CastShadow = true;
	GrenadeMesh->SetCollisionObjectType(ECC_PhysicsBody);
	GrenadeMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GrenadeMesh->SetCollisionResponseToAllChannels(ECR_Block);
	//GrenadeMesh->bPerBoneMotionBlur = false;
	GrenadeMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	GrenadeMesh->OnComponentHit.AddDynamic(this, &AOrionGrenade::OnHit);
	//GrenadeMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;

	RootComponent = GrenadeMesh;

	ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = GrenadeMesh;
	ProjectileMovement->InitialSpeed = 800.0f;
	ProjectileMovement->MaxSpeed = 800.0f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 1.0f;

	Damage = 1000;
	DamageRadius = 1250.0f;
	LifeTime = 2.5f;
	FXTime = 10.0f;
	bIsMiniGrenade = false;

	ExplosionScale = 1.0f;

	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AOrionGrenade::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOrionGrenade, bIsMiniGrenade);
}

void AOrionGrenade::Init(FVector dir)
{
	//spawn a projectile using our mesh
	ProjectileMovement->Velocity = dir;
}

void AOrionGrenade::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//make the mesh rotate to simulate actual rolling, doesn't have to be too pretty:p
}

void AOrionGrenade::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AOrionCharacter *HitPawn = Cast<AOrionCharacter>(OtherActor);

	if (HitPawn)
	{
		HitPawn->bDirectGrenadeHit = true;
		Destroy();
	}
}

void AOrionGrenade::Explode()
{
	//do some kind of effect (damage/smoke/emp etc)
	EventExplode();

	//play our explosion effect
	if (ExplosionFX)
	{
		UParticleSystemComponent *PSC = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, GetActorLocation());
		if (PSC)
		{
			float Life = FXTime * (bIsMiniGrenade ? 0.25f : 1.0f);// .0f;
			float Scale = ExplosionScale * (bIsMiniGrenade ? 0.25f : 1.0f);

			if (GetOwner() && Cast<AOrionCharacter>(GetOwner()))
			{
				AOrionPlayerController *PC = Cast<AOrionPlayerController>(Cast<AOrionCharacter>(GetOwner())->Controller);
				if (PC)
				{
					Life += float(PC->GetSkillValue(SKILL_GRENADECOOLDOWN)) * (bIsMiniGrenade ? 0.25f : 1.0f);
					Scale *= 1.0f + float(PC->GetSkillValue(SKILL_GRENADERADIUS)) / 100.0f;
				}
			}

			PSC->SetWorldScale3D(FVector(Scale));
			PSC->SetFloatParameter("LifeTime", Life);
		}
	}

	//play the explosion sound
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	}
}

void AOrionGrenade::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/*if (Role == ROLE_Authority)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AOrionGrenade::GoBoom, LifeTime, false);
	}*/
}

void AOrionGrenade::SetFuseTime(float FuseTime)
{
	if (Role == ROLE_Authority)
	{
		LifeTime = FuseTime;
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AOrionGrenade::GoBoom, LifeTime, false);
	}
}

void AOrionGrenade::GoBoom()
{
	Destroy();
}

void AOrionGrenade::Destroyed()
{
	Explode();

	Super::Destroyed();
}