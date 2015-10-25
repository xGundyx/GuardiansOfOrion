

#pragma once

#include "GameFramework/PlayerState.h"
#include "OrionTypes.h"
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
		FString LobbyTicket;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayFab)
		FString CharacterClass;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayFab)
		FPhotonServerInfo ServerInfo;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Score)
		int32 Kills;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Score)
		int32 Assists;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Score)
		int32 Deaths;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Score)
		FString ClassType;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayFab)
		int32 AssaultXP;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayFab)
		int32 SupportXP;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayFab)
		int32 ReconXP;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayFab)
		TArray<FUnlockedSkills> UnlockedSkills;

	UFUNCTION(BlueprintCallable, Category = Team)
		int32 GetTeamIndex();

	UFUNCTION(BlueprintCallable, Category = Team)
		void SetTeamIndex(int32 index);

	UFUNCTION(BlueprintCallable, Category = RPG)
		int32 GetXPIntoLevel();

	UFUNCTION(BlueprintCallable, Category = RPG)
		int32 GetXPToLevel();

	UFUNCTION(BlueprintCallable, Category = RPG)
		int32 GetCharacterLevel(ECharacterClass CharacterType);

	bool bInitialized;

	UFUNCTION()
		void OnRep_OrbEffects();

	UFUNCTION(BlueprintCallable, Category = RPG)
		void AddOrbEffect(EOrbType Type, float Duration);

	UPROPERTY(replicatedUsing = OnRep_OrbEffects)
		TArray<FOrbHelper> OrbEffects;

	void UpdateOrbEffects();

private:
	UPROPERTY(Replicated)
		int32 TeamIndex;
};
