

#pragma once

#include "GameFramework/Actor.h"
#include "OrionInventory.h"
#include "OrionInventoryGrid.h"
#include "OrionInventoryManager.generated.h"

/**
 * 
 */

/*USTRUCT()
struct FInventoryRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		TArray<AOrionInventory*> Row;

	FORCEINLINE	bool	operator==(const FInventoryRow other) const
	{
		return false;
	}
};

USTRUCT()
struct FInventoryGrid
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		TArray<FInventoryRow> Column;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		TArray<UObject*> Icons;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		int32 MaxHeight;
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		int32 MaxWidth;
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		int32 ItemSize;
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		int32 Padding;

	void CreateInventory(int32 height, int32 width, int32 size, int32 pad)
	{
		MaxHeight = height;
		MaxWidth = width;
		ItemSize = size;
		Padding = pad;

		while(Column.Num() < height)
		{
			FInventoryRow newColumn;
			while(newColumn.Row.Num() < width)
			{
				newColumn.Row.Add(NULL);
			}
			Column.Add(newColumn);
		}
	}
};*/

USTRUCT(BlueprintType)
struct FStatHelper
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		TEnumAsByte<EPrimaryStats> PrimaryStat;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		TEnumAsByte<ESecondaryStats> SecondaryStat;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		bool bPrimary;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		bool bInvert;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		bool bMainStat;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		int32 Value;

	FStatHelper()
	{
		bPrimary = false;
		bInvert = false;
		bMainStat = false;
		Value = 0.0f;
	}

	FORCEINLINE	bool	operator==(const FStatHelper other) const
	{
		if (bMainStat && other.bMainStat)
			return true;
		else if (bPrimary)
			return PrimaryStat == other.PrimaryStat && other.bPrimary;
		else
			return SecondaryStat == other.SecondaryStat && !other.bPrimary;
	}
};

USTRUCT(BlueprintType)
struct FCharacterStatEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		FString StatName;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		float Value;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		FString Desc;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		float DescValue;
};

USTRUCT(BlueprintType)
struct FInventoryHelper
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		AOrionInventoryGrid * Grid;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
		int32 Index;
};

USTRUCT()
struct FArrayHelper
{
	GENERATED_USTRUCT_BODY()

	int32 Primary[PRIMARYSTAT_NUM];
	int32 Secondary[SECONDARYSTAT_NUM];

	TArray<ESuperRareStat> LegendaryStats;

	int32 Defense;
	int32 ItemLevel;

	bool bAllLegendary;

	FArrayHelper()
	{
		Reset();
	}

	void Reset()
	{
		for (int32 i = 0; i < PRIMARYSTAT_NUM; i++)
			Primary[i] = 0;

		for (int32 i = 0; i < SECONDARYSTAT_NUM; i++)
			Secondary[i] = 0;

		LegendaryStats.Empty();

		Defense = 0;
		ItemLevel = 0;
		bAllLegendary = false;
	}
};

UCLASS()
class ORION_API AOrionInventoryManager : public AActor
{
	GENERATED_BODY()

public:
	AOrionInventoryManager(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		TArray<FCharacterStatEntry> GetEquippedStats();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		TArray<FStatHelper> GetChangedStats(FInventoryItem InventoryItem, FInventoryItem EquippedItem);

	void GetStatsFromSlot(AOrionInventoryGrid *Slot, FArrayHelper &Stats);

	//2 dimensional inventory grid representation
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *Grid;

	//equipped slots
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *HelmetSlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *BodySlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *LegsSlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *HandsSlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *WeaponSlot1;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *WeaponSlot2;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *BeltSlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *GadgetSlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *ShaderSlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *DisplaySlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *AbilitySlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *BootsSlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *GrenadeSlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *KnifeSlot;

	UFUNCTION(client, reliable, Category = Inventory)
		void ClientRedraw(EItemType type, int32 index);
		void ClientRedraw_Implementation(EItemType type, int32 index);

	UFUNCTION(server, reliable, WithValidation, Category = Inventory)
		void ServerHandleRightClick(AOrionInventoryGrid *theGrid, int32 index);
		bool ServerHandleRightClick_Validate(AOrionInventoryGrid *theGrid, int32 index);
		void ServerHandleRightClick_Implementation(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(server, reliable, WithValidation, Category = Inventory)
		void ServerSwapItems(AOrionInventoryGrid *theGrid1, int32 index1, AOrionInventoryGrid *theGrid2, int32 index2);
		bool ServerSwapItems_Validate(AOrionInventoryGrid *theGrid1, int32 index1, AOrionInventoryGrid *theGrid2, int32 index2);
		void ServerSwapItems_Implementation(AOrionInventoryGrid *theGrid1, int32 index1, AOrionInventoryGrid *theGrid2, int32 index2);

	void DrawInventory();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Inventory)
		int32 Money;

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void GiveMoney(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		int32 AddItemToInventory(AOrionInventoryGrid *theGrid, FInventoryItem newItem, int32 Index = -1);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void RemoveItemFromInventory(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool HandleRightClick(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void Init(AOrionPlayerController *PC);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void DestroyInventory();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void EquipItems();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		FInventoryItem GetItemAt(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		FInventoryItem GetItemAtSlot(EItemType Slot);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool SwapItems(AOrionInventoryGrid *theGrid1, int32 index1, AOrionInventoryGrid *theGrid2, int32 index2);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void ReduceItemCount(TSubclassOf<UOrionInventoryItem> Item, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		FInventoryItem FillInCraftedStats(FInventoryItem Item, int32 Level);

	UPROPERTY(Replicated)
		APlayerController *OwnerController;

	int32 GetLevelScaledValue(int32 Value, int32 ItemLevel);

	void UpdateEquippedStats();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool HasStat(ESuperRareStat Stat);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool SortInventory();

	//tell the server to do stuff to our items
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool DropItem(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool SellItem(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool CraftItem(FInventoryItem ItemToCraft, TArray<FInventoryHelper> ItemsToRemove);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool BreakdownItem(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool UseItem(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(server, reliable, WithValidation, Category = Inventory)
		void ServerBreakdownItem(AOrionInventoryGrid *theGrid, int32 index);
		bool ServerBreakdownItem_Validate(AOrionInventoryGrid *theGrid, int32 index) { return true; }
		void ServerBreakdownItem_Implementation(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(server, reliable, WithValidation, Category = Inventory)
		void ServerDropItem(AOrionInventoryGrid *theGrid, int32 index);
		bool ServerDropItem_Validate(AOrionInventoryGrid *theGrid, int32 index) { return true; }
		void ServerDropItem_Implementation(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(server, reliable, WithValidation, Category = Inventory)
		void ServerSellItem(AOrionInventoryGrid *theGrid, int32 index);
		bool ServerSellItem_Validate(AOrionInventoryGrid *theGrid, int32 index) { return true; }
		void ServerSellItem_Implementation(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(server, reliable, WithValidation, Category = Inventory)
		void ServerSortInventory();
		bool ServerSortInventory_Validate() { return true; }
		void ServerSortInventory_Implementation();

	UFUNCTION(server, reliable, WithValidation, Category = Inventory)
		void ServerUseItem(AOrionInventoryGrid *theGrid, int32 index);
		bool ServerUseItem_Validate(AOrionInventoryGrid *theGrid, int32 index) { return true; }
		void ServerUseItem_Implementation(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ReadPlayfabInventory"))
		void EventReadInventoryFromPlayfab();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SavePlayfabInventory"))
		void EventSaveInventoryToPlayfab();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "GiveItemToUser"))
		void EventGiveItemToUser(const TArray<FInventoryItem>& Items);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "UpdateItemStats"))
		void EventUpdateItemStats(FInventoryItem Item);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ConsumeItem"))
		void EventConsumeItem(FInventoryItem Item);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "GrantRandomItem"))
		void EventGrantRandomItem(EItemRarity Quality);

	void SaveInventory();

	//don't allow multiple operations of the same type to happen at the same time
	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		bool bGrantItemOperationInProgress;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		bool bSaveInventoryOperationInProgress;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		bool bReadInventoryOperationInProgress;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		bool bUpdateItemOperationInProgress;

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void SetItemInstanceID(FInventoryItem Item, FString ID);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		FString GetPrimaryStatName(EPrimaryStats Stat);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		FString GetSecondaryStatName(ESecondaryStats Stat);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		FString GetPathToClass(FInventoryItem Item);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		TArray<FInventoryItem> GetFullInventoryList();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		EPrimaryStats GetPrimaryStat(FString StatName);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		ESecondaryStats GetSecondaryStat(FString StatName);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		FRareStatsInfo GetRareStat(int32 Index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void ForceAddInventoryItem(FInventoryItem Item, FString ClassName);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		AOrionInventoryGrid *GetGridFromName(FString Slot);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void UpdateEquippedSlots();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "PlayUISound"))
		void EventPlayUISound();

	void SaveEquippedSlots();

	int32 GetPrimaryWeaponDamage();
	int32 GetSecondaryWeaponDamage();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool IsFullyInitialized();

	TArray<FEquippedSlot> EquippedSlots;

private:
	bool TryToEquip(AOrionInventoryGrid *theGrid, int32 index);
	bool TryToUnEquip(AOrionInventoryGrid *theGrid, int32 index);

	FArrayHelper EquippedStats;

	//breakdown generic classes
	TSubclassOf<class UOrionInventoryItem> DefaultWeaponPartsClass;
	TSubclassOf<class UOrionInventoryItem> DefaultArmorPartsClass;
	TSubclassOf<class UOrionInventoryItem> DefaultCommonClass;
	TSubclassOf<class UOrionInventoryItem> DefaultEnhancedClass;
	TSubclassOf<class UOrionInventoryItem> DefaultSuperEnhancedClass;
	TSubclassOf<class UOrionInventoryItem> DefaultRareClass;

	TArray<FInventoryItem> GrantItemQueue;

	FTimerHandle GrantItemTimer;
	FTimerHandle SaveInventoryTimer;

	void HandleGrantItemQueue();
	AOrionInventoryGrid *GetGridFromSlot(EItemType Slot);
};
