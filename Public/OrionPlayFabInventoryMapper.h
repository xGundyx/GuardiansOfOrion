#include "GameFramework/Actor.h"
#include "OrionInventoryItem.h"
#include "OrionPlayFabInventoryMapper.generated.h"
#pragma once

//map the playfab inventory item id to the class and any meshes needed
USTRUCT(BlueprintType)
struct FInventoryMapping
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Character)
		FString ItemID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Character)
		TSubclassOf<class UOrionInventoryItem> ItemLink;

	FInventoryMapping(){ ItemLink = nullptr; }
	FInventoryMapping(FString ID)
	{
		ItemID = ID;
		ItemLink = nullptr;
	}

	bool operator==(const FString Other) const { return ItemID == Other; }
};

UCLASS()
class ORION_API AOrionPlayFabInventoryMapper : public AActor
{
	GENERATED_BODY()

public:
	AOrionPlayFabInventoryMapper(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<FInventoryMapping> InventoryMappings;
};