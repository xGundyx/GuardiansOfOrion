// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionWeather.h"
#include "OrionPlayerController.h"

AOrionWeather::AOrionWeather(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	//RainPSC = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Rain"));
	//SnowPSC = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Snow"));
	bReplicates = true;
	bAlwaysRelevant = true;
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
	{
		if (Role == ROLE_Authority)
		{
			ChooseWeather();
			Cast<AOrionGRI>(GWorld->GameState)->SetWeather(this);
		}
	}

	/*if (RainPSC)
	{
		RainPSC->SetTemplate(Rain);

		RainPSC->RegisterComponentWithWorld(GetWorld());

		RainPSC->bAutoActivate = false;
		RainPSC->SetHiddenInGame(false);

		//RainPSC->AttachTo(RootComponent);
	}*/
}

void AOrionWeather::PostLoad()
{
	ALight::PostLoad();

	PrimaryActorTick.bCanEverTick = true;
}

void AOrionWeather::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// everyone
	DOREPLIFETIME(AOrionWeather, FogTarget);
	DOREPLIFETIME(AOrionWeather, RainTarget);
	DOREPLIFETIME(AOrionWeather, CloudTarget);
	DOREPLIFETIME(AOrionWeather, TheTime);
	DOREPLIFETIME(AOrionWeather, bIsRaining);
}

void AOrionWeather::ChooseWeather()
{
	if (!bIsRaining)
	{
		float Len = 240.0f + FMath::FRand()*240.0f;
		GetWorldTimerManager().SetTimer(CloudTimer, this, &AOrionWeather::StartClouds, Len, false);
		GetWorldTimerManager().SetTimer(RainTimer, this, &AOrionWeather::StartRaining, Len + 20.0f, false);
	}
	else
		StopRaining();
}

void AOrionWeather::ClearWeatherTimers()
{
	GetWorldTimerManager().ClearTimer(CloudTimer);// , this, &AOrionWeather::StartClouds);
	GetWorldTimerManager().ClearTimer(RainTimer);// , this, &AOrionWeather::StartRaining);
}

void AOrionWeather::ClearNight()
{
	ClearWeatherTimers();

	TheTime = SunSet + 120.0f;

	GetWorldTimerManager().SetTimer(WeatherTimer, this, &AOrionWeather::ChooseWeather, 120.0f + FMath::FRand()*120.0f, false);
}

void AOrionWeather::OnRep_Raining()
{
	if (bIsRaining)
		StartRaining();
	else
		StopRaining();
}

void AOrionWeather::StartRaining()
{
	ClearWeatherTimers();

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(PlayerOwner);

	//if (!Cast<AOrionPlayerController>(PlayerOwner)->RainPSC || !Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->Template)
	if (PC)
	{
		//RainPSC = ConstructObject<UParticleSystemComponent>(UParticleSystemComponent::StaticClass());
		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC = UGameplayStatics::SpawnEmitterAtLocation(PlayerOwner, Rain, GetActorLocation(), FRotator::ZeroRotator, false);
		//RainPSC = UGameplayStatics::SpawnEmitterAttached(Rain, RootComponent, NAME_None, GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false); 
		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->SetTemplate(Rain);

		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->RegisterComponentWithWorld(GetWorld());

		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->bAutoActivate = false;
		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->bAutoDestroy = false;
		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->SetHiddenInGame(false);
	}
	
	if (PC && PC->RainPSC)
	{
		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->ActivateSystem();
	}

	if (Role == ROLE_Authority)
	{
		bIsRaining = true;
		RainTarget = 1.0f;

		GetWorldTimerManager().SetTimer(WeatherTimer, this, &AOrionWeather::ChooseWeather, 60.0f + FMath::FRand()*120.0f, false);
	}
}

void AOrionWeather::StopRaining()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(PlayerOwner);

	if (PC && PC->RainPSC)
	{
		//if (RainPSC->Template)
		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->DeactivateSystem();
		//RainPSC->SetHiddenInGame(true);
		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC = NULL;
	}

	if (Role == ROLE_Authority)
	{
		bIsRaining = false;
		StopClouds();
		RainTarget = 0.0f;

		GetWorldTimerManager().SetTimer(WeatherTimer, this, &AOrionWeather::ChooseWeather, 60.0f, false);
	}
}

void AOrionWeather::StartSnowing()
{
	/*if (!SnowPSC)
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

	GetWorldTimerManager().SetTimer(this, &AOrionWeather::StartSnowing, 60.0f, false);*/
}

void AOrionWeather::StopSnowing()
{
	/*if (SnowPSC)
	{
		bIsSnowing = false;
		SnowPSC->DeactivateSystem();
	}

	CloudTarget = 0.0f;*/
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
	if (Role == ROLE_Authority)
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

	if (GetNetMode() != NM_DedicatedServer)
	{
		UpdateFog(DeltaSeconds);
		UpdateRain(DeltaSeconds);
		UpdateClouds(DeltaSeconds);

		EventUpdateWeather(DeltaSeconds);
	}
}

void AOrionWeather::UpdateFog(float DeltaSeconds)
{
	if (Fog && Fog->GetComponent())
	{
		//Fog->Component->SetFogDensity(MinFogDensity + (MaxFogDensity - MinFogDensity)*FogAmount);
		//Fog->Component->SetStartDistance(MinFogDistance + (MaxFogDistance - MinFogDistance)*FogAmount);
	}
}

void AOrionWeather::UpdateRain(float DeltaSeconds)
{
	if (!PlayerOwner)
	{
		PlayerOwner = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());
		if (!PlayerOwner)
			return;
	}

	///this is a hack to get the rain to keep falling, for some reason it keeps shutting off:/
	/*if (bIsRaining && Cast<AOrionPlayerController>(PlayerOwner)->RainPSC && Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->bWasCompleted)
	{
		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC = UGameplayStatics::SpawnEmitterAtLocation(PlayerOwner, Rain, GetActorLocation(), FRotator::ZeroRotator, false);

		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->SetTemplate(Rain);

		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->RegisterComponentWithWorld(GetWorld());

		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->bAutoActivate = false;
		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->bAutoDestroy = false;
		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->SetHiddenInGame(false);

		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->ActivateSystem();
	}*/

	if (Cast<AOrionPlayerController>(PlayerOwner)->RainPSC && Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->Template == Rain && Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->IsActive())
		Cast<AOrionPlayerController>(PlayerOwner)->RainPSC->SetWorldLocation(GetWeatherLocation());
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