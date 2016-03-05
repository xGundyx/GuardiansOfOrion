

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
	ItemLevel = 1;
	TimesHarvDowned = 0;
	TotalLimbsBlownOff = 0;
	ServerLocation = TEXT("US-EAST");
	bStatsEnabled = false;
	bIsLobby = false;
	bIsLocalCoop = false;
	bInitialized = false;

	HarvKills = 0; 
}

bool AOrionGRI::HarvHasMostKills()
{
	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
		if (C)
		{
			AOrionPRI *PRI = Cast<AOrionPRI>(C->PlayerState);
			if (PRI && PRI->Kills >= HarvKills)
				return false;
		}
	}

	return true;
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
	DOREPLIFETIME(AOrionGRI, bNightTime);
	DOREPLIFETIME(AOrionGRI, HarvKills);
	DOREPLIFETIME(AOrionGRI, ItemLevel);
	DOREPLIFETIME(AOrionGRI, OrionGameMode);
	DOREPLIFETIME(AOrionGRI, bReadyingUp);
	DOREPLIFETIME(AOrionGRI, bIsLobby);
	DOREPLIFETIME(AOrionGRI, bIsLocalCoop);
	DOREPLIFETIME(AOrionGRI, BonusTime);
	DOREPLIFETIME(AOrionGRI, bMatchIsOver);
	DOREPLIFETIME(AOrionGRI, bBonusWon);
}

void AOrionGRI::StartVote(EVoteType Type)
{
	//send a message to each client, asking for their vote
	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	VoteType = Type;
	Voters.Empty();

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
		if (C)
		{
			Voters.Add(C);
			C->VoteIndex = -1;
			C->ShowVoteOptions(Type);
		}
	}

	//vote lasts 2 minutes or until everyone has successfully voted
	GetWorldTimerManager().SetTimer(VoteTimer, this, &AOrionGRI::VoteEnded, 120.0, false);
}

void AOrionGRI::VoteEnded()
{
	int32 VoteTally[10];
	TArray<FString> Maps;

	for (int32 i = 0; i < 10; i++)
		VoteTally[i] = 0;

	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
		if (C)
		{
			if (C->VoteIndex >= 0 && C->VoteIndex < 10)
				VoteTally[C->VoteIndex]++;

			if (VoteType == VOTE_MAP)
				Maps = C->GetMaps();
		}
	}

	Voters.Empty();

	int32 MaxVote = 0;
	bool bTie;

	for (int32 i = 0; i < 10; i++)
	{
		if (VoteTally[i] == VoteTally[MaxVote])
			bTie = true;
		else if (VoteTally[i] > VoteTally[MaxVote])
		{
			MaxVote = i;
			bTie = false;
		}
	}

	//award the winner based on type
	switch (VoteType)
	{
	case VOTE_MAP:
		if (MaxVote < Maps.Num())
			NextMapVote = Maps[MaxVote];
		else
			NextMapVote = Maps[FMath::RandRange(0, Maps.Num() - 1)];

		//start new map in 10 seconds
		break;
	}
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

	//handle the voting situation
	if (GetWorldTimerManager().IsTimerActive(VoteTimer))
	{

	}

	//calculate the camera information here and the player will just grab the results
	if (bIsLocalCoop)
	{
		//FVector vCenter(0.0f);
		//int32 nPlayers = 0;

		FVector MinBounds(999999.0f, 999999.0f, 999999.0f);
		FVector MaxBounds(-999999.0f, -999999.0f, -999999.0f);

		TArray<FVector> Points;

		TArray<AActor*> Controllers;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

		FRotator rot = FRotator(-50.0f/*CameraPitch*/, 45.0f/*CameraYaw*/, 0.0f);

		for (int32 i = 0; i < Controllers.Num(); i++)
		{
			AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
			if (C)
			{
				AOrionCharacter *P = Cast<AOrionCharacter>(C->GetPawn());
				if (P)
				{
					//vCenter += P->GetActorLocation();
					//nPlayers++;

					//MinBounds.X = FMath::Min(MinBounds.X, P->GetActorLocation().X);
					//MinBounds.Y = FMath::Min(MinBounds.Y, P->GetActorLocation().Y);

					//MaxBounds.X = FMath::Max(MaxBounds.X, P->GetActorLocation().X);
					//MaxBounds.Y = FMath::Max(MaxBounds.Y, P->GetActorLocation().Y);

					Points.Add(FVector(FVector::DotProduct(P->GetActorLocation(), FRotationMatrix(rot).GetScaledAxis(EAxis::X)),
						FVector::DotProduct(P->GetActorLocation(), FRotationMatrix(rot).GetScaledAxis(EAxis::Y)),
						FVector::DotProduct(P->GetActorLocation(), FRotationMatrix(rot).GetScaledAxis(EAxis::Z))
						));
				}
			}
		}

		for (int32 i = 0; i < Points.Num(); i++)
		{
			MinBounds.X = FMath::Min(MinBounds.X, Points[i].X);
			MinBounds.Y = FMath::Min(MinBounds.Y, Points[i].Y);
			MinBounds.Z = FMath::Min(MinBounds.Z, Points[i].Z);

			MaxBounds.X = FMath::Max(MaxBounds.X, Points[i].X);
			MaxBounds.Y = FMath::Max(MaxBounds.Y, Points[i].Y);
			MaxBounds.Z = FMath::Max(MaxBounds.Z, Points[i].Z);
		}

		//if (nPlayers > 0)
		//	vCenter /= nPlayers;

		//use the max/min to calculate our camera distance with an fov of 60.0f
		float Distance = 2500.0f;

		//convert our camera location from camera space back to world space
		FVector vCenter = FVector((MaxBounds.X + MinBounds.X) / 2.0f, (MaxBounds.Y + MinBounds.Y) / 2.0f, (MaxBounds.Z + MinBounds.Z) / 2.0f);
		FVector wCenter = FVector(FVector::DotProduct(vCenter, FRotationMatrix(rot.GetInverse()).GetScaledAxis(EAxis::X)),
			FVector::DotProduct(vCenter, FRotationMatrix(rot.GetInverse()).GetScaledAxis(EAxis::Y)),
			FVector::DotProduct(vCenter, FRotationMatrix(rot.GetInverse()).GetScaledAxis(EAxis::Z)));

		CameraCenter = wCenter;

		float Scale = 2.5f;
		int32 x = 0;
		int32 y = 0;
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());
		if (PC)
		{
			PC->GetViewportSize(x, y);
			Scale = 2.0f * float(x) / float(y);
		}

		//add some extra buffer length to the bounds
		MaxBounds *= FVector(1.0f, 1.25f, Scale);
		MinBounds *= FVector(1.0f, 1.25f, Scale);

		Distance = FMath::Max(Distance, FMath::Max(MaxBounds.Y - MinBounds.Y, MaxBounds.Z - MinBounds.Z) / (2.0f * FMath::Tan(FMath::DegreesToRadians(30.0f))));

		CoopCameraLocation = wCenter - rot.Vector() * Distance/*CameraDistance*/ + FVector(0.0f/*CameraDistX*/, 0.0f, 0.0f);
		CoopCameraRotation = rot;
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

	bMatchIsOver = false;

	//initialize our mapper
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Mapper = GetWorld()->SpawnActor<AOrionPlayFabInventoryMapper>(DefaultMapperClass, SpawnInfo);

	AOrionGameMode *G = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());
	if (G)
		bNightTime = G->bNightTime;

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

