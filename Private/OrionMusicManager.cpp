// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameMenu.h"
#include "OrionMusicManager.h"


// Sets default values
AOrionMusicManager::AOrionMusicManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOrionMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MusicComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Music"));
	MusicComponent->bIsMusic = true;

	LastMusicType = MUSIC_NONE;

	//bFadingIn = false;
	//bFadingOut = false;
	NextSong = nullptr;
	CurrentSong = nullptr;
}

// Called when the game starts or when spawned
void AOrionMusicManager::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(MusicTickTimer, this, &AOrionMusicManager::TickMusic, 1.0f, true);
}

// Called every frame
void AOrionMusicManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//handle fade in/out
	/*if (bFadingOut)
	{
		MusicComponent->VolumeMultiplier = FMath::Max(0.0f, MusicComponent->VolumeMultiplier - DeltaTime * 0.5f);

		if (MusicComponent->VolumeMultiplier <= 0.0f)
		{
			bFadingOut = false;
			PlaySong(NextSong);
			bFadingIn = true;
		}
	}
	else if (bFadingIn)
	{
		MusicComponent->VolumeMultiplier = FMath::Min(VolumeMax, MusicComponent->VolumeMultiplier + DeltaTime * 0.5f);

		if (MusicComponent->VolumeMultiplier >= VolumeMax)
		{
			bFadingOut = false;
			bFadingIn = false;
		}
	}*/
}

void AOrionMusicManager::TickMusic()
{
	//if (MusicCue == nullptr)
	//	MusicCue = NewObject<USoundCue>(this, USoundCue::StaticClass());

	//if (!MusicCue)
	//	return;

	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (GRI)
	{
		if (GRI->bPlayingIdleMovie)
		{
			if (bPlayingMusic)
			{
				MusicComponent->FadeOut(2.0f, 0.0f);
				bPlayingMusic = false;
			}
			return;
		}

		EMusicType Type = MUSIC_NONE;
		float VolumeMultiplier = 1.0f;

		//only change music if a gameplay type has change, or if the previous song has ended
		if (Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()))
			Type = MUSIC_MENU;
		else if (GRI->DinosAliveInWave > 0)
		{
			if (GRI->bSideMission)
				Type = GRI->bBossMission ? MUSIC_BOSS : MUSIC_MISSION;
			else
				Type = MUSIC_GAMEPLAY;
		}
		else
			Type = MUSIC_AMBIENT;

		if (bPlayingMusic && LastMusicType == Type)
			return;

		//play the actual song!
		TArray<USoundWave*> SoundWave;
		switch (Type)
		{
		case MUSIC_GAMEPLAY:
			SoundWave = GameplayMusic;
			VolumeMultiplier = 1.0f;
			break;

		case MUSIC_MISSION:
			SoundWave = MissionMusic;
			VolumeMultiplier = 1.0f;
			break;

		case MUSIC_BOSS:
			SoundWave = BossMusic;
			VolumeMultiplier = 1.0f;
			break;

		case MUSIC_AMBIENT:
			SoundWave = AmbientMusic;
			VolumeMultiplier = 1.0f;
			break;

		case MUSIC_CREDITS:
			SoundWave = CreditsMusic;
			VolumeMultiplier = 1.0f;
			break;

		case MUSIC_LOADING:
			SoundWave = LoadingMusic;
			VolumeMultiplier = 1.0f;
			break;

		case MUSIC_MENU:
			SoundWave = MenuMusic;
			VolumeMultiplier = 1.0f;
			break;
		}

		if (SoundWave.Num() <= 0)
			return;

		int32 index = FMath::RandRange(0, SoundWave.Num() - 1);

		bPlayingMusic = true;

		VolumeMax = VolumeMultiplier;

		//make sure we don't play the same song back to back
		if (CurrentSong == SoundWave[index])
		{
			index++;
			if (index >= SoundWave.Num())
				index = 0;
		}

		NextSong = SoundWave[index];
		MusicComponent->FadeOut(2.0f, 0.0f);

		GetWorldTimerManager().SetTimer(MusicTimer, this, &AOrionMusicManager::PlaySong, 2.0f, false);

		LastMusicType = Type;
	}
}

void AOrionMusicManager::PlaySong()
{
	if (!NextSong)
	{
		ResetMusic();
		return;
	}

	MusicComponent->SetSound(NextSong);
	MusicComponent->FadeIn(2.0f, VolumeMax);

	CurrentSong = NextSong;

	GetWorldTimerManager().SetTimer(MusicTimer, this, &AOrionMusicManager::ResetMusic, FMath::Max(1.0f, NextSong->Duration - 2.0f), false);
}

void AOrionMusicManager::ResetMusic()
{
	bPlayingMusic = false;
}

