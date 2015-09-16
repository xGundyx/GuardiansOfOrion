

#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionMusicManager.h"
#include "OrionGRI.h"


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
	DOREPLIFETIME(AOrionGRI, bVictory);
	DOREPLIFETIME(AOrionGRI, bDefeat);
	DOREPLIFETIME(AOrionGRI, GlobalMessage);
	/*DOREPLIFETIME(AShooterGameState, NumTeams);
	DOREPLIFETIME(AShooterGameState, RemainingTime);
	DOREPLIFETIME(AShooterGameState, bTimerPaused);
	DOREPLIFETIME(AShooterGameState, TeamScores);*/
}

void AOrionGRI::HandleVictoryDefeat()
{
	//get the local player controller and handle any messages
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());

	if (PC)
	{
		if (bVictory)
			PC->ShowGlobalMessage(TEXT("VICTORY"));
		else if (bDefeat)
			PC->ShowGlobalMessage(TEXT("DEFEAT"));
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
			WorldTime = Weather->TheTime;
	}
}

FTimeOfDay AOrionGRI::GetWorldTime() const
{
	return WorldTime;
}

void AOrionGRI::BeginPlay()
{
	Super::BeginPlay();

	bVictory = false;
	bDefeat = false;

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
			Info.Title = FString::Printf(TEXT("Wave %i"), WaveNum);
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
			Info.Title = TEXT("Enemies Alive");
			Info.Desc = FString::Printf(TEXT("%i"), DinosAliveInWave);
		}
		break;
	}

	return Info;
}

