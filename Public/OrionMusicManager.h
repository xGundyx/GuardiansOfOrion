// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionMusicManager.generated.h"

UENUM()
enum EMusicType
{
	MUSIC_NONE,
	MUSIC_AMBIENT,
	MUSIC_GAMEPLAY,
	MUSIC_BOSS,
	MUSIC_LOADING,
	MUSIC_CREDITS,
	MUSIC_MENU,
	MUSIC_MISSION
};

UCLASS()
class ORION_API AOrionMusicManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrionMusicManager(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void TickMusic();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Music)
		TArray<USoundWave*> BossMusic;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Music)
		TArray<USoundWave*> MenuMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Music)
		TArray<USoundWave*> GameplayMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Music)
		TArray<USoundWave*> AmbientMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Music)
		TArray<USoundWave*> LoadingMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Music)
		TArray<USoundWave*> CreditsMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Music)
		TArray<USoundWave*> MissionMusic;

	TEnumAsByte<EMusicType> LastMusicType;

	bool bPlayingMusic;
	void ResetMusic();

	void PlaySong();

	USoundWave *CurrentSong;
	USoundWave *NextSong;

	float VolumeMax;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Music)
		class UAudioComponent *MusicComponent;

private:
	FTimerHandle MusicTimer;
	FTimerHandle MusicTickTimer;

	//USoundCue *MusicCue;
};
