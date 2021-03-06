// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
/*#if IS_SERVER
	#include "playfab/PlayFabServerAPI.h"
#else
	#include "playfab/PlayFabClientAPI.h"
	#include "OrionPhotonClient.h"
#endif*/
#include "PlayFabRequestProxy.h"
//#include "ClientConnector.h"
//#include "OrionTCPLink.generated.h"

/*using namespace PlayFab;

UENUM()
enum EPlayFabState
{
	PFSTATE_NONE,
	PFSTATE_LOGGINGIN,
	PFSTATE_CREATINGCHARACTER,
	PFSTATE_DELETINGCHARACTER,
	PFSTATE_CREATINGACCOUNT
};

UCLASS()
class ORION_API UOrionTCPLink : public UObject
{
	GENERATED_BODY()
public:
	UOrionTCPLink(const FObjectInitializer& ObjectInitializer);

#if IS_SERVER
	static bool Init();
	static void Update();

	static void SavePlayerStatistics(FString PlayerID, UOrionStats *Stats);
	static void OnSaveStats(ServerModels::UpdateUserStatisticsResult& result, void* userData);
	static void OnSaveStatsError(PlayFabError& error, void* userData);

	static void GetPlayerStats(void* Stats, FString PlayerID);
	static void OnGetStats(ServerModels::GetUserStatisticsResult& result, void* userData);
	static void OnGetStatsFailed(PlayFabError& error, void* userData);

	static void SaveCharacter(AOrionPlayerController *PC);
	static void OnSaveCharacterData(ServerModels::UpdateCharacterDataResult& result, void* userData);
	static void OnSaveCharacterDataError(PlayFabError& error, void* userData);

	static void GetCharacterData(AOrionPlayerController *PC);
	static void OnGetCharacterData(ServerModels::GetCharacterDataResult& result, void* userData);
	static void OnGetCharacterDataError(PlayFabError& error, void* userData);

private:
	static PlayFabServerAPI server;
#else
	static bool Init(AOrionPlayerController *Owner);
	static void Login(FString UserName, FString Password);
	static void OnLogin(ClientModels::LoginResult& result, void* userData);
	static void OnLoginError(PlayFabError& error, void* userData);

	static void OnGetCharacterList(ClientModels::RunCloudScriptResult& result, void* userData);
	static void OnGetCharacterListError(PlayFabError& error, void* userData);

	static void OnGetCloudURL(ClientModels::GetCloudScriptUrlResult& result, void* userData);
	static void OnGetCloudURLError(PlayFabError& error, void* userData);

	static void CreateAccount(FString UserName, FString Password, FString EMail);

	static void CreateCharacter(FString UserName, FString Gender, FString BaseColor, FString CharacterClass);

	static void OnCreateCharacter(ClientModels::RunCloudScriptResult& result, void* userData);
	static void OnCreateCharacterError(PlayFabError& error, void* userData);

	static void DeleteCharacter(FString CharacterID);

	static void OnDeleteCharacter(ClientModels::RunCloudScriptResult& result, void* userData);
	static void OnDeleteCharacterError(PlayFabError& error, void* userData);

	static void SelectCharacter(int32 Index);
	static void OnSelectCharacter(ClientModels::RunCloudScriptResult& result, void* userData);
	static void OnSelectCharacterError(PlayFabError& error, void* userData);

	static void GetCharacterData(AOrionPlayerController *PC);
	static void OnGetCharacterData(ClientModels::RunCloudScriptResult& result, void* userData);
	static void OnGetCharacterDataError(PlayFabError& error, void* userData);

	static void OnGetPhotonChatToken(ClientModels::GetPhotonAuthenticationTokenResult& result, void* userData);
	static void OnGetPhotonChatTokenError(PlayFabError& error, void* userData);

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

	static void SaveCharacter(AOrionPlayerController *PC);

	static FString SessionTicket;
	static FString PlayFabID;
	static bool NewlyCreated;
	static FString CloudScriptURL;
	static EPlayFabState PFState;
	static FString CurrentCharacterID;
	static FString ChatToken;

	static TArray<FCharacterData> CharacterDatas;

	static void OnCharacterData(ClientModels::GetUserDataResult& result, void* userData);
	static void OnCharacterDataFailed(PlayFabError& error, void* userData);
	static void OnCharacterFinalized(ClientModels::GetUserDataResult& result, void* userData);
	static void OnCharacterFinalizedFailed(PlayFabError& error, void* userData);

	static UClientConnector *connector;
	static AOrionPhotonClient *PhotonClient;

	static void ParseDisplayItems(TSharedPtr<FJsonObject> jObject, FCharacterData* Data);

	//photon chat hooks
	static void SendChatMessage(FString Channel, FString Message);
	static void ChatMessageReceived(FString ChannelName, TArray<FString> Messages);

private:
	static PlayFabClientAPI client;
#endif
};*/
