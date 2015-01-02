// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine/DirectionalLight.h"
#include "OrionWeather.generated.h"

USTRUCT()
struct FTimeOfDay
{
	GENERATED_USTRUCT_BODY()

	/** animation played on pawn (1st person view) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Time)
		float Hour;

	/** animation played on pawn (3rd person view) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Time)
		float Minute;
};

FORCEINLINE bool operator < (FTimeOfDay T1, FTimeOfDay T2)
{
	if (T1.Hour>T2.Hour)
		return false;
	else if (T1.Hour < T2.Hour)
		return true;
	else
		return T2.Minute>T1.Minute;
}

FORCEINLINE bool operator > (FTimeOfDay T1, FTimeOfDay T2)
{
	return !(T1 < T2);
}

FORCEINLINE FTimeOfDay operator + (FTimeOfDay T1, float Minutes)
{
	FTimeOfDay newTime;

	newTime.Minute = T1.Minute + Minutes;
	newTime.Hour = T1.Hour;

	if (newTime.Minute>=60.0)
	{
		newTime.Minute -= 60.0;
		newTime.Hour += 1.0;
		if (newTime.Hour >= 24.0)
		{
			newTime.Hour -= 24.0;
		}
	}

	return newTime;
}

FORCEINLINE FTimeOfDay operator - (FTimeOfDay T1, float Minutes)
{
	FTimeOfDay newTime;

	newTime.Minute = T1.Minute - Minutes;
	newTime.Hour = T1.Hour;

	if (newTime.Minute<0.0)
	{
		newTime.Minute += 60.0;
		newTime.Hour -= 1.0;
		if (newTime.Hour < 0.0)
		{
			newTime.Hour += 24.0;
		}
	}

	return newTime;
}

FORCEINLINE float operator - (FTimeOfDay T1, FTimeOfDay T2)
{
	float M1, M2;

	M1 = 60.0f*T1.Hour + T1.Minute;
	M2 = 60.0f*T2.Hour + T2.Minute;

	return M1 - M2;
}

FORCEINLINE float operator / (FTimeOfDay T1, FTimeOfDay T2)
{
	float M1, M2;

	M1 = 60.0f*T1.Hour + T1.Minute;
	M2 = 60.0f*T2.Hour + T2.Minute;

	return M1 / M2;
}

/**
*
*/
UCLASS()
class AOrionWeather : public ADirectionalLight
{
	GENERATED_BODY()
public:
	AOrionWeather(const FObjectInitializer& ObejctInitializer);
	//virtual void PostInitializeComponents() override;

	APlayerController* PlayerOwner;
	float FogTarget;
	float FogAmount;
	float RainTarget;
	float RainAmount;
	float CloudTarget;
	float CloudAmount;
	float SunYaw;

	virtual void PerfectDay();
	virtual void HeavyRain();
	virtual void ClearNight();
	virtual void ClearWeatherTimers();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditAnywhere, Category = Time)
		FTimeOfDay TheTime;

	UPROPERTY(EditAnywhere, Category = Time)
		FTimeOfDay SunSet;

	UPROPERTY(EditAnywhere, Category = Time)
		FTimeOfDay SunRise;

	UPROPERTY(EditAnywhere, Category = Weather)
		UParticleSystem* Rain;

	UPROPERTY(EditAnywhere, Category = Weather)
		UParticleSystem* Snow;

	UPROPERTY(EditAnywhere, Category = Weather)
		UParticleSystem* Lightning;

	UPROPERTY(EditAnywhere, Category = Weather)
		float RainWeight;

	UPROPERTY(EditAnywhere, Category = Weather)
		float SunWeight;

	UPROPERTY(EditAnywhere, Category = Weather)
		float FogWeight;

	UPROPERTY(EditAnywhere, Category = Weather)
		float CloudWeight;

	UPROPERTY(EditAnywhere, Category = Weather)
		float MaxFog;

	UPROPERTY(EditAnywhere, Category = Weather)
		float MinFog;

	UPROPERTY(EditAnywhere, Category = Weather)
		float MaxFogDensity;

	UPROPERTY(BlueprintReadWrite, Category = Weather)
		float MinFogDensity;

	UPROPERTY(EditAnywhere, Category = Weather)
		float MinFogDistance;

	UPROPERTY(BlueprintReadWrite, Category = Weather)
		float MaxFogDistance;

	UPROPERTY(EditAnywhere, Category = Weather)
		float DayStars;

	UPROPERTY(EditAnywhere, Category = Weather)
		float NightStars;

	UPROPERTY(EditAnywhere, Category = Weather)
		float NightTimeDarkness;

	UPROPERTY(EditAnywhere, Category = Weather)
		USoundCue* LightRainCue;

	UPROPERTY(EditAnywhere, Category = Weather)
		USoundCue* HeavyRainCue;

	UPROPERTY(EditAnywhere, Category = Weather)
		USoundCue* LightningCloseCue;

	UPROPERTY(EditAnywhere, Category = Weather)
		USoundCue* LightningFarCue;

	UPROPERTY(EditAnywhere, Category = Weather)
		AExponentialHeightFog* Fog;

	UPROPERTY(EditAnywhere, Category = Weather)
		ASkyLight* SkyLight;

	//blueprint functions
	UFUNCTION(BlueprintCallable, Category = Weather)
		AExponentialHeightFog* GetFog() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		ASkyLight* GetSkyLight() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetFogMin() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetFogMax() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetCloudAmount() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetRainAmount() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetSnowAmount() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetFogAmount() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetNightAmount() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetNightTimeDarkness() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetFogDistance() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetFogDensityMin() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetFogDensityMax() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetFogDistanceMin() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		float GetFogDistanceMax() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		USoundCue* GetLightRainCue() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		USoundCue* GetHeavyRainCue() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		USoundCue* GetLightningCloseCue() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		FVector GetWeatherLocation() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		USoundCue* GetLightningFarCue() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		UParticleSystem* GetRainParticle() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		UParticleSystem* GetSnowParticle() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		UParticleSystem* GetLightningParticle() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		FTimeOfDay GetTimeOfDay() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		bool IsNightTime() const;

	UFUNCTION(BlueprintCallable, Category = Weather)
		bool IsRaining() const;

	//UPROPERTY(EditDefaultsOnly, Category = Sun)
	//ADirectionalLight* TheSun;

	float LastUpdateTime;
	void UpdateWeather(float DeltaSeconds);
	void UpdateFog(float DeltaSeconds);
	void UpdateRain(float DeltaSeconds);
	void UpdateClouds(float DeltaSeconds);
	void ChooseWeather();

	void StartRaining();
	void StopRaining();

	void StartSnowing();
	void StopSnowing();

	void StartFog();
	void StopFog();

	void StartClouds();
	void StopClouds();

	bool bIsRaining;
	bool bIsSnowing;

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Every Tick"))
		virtual void EventUpdateWeather(float DeltaSeconds);

	virtual void PostLoad() override;
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void LoadedFromAnotherClass(const FName& OldClassName) override;
#endif
};
