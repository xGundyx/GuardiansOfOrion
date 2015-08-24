// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
//#include "SocketSubsystem.h"
//#include "Networking.h"
#include "SharedPointer.h"
#include "OrionTCPLink.h"
#include "StringConv.h"
//#include "ClientConnector.h"
#include "BackendMappings.h"
#include "OrionPRI.h"
#include "OrionStats.h"

/*using namespace Backend;

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
FString UOrionTCPLink::ChatToken = FString("");
AOrionPhotonClient *UOrionTCPLink::PhotonClient = nullptr;

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

void UOrionTCPLink::Update()
{
	server.Update();
}

void UOrionTCPLink::GetCharacterData(AOrionPlayerController *PC)
{
	if(!PC)
		return;

	ServerModels::GetCharacterDataRequest request;

	AOrionPRI *PRI = Cast<AOrionPRI>(PC->PlayerState);

	if (!PRI)
		return;

	request.PlayFabId = TCHAR_TO_UTF8(*PRI->PlayFabID);
	request.CharacterId = TCHAR_TO_UTF8(*PRI->CharacterID);

	server.GetCharacterReadOnlyData(request, OnGetCharacterData, OnGetCharacterDataError, PC);
}

void UOrionTCPLink::OnGetCharacterData(ServerModels::GetCharacterDataResult& result, void* userData)
{
	AOrionPlayerController *PC = static_cast<AOrionPlayerController*>(userData);

	if(PC)
	{
		PC->PopulateInventory(result.Data);
	}
}

void UOrionTCPLink::OnGetCharacterDataError(PlayFabError& error, void* userData)
{
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

	AOrionPRI *PRI = Cast<AOrionPRI>(PC->PlayerState);

	if (!PRI)
		return;

	request.PlayFabId = TCHAR_TO_UTF8(*PRI->PlayFabID);
	request.CharacterId = TCHAR_TO_UTF8(*PRI->CharacterID);
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
	PhotonClient = nullptr; //this is needed to reinit the photon client when reconnecting

	PFState = PFSTATE_NONE;

	return true;
}

void UOrionTCPLink::Update()
{
	client.Update();
}

//this is called from a non dedicated server host, need to allow grabbing of other non local controller's data
void UOrionTCPLink::GetCharacterData(AOrionPlayerController *PC)
{
	if (PC == nullptr)
		return;

	AOrionPRI *PRI = Cast<AOrionPRI>(PC->PlayerState);
	if (PRI)
	{
		if (PRI->CharacterID.IsEmpty())
			return;

		ClientModels::RunCloudScriptRequest request;
		request.ActionId = std::string("GetFullCharacterData");
		request.ParamsEncoded = std::string("{\"CharacterID\":\"") + TCHAR_TO_UTF8(*PRI->CharacterID) +
			std::string("\"}");

		client.RunCloudScript(request, UOrionTCPLink::OnGetCharacterData, UOrionTCPLink::OnGetCharacterDataError, PC);
	}
}

void UOrionTCPLink::OnGetCharacterData(ClientModels::RunCloudScriptResult& result, void* userData)
{
	AOrionPlayerController *PC = static_cast<AOrionPlayerController*>(userData);

	if (PC == nullptr)
		return;

	std::string theResults = result.ResultsEncoded;

	FString JsonRaw = UTF8_TO_TCHAR(theResults.c_str());

	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		TSharedPtr<FJsonObject> DataArray = JsonParsed->GetObjectField(TEXT("CharacterData"));
		if (DataArray.IsValid())
		{
			TSharedPtr<FJsonObject> DataObject = DataArray->GetObjectField("data");

			PC->PopulateInventory(DataObject);
		}
	}
}

void UOrionTCPLink::OnGetCharacterDataError(PlayFabError& error, void* userData)
{
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
			PFState = PFSTATE_NONE;
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
	if (Index >= CharacterDatas.Num())
		return;

	ClientModels::RunCloudScriptRequest request;
	request.ActionId = std::string("GetFullCharacterData");
	request.ParamsEncoded = std::string("{\"CharacterID\":\"") + TCHAR_TO_UTF8(*CharacterDatas[Index].CharacterID) +
		std::string("\"}");

	CurrentCharacterID = TCHAR_TO_UTF8(*CharacterDatas[Index].CharacterID);
	if (PlayerOwner)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(PlayerOwner->PlayerState);
		if (PRI)
			PRI->CharacterID = CurrentCharacterID;
	}

	client.RunCloudScript(request, UOrionTCPLink::OnSelectCharacter, UOrionTCPLink::OnSelectCharacterError);
}

void UOrionTCPLink::OnSelectCharacter(ClientModels::RunCloudScriptResult& result, void* userData)
{
	std::string theResults = result.ResultsEncoded;

	FString JsonRaw = UTF8_TO_TCHAR(theResults.c_str());

	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		TSharedPtr<FJsonObject> DataArray = JsonParsed->GetObjectField(TEXT("CharacterData"));
		if (DataArray.IsValid())
		{
			TSharedPtr<FJsonObject> DataObject = DataArray->GetObjectField("data");

			if (PlayerOwner)
				PlayerOwner->PopulateInventory(DataObject);
		}
	}

	//for now just have the player pick a character and connect directly to our test server (will need some kind of lobby/browser/matchmaking screen)
	if (PlayerOwner)
		PlayerOwner->ConsoleCommand("open 192.168.1.64:9876");
}

void UOrionTCPLink::OnSelectCharacterError(PlayFabError& error, void* userData)
{

}

void UOrionTCPLink::GetPlayerStats(void *Stats)
{
	return;
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

	if (PlayerOwner)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(PlayerOwner->PlayerState);

		if (PRI)
		{
			PRI->PlayFabID = UOrionTCPLink::PlayFabID;
			PRI->SessionTicket = UOrionTCPLink::SessionTicket;
			PlayerOwner->ServerSetPlayFabInfo(UOrionTCPLink::PlayFabID, UOrionTCPLink::SessionTicket, UOrionTCPLink::CurrentCharacterID);
		}
	}

	ClientModels::GetCloudScriptUrlRequest request;
	//request.Version = 4;
	request.Testing = false;

	client.GetCloudScriptUrl(request, UOrionTCPLink::OnGetCloudURL, UOrionTCPLink::OnGetCloudURLError);

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

	PFState = PFSTATE_NONE;
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
				Data.Level = FCString::Atoi(*CharacterObject->GetStringField("Level"));
				Data.Sex = CharacterObject->GetStringField("Gender");
				Data.SuitColor = CharacterObject->GetStringField("SuitColor");

				ParseDisplayItems(CharacterObject, &Data);

				CharacterDatas.Add(Data);
			}
		}
	}

	ClientModels::GetPhotonAuthenticationTokenRequest request;
	request.PhotonApplicationId = std::string("7ad31ad4-fd07-4343-ab4a-f40570b0dfd6"); //chat
	//request.PhotonApplicationId = std::string("899b07e0-1737-4e33-a93d-9bdd550f17ba"); //realtime

	client.GetPhotonAuthenticationToken(request, UOrionTCPLink::OnGetPhotonChatToken, UOrionTCPLink::OnGetPhotonChatTokenError);
}

void UOrionTCPLink::ParseDisplayItems(TSharedPtr<FJsonObject> jObject, FCharacterData* Data)
{
	TSharedPtr<FJsonObject> tObject = jObject->GetObjectField("Inventory");

	TArray<TSharedPtr<FJsonValue>> InventoryObjects = tObject->GetArrayField(TEXT("Inventory"));

	for (int32 i = 0; i < InventoryObjects.Num(); i++)
	{
		TSharedPtr<FJsonObject> InvObject = InventoryObjects[i]->AsObject();

		FString slot = InvObject->GetStringField("ItemClass");

		if (slot == TEXT("Helmet"))
		{
			Data->HelmetID.ItemName = InvObject->GetStringField("ItemId");
			Data->HelmetID.InstanceID = InvObject->GetStringField("ItemInstanceID");
		}
		else if (slot == TEXT("Chest"))
		{
			Data->ChestID.ItemName = InvObject->GetStringField("ItemId");
			Data->ChestID.InstanceID = InvObject->GetStringField("ItemInstanceID");
		}
		else if (slot == TEXT("Arms"))
		{
			Data->HandsID.ItemName = InvObject->GetStringField("ItemId");
			Data->HandsID.InstanceID = InvObject->GetStringField("ItemInstanceID");
		}
		else if (slot == TEXT("Legs"))
		{
			Data->LegsID.ItemName = InvObject->GetStringField("ItemId");
			Data->LegsID.InstanceID = InvObject->GetStringField("ItemInstanceID");
		}
	}
}

void UOrionTCPLink::OnGetPhotonChatToken(ClientModels::GetPhotonAuthenticationTokenResult& result, void* userData)
{
	//process each character
	AOrionPlayerController *pPlayer = PlayerOwner;

	ChatToken = UTF8_TO_TCHAR(result.PhotonCustomAuthenticationToken.c_str());

	if (!PhotonClient && pPlayer)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.bNoCollisionFail = true;
		PhotonClient = pPlayer->GetWorld()->SpawnActor<AOrionPhotonClient>(AOrionPhotonClient::StaticClass(), SpawnInfo);
	}

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

void UOrionTCPLink::OnGetPhotonChatTokenError(PlayFabError& error, void* userData)
{
	AOrionPlayerController *pPlayer = PlayerOwner;

	if (pPlayer && PFState == PFSTATE_LOGGINGIN)
		pPlayer->EventLoginComplete(false, FString("Login Failed: Unable to Connect to Photon!"));

	PFState = PFSTATE_NONE;
}

void UOrionTCPLink::SendChatMessage(FString Channel, FString Message)
{
	if (PhotonClient)
	{
		PhotonClient->SendChatMessage(Channel, Message);
	}
}

void UOrionTCPLink::ChatMessageReceived(FString ChannelName, TArray<FString> Messages)
{
	AOrionPlayerController *pPlayer = PlayerOwner;

	//send the message to our in game chat windows
	if (pPlayer && pPlayer->GetChatManager())
	{
		for (int32 i = 0; i < Messages.Num(); i++)
			pPlayer->GetChatManager()->AddChatEntry(ChannelName, Messages[i]);
	}
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
}*/
