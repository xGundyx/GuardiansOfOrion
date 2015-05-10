

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

UCLASS()
class ORION_API AOrionInventoryManager : public AActor
{
	GENERATED_BODY()

public:
	AOrionInventoryManager(const FObjectInitializer& ObjectInitializer);

	//2 dimensional inventory grid representation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *Grid;

	//equipped slots
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *HelmetSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *BodySlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *LegsSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *HandsSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *WeaponSlot1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *WeaponSlot2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *RingSlot1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *RingSlot2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *ShieldSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *NeckSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *BeltSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *GadgetSlot;

	//percentage values of how much ammo we have
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Inventory)
		float PrimaryAmmo;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Inventory)
		float SecondaryAmmo;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Inventory)
		int32 Money;

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void GiveMoney(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		int32 AddItemToInventory(UOrionInventoryGrid *theGrid, AOrionInventory* newItem);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void RemoveItemFromInventory(UOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void Init();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void DestroyInventory();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void EquipItems(AOrionCharacter *aPawn, EItemType SlotType);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		AOrionInventory *GetItemAt(UOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool SwapItems(UOrionInventoryGrid *theGrid1, int32 index1, UOrionInventoryGrid *theGrid2, int32 index2);

	APlayerController *OwnerController;
};
