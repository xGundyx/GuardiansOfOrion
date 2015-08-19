// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
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
		EMusicType Type = MUSIC_NONE;
		float VolumeMultiplier = 1.0f;

		//only change music if a gameplay type has change, or if the previous song has ended
		if (GRI->DinosAliveInWave > 0)
			Type = MUSIC_GAMEPLAY;
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

		case MUSIC_AMBIENT:
			SoundWave = AmbientMusic;
			VolumeMultiplier = 0.1f;
			break;
		}

		if (SoundWave.Num() <= 0)
			return;

		int32 index = FMath::RandRange(0, SoundWave.Num() - 1);

		bPlayingMusic = true;
		
		MusicComponent->SetSound(SoundWave[index]);
		MusicComponent->VolumeMultiplier = VolumeMultiplier;
		MusicComponent->Play(0.0f);

		GetWorldTimerManager().SetTimer(MusicTimer, this, &AOrionMusicManager::ResetMusic, SoundWave[index]->Duration, true);

		LastMusicType = Type;
	}
}

void AOrionMusicManager::ResetMusic()
{
	bPlayingMusic = false;
}

