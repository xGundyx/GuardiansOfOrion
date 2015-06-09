

#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionGRI.h"


AOrionGRI::AOrionGRI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
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
	/*DOREPLIFETIME(AShooterGameState, NumTeams);
	DOREPLIFETIME(AShooterGameState, RemainingTime);
	DOREPLIFETIME(AShooterGameState, bTimerPaused);
	DOREPLIFETIME(AShooterGameState, TeamScores);*/
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
	if (!bTeamGame)
		return false;

	if (!Actor1 || !Actor2)
		return false;

	if (Actor1->GetTeamIndex() == Actor2->GetTeamIndex())
		return true;

	return false;
}

void AOrionGRI::Tick(float DeltaSeconds)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	if (Weather)
	{
		Weather->UpdateWeather(DeltaSeconds);
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

	if (Role == ROLE_Authority)
		InitTeams();
}

bool AOrionGRI::AddPlayerToTeam(AOrionPlayerController *PC, int32 Index)
{
	for (int32 i = 0; i < Teams.Num(); i++)
	{
		if (Teams[i].TeamIndex == Index)
		{
			Teams[i].TeamMembers.AddUnique(Cast<AOrionPRI>(PC->PlayerState));
			return true;
		}
	}

	return false;
}

void AOrionGRI::RemovePlayerFromTeam(AOrionPlayerController *PC, int32 Index)
{
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
	FTeamInfo newTeam;

	//Purple Team
	newTeam.TeamIndex = 0;
	Teams.Add(newTeam);

	//Black Team
	newTeam.TeamIndex = 1;
	Teams.Add(newTeam);

	//Random Dino Team
	newTeam.TeamIndex = 2;
	Teams.Add(newTeam);
}

