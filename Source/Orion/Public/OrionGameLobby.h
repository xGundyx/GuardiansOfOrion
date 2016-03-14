// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OrionGameMode.h"
#include "OrionPlayerController.h"
#include "OrionGameLobby.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionGameLobby : public AOrionGameMode
{
	GENERATED_BODY()
public:
	AOrionGameLobby(const FObjectInitializer& ObjectInitializer);
	virtual void InitGRI() override;
	virtual void BeginPlay() override;
	
	virtual void HandleMatchHasStarted() override;
	void HandleRespawns();
	void HandleParties();
	virtual void SetSpawnTimer() override;
	virtual void HandleEmptyServer(AController *Exiting = nullptr) override;
	virtual FString InitNewPlayer(class APlayerController* NewPlayerController, const TSharedPtr<const FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal) override;

	FTimerHandle EmptyServerTimer;

	UPROPERTY(BlueprintReadWrite, Category = Lobby)
		TArray<FSpaceParty> SpaceParties;

	virtual void Logout(AController* Exiting) override;

	virtual void AddChatMessage(const FString &msg, bool bTeamMsg, const FString &PartyName) override;

	void UpdatePartyPlayer(FString PartyName, AOrionPlayerController *Member, int32 sLevel, FString sClass);
	void KickPlayerFromParty(AOrionPRI *Player, const FString &PartyName);
	UFUNCTION(BlueprintCallable, Category = Lobby)
		void InvitePlayerToLobby(AOrionPRI *Player, const FString &PartyName, const FString &Inviter);
	UFUNCTION(BlueprintCallable, Category = Lobby)
		void CreateParty(AOrionPlayerController *Leader, FString PartyName, FString MapName, FString Diff, FString Gamemode, FString DiffScale, FString MinILevel, FString Region, FString TOD, FString Privacy);
	UFUNCTION(BlueprintCallable, Category = Lobby)
		void DestroyParty(FString PartyName);
	UFUNCTION(BlueprintCallable, Category = Lobby)
		void AddPlayerToParty(AOrionPlayerController *Member, FString PartyName);
	UFUNCTION(BlueprintCallable, Category = Lobby)
		void RemovePlayerFromParty(AOrionPlayerController *Member, FString PartyName, bool bCreateNew = true);
	UFUNCTION(BlueprintCallable, Category = Lobby)
		void UpdatePartySettings(AOrionPlayerController *Leader, FString PartyName, FString MapName, FString Diff, FString Gamemode, FString DiffScale, FString MinILevel, FString Region, FString TOD, FString Privacy, const FString &IP, const FString &LID);
};
