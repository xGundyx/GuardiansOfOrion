

#pragma once

#include "GameFramework/PlayerState.h"
#include "OrionPRI.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionPRI : public APlayerState
{
	GENERATED_BODY()
public:
	AOrionPRI(const FObjectInitializer& ObjectInitializer);

	void SeamlessTravelTo(APlayerState * NewPlayerState);
	void CopyProperties(class APlayerState* PlayerState);
	
	UFUNCTION()
		void OnRep_InventoryManager();

	bool IsOnShip();

	//player inventory, only replicated to owner
	UPROPERTY(ReplicatedUsing = OnRep_InventoryManager, BlueprintReadOnly, Category = Inventory)
		AOrionInventoryManager *InventoryManager;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Pawn)
		bool bOnShip;

	FString PlayFabID;
	FString SessionTicket;
	FString CharacterID;
};
