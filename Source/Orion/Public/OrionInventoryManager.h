

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
		AOrionInventoryGrid *RingSlot1;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *RingSlot2;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *ShieldSlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *NeckSlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *BeltSlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryGrid *GadgetSlot;

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
		int32 AddItemToInventory(AOrionInventoryGrid *theGrid, AOrionInventory* newItem, int32 Index = -1);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void RemoveItemFromInventory(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool HandleRightClick(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void Init(AOrionPlayerController *PC);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void DestroyInventory();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void EquipItems(AOrionCharacter *aPawn, EItemType SlotType);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		AOrionInventory *GetItemAt(AOrionInventoryGrid *theGrid, int32 index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool SwapItems(AOrionInventoryGrid *theGrid1, int32 index1, AOrionInventoryGrid *theGrid2, int32 index2);

	APlayerController *OwnerController;

private:
	bool TryToEquip(AOrionCharacter *MyPawn, AOrionInventoryGrid *theGrid, int32 index);
	bool TryToUnEquip(AOrionCharacter *MyPawn, AOrionInventoryGrid *theGrid, int32 index);
};
