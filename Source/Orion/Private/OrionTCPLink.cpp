// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "SocketSubsystem.h"
#include "Networking.h"
#include "SharedPointer.h"
#include "OrionTCPLink.h"
#include "StringConv.h"
#include "ClientConnector.h"
#include "BackendMappings.h"
#include "OrionStats.h"

using namespace Backend;

void GlobalErrorHandler(PlayFabError& error, void* userData);

#if IS_SERVER
PlayFabServerAPI UOrionTCPLink::server;
#else

void OnAddUser(ClientModels::RegisterPlayFabUserResult& result, void* userData);
void OnAddUserError(PlayFabError& error, void* userData);

AOrionPlayerController *UOrionTCPLink::PlayerOwner = NULL;
PlayFabClientAPI UOrionTCPLink::client;
FString UOrionTCPLink::SessionTicket = FString("");
FString UOrionTCPLink::PlayFabID = FString("");
FString UOrionTCPLink::CloudScriptURL = FString("");
bool UOrionTCPLink::NewlyCreated = false;
TArray<FCharacterData> UOrionTCPLink::CharacterDatas;
EPlayFabState UOrionTCPLink::PFState = PFSTATE_NONE;
FString UOrionTCPLink::CurrentCharacterID = FString("");

UClientConnector *UOrionTCPLink::connector;

#endif

UOrionTCPLink::UOrionTCPLink(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//server side calls go here
#if IS_SERVER
bool UOrionTCPLink::Init()
{
	PlayFabSettings::useDevelopmentEnvironment = true;
	PlayFabSettings::serverURL = "https://93F0.playfabapi.com";
	PlayFabSettings::titleId = "93F0";
	PlayFabSettings::globalErrorHandler = GlobalErrorHandler;
	PlayFabSettings::developerSecretKey = "TYGNKQGRFUHWNJ3EQ38F759ANUIAP8JY9R1G4BAEMTKUBNW5GG";

	return true;
}

void UOrionTCPLink::SavePlayerStatistics(FString PlayerID, UOrionStats *Stats)
{
	ServerModels::UpdateUserStatisticsRequest request;

	request.PlayFabId = TCHAR_TO_UTF8(*PlayerID);
	if (Stats)
		request.UserStatistics = Stats->GetStatsMap();

	server.UpdateUserStatistics(request, OnSaveStats, OnSaveStatsError);
}

void UOrionTCPLink::OnSaveStats(ServerModels::UpdateUserStatisticsResult& result, void* userData)
{

}

void UOrionTCPLink::OnSaveStatsError(PlayFabError& error, void* userData)
{

}

void UOrionTCPLink::GetPlayerStats(void *Stats, FString PlayerID)
{
	ServerModels::GetUserStatisticsRequest request;

	request.PlayFabId = TCHAR_TO_UTF8(*PlayerID);

	server.GetUserStatistics(request, OnGetStats, OnGetStatsFailed, Stats);
}

//save player data when they leave the server
void UOrionTCPLink::SaveCharacter(AOrionPlayerController *PC)
{
	if(!PC)
		return;

	ServerModels::UpdateCharacterDataRequest request;

	request.PlayFabId = TCHAR_TO_UTF8(*PC->PlayFabID);
	request.CharacterId = TCHAR_TO_UTF8(*PC->CharacterID);
	request.Data = PC->GetInventoryData();
	request.Permission = ServerModels::UserDataPermissionPublic;

	server.UpdateCharacterReadOnlyData(request, UOrionTCPLink::OnSaveCharacterData, UOrionTCPLink::OnSaveCharacterDataError, PC);
}

void UOrionTCPLink::OnSaveCharacterData(ServerModels::UpdateCharacterDataResult& result, void* userData)
{
}

void UOrionTCPLink::OnSaveCharacterDataError(PlayFabError& error, void* userData)
{
}

void UOrionTCPLink::OnGetStats(ServerModels::GetUserStatisticsResult& result, void* userData)
{
	UOrionStats *Stats = static_cast<UOrionStats*>(userData);

	if(Stats)
	{
		TArray<FPlayerStats> aStats;

		for (std::map<std::string, PlayFab::Int32>::iterator it = result.UserStatistics.begin(); it != result.UserStatistics.end(); ++it)
		{
			FPlayerStats newStat;

			newStat.StatID = FStatID(atoi(it->first.c_str()));
			int32 index = int32(newStat.StatID) - 1;
			newStat.StatType = Stats->aStats[index].StatType;
			newStat.StatName = Stats->aStats[index].StatName;
		
			if (newStat.StatType == STATTYPE_INT)
				newStat.StatValueInt = int32(it->second);
			else
				newStat.StatValueFloat = float(it->second);

			aStats.Add(newStat);
		}

		Stats->aStats = aStats;
		Stats->bInitialized = true;
	}
}

void UOrionTCPLink::OnGetStatsFailed(PlayFabError& error, void* userData)
{
	UOrionStats *Stats = static_cast<UOrionStats*>(userData);

	if(Stats)
	{
		TArray<FPlayerStats> aStats;

		Stats->aStats = aStats;
		Stats->bInitialized = true;
	}
}

//client side calls go here
#else
bool UOrionTCPLink::Init(AOrionPlayerController *Owner)
{
	PlayFabSettings::useDevelopmentEnvironment = true;
	PlayFabSettings::serverURL = "https://93F0.playfabapi.com";
	PlayFabSettings::titleId = "93F0";
	PlayFabSettings::globalErrorHandler = GlobalErrorHandler;

	PlayerOwner = Owner;

	return true;
}

void UOrionTCPLink::Update()
{
	client.Update();
}

//create an account, once created, tell the backend to initialize some values for us
void UOrionTCPLink::CreateAccount(FString UserName, FString Password, FString EMail)
{
	if (PFState != PFSTATE_NONE)
		return;

	ClientModels::RegisterPlayFabUserRequest request;

	request.TitleId = "93F0";
	request.Username = TCHAR_TO_UTF8(*UserName);
	request.Password = TCHAR_TO_UTF8(*Password);
	request.Email = TCHAR_TO_UTF8(*EMail);

	PFState = PFSTATE_CREATINGACCOUNT;

	client.RegisterPlayFabUser(request, OnAddUser, OnAddUserError);
}

void UOrionTCPLink::CreateCharacter(FString UserName, FString Gender, FString BaseColor, FString CharacterClass)
{
	if (PFState != PFSTATE_NONE)
		return;

	ClientModels::RunCloudScriptRequest request;
	request.ActionId = std::string("CreateCharacter");
	request.ParamsEncoded = std::string("{\"CharacterName\":\"") + TCHAR_TO_UTF8(*UserName) + 
		std::string("\",\"CharacterType\":\"") + TCHAR_TO_UTF8(*CharacterClass) + 
		std::string("\",\"Gender\":\"") + TCHAR_TO_UTF8(*Gender) +
		std::string("\",\"SuitColor\":\"") + TCHAR_TO_UTF8(*BaseColor) +
		std::string("\"}");

	client.RunCloudScript(request, UOrionTCPLink::OnCreateCharacter, UOrionTCPLink::OnCreateCharacterError);

	PFState = PFSTATE_CREATINGCHARACTER;

	/*ClientModels::UpdateUserDataRequest request;

	request.Permission = ClientModels::UserDataPermission::UserDataPermissionPublic;

	std::map<std::string, std::string> newData;

	newData["CharacterName"] = TCHAR_TO_UTF8(*UserName);
	newData["CharacterGender"] = TCHAR_TO_UTF8(*Gender);
	newData["CharacterColor"] = TCHAR_TO_UTF8(*BaseColor);
	request.Data = newData;

	client.UpdateUserData(request, OnCreateCharacter, OnAddUserError);*/

	/*FString Delim = FString(",");
	if(connector == nullptr)
		connector = ConstructObject<UClientConnector>(UClientConnector::StaticClass());

	if (connector)
	{
		FString Info = FString("CreateCharacter") + Delim + PlayFabID + Delim + UserName + Delim + Gender + Delim + BaseColor + FString("\r\n\r\n");
		connector->SendInfo(Info);
	}*/
}

void UOrionTCPLink::SaveCharacter(AOrionPlayerController *PC)
{
}

void UOrionTCPLink::OnCreateCharacter(ClientModels::RunCloudScriptResult& result, void* userData)
{
	if (!PlayerOwner)
		return;

	std::string theResults = result.ResultsEncoded;
	FString JsonRaw = UTF8_TO_TCHAR(theResults.c_str());

	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		if (JsonParsed->GetStringField(TEXT("SUCCESS")) == TEXT("0"))
		{
			PlayerOwner->EventCharacterCreationComplete(false, "Error: Character Limit Has Been Reached!");
			return;
		}
	}

	//just read in our characters again
	ClientModels::RunCloudScriptRequest request;
	request.ActionId = std::string("GetCharacters");

	client.RunCloudScript(request, UOrionTCPLink::OnGetCharacterList, UOrionTCPLink::OnGetCharacterListError);

	//PFState = PFSTATE_NONE;

	//PlayerOwner->EventCharacterCreationComplete(true, TEXT("Character Created Successfully!"));
}

void UOrionTCPLink::OnCreateCharacterError(PlayFabError& error, void* userData)
{
	if (!PlayerOwner)
		return;

	PFState = PFSTATE_NONE;

	PlayerOwner->EventCharacterCreationComplete(false, TEXT("Character Creation Failed!"));
}

void UOrionTCPLink::DeleteCharacter(FString CharacterID)
{
	if (PFState != PFSTATE_NONE)
		return;

	ClientModels::RunCloudScriptRequest request;
	request.ActionId = std::string("DeleteCharacter");
	request.ParamsEncoded = std::string("{\"CharacterID\":\"") + TCHAR_TO_UTF8(*CharacterID) + std::string("\"}");

	client.RunCloudScript(request, UOrionTCPLink::OnDeleteCharacter, UOrionTCPLink::OnDeleteCharacterError);

	PFState = PFSTATE_DELETINGCHARACTER;
}

void UOrionTCPLink::OnDeleteCharacter(ClientModels::RunCloudScriptResult& result, void* userData)
{
	if (!PlayerOwner)
		return;

	//save our id CharacterId
	//result.ResultsEncoded

	//PFState = PFSTATE_NONE;

	//PlayerOwner->EventCharacterDeletionComplete(true);

	ClientModels::RunCloudScriptRequest request;
	request.ActionId = std::string("GetCharacters");

	client.RunCloudScript(request, UOrionTCPLink::OnGetCharacterList, UOrionTCPLink::OnGetCharacterListError);
}

void UOrionTCPLink::OnDeleteCharacterError(PlayFabError& error, void* userData)
{
	if (!PlayerOwner)
		return;

	PFState = PFSTATE_NONE;

	PlayerOwner->EventCharacterDeletionComplete(false);
}

//get the backend to tell us if this character info is legit or not
void UOrionTCPLink::SelectCharacter(int32 Index)
{
	/*FString Delim = FString(",");
	if (connector == nullptr)
		connector = ConstructObject<UClientConnector>(UClientConnector::StaticClass());

	if (connector)
	{
		FString Info = FString("SelectCharacter") + Delim + PlayFabID + Delim + Index + FString("\r\n\r\n");
		connector->SendInfo(Info);
	}*/
}

void UOrionTCPLink::GetPlayerStats(void *Stats)
{
	//ClientModels::GetUserStatisticsRequest request;
	//request.PlayFabID = TCHAR_TO_UTF8(*PlayFabID);
	client.GetUserStatistics(OnGetStats, OnGetStatsFailed, Stats);
}

void UOrionTCPLink::OnGetStats(ClientModels::GetUserStatisticsResult& result, void* userData)
{
	UOrionStats *Stats = static_cast<UOrionStats*>(userData);

	if(Stats)
	{
		TArray<FPlayerStats> aStats;

		Stats->aStats = aStats;
		Stats->bInitialized = true;
	}
}

void UOrionTCPLink::OnGetStatsFailed(PlayFabError& error, void* userData)
{
	UOrionStats *Stats = static_cast<UOrionStats*>(userData);

	if(Stats)
	{
		TArray<FPlayerStats> aStats;

		Stats->aStats = aStats;
		Stats->bInitialized = true;
	}
}

void UOrionTCPLink::Login(FString UserName, FString Password)
{
	//PlayFabClientAPI client;

	if (PFState != PFSTATE_NONE)
		return;

	ClientModels::LoginWithPlayFabRequest request;
	request.TitleId = "93F0";
	request.Username = TCHAR_TO_UTF8(*UserName);
	request.Password = TCHAR_TO_UTF8(*Password);

	PFState = PFSTATE_LOGGINGIN;

	client.LoginWithPlayFab(request, UOrionTCPLink::OnLogin, UOrionTCPLink::OnLoginError);
}

void UOrionTCPLink::LoginComplete(bool bSucceded, FString msg)
{
	if (!PlayerOwner)
		return;

	AOrionPlayerController *pPlayer = PlayerOwner;

	if (bSucceded)
	{
		//pPlayer->EventLoginComplete(true, FString(""), FString(""));
		//RetrieveCharacterData();
		/*FString Delim = FString(",");
		if(connector == nullptr)
			connector = ConstructObject<UClientConnector>(UClientConnector::StaticClass());

		if (connector)
		{
			FString Info = FString("CreateAccount") + Delim + PlayFabID + FString("\r\n\r\n");
			connector->SendInfo(Info);
		}*/
	}
	if (!bSucceded)
		pPlayer->EventLoginComplete(false, msg);
}

void UOrionTCPLink::RetrieveCharacterData(bool bLoggingIn)
{
	std::list<std::string> Keys;

	Keys = GetCharacterInfo();

	ClientModels::GetUserDataRequest request;

	request.PlayFabId = TCHAR_TO_UTF8(*PlayFabID);
	request.Keys = Keys;

	if(bLoggingIn)
		client.GetUserReadOnlyData(request, OnCharacterData, OnCharacterDataFailed);
	else
		client.GetUserReadOnlyData(request, OnCharacterFinalized, OnCharacterFinalizedFailed);
}

void UOrionTCPLink::OnCharacterFinalized(ClientModels::GetUserDataResult& result, void* userData)
{
	AOrionPlayerController *pPlayer = PlayerOwner;

	CharacterDatas.Empty();

	for(std::map<std::string, ClientModels::UserDataRecord>::iterator it = result.Data.begin(); it!=result.Data.end(); ++it)
	{
		AddToCharacterData(it->first, it->second);
	}

	//tell the player to redraw the character stuff
	if(pPlayer)
	{
		pPlayer->EventSetCharacterDatas(CharacterDatas);
		pPlayer->EventCharacterFinalized(true, FString("Login Successful!"));
	}
}

void UOrionTCPLink::OnCharacterData(ClientModels::GetUserDataResult& result, void* userData)
{
	AOrionPlayerController *pPlayer = PlayerOwner;

	if(pPlayer)
		pPlayer->EventLoginComplete(true, FString("Login Successful!"));

	CharacterDatas.Empty();

	for(std::map<std::string, ClientModels::UserDataRecord>::iterator it = result.Data.begin(); it!=result.Data.end(); ++it)
	{
		AddToCharacterData(it->first, it->second);
	}

	//tell the player to redraw the character stuff
	if(pPlayer)
		pPlayer->EventSetCharacterDatas(CharacterDatas);
}

void UOrionTCPLink::AddToCharacterData(const std::string first, const ClientModels::UserDataRecord second)
{
	//trim the number from the end of first and that will be our slot index
	/*std::string title = first.substr(0, first.length()-1);
	int32 index = atoi(first.substr(first.length()-1).c_str());

	if(title == CharacterName)
		CharacterDatas[index-1].pName = UTF8_TO_TCHAR(second.Value.c_str());
	else if(title == CharacterLevel)
		CharacterDatas[index-1].Level = atoi(second.Value.c_str());
	else if(title == CharacterExp)
		CharacterDatas[index-1].Exp = atoi(second.Value.c_str());
	else if(title == CharacterHelmet)
		CharacterDatas[index-1].HelmetID = atoi(second.Value.c_str());
	else if(title == CharacterChest)
		CharacterDatas[index-1].ChestID = atoi(second.Value.c_str());
	else if(title == CharacterHands)
		CharacterDatas[index-1].HandsID = atoi(second.Value.c_str());
	else if(title == CharacterLegs)
		CharacterDatas[index-1].LegsID = atoi(second.Value.c_str());
	else if(title == CharacterSex)
		CharacterDatas[index-1].Sex = UTF8_TO_TCHAR(second.Value.c_str());
	else if(title == CharacterWeapon)
		CharacterDatas[index-1].Weapon = atoi(second.Value.c_str());*/
}

void UOrionTCPLink::OnCharacterDataFailed(PlayFabError& error, void* userData)
{
	AOrionPlayerController *pPlayer = PlayerOwner;

	if(pPlayer)
		pPlayer->EventLoginComplete(false, FString("Login Failed!"));
}

void UOrionTCPLink::OnCharacterFinalizedFailed(PlayFabError& error, void* userData)
{
	AOrionPlayerController *pPlayer = PlayerOwner;

	if(pPlayer)
		pPlayer->EventCharacterFinalized(true, FString("Character Creation Failed!"));
}

void UOrionTCPLink::CharacterCreationComplete(bool bSucceded, FString Message)
{
	if (!PlayerOwner)
		return;

	//refresh our character loadout
	if(bSucceded)
		RetrieveCharacterData(false);

	PlayerOwner->EventCharacterCreationComplete(bSucceded, Message);
}

void UOrionTCPLink::AccountCreationComplete(bool bSucceded, FString Message)
{
	if (!PlayerOwner)
		return;

	if(bSucceded)
		RetrieveCharacterData(true);
	
	//if succeded, change status message in umg
	PlayerOwner->EventAccountCreationComplete(bSucceded, Message);
}

void UOrionTCPLink::SelectCharacterComplete(bool bSucceded, FString Message)
{
	if (!PlayerOwner)
		return;

	PlayerOwner->EventSelectCharacterComplete(bSucceded, Message);
}

void UOrionTCPLink::AddUserComplete(bool bSucceded, FString msg)
{
	if (!PlayerOwner)
		return;

	AOrionPlayerController *pPlayer = PlayerOwner;

	if (bSucceded)
		pPlayer->EventAccountCreated(true, msg);
	if (!bSucceded)
		pPlayer->EventAccountCreated(false, msg);
}

void UOrionTCPLink::OnLogin(ClientModels::LoginResult& result, void* userData)
{
	UOrionTCPLink::SessionTicket = UTF8_TO_TCHAR(result.SessionTicket.c_str());
	UOrionTCPLink::PlayFabID = UTF8_TO_TCHAR(result.PlayFabId.c_str());
	UOrionTCPLink::NewlyCreated = result.NewlyCreated;

	ClientModels::GetCloudScriptUrlRequest request;
	//request.Version = 4;
	request.Testing = false;

	client.GetCloudScriptUrl(request, UOrionTCPLink::OnGetCloudURL, UOrionTCPLink::OnGetCloudURLError);

	/*FString Delim = FString(",");
	if(UOrionTCPLink::connector != nullptr)
		UOrionTCPLink::connector->CloseSockets();
	
	UOrionTCPLink::connector = ConstructObject<UClientConnector>(UClientConnector::StaticClass());

	if (UOrionTCPLink::connector)
	{
		FString Info = FString("CreateAccount") + Delim + UOrionTCPLink::PlayFabID + FString("\r\n\r\n");
		UOrionTCPLink::connector->SendInfo(Info);
	}*/

	////UOrionTCPLink::LoginComplete(true, 0);
}

void UOrionTCPLink::OnLoginError(PlayFabError& error, void* userData)
{
	switch(error.ErrorCode)
	{
	case 1001:
		UOrionTCPLink::LoginComplete(false, "Account Not Found!");
		break;
	case 1002:
		UOrionTCPLink::LoginComplete(false, "This Account Has Been Banned!");
		break;
	case 1003:
		UOrionTCPLink::LoginComplete(false, "Invalid Username or Password!");
		break;
	default:
		UOrionTCPLink::LoginComplete(false, "An Error Has Occured!");
		break;
	}
}

void UOrionTCPLink::OnGetCharacterData(ClientModels::GetCharacterDataResult& result, void* userData)
{

}

void UOrionTCPLink::OnGetChracterDataError(PlayFabError& error, void* userData)
{

}

void UOrionTCPLink::OnGetCloudURL(ClientModels::GetCloudScriptUrlResult& result, void* userData)
{
	UOrionTCPLink::CloudScriptURL = UTF8_TO_TCHAR(result.Url.c_str());

	//now that we have our url, call the script to grab our character information
	ClientModels::RunCloudScriptRequest request;
	request.ActionId = std::string("GetCharacters");
	
	client.RunCloudScript(request, UOrionTCPLink::OnGetCharacterList, UOrionTCPLink::OnGetCharacterListError);
}

void UOrionTCPLink::OnGetCloudURLError(PlayFabError& error, void* userData)
{
	AOrionPlayerController *pPlayer = PlayerOwner;

	PFState = PFSTATE_NONE;

	if (pPlayer)
		pPlayer->EventLoginComplete(true, FString("Failed to retrieve character info!"));
}

void UOrionTCPLink::OnGetCharacterList(ClientModels::RunCloudScriptResult& result, void* userData)
{
	//read in our character data
	std::string theResults = result.ResultsEncoded;

	FString JsonRaw = UTF8_TO_TCHAR(theResults.c_str());

	CharacterDatas.Empty();

	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		TSharedPtr<FJsonObject> PlayersArray = JsonParsed->GetObjectField(TEXT("CharacterData"));
		if (PlayersArray.IsValid())
		{
			TArray<TSharedPtr<FJsonValue>> CharacterObjects = PlayersArray->GetArrayField(TEXT("Characters"));
			//PlayersArray = OverheadObject->GetArrayField(TEXT("Characters"));

			for (int32 i = 0; i < CharacterObjects.Num(); i++)
			{
				FCharacterData Data;

				TSharedPtr<FJsonObject> CharacterObject = CharacterObjects[i]->AsObject();
				Data.pName = CharacterObject->GetStringField("CharacterName");
				Data.pClass = CharacterObject->GetStringField("CharacterType");
				Data.CharacterID = CharacterObject->GetStringField("CharacterId");

				CharacterDatas.Add(Data);
			}
		}
	}

	//process each character
	AOrionPlayerController *pPlayer = PlayerOwner;

	if (pPlayer && PFState == PFSTATE_LOGGINGIN)
		pPlayer->EventLoginComplete(true, FString("Login Successful!"));

	//tell the player to redraw the character stuff
	if (pPlayer)
		pPlayer->EventSetCharacterDatas(CharacterDatas);

	if (pPlayer && PFState == PFSTATE_DELETINGCHARACTER)
		pPlayer->EventCharacterDeletionComplete(true);

	if (pPlayer && PFState == PFSTATE_CREATINGCHARACTER)
		pPlayer->EventCharacterCreationComplete(true, "SUCCESS");

	PFState = PFSTATE_NONE;
}

void UOrionTCPLink::OnGetCharacterListError(PlayFabError& error, void* userData)
{
	AOrionPlayerController *pPlayer = PlayerOwner;

	if (pPlayer && PFState == PFSTATE_LOGGINGIN)
		pPlayer->EventLoginComplete(false, FString("Login Failed!"));

	if (pPlayer && PFState == PFSTATE_DELETINGCHARACTER)
		pPlayer->EventCharacterDeletionComplete(false);

	if (pPlayer && PFState == PFSTATE_CREATINGCHARACTER)
		pPlayer->EventCharacterCreationComplete(false, "CHARACTER CREATION FAILED");

	PFState = PFSTATE_NONE;
}

void OnAddUser(ClientModels::RegisterPlayFabUserResult& result, void* userData)
{
	UOrionTCPLink::PFState = PFSTATE_NONE;
	UOrionTCPLink::AddUserComplete(true, "Account Created Successfully!");
}

void OnAddUserError(PlayFabError& error, void* userData)
{
	UOrionTCPLink::PFState = PFSTATE_NONE;

	switch(error.ErrorCode)
	{
	case 1005:
		UOrionTCPLink::AddUserComplete(false, "Invalid EMail address!");
		break;
	case 1006:
		UOrionTCPLink::AddUserComplete(false, "EMail address already in use!");
		break;
	case 1009:
		UOrionTCPLink::AddUserComplete(false, "Username Not Available!");
		break;
	case 1007:
		UOrionTCPLink::AddUserComplete(false, "Invalid Username!");
		break;
	case 1008:
		UOrionTCPLink::AddUserComplete(false, "Invalid Password!");
		break;
	case 1000:
		UOrionTCPLink::AddUserComplete(false, "Invalid Characters Used!");
		break;
	default:
		UOrionTCPLink::AddUserComplete(false, "An Error Has Occured!");
		break;
	}
}
#endif

void GlobalErrorHandler(PlayFabError& error, void* userData)
{
	printf("Got a global error\n");
}
