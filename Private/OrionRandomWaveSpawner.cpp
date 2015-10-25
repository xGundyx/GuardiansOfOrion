#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionRandomWaveSpawner.h"

////TArray< TSubclassOf<class AOrionCharacter> > AOrionRandomWaveSpawner::SpawnClasses;

// Sets default values
AOrionRandomWaveSpawner::AOrionRandomWaveSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	//PreviewMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Preview"));
	//PreviewMesh->AlwaysLoadOnClient = true;
	//PreviewMesh->AlwaysLoadOnServer = true;
	//PreviewMesh->bHiddenInGame = true;

	//RootComponent = PreviewMesh;

	bRaptor = true;
	bCompy = true;
	bDilo = true;
	bTRex = true;
	bTriceratops = true;
	bRham = true;
	bPtera = true;
	bAnky = true;
	bKruger = true;
	bJeckyl = true;
	bNamor = true;
	bViper = true;
	bOrbDroid = true;
	bBones = true;
	bAssault = true;
	bSupport = true;
	bRecon = true;
	bTech = true;
	bDemo = true;

	SpawnRadius = 5000.0f;
	MaxToSpawnPerFrame = 10;

	SetActorHiddenInGame(true);

	static ConstructorHelpers::FObjectFinder<UBlueprint> FilterObject(TEXT("/Game/AI/Blueprints/NavFilter_LargeDino.NavFilter_LargeDino"));
	if (FilterObject.Object != NULL)
	{
		DefaultFilterClass = (UClass*)FilterObject.Object->GeneratedClass;
	}
}

TSubclassOf<AOrionCharacter> AOrionRandomWaveSpawner::GetBlueprintFromPath(FString path)
{
	FStringAssetReference ref(path);

	UObject* obj = ref.TryLoad();//.ResolveObject();

	if (obj)
	{
		UBlueprint* gen = Cast<UBlueprint>(obj);
		if (gen)
		{
			TSubclassOf<AOrionCharacter> CharClass = *gen->GeneratedClass;
			return CharClass;
		}
	}

	return nullptr;
}

void AOrionRandomWaveSpawner::BeginPlay()
{
	Super::BeginPlay();

	ClassEnabled.Add(bRaptor);
	ClassEnabled.Add(bCompy);
	ClassEnabled.Add(bDilo);
	ClassEnabled.Add(bTRex);
	ClassEnabled.Add(bTriceratops);
	ClassEnabled.Add(bRham);
	ClassEnabled.Add(bPtera);
	ClassEnabled.Add(bAnky);
	ClassEnabled.Add(bKruger);
	ClassEnabled.Add(bJeckyl);
	ClassEnabled.Add(bNamor);
	ClassEnabled.Add(bViper);
	ClassEnabled.Add(bOrbDroid);
	ClassEnabled.Add(bBones);
	ClassEnabled.Add(bAssault);
	ClassEnabled.Add(bSupport);
	ClassEnabled.Add(bRecon);
	ClassEnabled.Add(bTech);
	ClassEnabled.Add(bDemo);

	SpawnClasses.Empty();

	//this must match EAISpawnType enum
	//if (SpawnClasses.Num() == 0)
	//{
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Raptor.bot_Raptor'")));//raptor
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Compy.bot_Compy'")));//compy
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Dilo.bot_Dilo'")));//dilo
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_TRex.bot_TRex'")));//trex
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Triceratops.bot_Triceratops'")));//trike
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Rham.bot_Rham'")));//rham
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Raptor.bot_Raptor'")));//ptera
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Anky.bot_Anky'")));//anky
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Creatures/Blueprints/KrugerSpawn.KrugerSpawn'")));//kruger
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Creatures/Blueprints/JeckylBot.JeckylBot'")));//jeckyl
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Creatures/Blueprints/NamorBot.NamorBot'")));//namor
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Raptor.bot_Raptor'")));//viper
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Bots/Blueprints/OrbBot.OrbBot'")));//orbdroid
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Bots/Blueprints/BonesBot.BonesBot'")));//bones
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Raptor.bot_Raptor'")));//assault
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Raptor.bot_Raptor'")));//support
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Raptor.bot_Raptor'")));//recon
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Raptor.bot_Raptor'")));//tech
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Raptor.bot_Raptor'")));//demo
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Raptor.bot_Raptor'")));//allo
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Raptor.bot_Raptor'")));//para
		SpawnClasses.Add(GetBlueprintFromPath(TEXT("Blueprint'/Game/Dinosaurs/Blueprints/bot_Raptor.bot_Raptor'")));//steg
	//}
}

void AOrionRandomWaveSpawner::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AOrionRandomWaveSpawner::SpawnWave(int32 TypesToSpawn[SPAWN_NUM], AActor *Focus)
{
	if (Role != ROLE_Authority)
		return;

	if (GetWorld() == nullptr || GetWorld()->GetNavigationSystem() == nullptr)
		return;

	FocusActor = Focus;

	FailedToSpawn.Empty();

	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

	for (int32 i = 0; i < SPAWN_NUM; i++)
	{
		////if (!ClassEnabled[i])
		////	continue;

		for (int32 j = 0; j < TypesToSpawn[i]; j++)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			FVector Loc;

			Loc = GetWorld()->GetNavigationSystem()->GetRandomReachablePointInRadius(GetWorld(), GetActorLocation(), 1000.0f/*SpawnRadius*/, (ANavigationData*)0, DefaultFilterClass);

			TSharedPtr<const FNavigationQueryFilter> QueryFilter = UNavigationQueryFilter::GetQueryFilter(GetWorld()->GetNavigationSystem()->MainNavData, DefaultFilterClass);

			if (!GetWorld()->GetNavigationSystem()->TestPathSync(FPathFindingQuery(nullptr, GetWorld()->GetNavigationSystem()->MainNavData, GetActorLocation(), Loc, QueryFilter)))
				continue;

			AOrionCharacter* NewPawn = GetWorld()->SpawnActor<AOrionCharacter>(SpawnClasses[i], Loc + FVector(0, 0, 150.0f), GetActorRotation(), SpawnInfo);
			if (NewPawn)
			{
				if (Game)
					Game->AddActiveWaveEnemy(NewPawn);

				NewPawn->bRun = true;
				//NewPawn->FocusArea = FocusLocation;
				NewPawn->FocusActor = FocusActor;
				NewPawn->SpawnDefaultController();
				NewPawn->SetAIType(AI_HUNTING);
				NewPawn->SpawnType = i;
			}
			else
			{
				FailedToSpawn.Add(i);
				continue;
			}
		}
	}

	if (FailedToSpawn.Num() > 0)
	{
		GetWorldTimerManager().SetTimer(FailedTimer, this, &AOrionRandomWaveSpawner::SpawnFailures, 1.0f, false);
	}
}

void AOrionRandomWaveSpawner::SpawnFailures()
{
	TArray<int32> MoreFailures;

	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

	for (int32 i = 0; i < FailedToSpawn.Num(); i++)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		FVector Loc;

		Loc = GetWorld()->GetNavigationSystem()->GetRandomReachablePointInRadius(GetWorld(), GetActorLocation(), 1000.0f/*SpawnRadius*/, (ANavigationData*)0, DefaultFilterClass);

		AOrionCharacter* NewPawn = GetWorld()->SpawnActor<AOrionCharacter>(SpawnClasses[FailedToSpawn[i]], Loc + FVector(0, 0, 150.0f), GetActorRotation(), SpawnInfo);
		if (NewPawn)
		{
			if (Game)
				Game->AddActiveWaveEnemy(NewPawn);

			NewPawn->bRun = true;
			NewPawn->FocusActor = FocusActor;
			NewPawn->SpawnDefaultController();
			NewPawn->SetAIType(AI_HUNTING);
			NewPawn->SpawnType = FailedToSpawn[i];
		}
		else
		{
			MoreFailures.Add(FailedToSpawn[i]);
			continue;
		}
	}

	FailedToSpawn.Empty();
	FailedToSpawn = MoreFailures;

	if (FailedToSpawn.Num() > 0)
	{
		GetWorldTimerManager().SetTimer(FailedTimer, this, &AOrionRandomWaveSpawner::SpawnFailures, 1.0f, false);
	}
}
