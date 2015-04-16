#include "OrionTypes.generated.h"
#pragma once

#define ORION_SURFACE_Default		SurfaceType_Default
#define ORION_SURFACE_Concrete		SurfaceType1
#define ORION_SURFACE_Dirt			SurfaceType2
#define ORION_SURFACE_Water			SurfaceType3
#define ORION_SURFACE_Metal			SurfaceType4
#define ORION_SURFACE_Wood			SurfaceType5
#define ORION_SURFACE_Grass			SurfaceType6
#define ORION_SURFACE_Glass			SurfaceType7
#define ORION_SURFACE_Flesh			SurfaceType8

USTRUCT()
struct FDecalData
{
	GENERATED_USTRUCT_BODY()

	/** material */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
		UMaterial* DecalMaterial;

	/** quad size (width & height) */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
		float DecalSize;

	/** lifespan */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
		float LifeSpan;

	/** defaults */
	FDecalData()
		: DecalSize(256.f)
		, LifeSpan(10.f)
	{
	}
};

UENUM()
enum EFoodType
{
	FOOD_GRASS,
	FOOD_FRUIT,
	FOOD_VEGETABLE,
	FOOD_DEADMEATSMALL,
	FOOD_DEADMEATLARGE
};

USTRUCT(BlueprintType)
struct FPrimaryStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		int32 Dexterity;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		int32 Strength;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		int32 Intelligence;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		int32 Discipline;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		int32 Vitality;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		int32 Power;
};

USTRUCT(BlueprintType)
struct FSecondaryStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float CriticalHitChance;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float CriticalHitMultiplier;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float BluntDamageReduction;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float PiercingDamageReduction;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float FireDamageReduction;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float IceDamageReduction;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float LightningDamageReduction;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float EnergyDamageReduction;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float ExplosionDamageReduction;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float PoisonDamageReduction;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float MovementSpeed;
};

USTRUCT(BlueprintType)
struct FBonusStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float BonusShotgunDamage;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float BonusPistolDamage;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float BonusSMGDamage;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float BonusSniperDamage;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float BonusAutoRifleDamage;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float ReloadSpeed;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		int32 BonusJumpCharges;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float BonusMagazineSize;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float BonusClipSize;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float Accuracy;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		float ReviveSpeed;
};

UENUM(BlueprintType)
enum ECharacterClass
{
	CLASS_ASSAULT,
	CLASS_SUPPORT,
	CLASS_RECON,
	CLASS_DEMO,
	CLASS_TECH
};

UENUM(BlueprintType)
enum ESuperRareStat
{
	RARESTAT_INSTANTREVIVE,
	RARESTAT_OVERCHARGEXTREME,
	RARESTAT_HOVER,
	RARESTAT_TWOSTONESONEBIRD,
	RARESTAT_TEAMCLOAK,
	RARESTAT_GHOST,

	RARESTAT_NUM //don't put anything after this
};

USTRUCT(BlueprintType)
struct FRareStatsInfo
{
	GENERATED_USTRUCT_BODY()

	FString Name; //user readable name
	FString Desc; //description
	ESuperRareStat Index; //for easy indexing
	ECharacterClass CharacterClass; //class specific buffs only!

	FRareStatsInfo()
	{
		Name = TEXT("INVALID");
		Desc = TEXT("INVALID");
		Index = RARESTAT_INSTANTREVIVE;
		CharacterClass = CLASS_ASSAULT;
	}

	FRareStatsInfo(FString StatName, FString StatDesc, ESuperRareStat StatIndex, ECharacterClass StatClass)
	{
		Name = StatName;
		Desc = StatDesc;
		Index = StatIndex;
		CharacterClass = StatClass;
	}
};

USTRUCT(BlueprintType)
struct FRareStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		TArray<FRareStatsInfo> StatsInfo;

	//create the defaults
	FRareStats()
	{
		StatsInfo.Add(FRareStatsInfo(TEXT("INSTANT REVIVE"), TEXT("INSTANTLY REVIVE FALLEN TEAMMATES"), RARESTAT_INSTANTREVIVE, CLASS_ASSAULT));
		StatsInfo.Add(FRareStatsInfo(TEXT("OVERCHARE X-TREME"), TEXT("DEAL DAMAGE TO ALL ENEMIES ON SCREEN"), RARESTAT_OVERCHARGEXTREME, CLASS_ASSAULT));
		StatsInfo.Add(FRareStatsInfo(TEXT("INSTANT REVIVE"), TEXT("HOVER COSTS 50% LESS FUEL"), RARESTAT_HOVER, CLASS_SUPPORT));
		StatsInfo.Add(FRareStatsInfo(TEXT("OVERCHARE X-TREME"), TEXT("THROW 3 GRENADES AT ONCE"), RARESTAT_TWOSTONESONEBIRD, CLASS_SUPPORT));
		StatsInfo.Add(FRareStatsInfo(TEXT("INSTANT REVIVE"), TEXT("USING CLOAK MAKES ALL TEAMMATES CLOAK FOR 5 SECONDS"), RARESTAT_TEAMCLOAK, CLASS_RECON));
		StatsInfo.Add(FRareStatsInfo(TEXT("OVERCHARE X-TREME"), TEXT("WALK THROUGH ENEMIES WHILE CLOAKED"), RARESTAT_GHOST, CLASS_RECON));
	};
};