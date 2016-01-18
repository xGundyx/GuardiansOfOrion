#include "OrionTypes.generated.h"
#pragma once

//files with custom changes in them
//Runtime/Online/OnlineSubsystemSteam/Private/OnlineSessionAsyncLobbySteam.cpp
//Runtime/Online/OnlineSubsystemSteam/Private/OnlineSessionAsyncServerSteam.cpp
//Runtime/Renderer/Private/ShadowSetup.cpp
//Runtime/UMB/Public/Blueprint/UserWidget.h
//G:\Users\Gundy\Documents\UnrealEngine\Engine\Source\Runtime\Engine\Classes\PhysicsEngine\DestructibleActor.h

class AOrionInventory;
class UOrionInventoryItem;

#if WITH_EDITOR
	#define WITH_CHEATS 0
#else
	#define WITH_CHEATS 0
#endif

#define ORION_SURFACE_Default		SurfaceType_Default
#define ORION_SURFACE_Concrete		SurfaceType1
#define ORION_SURFACE_Dirt			SurfaceType2
#define ORION_SURFACE_Water			SurfaceType3
#define ORION_SURFACE_Metal			SurfaceType4
#define ORION_SURFACE_Wood			SurfaceType5
#define ORION_SURFACE_Grass			SurfaceType6
#define ORION_SURFACE_Glass			SurfaceType7
#define ORION_SURFACE_Flesh			SurfaceType8
#define ORION_SURFACE_Shield		SurfaceType9

UENUM(BlueprintType)
enum EGibType
{
	GIB_HEAD,
	GIB_LEFTARM,
	GIB_RIGHTARM,
	GIB_LEFTLEG,
	GIB_RIGHTLEG
};

UENUM()
enum EAISpawnType
{
	SPAWN_RAPTOR,
	SPAWN_COMPY,
	SPAWN_DILO,
	SPAWN_TREX,
	SPAWN_TRIKE,
	SPAWN_RHAM,
	SPAWN_PTERA,
	SPAWN_ANKY,
	SPAWN_KRUGER,
	SPAWN_JECKYL,
	SPAWN_NAMOR,
	SPAWN_VIPER,
	SPAWN_ORBDROID,
	SPAWN_BONES,
	SPAWN_ASSAULT,
	SPAWN_SUPPORT,
	SPAWN_RECON,
	SPAWN_DEMO,
	SPAWN_TECH,
	SPAWN_ALLO,
	SPAWN_PARA,
	SPAWN_STEG,
	SPAWN_GRUMPS,

	SPAWN_NUM
};

UENUM(BlueprintType)
enum EGameDifficulty
{
	DIFF_EASY,
	DIFF_MEDIUM,
	DIFF_HARD,
	DIFF_INSANE,
	DIFF_REDIKULOUS
};

UENUM(BlueprintType)
enum EAIType
{
	AI_STATIONARY,
	AI_WANDER,
	AI_HUNTING
};

USTRUCT(BlueprintType)
struct FDecalHelper
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		UMaterialInstanceDynamic *Mat;

	float TimeLeft;

	bool operator==(const FDecalHelper Other) const { return Mat == Other.Mat; }
};

USTRUCT(BlueprintType)
struct FSpawnNumbers
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumRaptor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumCompy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumDilo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumTRex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumTrike;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumRham;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumPtera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumAnky;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumKruger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumJeckyl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumNamor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumViper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumOrb;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumBones;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumAssault;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumSupport;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumRecon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumDemo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumTech;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumAllo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumPara;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumSteg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Generator)
		int32 NumGrumps;
};

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
enum ERollDir
{
	ROLL_NONE,
	ROLL_FORWARDS,
	ROLL_BACKWARDS,
	ROLL_LEFT,
	ROLL_RIGHT
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
	//PRIMARYSTAT_DEFENSE,
	//PRIMARYSTAT_DAMAGE,

	PRIMARYSTAT_NUM
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
	SECONDARYSTAT_EXPBOOST, 
	SECONDARYSTAT_FIREDAMAGE,
	SECONDARYSTAT_LIGHTNINGDAMAGE,
	SECONDARYSTAT_ICEDAMAGE,
	SECONDARYSTAT_POISONDAMAGE,
	SECONDARYSTAT_CORROSIVEDAMAGE,
	SECONDARYSTAT_GOLDFIND,
	SECONDARYSTAT_MAGICFIND,
	SECONDARYSTAT_LARGEDINOBOOST,
	SECONDARYSTAT_ROBOTBOOST,

	SECONDARYSTAT_NUM
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		TEnumAsByte<EPrimaryStats> StatType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		int32 MinValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		int32 MaxValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		int32 Value;
};

USTRUCT(BlueprintType)
struct FSecondaryItemStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		TEnumAsByte<ESecondaryStats> StatType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		int32 MinValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		int32 MaxValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		int32 Value;
};

UENUM(BlueprintType)
enum ESuperRareStat
{
	//head
	RARESTAT_PISTOLBONUSDAMAGE,
	RARESTAT_BONUSCRITCHANCE, //15% bonus crit chance
	RARESTAT_BONUSCRITDAMAGE, //50% bonus crit damage

	//body
	RARESTAT_BULLETDODGER, //dodge 50% of bullet based attacks
	RARESTAT_FATALITYIMMUNE, //immune to all fatalities
	RARESTAT_PROJECTILEDAMAGEBONUS,

	//hands
	RARESTAT_SUPERFASTREVIVE,
	RARESTAT_AUTORIFLEBONUSDAMAGE,
	RARESTAT_SHOTGUNBONUSDAMAGE,
	RARESTAT_LMGBONUSDAMAGE,

	//belt
	RARESTAT_MAGICFIND,
	RARESTAT_GOLDFIND,
	RARESTAT_BONUSMELEE, //50% stronger melee

	//legs
	RARESTAT_FREEBLINK, //blink for free if done right after non bonus blink ends
	RARESTAT_EXTRADOWN, //gain one extra downed attempt
	RARESTAT_MOREORBS, //double orb drop rate

	//boots
	RARESTAT_LARGEDINODR, //25% less damage from large dinos
	RARESTAT_ROBOTDR, //25% less damage from robots
	RARESTAT_ORBLENGTH, //10 seconds added to every orb
	RARESTAT_DILOSPITHEAL, //dilo spit pools heal you instead of damage you
	RARESTAT_KNOCKBACKIMMUNE, //knockback attacks deal half damage to you and don't knock you back

	//grenade 
	RARESTAT_FRAGGRENADE,
	RARESTAT_EMPGRENADE,
	RARESTAT_SMOKEGRENADE,
	RARESTAT_STUNGRENADE,
	RARESTAT_NAPALMGRENADE,
	RARESTAT_MULTIGRENADE, //grenade explodes into 4 mini frag grenades

	//primary
	RARESTAT_NORELOAD, //never have to reload, but slower firing rate
	RARESTAT_EXPLODEKILLS, //enemies killed will explode and do radius damage
	RARESTAT_DOUBLEDAMAGENOSHIELD, //double damage when shield is offf

	//secondary
	RARESTAT_KILLRELOAD, //earning a kill instantly reloads this weapon, but clip size is halved
	RARESTAT_BONUSXP, //kills with this weapon grant 25% bonus xp for yourself
	RARESTAT_REGENENERGY, //kills with this weapon grant 10% energy refunded

	//gadget
	RARESTAT_SUPERHEALER, //regen gun heals downed teammates super fast
	RARESTAT_REGENFIRE, //regen gun lights enemies on fire
	RARESTAT_REGENFREEZE, //regen gun freezes small dinos in place for 3 seconds

	//ability
	RARESTAT_SENTRYROCKETS,
	RARESTAT_SENTRYGRENADES,
	RARESTAT_SENTRYFLAMER,
	RARESTAT_JETPACKGRENADES, //drop a frag grenade every second while jetpacking
	RARESTAT_JETPACKIMMNUE, //immune when jetpacking and 2 seconds after jetpack is over
	RARESTAT_JETPACKRANDOMORB, //receive a random orb effect at the end of the jetpack 
	RARESTAT_BUBBLEREVIVE,
	RARESTAT_BUBBLEHARVESTER,
	RARESTAT_BUBBLEDAMAGE, //players inside deal double damage
	RARESTAT_CLOAKIMMUNE,
	RARESTAT_CLOAKMELEE, //melee is 5 times stronger while cloaked
	RARESTAT_CLOAKREGEN, //can use regen gun without breaking cloak
	RARESTAT_PYROINVULNERABLE,
	RARESTAT_PYROHEALS, //heal teammates and harvester
	RARESTAT_PYROFREEZE, //freeze enemies instead of burning them, frozen enemies move slower

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

UENUM(BlueprintType)
enum EItemType
{
	ITEM_HELMET,
	ITEM_CHEST,
	ITEM_LEGS,
	ITEM_HANDS,
	ITEM_BOOTS,
	ITEM_PRIMARYWEAPON,
	ITEM_SECONDARYWEAPON,
	ITEM_GADGET,
	ITEM_BELT,
	ITEM_CAPE,
	ITEM_USEABLE,
	ITEM_ANY,
	ITEM_GENERICCRAFTING,
	ITEM_SHADER,
	ITEM_DISPLAYARMOR,
	ITEM_ABILITY,
	ITEM_BREAKDOWNABLE,
	ITEM_GRENADE
};

USTRUCT(BlueprintType)
struct FRareStatsInfo
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, Category = Stats)
		FString Desc; //description

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		TEnumAsByte<ESuperRareStat> Index; //for easy indexing

	//UPROPERTY(BlueprintReadWrite, Category = Stats)
	//	TEnumAsByte<ECharacterClass> Class;

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		TEnumAsByte<EItemType> Slot;

	FRareStatsInfo()
	{
		Desc = TEXT("INVALID");
		Index = RARESTAT_PISTOLBONUSDAMAGE;
		Slot = ITEM_HELMET;
	}

	FRareStatsInfo(FString StatDesc, ESuperRareStat StatIndex, EItemType StatSlot)
	{
		Desc = StatDesc;
		Index = StatIndex;
		Slot = StatSlot;
	}
};

USTRUCT()
struct FDecodeItemInfo
{
	GENERATED_USTRUCT_BODY()

	//path to the blueprint that holds most of our data
	FString ItemPath;

	FString ItemName;
	FString ItemDesc;
	EItemRarity Rarity;
	EItemType Slot;
	int32 ItemLevel;
	int32 MainStat;
	int32 SellValue;

	TArray< TSubclassOf<class UOrionInventoryItem> > BreakdownClasses;

	TSubclassOf<UOrionInventoryItem> ItemClass;

	TArray<FPrimaryItemStats> PrimaryStats;
	TArray<FSecondaryItemStats> SecondaryStats;
	TArray<FRareStatsInfo> RareStats;

	FString GetItemQuality()
	{
		FString quality = TEXT("common");

		switch (Rarity)
		{
		case RARITY_LEGENDARY:
			quality = TEXT("legendary");
			break;
		case RARITY_SUPERENHANCED:
			quality = TEXT("senhanced");
			break;
		case RARITY_ENHANCED:
			quality = TEXT("enhanced");
			break;
		default:
			break;
		}

		return quality;
	}

	float GetRarityMultiplier()
	{
		switch (Rarity)
		{
		case RARITY_LEGENDARY:
			return 1.0f;
			break;
		case RARITY_SUPERENHANCED:
			return 0.9f;
			break;
		case RARITY_ENHANCED:
			return 0.8f;
			break;
		default:
			return 0.6f;
		}
	}

	int32 GetMaxStatValue()
	{
		switch (Slot)
		{
		case ITEM_HELMET:
			return ItemLevel;
			break;
		case ITEM_CHEST:
			return ItemLevel * 1.5f;
			break;
		case ITEM_HANDS:
			return ItemLevel;
			break;
		case ITEM_BELT:
			return ItemLevel * 0.4f;
			break;
		case ITEM_LEGS:
			return ItemLevel * 1.5f;
			break;
		case ITEM_BOOTS:
			return ItemLevel * 0.6f;
			break;
		case ITEM_PRIMARYWEAPON:
			return ItemLevel * 2;
			break;
		case ITEM_SECONDARYWEAPON:
			return ItemLevel;
			break;
		case ITEM_GADGET:
			return ItemLevel;
			break;
		case ITEM_ABILITY:
			return ItemLevel;
			break;
		default:
			return 0;
			break;
		}

		return 0;
	}

	int32 GetMaxSecondaryStatValue(ESecondaryStats Stat)
	{
		switch (Stat)
		{
		case SECONDARYSTAT_CRITICALHITCHANCE:
			switch (Slot)
			{
			case ITEM_PRIMARYWEAPON:
				return 20;
				break;
			case ITEM_SECONDARYWEAPON:
				return 15;
				break;
			case ITEM_HELMET:
			case ITEM_HANDS:
			case ITEM_BELT:
				return 10;
			}
			break;
		case SECONDARYSTAT_CRITICALHITMULTIPLIER:
			switch (Slot)
			{
			case ITEM_PRIMARYWEAPON:
				return 25;
				break;
			case ITEM_SECONDARYWEAPON:
				return 25;
				break;
			case ITEM_HELMET:
			case ITEM_HANDS:
			case ITEM_BELT:
			case ITEM_CHEST:
			case ITEM_LEGS:
			case ITEM_BOOTS:
				return 10;
			}
			break;
		case SECONDARYSTAT_DAMAGEREDUCTION:
			switch (Slot)
			{
			case ITEM_HELMET:
			case ITEM_HANDS:
			case ITEM_BELT:
			case ITEM_CHEST:
			case ITEM_LEGS:
			case ITEM_BOOTS:
				return 5;
			}
			break;
		case SECONDARYSTAT_DAMAGEREDUCTIONPIERCING:
		case SECONDARYSTAT_DAMAGEREDUCTIONEXPLOSIVE:
		case SECONDARYSTAT_DAMAGEREDUCTIONELEMENTAL:
		case SECONDARYSTAT_DAMAGEREDUCTIONPOISON:
		case SECONDARYSTAT_DAMAGEREDUCTIONBLUNT:
			switch (Slot)
			{
			case ITEM_HELMET:
			case ITEM_HANDS:
			case ITEM_BELT:
			case ITEM_CHEST:
			case ITEM_LEGS:
			case ITEM_BOOTS:
				return 10;
			}
			break;
		case SECONDARYSTAT_EXPBOOST:
			return 20;
			break;
		case SECONDARYSTAT_FIREDAMAGE:
		case SECONDARYSTAT_LIGHTNINGDAMAGE:
		case SECONDARYSTAT_ICEDAMAGE:
		case SECONDARYSTAT_POISONDAMAGE:
		case SECONDARYSTAT_CORROSIVEDAMAGE:
			switch (Slot)
			{
			case ITEM_PRIMARYWEAPON:
			case ITEM_SECONDARYWEAPON:
				return ItemLevel / 2.0f;
			}
			break;
		case SECONDARYSTAT_GOLDFIND:
			return 10;
			break;
		case SECONDARYSTAT_MAGICFIND:
			return 10;
			break;
		case SECONDARYSTAT_LARGEDINOBOOST:
		case SECONDARYSTAT_ROBOTBOOST:
			switch (Slot)
			{
			case ITEM_PRIMARYWEAPON:
			case ITEM_SECONDARYWEAPON:
			case ITEM_GADGET:
			case ITEM_ABILITY:
				return 5;
			}
			break;
		}

		return 0;
	}
};

//takes an encoded string and decodes it, updating this inventory item to be that type
static FDecodeItemInfo DecodeItem(FString code)
{
	FDecodeItemInfo Info;

	return Info;
}

static EItemRarity GetRarityFromFString(FString rarity)
{
	if (rarity.Equals(TEXT("common")))
		return RARITY_COMMON;
	else if (rarity.Equals(TEXT("legendary")))
		return RARITY_LEGENDARY;
	else if (rarity.Equals(TEXT("senhanced")))
		return RARITY_SUPERENHANCED;
	else if (rarity.Equals(TEXT("enhanced")))
		return RARITY_ENHANCED;

	return RARITY_COMMON;
}

//returns the encoded value of this inventory item
static FString EncodeItem(FDecodeItemInfo Item)
{
	FString RawData;

	/*TSharedPtr<FJsonObject> JsonParsed = MakeShareable(new FJsonObject());

	JsonParsed->SetStringField(TEXT("ItemName"), Item.ItemName);
	JsonParsed->SetStringField(TEXT("ItemQuality"), Item.GetItemQuality());
	JsonParsed->SetStringField(TEXT("ItemLevel"), FString("") + Item.ItemLevel);
	JsonParsed->SetStringField(TEXT("ItemPath"), Item.ItemPath);

	TArray<TSharedPtr<FJsonValue>> PrimaryStats;

	for (int32 i = 0; i < Item.PrimaryStats.Num(); i++)
	{
		TSharedPtr<FJsonObject> JsonStat = MakeShareable(new FJsonObject());

		JsonStat->SetStringField(TEXT("StatType"), FString(TEXT("") + int32(Item.PrimaryStats[i].StatType)));
		JsonStat->SetStringField(TEXT("StatMax"), FString(TEXT("") + Item.PrimaryStats[i].MaxValue));
		JsonStat->SetStringField(TEXT("StatMin"), FString(TEXT("") + Item.PrimaryStats[i].MinValue));

		TSharedPtr<FJsonValueObject> TempObj = MakeShareable(new FJsonValueObject(JsonStat));

		PrimaryStats.Add(TempObj);
	}

	JsonParsed->SetArrayField(TEXT("PrimaryStats"), PrimaryStats);

	TArray<TSharedPtr<FJsonValue>> SecondaryStats;

	for (int32 i = 0; i < Item.SecondaryStats.Num(); i++)
	{
		TSharedPtr<FJsonObject> JsonStat = MakeShareable(new FJsonObject());

		JsonStat->SetStringField(TEXT("StatType"), FString(TEXT("") + int32(Item.SecondaryStats[i].StatType)));
		JsonStat->SetStringField(TEXT("StatMax"), FString(TEXT("") + Item.SecondaryStats[i].MaxValue));
		JsonStat->SetStringField(TEXT("StatMin"), FString(TEXT("") + Item.SecondaryStats[i].MinValue));

		TSharedPtr<FJsonValueObject> TempObj = MakeShareable(new FJsonValueObject(JsonStat));

		SecondaryStats.Add(TempObj);
	}

	JsonParsed->SetArrayField(TEXT("SecondaryStats"), SecondaryStats);

	TArray<TSharedPtr<FJsonValue>> RareStats;

	for (int32 i = 0; i < Item.RareStats.Num(); i++)
	{
		TSharedPtr<FJsonObject> JsonStat = MakeShareable(new FJsonObject());

		JsonStat->SetStringField(TEXT("StatType"), FString(TEXT("") + int32(Item.RareStats[i].StatType)));
		JsonStat->SetStringField(TEXT("StatMax"), FString(TEXT("") + Item.RareStats[i].MaxValue));
		JsonStat->SetStringField(TEXT("StatMin"), FString(TEXT("") + Item.RareStats[i].MinValue));

		TSharedPtr<FJsonValueObject> TempObj = MakeShareable(new FJsonValueObject(JsonStat));

		RareStats.Add(TempObj);
	}

	JsonParsed->SetArrayField(TEXT("RareStats"), RareStats);

	TSharedRef< TJsonWriter<> > JsonWriter = TJsonWriterFactory<>::Create(&RawData);

	FJsonSerializer::Serialize(JsonParsed.ToSharedRef(), JsonWriter);*/

	return RawData;
}

UENUM(BlueprintType)
enum ECharacterClass
{
	CLASS_ASSAULT,
	CLASS_SUPPORT,
	CLASS_RECON,
	CLASS_TECH,
	CLASS_PYRO
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
		//helmet
		StatsInfo.Add(FRareStatsInfo(TEXT("50% bonus damage to pistols"), RARESTAT_PISTOLBONUSDAMAGE, ITEM_HELMET));
		StatsInfo.Add(FRareStatsInfo(TEXT("15% bonus to critical hit chance"), RARESTAT_BONUSCRITCHANCE, ITEM_HELMET));
		StatsInfo.Add(FRareStatsInfo(TEXT("50% bonus to critical damage"), RARESTAT_BONUSCRITDAMAGE, ITEM_HELMET));

		//body
		StatsInfo.Add(FRareStatsInfo(TEXT("50% of bullet attacks pass right through you"), RARESTAT_BULLETDODGER, ITEM_CHEST));
		StatsInfo.Add(FRareStatsInfo(TEXT("Immune to all fatality attacks"), RARESTAT_FATALITYIMMUNE, ITEM_CHEST));
		StatsInfo.Add(FRareStatsInfo(TEXT("50% damage bonus to projectile weapons"), RARESTAT_PROJECTILEDAMAGEBONUS, ITEM_CHEST));

		//hands
		StatsInfo.Add(FRareStatsInfo(TEXT("Revive players twice as fast"), RARESTAT_SUPERFASTREVIVE, ITEM_HANDS));
		StatsInfo.Add(FRareStatsInfo(TEXT("50% bonus damage to auto-rifles"), RARESTAT_AUTORIFLEBONUSDAMAGE, ITEM_HANDS));
		StatsInfo.Add(FRareStatsInfo(TEXT("50% bonus damage to shotguns"), RARESTAT_SHOTGUNBONUSDAMAGE, ITEM_HANDS));
		StatsInfo.Add(FRareStatsInfo(TEXT("50% bonus damage to light machine guns"), RARESTAT_LMGBONUSDAMAGE, ITEM_HANDS));

		//belt
		StatsInfo.Add(FRareStatsInfo(TEXT("100% increased item drops"), RARESTAT_MAGICFIND, ITEM_BELT));
		StatsInfo.Add(FRareStatsInfo(TEXT("100% increased gold dropped from enemies"), RARESTAT_GOLDFIND, ITEM_BELT));
		StatsInfo.Add(FRareStatsInfo(TEXT("50% increase to melee damage"), RARESTAT_BONUSMELEE, ITEM_BELT));

		//legs
		StatsInfo.Add(FRareStatsInfo(TEXT("blink for free for 1 second after normal blink ends"), RARESTAT_FREEBLINK, ITEM_LEGS));
		StatsInfo.Add(FRareStatsInfo(TEXT("Gain one extra down"), RARESTAT_EXTRADOWN, ITEM_LEGS));
		StatsInfo.Add(FRareStatsInfo(TEXT("double amount of orbs dropped from all sources"), RARESTAT_MOREORBS, ITEM_LEGS));

		//boots
		StatsInfo.Add(FRareStatsInfo(TEXT("25% less damage taken from large dinos"), RARESTAT_LARGEDINODR, ITEM_BOOTS));
		StatsInfo.Add(FRareStatsInfo(TEXT("25% less damage taken from robots"), RARESTAT_ROBOTDR, ITEM_BOOTS));
		StatsInfo.Add(FRareStatsInfo(TEXT("Orbs last 10 seconds longer"), RARESTAT_ORBLENGTH, ITEM_BOOTS));
		StatsInfo.Add(FRareStatsInfo(TEXT("dilo spit pools heal you instead of damaging you"), RARESTAT_DILOSPITHEAL, ITEM_BOOTS));
		StatsInfo.Add(FRareStatsInfo(TEXT("knockback attacks deal half damage to you and don't knock you back"), RARESTAT_KNOCKBACKIMMUNE, ITEM_BOOTS));

		//grenade 
		StatsInfo.Add(FRareStatsInfo(TEXT("grenade explosion drops a frag grenade"), RARESTAT_FRAGGRENADE, ITEM_GRENADE));
		StatsInfo.Add(FRareStatsInfo(TEXT("grenade explosion drops an emp grenade"), RARESTAT_EMPGRENADE, ITEM_GRENADE));
		StatsInfo.Add(FRareStatsInfo(TEXT("grenade explosion drops a smoke grenade"), RARESTAT_SMOKEGRENADE, ITEM_GRENADE));
		StatsInfo.Add(FRareStatsInfo(TEXT("grenade explosion drops a stun grenade"), RARESTAT_STUNGRENADE, ITEM_GRENADE));
		StatsInfo.Add(FRareStatsInfo(TEXT("grenade explosion drops a napalm grenade"), RARESTAT_NAPALMGRENADE, ITEM_GRENADE));
		StatsInfo.Add(FRareStatsInfo(TEXT("grenade explodes into 4 mini frag grenades"), RARESTAT_MULTIGRENADE, ITEM_GRENADE));

		//primary
		StatsInfo.Add(FRareStatsInfo(TEXT("this weapon never needs a reload, but fires at half speed"), RARESTAT_NORELOAD, ITEM_PRIMARYWEAPON));
		StatsInfo.Add(FRareStatsInfo(TEXT("enemies killed with this weapon explode"), RARESTAT_EXPLODEKILLS, ITEM_PRIMARYWEAPON));
		StatsInfo.Add(FRareStatsInfo(TEXT("double damage when your shields are down"), RARESTAT_DOUBLEDAMAGENOSHIELD, ITEM_PRIMARYWEAPON));

		//secondary
		StatsInfo.Add(FRareStatsInfo(TEXT("kills instantly reload this weapon, but clip size is halved"), RARESTAT_KILLRELOAD, ITEM_SECONDARYWEAPON));
		StatsInfo.Add(FRareStatsInfo(TEXT("kills with the weapon grant 25% extra experience"), RARESTAT_BONUSXP, ITEM_SECONDARYWEAPON));
		StatsInfo.Add(FRareStatsInfo(TEXT("kills with this weapon grant 10% ability energy recharge"), RARESTAT_REGENENERGY, ITEM_SECONDARYWEAPON));

		//gadget
		StatsInfo.Add(FRareStatsInfo(TEXT("regen gun heals downed teammates super fast"), RARESTAT_SUPERHEALER, ITEM_GADGET));
		StatsInfo.Add(FRareStatsInfo(TEXT("regen gun lights enemies on fire"), RARESTAT_REGENFIRE, ITEM_GADGET));
		StatsInfo.Add(FRareStatsInfo(TEXT("regen gun freezes enemies in place"), RARESTAT_REGENFREEZE, ITEM_GADGET));

		//ability
		StatsInfo.Add(FRareStatsInfo(TEXT("sentry gun now shoots rockets"), RARESTAT_SENTRYROCKETS, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("sentry gun now shoots frag grenades"), RARESTAT_SENTRYGRENADES, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("sentry gun now shoots flames instead of bullets"), RARESTAT_SENTRYFLAMER, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("drop a frag grenade every second while jetpacking"), RARESTAT_JETPACKGRENADES, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("take no damage while jetpacking and for 2 seconds afterwards"), RARESTAT_JETPACKIMMNUE, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("receive a random orb at the end of jetpacking"), RARESTAT_JETPACKRANDOMORB, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("healy bubble can revive downed players"), RARESTAT_BUBBLEREVIVE, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("healy bubble can repair the harvester"), RARESTAT_BUBBLEHARVESTER, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("players within the healy bubble do double damage"), RARESTAT_BUBBLEDAMAGE, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("take no damage while cloaking"), RARESTAT_CLOAKIMMUNE, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("melee is 5 times stronger while cloaking"), RARESTAT_CLOAKMELEE, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("can use regen gun while cloaked"), RARESTAT_CLOAKREGEN, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("take no damage while flamethrower is active"), RARESTAT_PYROINVULNERABLE, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("flamethrower heals allies as well as damages enemies"), RARESTAT_PYROHEALS, ITEM_ABILITY));
		StatsInfo.Add(FRareStatsInfo(TEXT("flamethrower freezes enemies instead of burning them"), RARESTAT_PYROFREEZE, ITEM_ABILITY));
	};
};

static FRareStats RareStats;

#define BASEXP 10000
#define XPINCREASE 2500

static int32 CalculateLevel(int32 XP)
{
	int32 XPRemaining = XP - BASEXP;
	int32 Level = 1;

	while (XPRemaining >= 0)
	{
		Level++;
		XPRemaining -= (BASEXP + (XPINCREASE * (Level - 1)));
	}

	return Level;
}

static int32 CalculateExpToLevel(int32 Level)
{
	int32 XP = 0;

	for (int32 i = 0; i < Level - 1; i++)
		XP += BASEXP + (XPINCREASE * i);

	return XP;
}

USTRUCT(BlueprintType)
struct FPhotonServerInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Photon)
		FString RoomName;
	UPROPERTY(BlueprintReadWrite, Category = Photon)
		FString MapName;
	UPROPERTY(BlueprintReadWrite, Category = Photon)
		FString Difficulty;
	UPROPERTY(BlueprintReadWrite, Category = Photon)
		FString IP; //include port for ease
	UPROPERTY(BlueprintReadWrite, Category = Photon)
		FString Ticket;
	UPROPERTY(BlueprintReadWrite, Category = Photon)
		FString Privacy;
	UPROPERTY(BlueprintReadWrite, Category = Photon)
		FString LobbyID;
	UPROPERTY(BlueprintReadWrite, Category = Photon)
		FString TOD;
	UPROPERTY(BlueprintReadWrite, Category = Photon)
		FString GameMode;
};

UENUM(BlueprintType)
enum ESkillTreeUnlocks
{
	SKILL_NONE = 0,
	SKILL_MELEEDAMAGE,
	SKILL_GRENADECOOLDOWN,
	SKILL_ROLLSPEED,
	SKILL_ORBDROPRATE,
	SKILL_SECONDARYDAMAGE,
	SKILL_BLINKDISTANCE,
	SKILL_ORBDURATION,
	SKILL_PRIMARYDAMAGE,
	SKILL_GRENADERADIUS,
	SKILL_EXTRABLINK,
	SKILL_EXTRAGRENADE,
	SKILL_PISTOLORBS,
	SKILL_KNIFEORBS,
	SKILL_REGENENERGY,
	SKILL_CLIPSIZE,
	SKILL_FASTERHEALING,
	SKILL_FRAGGRENADEDAMAGE,
	SKILL_FIRERATE,
	SKILL_BEAMLENGTH,
	SKILL_RELOADSPEED,
	SKILL_KILLLEECH,
	SKILL_REGENCHAIN,
	SKILL_SPRINTSPEED,
	SKILL_AIMSPEED,
	SKILL_JETPACKSPEED,
	SKILL_ROBOTDAMAGEREDUCTION,
	SKILL_SHIELDREGENSPEED,
	SKILL_JETPACKHEIGHT,
	SKILL_DAMAGETOLARGEDINOS,
	SKILL_JETPACKDAMAGEREDUCTION,
	SKILL_REVIVESPEED,
	SKILL_JETPACKBURN,
	SKILL_HEALTHREGEN,
	SKILL_SMOKEDURATION,
	SKILL_BUBBLESTRENGTH,
	SKILL_BUBBLERADIUS,
	SKILL_BUBBLEREGEN,
	SKILL_BUBBLEEXPLODE,
	SKILL_EMPGENERATORHEAL,
	SKILL_CLOAKSPEED,
	SKILL_CLOAKREGEN,
	SKILL_CLOAKDURATION,
	SKILL_CLOAKTEAMMATES,
	SKILL_TURRETSPEED,
	SKILL_TURRETDURATION,
	SKILL_TURRETRANGE,
	SKILL_TURRETDAMAGE,

	SKILL_NUMPLUSONE
};

USTRUCT(BlueprintType)
struct FUnlockedSkills
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Skills)
		int32 Points;

	UPROPERTY(BlueprintReadWrite, Category = Skills)
		int32 MaxPoints;

	UPROPERTY(BlueprintReadWrite, Category = Skills)
		int32 Modifier;
};

UENUM(BlueprintType)
enum EOrbType
{
	ORB_HEALTH, //green health regen
	ORB_STOPPING, //red stopping power
	ORB_EXP, //yellow xp
	ORB_ROF, //blue rate of fire increase
	ORB_SPEED, //purple speed 
	ORB_STRENGTH //orange strength
};

USTRUCT()
struct FOrbHelper
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Orb)
		TEnumAsByte<EOrbType> Type;

	UPROPERTY(BlueprintReadOnly, Category = Orb)
		int32 TimeLeft;

	UPROPERTY(BlueprintReadOnly, Category = Orb)
		FVector Color;

	float TimeStarted;

	UPROPERTY(BlueprintReadOnly, Category = Orb)
		float Duration;
};

USTRUCT()
struct FOrbEffectHelper
{
	GENERATED_USTRUCT_BODY()

	TEnumAsByte<EOrbType> Type;
	UParticleSystemComponent *PSC;
};

USTRUCT(BlueprintType)
struct FCooldownAbility
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Skill)
		float Energy;

	UPROPERTY(BlueprintReadWrite, Category = Skill)
		int32 SecondsToRecharge;
};

USTRUCT(BlueprintType)
struct FFriendListData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Friend)
		FString PlayerName;

	UPROPERTY(BlueprintReadOnly, Category = Friend)
		bool bOnline;

	UPROPERTY(BlueprintReadOnly, Category = Friend)
		bool bPlayingGame;

	UPROPERTY(BlueprintReadOnly, Category = Friend)
		FString SteamID;

	//id to connect to photon lobby, which in turn connects to the actual game server
	UPROPERTY(BlueprintReadOnly, Category = Friend)
		FString LobbyID;

	UPROPERTY(BlueprintReadOnly, Category = Friend)
		UTexture2D *Avatar;

	bool operator<(const FFriendListData Other) const { return PlayerName < Other.PlayerName; }
};

UENUM(BlueprintType)
enum EHUDStatusType
{
	HUD_NONE,
	HUD_TYPING,
	HUD_DOWNED,
	HUD_RADIO,
	HUD_MEDIC
};

UENUM(BlueprintType)
enum EVoiceType
{
	VOICE_GRENADE,//
	VOICE_TOSSGRENADE,
	VOICE_LOOKATMEGO,//
	VOICE_GOINGSTEALTH,//
	VOICE_HEALYBUBBLE,//
	VOICE_CANTTOUCHTHIS,
	VOICE_FRAGOUT,//
	VOICE_EMPOUT,//
	VOICE_CLOAKING,
	VOICE_SMOKEOUT,//

	VOICE_BOSSKILL,//

	VOICE_AFFIRMATIVE,
	VOICE_NEGATIVE,
	VOICE_SITTIGHT,
	VOICE_IMONIT,
	VOICE_WHAT,

	VOICE_TAKEOBJECTIVE,//
	VOICE_MOVEOUT,
	VOICE_GOGOGO,
	VOICE_TAKINGDAMAGE,//
	VOICE_REQUESTBACKUP,
	VOICE_COVERINGFIRE,
	VOICE_INEEDBACKUP,//
	VOICE_STICKTOGETHER,
	VOICE_DEFENCEHARVESTER,//
	VOICE_FOLLOWME,

	VOICE_REVIVE,//
	VOICE_DROPSHIP,//

	VOICE_WHATISTHAT,//
	VOICE_TREX,//
	VOICE_INCOMING,
	VOICE_NAMOR,//
	VOICE_KRUGER,//
	VOICE_ANKY,//
	VOICE_TRIKE,//
	VOICE_JECKYL,//
	VOICE_ORB,//
	VOICE_GRUMPS,//
	VOICE_BONES,//
	VOICE_DRONES,

	VOICE_TEAMMATEDOWN,//
	VOICE_WAVEEND,//
	VOICE_WAVESTART,//
	VOICE_WOUNDED,//

	VOICE_NUM,

	VOICE_RANDOM
};

USTRUCT(BlueprintType)
struct FVoiceHelper
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
		TEnumAsByte<EVoiceType> Type;
	UPROPERTY(EditDefaultsOnly)
		TArray<USoundWave*> Voice;
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		FString ItemName;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		FString ItemDesc;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory)
		TSubclassOf<UOrionInventoryItem> ItemClass;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		int32 Amount;

	//main stat is defense or weapon damage
	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		int32 MainStat;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		TArray<FPrimaryItemStats> PrimaryStats;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		TArray<FSecondaryItemStats> SecondaryStats;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		TArray<FRareStatsInfo> RareStats;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		TEnumAsByte<EItemRarity> Rarity;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		TEnumAsByte<EItemType> Slot;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		int32 ItemLevel;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		int32 SellValue;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		TArray< TSubclassOf<class UOrionInventoryItem> > BreakdownClasses;

	void Reset()
	{
		ItemName = "";
		ItemDesc = "";
		ItemClass = nullptr;
		Amount = 0;
		SellValue = 0;

		PrimaryStats.Empty();
		SecondaryStats.Empty();
		RareStats.Empty();
	}

	//sort by item quality, and then by item type
	bool operator<(const FInventoryItem Other) const 
	{ 
		if (Slot == Other.Slot)
		{
			if (Rarity == Other.Rarity)
			{
				if (ItemLevel == Other.ItemLevel)
					return Amount < Amount;
				else
					return ItemLevel < Other.ItemLevel;
			}
			else
				return Rarity < Other.Rarity;
		}
		else
			return Slot < Other.Slot; 
	}
};