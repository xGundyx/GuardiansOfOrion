

#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionMusicManager.h"
#include "OrionGRI.h"

#define MAX_RAGDOLLS 7

AOrionGRI::AOrionGRI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> MapperObject(TEXT("/Game/InventoryLists/GooInventoryMapper"));
	if (MapperObject.Object != NULL)
	{
		DefaultMapperClass = (UClass*)MapperObject.Object->GeneratedClass;
	}

	PrimaryActorTick.bCanEverTick = true;
	bAlwaysShowCursor = false;
	bTeamGame = false;

	GameOverCountDown = -1;
	SecondsTillNextWave = -1;

	bPlayingIdleMovie = false;

	Difficulty = DIFF_MEDIUM;
	TimesHarvDowned = 0;
	TotalLimbsBlownOff = 0;
	ServerLocation = TEXT("US-EAST");
	bStatsEnabled = false;
}

void AOrionGRI::AddRagdoll(AOrionCharacter *Ragdoll)
{
	ActiveRagdolls.AddUnique(Ragdoll);

	ValidateRagdollArray();

	if (ActiveRagdolls.Num() > MAX_RAGDOLLS)
	{
		//if(ActiveRagdolls[0]->Destroy())
		ActiveRagdolls[0]->TurnOff();
		ActiveRagdolls[0]->SetActorHiddenInGame(true);
		ActiveRagdolls[0]->SetLifeSpan(1.0f);
		ActiveRagdolls.RemoveAt(0);
	}
}

void AOrionGRI::ValidateRagdollArray()
{
	for (auto Itr(ActiveRagdolls.CreateIterator()); Itr; ++Itr)
	{
		AOrionCharacter *rag = *Itr;

		if (!rag || !rag->IsValidLowLevel())
			ActiveRagdolls.Remove(rag);
	}
}

void AOrionGRI::RemoveRagdoll(AOrionCharacter *Ragdoll)
{
	ActiveRagdolls.Remove(Ragdoll);
}

void AOrionGRI::OnRep_WaveNum()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());

	if (PC)
		PC->EventSetWave(WaveNum < 10 ? FString::Printf(TEXT("WAVE 0%i"), WaveNum) : FString::Printf(TEXT("WAVE %i"), WaveNum));
}

void AOrionGRI::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOrionGRI, WorldTime);
	DOREPLIFETIME(AOrionGRI, bAlwaysShowCursor);
	DOREPLIFETIME(AOrionGRI, bTopDown);
	DOREPLIFETIME(AOrionGRI, bTeamGame);
	DOREPLIFETIME(AOrionGRI, Teams);
	DOREPLIFETIME(AOrionGRI, Weather);
	DOREPLIFETIME(AOrionGRI, WarmupTimeRemaining);
	DOREPLIFETIME(AOrionGRI, MatchTimeRemaining);
	DOREPLIFETIME(AOrionGRI, WaveNum);
	DOREPLIFETIME(AOrionGRI, TotalWaves);
	DOREPLIFETIME(AOrionGRI, DinosAliveInWave);
	DOREPLIFETIME(AOrionGRI, MissionObjective);
	DOREPLIFETIME(AOrionGRI, bSideMission);
	DOREPLIFETIME(AOrionGRI, bBossMission);
	DOREPLIFETIME(AOrionGRI, MissionLocation);
	DOREPLIFETIME(AOrionGRI, PlayerList);
	DOREPLIFETIME(AOrionGRI, GameOverMessage);
	DOREPLIFETIME(AOrionGRI, GlobalMessage);
	DOREPLIFETIME(AOrionGRI, HeaderMessage);
	DOREPLIFETIME(AOrionGRI, GameOverCountDown);
	DOREPLIFETIME(AOrionGRI, SecondsTillNextSpawn);
	DOREPLIFETIME(AOrionGRI, SecondsTillNextWave);
	/*DOREPLIFETIME(AShooterGameState, NumTeams);
	DOREPLIFETIME(AShooterGameState, RemainingTime);
	DOREPLIFETIME(AShooterGameState, bTimerPaused);
	DOREPLIFETIME(AShooterGameState, TeamScores);*/
	DOREPLIFETIME(AOrionGRI, TimesHarvDowned);
	DOREPLIFETIME(AOrionGRI, TotalLimbsBlownOff);
	DOREPLIFETIME(AOrionGRI, ServerLocation);
	DOREPLIFETIME(AOrionGRI, bStatsEnabled);
	DOREPLIFETIME(AOrionGRI, Difficulty);
	DOREPLIFETIME(AOrionGRI, MapName);
	DOREPLIFETIME(AOrionGRI, PhotonGUID);
}

void AOrionGRI::HandleVictoryDefeat()
{
	//get the local player controller and handle any messages
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());

	if (PC)
	{
		PC->ShowGlobalMessage(GameOverMessage);
	}
}

void AOrionGRI::OnRep_GlobalMessage()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());

	if (PC)
		PC->ShowGlobalMessage(GlobalMessage);
}

void AOrionGRI::SetGlobalMessage(FString msg)
{
	GlobalMessage = msg;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());

	if (PC)
		PC->ShowGlobalMessage(GlobalMessage);
}

void AOrionGRI::OnRep_HeaderMessage()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());

	if (PC)
		PC->ShowGlobalMessageHeader(HeaderMessage.Title, HeaderMessage.Desc);
}

void AOrionGRI::SetHeaderMessage(FString Title, FString Desc)
{
	HeaderMessage.Title = Title;
	HeaderMessage.Desc = Desc;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());

	if (PC)
		PC->ShowGlobalMessageHeader(HeaderMessage.Title, HeaderMessage.Desc);
}

bool AOrionGRI::IsTopDownGame()
{
	return bTopDown;
}

void AOrionGRI::SetWeather(AOrionWeather *theWeather)
{
	Weather = theWeather;
}

bool AOrionGRI::OnSameTeam(AOrionPRI *Actor1, AOrionPRI *Actor2)
{
	//if (!bTeamGame)
	//	return false;

	if (!Actor1 || !Actor2)
		return false;

	if (Actor1->GetTeamIndex() == Actor2->GetTeamIndex())
		return true;

	return false;
}

void AOrionGRI::Tick(float DeltaSeconds)
{
	if (Weather)
	{
		Weather->UpdateWeather(DeltaSeconds);
		if (Role == ROLE_Authority)
		{
			WorldTime = Weather->TheTime;
		}
	}
}

FString AOrionGRI::GetMapName()
{
	FString Map = GetWorld()->GetMapName().ToUpper();

	int32 index = Map.Find("GOO-");

	if (index > INDEX_NONE)
		Map = Map.Mid(index + 4);

	return Map;
}

FTimeOfDay AOrionGRI::GetWorldTime() const
{
	return WorldTime;
}

void AOrionGRI::BeginPlay()
{
	Super::BeginPlay();

	//initialize our mapper
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Mapper = GetWorld()->SpawnActor<AOrionPlayFabInventoryMapper>(DefaultMapperClass, SpawnInfo);

	//spawn us a music manager, only do this client side
	if (GetWorld()->GetNetMode() == ENetMode::NM_Client || GetWorld()->GetNetMode() == ENetMode::NM_ListenServer || GetWorld()->GetNetMode() == ENetMode::NM_Standalone)
	{
		AOrionGameMode *Game = Cast<AOrionGameMode>(AOrionGameMode::StaticClass()->GetDefaultObject());
		if (Game)
		{
			MusicManager = GetWorld()->SpawnActor<AOrionMusicManager>(Game->DefaultMusicClass, SpawnInfo);
		}
	}
}

bool AOrionGRI::AddPlayerToTeam(AOrionPlayerController *PC, int32 Index)
{
	if (Role == ROLE_Authority && Teams.Num() <= 0)
		InitTeams();

	if (!PC || Teams.Num() <= Index)
		return false;

	for (int32 i = 0; i < Teams.Num(); i++)
	{
		if (Teams[i].TeamIndex == Index)
		{
			Teams[i].TeamMembers.AddUnique(Cast<AOrionPRI>(PC->PlayerState));
			AOrionPRI *PRI = Cast<AOrionPRI>(PC->PlayerState);

			if (PRI)
				PRI->SetTeamIndex(Index);

			return true;
		}
	}

	return false;
}

void AOrionGRI::RemovePlayerFromTeam(AOrionPlayerController *PC, int32 Index)
{
	if (!PC || Teams.Num() <= Index)
		return;

	for (int32 i = 0; i < Teams.Num(); i++)
	{
		if (Teams[i].TeamIndex == Index)
		{
			Teams[i].TeamMembers.Remove(Cast<AOrionPRI>(PC->PlayerState));
			return;
		}
	}
}

void AOrionGRI::InitTeams()
{
	Teams.Empty();

	FTeamInfo newTeam;

	//Purple Team
	newTeam.TeamIndex = 0;
	Teams.Add(newTeam);

	//Black Team
	newTeam.TeamIndex = 1;
	Teams.Add(newTeam);

	//Random Dino Team
	////newTeam.TeamIndex = 2;
	////Teams.Add(newTeam);
}

FMissionInfo AOrionGRI::GetMission(int32 Index)
{
	FMissionInfo Info;

	switch (Index)
	{
	//mission 1 is always the main objective you are currently on
	case 1:
		Info.Title = MissionObjective != TEXT("") ? (bSideMission ? TEXT("SIDE OBJECTIVE") : TEXT("MAIN OBJECTIVE")) : TEXT("");
		Info.Desc = MissionObjective;
		break;
	//sub mission or wave counter
	case 2:
		if (bSideMission)
		{
			Info.Title = TEXT("SIDE OBJECTIVE");
			Info.Desc = FString::Printf(TEXT("ENEMIES - %i"), DinosAliveInWave);
		}
		else if (DinosAliveInWave <= 0)
		{
			Info.Title = TEXT("");
			Info.Desc = TEXT("");
		}
		else
		{
			Info.Title = WaveNum < 10 ? FString::Printf(TEXT("WAVE 0%i"), WaveNum) : FString::Printf(TEXT("WAVE %i"), WaveNum);
			Info.Desc = Info.Desc = FString::Printf(TEXT("%i ENEMIES"), DinosAliveInWave);
		}
		break;
	case 3:
		if (DinosAliveInWave <= 0 || bSideMission)
		{
			Info.Title = TEXT("");
			Info.Desc = TEXT("");
		}
		else
		{
			Info.Title = TEXT("ENEMIES ALIVE");
			Info.Desc = FString::Printf(TEXT("%i"), DinosAliveInWave);
		}
		break;
	}

	return Info;
}

