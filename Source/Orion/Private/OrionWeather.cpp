// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionWeather.h"
#include "OrionPlayerController.h"

AOrionWeather::AOrionWeather(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	///RainPSC = PCIP.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Rain"));
	SnowPSC = PCIP.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Snow"));

	/*if (RainPSC)
		RainPSC->SetTemplate(Rain);

	RainPSC->RegisterComponentWithWorld(GetWorld());

	RainPSC->bAutoActivate = false;
	RainPSC->SetHiddenInGame(false);*/
}

#if WITH_EDITOR
void AOrionWeather::LoadedFromAnotherClass(const FName& OldClassName)
{
}

void AOrionWeather::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{

}
#endif

void AOrionWeather::BeginPlay()
{
	Super::BeginPlay();

	//register us with the GRI
	if (Cast<AOrionGRI>(GWorld->GameState))
		Cast<AOrionGRI>(GWorld->GameState)->SetWeather(this);
}

void AOrionWeather::PostLoad()
{
	ALight::PostLoad();

	PrimaryActorTick.bCanEverTick = true;
}

void AOrionWeather::ChooseWeather()
{
	if (!bIsRaining)
	{
		float Len = 120.0f + FMath::FRand()*120.0f;
		GetWorldTimerManager().SetTimer(this, &AOrionWeather::StartClouds, Len, false);
		GetWorldTimerManager().SetTimer(this, &AOrionWeather::StartRaining, Len + 30.0f, false);
	}
	else
		StopRaining();
}

void AOrionWeather::ClearWeatherTimers()
{
	GetWorldTimerManager().ClearTimer(this, &AOrionWeather::StartClouds);
	GetWorldTimerManager().ClearTimer(this, &AOrionWeather::StartRaining);
}

void AOrionWeather::ClearNight()
{
	ClearWeatherTimers();

	TheTime = SunSet + 120.0f;

	GetWorldTimerManager().SetTimer(this, &AOrionWeather::ChooseWeather, 120.0f + FMath::FRand()*120.0f, false);
}

void AOrionWeather::StartRaining()
{
	ClearWeatherTimers();

	if (!RainPSC)
	{
		RainPSC = ConstructObject<UParticleSystemComponent>(UParticleSystemComponent::StaticClass());
		RainPSC->SetTemplate(Rain);

		RainPSC->RegisterComponentWithWorld(GetWorld());

		RainPSC->bAutoActivate = false;
		RainPSC->SetHiddenInGame(false);
	}
	
	if (RainPSC)
	{
		//FVector Origin = GetWeatherLocation();
		//FRotator Dir(0, 0, 0);
		
		RainPSC->ActivateSystem();
		//RainPSC->SetWorldLocation(Origin);
		bIsRaining = true;
		RainTarget = 1.0f;
	}

	GetWorldTimerManager().SetTimer(this, &AOrionWeather::ChooseWeather, 120.0f + FMath::FRand()*120.0f, false);
}

void AOrionWeather::StopRaining()
{
	if (RainPSC)
	{
		bIsRaining = false;
		if (RainPSC->Template)
			RainPSC->DeactivateSystem();
		RainTarget = 0.0f;
		//RainPSC = NULL;
	}

	StopClouds();

	GetWorldTimerManager().SetTimer(this, &AOrionWeather::ChooseWeather, 60.0f, false);
}

void AOrionWeather::StartSnowing()
{
	if (!SnowPSC)
	{
		FVector Origin = GetWeatherLocation();
		FRotator Dir(0, 0, 0);
		//SnowPSC = UGameplayStatics::SpawnEmitterAtLocation(this, Snow, Origin, Dir);
	}
	if (SnowPSC)
	{
		bIsSnowing = true;
		SnowPSC->ActivateSystem();
	}

	GetWorldTimerManager().SetTimer(this, &AOrionWeather::StartSnowing, 60.0f, false);
}

void AOrionWeather::StopSnowing()
{
	if (SnowPSC)
	{
		bIsSnowing = false;
		SnowPSC->DeactivateSystem();
	}

	CloudTarget = 0.0f;
}

void AOrionWeather::StartClouds()
{
	FogTarget = 1.0;
	CloudTarget = 1.0f;
}

void AOrionWeather::StopClouds()
{
	FogTarget = 0.0;
	CloudTarget = 0.0f;
}

FTimeOfDay AOrionWeather::GetTimeOfDay() const
{
	return TheTime;
}

bool AOrionWeather::IsNightTime() const
{
	return TheTime>SunSet || TheTime<SunRise;
}

bool AOrionWeather::IsRaining() const
{
	return false;
}

void AOrionWeather::PerfectDay()
{
	ClearWeatherTimers();

	TheTime = SunRise + 60.0f;
	StopRaining();
	StopClouds();
}

void AOrionWeather::HeavyRain()
{
	StartClouds();
	StartRaining();
}

void AOrionWeather::UpdateWeather(float DeltaSeconds)
{
	if (GetWorld() == NULL)
		return;

	//float DeltaSeconds = GetWorld()->TimeSeconds - LastUpdateTime;
	//LastUpdateTime = GetWorld()->TimeSeconds;
	//advance the time
	TheTime = TheTime + DeltaSeconds;

	FRotator rot;
	FTimeOfDay MidNight;
	MidNight.Hour = 24.0f;
	MidNight.Minute = 0.0f;

	if (TheTime > SunSet)
		rot.Pitch = -180.0f - ((TheTime - SunSet) / (MidNight - SunSet)) * 90.0f;
	else if (TheTime < SunRise)
		rot.Pitch = 90.0f - (TheTime / SunRise) * 90.0f;
	else
		rot.Pitch = ((TheTime - SunRise) / (SunSet - SunRise)) * -180.0f;

	if (SunYaw == 0.0f)
		SunYaw = GetActorRotation().Yaw;

	rot.Yaw = SunYaw;
	rot.Roll = 0.0f;

	SetActorRotation(rot);

	//update our interpolation values
	if (FogTarget > FogAmount)
		FogAmount = FMath::Min(FogAmount + DeltaSeconds*0.05f, FogTarget);
	else
		FogAmount = FMath::Max(FogAmount - DeltaSeconds*0.05f, FogTarget);

	if (RainTarget > RainAmount)
		RainAmount = FMath::Min(RainAmount + DeltaSeconds*0.6f, RainTarget);
	else
		RainAmount = FMath::Max(RainAmount - DeltaSeconds*0.6f, RainTarget);

	if (CloudTarget > CloudAmount)
		CloudAmount = FMath::Min(CloudAmount + DeltaSeconds*0.1f, CloudTarget);
	else
		CloudAmount = FMath::Max(CloudAmount - DeltaSeconds*0.1f, CloudTarget);

	UpdateFog(DeltaSeconds);
	UpdateRain(DeltaSeconds);
	UpdateClouds(DeltaSeconds);

	EventUpdateWeather(DeltaSeconds);
}

void AOrionWeather::UpdateFog(float DeltaSeconds)
{
	if (Fog && Fog->Component)
	{
		//Fog->Component->SetFogDensity(MinFogDensity + (MaxFogDensity - MinFogDensity)*FogAmount);
		//Fog->Component->SetStartDistance(MinFogDistance + (MaxFogDistance - MinFogDistance)*FogAmount);
	}
}

void AOrionWeather::UpdateRain(float DeltaSeconds)
{
	if (RainPSC && RainPSC->Template == Rain && RainPSC->IsActive())
		RainPSC->SetWorldLocation(GetWeatherLocation());
}

void AOrionWeather::UpdateClouds(float DeltaSeconds)
{
}

float AOrionWeather::GetCloudAmount() const
{
	return CloudAmount;
}

float AOrionWeather::GetRainAmount() const
{
	return RainAmount;
}

float AOrionWeather::GetSnowAmount() const
{
	return RainAmount;
}

float AOrionWeather::GetFogAmount() const
{
	return MinFogDensity + (MaxFogDensity - MinFogDensity)*FogAmount;
}

float AOrionWeather::GetFogDistance() const
{
	return MinFogDistance + (MaxFogDistance - MinFogDistance)*FogAmount;
}

float AOrionWeather::GetNightAmount() const
{
	if (TheTime > SunSet - 60.0f)
		return FMath::Max(0.0f, 1.0f - ((TheTime - (SunSet - 60.0f)) / 120.0f));
	else if (TheTime < SunRise + 60.0f)
		return FMath::Max(0.0f, 1.0f - ((SunRise + 60.0f - TheTime) / 120.0f));
	else
		return 1.0f;
}

float AOrionWeather::GetNightTimeDarkness() const
{
	return NightTimeDarkness;
}

FVector AOrionWeather::GetWeatherLocation() const
{
	FVector pos;
	FRotator rot;

	if (PlayerOwner == NULL)
		return FVector(0);

	if (PlayerOwner->GetPawnOrSpectator())
		return PlayerOwner->GetPawnOrSpectator()->GetActorLocation();

	PlayerOwner->GetPlayerViewPoint(pos, rot);
	return pos;
}

ASkyLight* AOrionWeather::GetSkyLight() const
{
	return SkyLight;
}

float AOrionWeather::GetFogMin() const
{
	return MinFog;
}

float AOrionWeather::GetFogMax() const
{
	return MaxFog;
}

float AOrionWeather::GetFogDensityMin() const
{
	return MinFogDensity;
}

float AOrionWeather::GetFogDensityMax() const
{
	return MaxFogDensity;
}

float AOrionWeather::GetFogDistanceMin() const
{
	return MinFogDistance;
}

float AOrionWeather::GetFogDistanceMax() const
{
	return MaxFogDistance;
}

AExponentialHeightFog* AOrionWeather::GetFog() const
{
	return Fog;
}

UParticleSystem* AOrionWeather::GetRainParticle() const
{
	return Rain;
}

UParticleSystem* AOrionWeather::GetSnowParticle() const
{
	return Snow;
}

UParticleSystem* AOrionWeather::GetLightningParticle() const
{
	return Lightning;
}

USoundCue* AOrionWeather::GetLightRainCue() const
{
	return LightRainCue;
}

USoundCue* AOrionWeather::GetHeavyRainCue() const
{
	return HeavyRainCue;
}

USoundCue* AOrionWeather::GetLightningCloseCue() const
{
	return LightningCloseCue;
}

USoundCue* AOrionWeather::GetLightningFarCue() const
{
	return LightningFarCue;
}