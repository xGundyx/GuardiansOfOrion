// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#if IS_SERVER
	#include "playfab/PlayFabServerAPI.h"
#else
	#include "playfab/PlayFabClientAPI.h"
#endif
#include "ClientConnector.h"
#include "OrionTCPLink.generated.h"

using namespace PlayFab;

/**
 * 
 */

UCLASS()
class ORION_API UOrionTCPLink : public UObject
{
	GENERATED_BODY()
public:
	UOrionTCPLink(const FObjectInitializer& ObjectInitializer);

#if IS_SERVER
	static bool Init();

	static void SavePlayerStatistics(FString PlayerID, UOrionStats *Stats);
	static void OnSaveStats(ServerModels::UpdateUserStatisticsResult& result, void* userData);
	static void OnSaveStatsError(PlayFabError& error, void* userData);

	static void GetPlayerStats(void* Stats, FString PlayerID);
	static void OnGetStats(ServerModels::GetUserStatisticsResult& result, void* userData);
	static void OnGetStatsFailed(PlayFabError& error, void* userData);
private:
	static PlayFabServerAPI server;
#else
	static bool Init(AOrionPlayerController *Owner);
	static void Login(FString UserName, FString Password);
	static void CreateAccount(FString UserName, FString Password, FString EMail);
	static void CreateCharacter(FString UserName, FString Gender, FString BaseColor);
	static void SelectCharacter(int32 Index);

	//void GlobalErrorHandler(PlayFabError& error, void* userData);

	static void LoginComplete(bool bSuccess, FString error);
	static void AddUserComplete(bool bSuccess, FString error);
	static void CharacterCreationComplete(bool bSucceded, FString Message);
	static void AccountCreationComplete(bool bSucceded, FString Message);
	static void SelectCharacterComplete(bool bSucceded, FString Message);

	static void Update();

	//grab our character read only data from playfab
	static void RetrieveCharacterData(bool bLogginIn);
	static void AddToCharacterData(const std::string first, const ClientModels::UserDataRecord second);

	static void GetPlayerStats(void* Stats);
	//static void GetPlayerAchievements(FString Player);
	static void OnGetStats(ClientModels::GetUserStatisticsResult& result, void* userData);
	static void OnGetStatsFailed(PlayFabError& error, void* userData);

	static AOrionPlayerController *PlayerOwner;

	static FString SessionTicket;
	static FString PlayFabID;
	static bool NewlyCreated;

	static FCharacterData CharacterDatas[4];

	static void OnCharacterData(ClientModels::GetUserDataResult& result, void* userData);
	static void OnCharacterDataFailed(PlayFabError& error, void* userData);
	static void OnCharacterFinalized(ClientModels::GetUserDataResult& result, void* userData);
	static void OnCharacterFinalizedFailed(PlayFabError& error, void* userData);

	static UClientConnector *connector;

private:
	static PlayFabClientAPI client;
#endif
};
