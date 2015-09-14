

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

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Pawn)
		class AOrionCharacter *ControlledPawn;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayFab)
		FString PlayFabID;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayFab)
		FString SessionTicket;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayFab)
		FString CharacterID;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayFab)
		FString PlayFabName;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayFab)
		int32 CharacterXP;

	UFUNCTION(BlueprintCallable, Category = Team)
		int32 GetTeamIndex();

	UFUNCTION(BlueprintCallable, Category = Team)
		void SetTeamIndex(int32 index);

private:
	UPROPERTY(Replicated)
		int32 TeamIndex;
};
