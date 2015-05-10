#include "OrionTypes.generated.h"
#pragma once

class AOrionInventory;

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

UENUM(BlueprintType)
enum EPrimaryStats
{
	PRIMARYSTAT_STRENGTH,
	PRIMARYSTAT_DEXTERITY,
	PRIMARYSTAT_INTELLIGENCE,
	PRIMARYSTAT_DISCIPLINE,
	PRIMARYSTAT_VITALITY,
	PRIMARYSTAT_POWER
};

UENUM(BlueprintType)
enum ESecondaryStats
{
	SECONDARYSTAT_CRITICALHITCHANCE,
	SECONDARYSTAT_CRITICALHITMULTIPLIER,
	SECONDARYSTAT_DAMAGEREDUCTION,
	SECONDARYSTAT_DAMAGEREDUCTIONBLUNT,
	SECONDARYSTAT_DAMAGEREDUCTIONPIERCING,
	SECONDARYSTAT_DAMAGEREDUCTIONEXPLOSIVE,
	SECONDARYSTAT_DAMAGEREDUCTIONELEMENTAL,
	SECONDARYSTAT_DAMAGEREDUCTIONPOISON,
	SECONDARYSTAT_MOVEMENTSPEED,
	SECONDARYSTAT_EXPBOOST
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
enum EItemRarity
{
	RARITY_COMMON,
	RARITY_ENHANCED,
	RARITY_SUPERENHANCED,
	RARITY_LEGENDARY
};

USTRUCT(BlueprintType)
struct FPrimaryItemStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Stats)
		TEnumAsByte<EPrimaryStats> StatType;
	UPROPERTY(EditDefaultsOnly, Category = Stats)
		int32 MinValue;
	UPROPERTY(EditDefaultsOnly, Category = Stats)
		int32 MaxValue;
};

USTRUCT(BlueprintType)
struct FSecondaryItemStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Stats)
		TEnumAsByte<ESecondaryStats> StatType;
	UPROPERTY(EditDefaultsOnly, Category = Stats)
		int32 MinValue;
	UPROPERTY(EditDefaultsOnly, Category = Stats)
		int32 MaxValue;
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
struct FRareItemStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Stats)
		TEnumAsByte<ESuperRareStat> StatType;
	UPROPERTY(EditDefaultsOnly, Category = Stats)
		int32 MinValue;
	UPROPERTY(EditDefaultsOnly, Category = Stats)
		int32 MaxValue;
};

USTRUCT()
struct FDecodeItemInfo
{
	GENERATED_USTRUCT_BODY()

	FString ItemName;
	EItemRarity Rarity;
	int32 RequiredLevel;

	TArray<FPrimaryItemStats> PrimaryStats;
	TArray<FSecondaryItemStats> SecondaryStats;
	TArray<FRareItemStats> RareStats;
};

//takes an encoded string and decodes it, updating this inventory item to be that type
static FDecodeItemInfo DecodeItem(FString code)
{
	FDecodeItemInfo Info;

	return Info;
}

//returns the encoded value of this inventory item
static FString EncodeItem(FDecodeItemInfo Item)
{
	return TEXT("");
}

UENUM(BlueprintType)
enum EItemType
{
	ITEM_HELMET,
	ITEM_CHEST,
	ITEM_LEGS,
	ITEM_HANDS,
	ITEM_PRIMARYWEAPON,
	ITEM_SECONDARYWEAPON,
	ITEM_GADGET,
	ITEM_RING,
	ITEM_NECK,
	ITEM_BELT,
	ITEM_CAPE,
	ITEM_USEABLE,
	ITEM_ANY,
	ITEM_SHIELD
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
		StatsInfo.Add(FRareStatsInfo(TEXT("HOVERABLE"), TEXT("HOVER COSTS 50% LESS FUEL"), RARESTAT_HOVER, CLASS_SUPPORT));
		StatsInfo.Add(FRareStatsInfo(TEXT("TWO STONES ONE BIRD"), TEXT("THROW 3 GRENADES AT ONCE"), RARESTAT_TWOSTONESONEBIRD, CLASS_SUPPORT));
		StatsInfo.Add(FRareStatsInfo(TEXT("TEAM CLOAK"), TEXT("USING CLOAK MAKES ALL TEAMMATES CLOAK FOR 5 SECONDS"), RARESTAT_TEAMCLOAK, CLASS_RECON));
		StatsInfo.Add(FRareStatsInfo(TEXT("GHOST"), TEXT("WALK THROUGH ENEMIES WHILE CLOAKED"), RARESTAT_GHOST, CLASS_RECON));
	};
};

static FRareStats RareStats;