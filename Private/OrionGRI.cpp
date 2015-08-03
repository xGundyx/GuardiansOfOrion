

#include "Orion.h"
#include "OrionGameMode.h"
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

	//initialize our mapper
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Mapper = GetWorld()->SpawnActor<AOrionPlayFabInventoryMapper>(DefaultMapperClass, SpawnInfo);
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

