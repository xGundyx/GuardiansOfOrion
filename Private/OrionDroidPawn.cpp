

#include "Orion.h"
#include "OrionAIController.h"
#include "OrionDroidPawn.h"


AOrionDroidPawn::AOrionDroidPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

float AOrionDroidPawn::GetFinRotation(float DeltaTime)
{
	FinsRotation += DeltaTime * 90.0f;

	if (FinsRotation > 360.0f)
		FinsRotation -= 360.0f;
	else if (FinsRotation < 0.0f)
		FinsRotation += 360.0f;

	return FinsRotation;
}

//fire weapon from weapon socket
void AOrionDroidPawn::HandleSpecialWeaponFire(FName SocketName)
{
	if (GetMesh() && CurrentWeapon)
	{
		FVector pos;
		FVector dir;
		FRotator rot;

		if (CurrentWeapon->bProjectile)
		{
			AOrionAIController *C = Cast<AOrionAIController>(Controller);

			GetMesh()->GetSocketWorldLocationAndRotation(SocketName, pos, rot);

			if (C && C->GetEnemy())
			{
				FVector TossVelocity;
				if (UGameplayStatics::SuggestProjectileVelocity(GetWorld(), TossVelocity, pos + GetActorRotation().Vector() * 50.0f, C->GetEnemy()->GetActorLocation(), 800.0f, false))
					dir = TossVelocity;
				else
					dir = (GetActorRotation().Vector() + FVector(0.0f, 0.0f, 0.5f)).GetSafeNormal() * 800.0f;
			}
			else
				dir = (GetActorRotation().Vector() + FVector(0.0f, 0.0f, 0.5f)).GetSafeNormal() * 800.0f;
		}
		else
			dir = rot.Vector();

		CurrentWeapon->FireSpecial(SocketName, dir);
	}
}

float AOrionDroidPawn::GetFrontRotation(float DeltaTime)
{
	float v = FVector::DotProduct(GetVelocity().GetSafeNormal(), GetActorRotation().Vector());

	FrontRotation += DeltaTime * 180.0f * v;

	if (FrontRotation > 360.0f)
		FrontRotation -= 360.0f;
	else if (FrontRotation < 0.0f)
		FrontRotation += 360.0f;

	return FrontRotation;
}

float AOrionDroidPawn::GetBackRotation(float DeltaTime)
{
	float v = FVector::DotProduct(GetVelocity().GetSafeNormal(), GetActorRotation().Vector());

	BackRotation -= DeltaTime * 180.0f * v;

	if (BackRotation > 360.0f)
		BackRotation -= 360.0f;
	else if (BackRotation < 0.0f)
		BackRotation += 360.0f;

	return BackRotation;
}

FVector2D AOrionDroidPawn::GetAim(float DeltaTime)
{
	FVector pos;
	FRotator rot;
	APawn *Target;

	if (!Controller)
		return FVector2D(AimYaw, AimPitch);

	Target = Cast<AOrionAIController>(Controller)->GetEnemy();

	if (Target == NULL || !Target->IsValidLowLevel())
	{
		AimYaw = 0.0f;
		AimPitch = 0.0f;
		return FVector2D(AimYaw, AimPitch);
	}

	GetPawnMesh()->GetSocketWorldLocationAndRotation(FName("Aim"), pos, rot);

	//make sure the target isn't too far away
	if ((Target->GetActorLocation() - pos).Size2D() > 5000.0)
	{
		AimYaw = 0.0f;
		AimPitch = 0.0f;
		return FVector2D(AimYaw, AimPitch);
	}

	FVector Right = FVector::CrossProduct(FVector(0.0f, 0.0f, 1.0f), GetActorRotation().Vector());

	FVector AimDirWS = Cast<AOrionAIController>(Controller)->GetEnemy()->GetActorLocation() + AimAjustment - pos + FVector(0.0f, 0.0f, 75.0f);
	AimDirWS.Normalize();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	//we need to offset the aim a bit based on player third person camera offsets
	FVector2D start = CurrentAim2D;
	const FVector2D end = FVector2D((0.0 + AimRotLS.Yaw) / 90.0, AimRotLS.Pitch / 90.0);

	if (start.X > 1.0 && end.X < -1.0)
		start.X = -2.0 - (2.0 - start.X);
	else if (start.X < -1.0 && end.X > 1.0)
		start.X = 2.0 + (start.X + 2.0);

	CurrentAim2D = FMath::Vector2DInterpTo(start, end, DeltaTime, 3.0);

	AimYaw = CurrentAim2D.X;
	AimPitch = CurrentAim2D.Y;

	return CurrentAim2D;
}

void AOrionDroidPawn::BeginPlay()
{
	Super::BeginPlay();

	DoBlinkEffect(true, GetActorLocation());

	RandomVoiceInterval = FMath::FRandRange(10.0f, 20.0f);
}

void AOrionDroidPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetWorld()->TimeSeconds - LastVoiceTime > RandomVoiceInterval)
	{
		RandomVoiceInterval = FMath::FRandRange(10.0f, 20.0f);

		TArray<int32> RandomVoices;

		RandomVoices.Add(VOICE_LOOKATMEGO);
		RandomVoices.Add(VOICE_CANTTOUCHTHIS);
		RandomVoices.Add(VOICE_AFFIRMATIVE);
		RandomVoices.Add(VOICE_NEGATIVE);
		RandomVoices.Add(VOICE_SITTIGHT);
		RandomVoices.Add(VOICE_IMONIT);

		RandomVoices.Add(VOICE_TAKEOBJECTIVE);
		RandomVoices.Add(VOICE_MOVEOUT);
		RandomVoices.Add(VOICE_GOGOGO);
		RandomVoices.Add(VOICE_REQUESTBACKUP);
		RandomVoices.Add(VOICE_COVERINGFIRE);
		RandomVoices.Add(VOICE_INEEDBACKUP);
		RandomVoices.Add(VOICE_STICKTOGETHER);

		int32 index = FMath::RandRange(0, RandomVoices.Num() - 1);
		PlayVoice(EVoiceType(RandomVoices[index]));
	}
}