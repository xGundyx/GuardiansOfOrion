// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameLobby.h"

AOrionGameLobby::AOrionGameLobby(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("/Game/Character/Blueprints/BaseLobbyPawn"));
	if (PlayerPawnObject.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> SpecObject(TEXT("/Game/Character/Blueprints/BaseSpectator"));
	if (SpecObject.Object != NULL)
	{
		SpectatorClass = (UClass*)SpecObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> PickupObject(TEXT("/Game/Pickups/Blueprints/BasePickup"));
	if (PickupObject.Object != NULL)
	{
		DefaultPickupClass = (UClass*)PickupObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> PickupOrbObject(TEXT("/Game/Pickups/Blueprints/BasePickupOrb"));
	if (PickupOrbObject.Object != NULL)
	{
		DefaultPickupOrbClass = (UClass*)PickupOrbObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> PickupCoinObject(TEXT("/Game/Pickups/Blueprints/BasePickupCoins"));
	if (PickupCoinObject.Object != NULL)
	{
		DefaultPickupCoinClass = (UClass*)PickupCoinObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint>MusicObject(TEXT("/Game/Music/GOOMusic/GOOMusicManager"));
	if (MusicObject.Object != NULL)
	{
		DefaultMusicClass = (UClass*)MusicObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerControllerObject(TEXT("/Game/Character/Blueprints/BasePlayerController"));
	if (PlayerControllerObject.Object != NULL)
	{
		PlayerControllerClass = (UClass*)PlayerControllerObject.Object->GeneratedClass;
	}
	//PlayerControllerClass = AOrionPlayerController::StaticClass();

	GameStateClass = AOrionGRI::StaticClass();

	PlayerStateClass = AOrionPRI::StaticClass();

	// use our custom HUD class
	//HUDClass = AOrionHUD::StaticClass();

	//this will keep players connected and keep their data persistent across map changes
	bUseSeamlessTravel = true;

	bAlwaysShowCursor = false;

	bTopDown = false;

	bSinglePlayer = false;

	//no team means every player is on the same side
	bTeamGame = false;

	TimeLimit = 0;
	WarmupTime = 0;
	bWarmingUp = true;

	// do this so the match doesn't accidently start on us
	bDelayedStart = true;

	DinoKillZ = -100000.0f;

	InactivePlayerStateLifeSpan = 0.1f;
}

void AOrionGameLobby::HandleEmptyServer(AController *Exiting)
{
	//if there are 0 players left after this player leaves, close the server
	TArray<AActor*> Controllers;
	int32 Counter = 0;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
		if (C)
			Counter++;
	}

	if (GEngine && Counter == 0 && !GetWorldTimerManager().IsTimerActive(EmptyServerTimer))
	{
		GetWorldTimerManager().SetTimer(EmptyServerTimer, this, &AOrionGameMode::CloseGame, 120.0f, false);
	}
	else if (GEngine && Counter > 0 && GetWorldTimerManager().IsTimerActive(EmptyServerTimer))
	{
		GetWorldTimerManager().ClearTimer(EmptyServerTimer);
	}
}

void AOrionGameLobby::InitGRI()
{
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (!GRI)
	{
		GetWorldTimerManager().SetTimer(GRITimer, this, &AOrionGameLobby::InitGRI, 0.1, false);
		return;
	}

	GRI->bTeamGame = bTeamGame;
	GRI->bAlwaysShowCursor = bAlwaysShowCursor;
	GRI->bTopDown = bTopDown;
	GRI->bIsLobby = true;
	GRI->bInitialized = true;

	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
		if (C)
		{
			C->SetThirdPerson();
		}
	}
}

void AOrionGameLobby::BeginPlay()
{
	Super::BeginPlay();

	if (GameSession)
		GameSession->MaxPlayers = 40;
}

void AOrionGameLobby::SetSpawnTimer()
{
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this, &AOrionGameLobby::HandleRespawns, 1.0f, true);
}

void AOrionGameLobby::HandleRespawns()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(*Iterator);
		if (PC && PC->IsValidLowLevel())
		{
			//ignore players who are waiting for auth
#if !WITH_EDITOR
			if (!PC->bAuthenticated)
				continue;
#endif
			AOrionPRI *PRI = Cast<AOrionPRI>(PC->PlayerState);

			if (!PRI)
				continue;

			if (((PC->IsInState(NAME_Spectating) && PC->bPlayerIsWaiting) || PC->IsInState(NAME_Inactive)) && !PC->IsFrozen())
			{
				RestartPlayer(PC);
			}
		}
	}

	HandleEmptyServer();
	HandleParties();
}

void AOrionGameLobby::HandleMatchHasStarted()
{
	GameSession->HandleMatchHasStarted();

	// start human players first
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = *Iterator;
		if ((PlayerController->GetPawn() == NULL) && PlayerCanRestart(PlayerController))
		{
			//RestartPlayer(PlayerController);
		}
	}

	// Make sure level streaming is up to date before triggering NotifyMatchStarted
	GEngine->BlockTillLevelStreamingCompleted(GetWorld());

	// First fire BeginPlay, if we haven't already in waiting to start match
	GetWorldSettings()->NotifyBeginPlay();

	// Then fire off match started
	GetWorldSettings()->NotifyMatchStarted();

	// if passed in bug info, send player to right location
	FString BugLocString = UGameplayStatics::ParseOption(OptionsString, TEXT("BugLoc"));
	FString BugRotString = UGameplayStatics::ParseOption(OptionsString, TEXT("BugRot"));
	if (!BugLocString.IsEmpty() || !BugRotString.IsEmpty())
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = *Iterator;
			if (PlayerController->CheatManager != NULL)
			{
				//`log( "BugLocString:" @ BugLocString );
				//`log( "BugRotString:" @ BugRotString );

				PlayerController->CheatManager->BugItGoString(BugLocString, BugRotString);
			}
		}
	}

	HandleRespawns();
	//GetWorldTimerManager().SetTimer(RespawnTimer, this, &AOrionGameTopDown::HandleRespawns, 25.0f, false);
}

FString AOrionGameLobby::InitNewPlayer(class APlayerController* NewPlayerController, const TSharedPtr<const FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal)
{
	FString Ret = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	//see if the player needs to join a party
	FString pfTeam = UGameplayStatics::ParseOption(Options, TEXT("TeamName"));

	if (pfTeam != "")
	{
		AddPlayerToParty(Cast<AOrionPlayerController>(NewPlayerController), pfTeam);
	}
	else
	{
		//add new players to empty solo parties, makes things easier:p
		int32 Rand = FMath::RandRange(0, 999999999);
		FString pName = FString::Printf(TEXT("%i"), Rand);
		CreateParty(Cast<AOrionPlayerController>(NewPlayerController), pName, "", "", "", "", "", "", "", "");
	}

	return Ret;
}

void AOrionGameLobby::CreateParty(AOrionPlayerController *Leader, FString PartyName, FString MapName, FString Diff, FString Gamemode, FString DiffScale, FString MinILevel, FString Region, FString TOD, FString Privacy)
{
	if (!Leader)
		return;

	//if this player is already in a party, don't allow this!
	AOrionPRI *PRI = Cast<AOrionPRI>(Leader->PlayerState);
	if (PRI)
	{
		if (PRI->MyParty.PartyName != "")
			return;

		FSpaceParty NewParty;

		NewParty.PartyName = PartyName;
		NewParty.MapName = MapName;
		NewParty.Difficulty = Diff;
		NewParty.GameMode = Gamemode;
		NewParty.DifficultyScale = DiffScale;
		NewParty.MinILevel = MinILevel;
		NewParty.Region = Region;
		NewParty.TOD = TOD;
		NewParty.Privacy = Privacy;
		NewParty.IP = "";
		NewParty.LobbyID = "";

		SpaceParties.Add(NewParty);

		AddPlayerToParty(Leader, PartyName);

		Leader->ClientPartyCreated(true);
	}
	else
		Leader->ClientPartyCreated(false);
}

void AOrionGameLobby::DestroyParty(FString PartyName)
{
	FSpaceParty FindParty;
	FindParty.PartyName = PartyName;

	int32 index = SpaceParties.Find(FindParty);

	if (index != INDEX_NONE)
	{
		for (int32 i = 0; i < SpaceParties[index].PartyMembers.Num(); i++)
		{
			if (SpaceParties[index].PartyMembers[i].PC)
			{
				SpaceParties[index].PartyMembers[i].PC->CurrentPartyName = "";
				FSpaceParty DummyParty;
				AOrionPRI *PRI = Cast<AOrionPRI>(SpaceParties[index].PartyMembers[i].PC->PlayerState);
				if (PRI)
					PRI->MyParty = DummyParty;
			}
		}
		SpaceParties.RemoveAt(index);
	}
}

void AOrionGameLobby::UpdatePartyPlayer(FString PartyName, AOrionPlayerController *Member, int32 sLevel, FString sClass)
{
	FSpaceParty FindParty;
	FindParty.PartyName = PartyName;

	int32 index = SpaceParties.Find(FindParty);

	if (index != INDEX_NONE && Member)
	{
		for (int32 i = 0; i < SpaceParties[index].PartyMembers.Num(); i++)
		{
			if (SpaceParties[index].PartyMembers[i].PC == Member)
			{
				SpaceParties[index].PartyMembers[i].PlayerClass = sClass;
				SpaceParties[index].PartyMembers[i].PlayerLevel = sLevel;
			}
		}

		for (int32 i = 0; i < SpaceParties[index].PartyMembers.Num(); i++)
		{
			if (SpaceParties[index].PartyMembers[i].PC)
			{
				AOrionPRI *aPRI = Cast<AOrionPRI>(SpaceParties[index].PartyMembers[i].PC->PlayerState);
				if (aPRI)
					aPRI->MyParty = SpaceParties[index];
			}
		}
	}
}

void AOrionGameLobby::AddPlayerToParty(AOrionPlayerController *Member, FString PartyName)
{
	FSpaceParty FindParty;
	FindParty.PartyName = PartyName;

	int32 index = SpaceParties.Find(FindParty);

	if (index != INDEX_NONE && Member)
	{
		//make sure this party is not full, send the client a message
		if (SpaceParties[index].PartyMembers.Num() >= 4)
			return;

		AOrionPRI *PRI = Cast<AOrionPRI>(Member->PlayerState);

		if (PRI)
		{
			//remove this player from any old parties he/she may have been in
			RemovePlayerFromParty(Member, PRI->MyParty.PartyName);

			FSpacePartyMember NewMember;
			NewMember.PC = Member;
			NewMember.PRI = Cast<AOrionPRI>(Member->PlayerState);
			NewMember.PlayerName = PRI->PlayFabName;
			NewMember.PlayerClass = PRI->ClassType;
			NewMember.PlayerLevel = PRI->GetCharacterLevelFromClass(PRI->ClassType);
			NewMember.PlayerILevel = Member->ILevel;

			SpaceParties[index].PartyMembers.AddUnique(NewMember);

			Member->CurrentPartyName = PartyName;

			for (int32 i = 0; i < SpaceParties[index].PartyMembers.Num(); i++)
			{
				if (SpaceParties[index].PartyMembers[i].PC)
				{
					AOrionPRI *aPRI = Cast<AOrionPRI>(SpaceParties[index].PartyMembers[i].PC->PlayerState);
					if (aPRI)
						aPRI->MyParty = SpaceParties[index];
				}
			}
		}
	}
}

void AOrionGameLobby::RemovePlayerFromParty(AOrionPlayerController *Member, FString PartyName)
{
	FSpaceParty FindParty;
	FindParty.PartyName = PartyName;

	int32 index = SpaceParties.Find(FindParty);

	if (index != INDEX_NONE)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(Member->PlayerState);

		if (PRI && Member && SpaceParties[index].PartyMembers.Num() > 1)
		{
			FSpacePartyMember NewMember;
			NewMember.PlayerName = PRI->PlayFabName;

			Member->CurrentPartyName = "";

			for (int32 i = 0; i < SpaceParties[index].PartyMembers.Num(); i++)
			{
				if (SpaceParties[index].PartyMembers[i].PC == Member)
				{
					FSpaceParty DummyParty;
					AOrionPRI *PRI = Cast<AOrionPRI>(SpaceParties[index].PartyMembers[i].PC->PlayerState);
					if (PRI)
						PRI->MyParty = DummyParty;
				}
			}
			FSpacePartyMember RemoveMember;
			RemoveMember.PC = Member;
			SpaceParties[index].PartyMembers.Remove(RemoveMember);

			for (int32 i = 0; i < SpaceParties[index].PartyMembers.Num(); i++)
			{
				if (SpaceParties[index].PartyMembers[i].PC)
				{
					AOrionPRI *aPRI = Cast<AOrionPRI>(SpaceParties[index].PartyMembers[i].PC->PlayerState);
					if (aPRI)
						aPRI->MyParty = SpaceParties[index];
				}
			}
		}
	}
}

void AOrionGameLobby::KickPlayerFromParty(AOrionPRI *Player, const FString &PartyName)
{
	FSpaceParty FindParty;
	FindParty.PartyName = PartyName;

	int32 index = SpaceParties.Find(FindParty);

	if (index != INDEX_NONE)
	{
		if (Player && SpaceParties[index].PartyMembers.Num() > 0)
		{
			//can't kick the leader! he has to leave on his own
			if (Player == SpaceParties[index].PartyMembers[0].PRI)
				return;

			FSpaceParty DummyParty;
			Player->MyParty = DummyParty;
			
			FSpacePartyMember RemoveMember;
			RemoveMember.PRI = Player;
			SpaceParties[index].PartyMembers.Remove(RemoveMember);

			for (int32 i = 0; i < SpaceParties[index].PartyMembers.Num(); i++)
			{
				if (SpaceParties[index].PartyMembers[i].PC)
				{
					AOrionPRI *aPRI = Cast<AOrionPRI>(SpaceParties[index].PartyMembers[i].PC->PlayerState);
					if (aPRI)
						aPRI->MyParty = SpaceParties[index];
				}
			}

			TArray<AActor*> Controllers;
			int32 Counter = 0;

			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

			for (int32 i = 0; i < Controllers.Num(); i++)
			{
				AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
				if (C && C->PlayerState == Player)
				{
					int32 Rand = FMath::RandRange(0, 999999999);
					FString pName = FString::Printf(TEXT("%i"), Rand);
					CreateParty(C, pName, "", "", "", "", "", "", "", "");

					return;
				}
			}
		}
	}
}

//make sure parties are all valid
void AOrionGameLobby::HandleParties()
{
	for (int32 i = 0; i < SpaceParties.Num(); i++)
	{
		if (SpaceParties[i].PartyMembers.Num() == 0)
			DestroyParty(SpaceParties[i].PartyName);
	}
}

//invite a sever player to our party
void AOrionGameLobby::InvitePlayerToLobby(AOrionPRI *Player, const FString &PartyName, const FString &Inviter)
{
	FSpaceParty FindParty;
	FindParty.PartyName = PartyName;

	int32 index = SpaceParties.Find(FindParty);

	if (index != INDEX_NONE)
	{
		//is party full?
		if (SpaceParties[index].PartyMembers.Num() >= 4)
			return;

		//send this player an invite request
		TArray<AActor*> Controllers;
		int32 Counter = 0;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

		for (int32 i = 0; i < Controllers.Num(); i++)
		{
			AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
			if (C && C->PlayerState == Player)
			{
				C->ReceivePartyInvite(Inviter, PartyName);
			}
		}
	}
	else
	{
		//party does not exist:x
	}
}

//update party settings
void AOrionGameLobby::UpdatePartySettings(AOrionPlayerController *Leader, FString PartyName, FString MapName, FString Diff, FString Gamemode, FString DiffScale, FString MinILevel, FString Region, FString TOD, FString Privacy, const FString &IP, const FString &LID)
{
	FSpaceParty FindParty;
	FindParty.PartyName = PartyName;

	int32 index = SpaceParties.Find(FindParty);

	if (index != INDEX_NONE)
	{
		SpaceParties[index].PartyName = PartyName;
		SpaceParties[index].MapName = MapName;
		SpaceParties[index].Difficulty = Diff;
		SpaceParties[index].GameMode = Gamemode;
		SpaceParties[index].DifficultyScale = DiffScale;
		SpaceParties[index].MinILevel = MinILevel;
		SpaceParties[index].Region = Region;
		SpaceParties[index].TOD = TOD;
		SpaceParties[index].Privacy = Privacy;
		SpaceParties[index].IP = IP;
		SpaceParties[index].LobbyID = LID;

		for (int32 i = 0; i < SpaceParties[index].PartyMembers.Num(); i++)
		{
			if (SpaceParties[index].PartyMembers[i].PRI)
			{
				SpaceParties[index].PartyMembers[i].PRI->MyParty = SpaceParties[index];
			}
		}
	}
}



