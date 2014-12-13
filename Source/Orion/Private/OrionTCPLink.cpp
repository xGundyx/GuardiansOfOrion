// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "SocketSubsystem.h"
#include "Networking.h"
#include "SharedPointer.h"
#include "OrionTCPLink.h"
#include "StringConv.h"
#include "ClientConnector.h"
#include "BackendMappings.h"

using namespace Backend;

#if IS_SERVER
#else
void GlobalErrorHandler(PlayFabError& error, void* userData);

void OnAddUser(ClientModels::RegisterPlayFabUserResult& result, void* userData);
void OnAddUserError(PlayFabError& error, void* userData);

void OnLogin(ClientModels::LoginResult& result, void* userData);
void OnLoginError(PlayFabError& error, void* userData);

AOrionPlayerController *UOrionTCPLink::PlayerOwner = NULL;
PlayFabClientAPI UOrionTCPLink::client;
FString UOrionTCPLink::SessionTicket = FString("");
FString UOrionTCPLink::PlayFabID = FString("");
bool UOrionTCPLink::NewlyCreated = false;
FCharacterData UOrionTCPLink::CharacterDatas[4];
#endif

UClientConnector *UOrionTCPLink::connector;
UOrionTCPLink::UOrionTCPLink(const FObjectInitializer& ObejctInitializer)
	: Super(ObejctInitializer)
{

}

//server side calls go here
#if IS_SERVER
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
	ClientModels::RegisterPlayFabUserRequest request;
	request.TitleId = "93F0";
	request.Username = TCHAR_TO_UTF8(*UserName);
	request.Password = TCHAR_TO_UTF8(*Password);
	request.Email = TCHAR_TO_UTF8(*EMail);

	client.RegisterPlayFabUser(request, OnAddUser, OnAddUserError);
}

void UOrionTCPLink::CreateCharacter(FString UserName, FString Gender, FString BaseColor)
{
	/*ClientModels::UpdateUserDataRequest request;

	request.Permission = ClientModels::UserDataPermission::UserDataPermissionPublic;

	std::map<std::string, std::string> newData;

	newData["CharacterName"] = TCHAR_TO_UTF8(*UserName);
	newData["CharacterGender"] = TCHAR_TO_UTF8(*Gender);
	newData["CharacterColor"] = TCHAR_TO_UTF8(*BaseColor);
	request.Data = newData;

	client.UpdateUserData(request, OnCreateCharacter, OnAddUserError);*/

	FString Delim = FString(",");
	if(connector == nullptr)
		connector = ConstructObject<UClientConnector>(UClientConnector::StaticClass());

	if (connector)
	{
		FString Info = FString("CreateCharacter") + Delim + PlayFabID + Delim + UserName + Delim + Gender + Delim + BaseColor + FString("\r\n\r\n");
		connector->SendInfo(Info);
	}
}

//get the backend to tell us if this character info is legit or not
void UOrionTCPLink::SelectCharacter(int32 Index)
{
	FString Delim = FString(",");
	if (connector == nullptr)
		connector = ConstructObject<UClientConnector>(UClientConnector::StaticClass());

	if (connector)
	{
		FString Info = FString("SelectCharacter") + Delim + PlayFabID + Delim + Index + FString("\r\n\r\n");
		connector->SendInfo(Info);
	}
}

void UOrionTCPLink::Login(FString UserName, FString Password)
{
	//PlayFabClientAPI client;

	ClientModels::LoginWithPlayFabRequest request;
	request.TitleId = "93F0";
	request.Username = TCHAR_TO_UTF8(*UserName);
	request.Password = TCHAR_TO_UTF8(*Password);

	client.LoginWithPlayFab(request, OnLogin,OnLoginError);
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
		FString Delim = FString(",");
		if(connector == nullptr)
			connector = ConstructObject<UClientConnector>(UClientConnector::StaticClass());

		if (connector)
		{
			FString Info = FString("CreateAccount") + Delim + PlayFabID + FString("\r\n\r\n");
			connector->SendInfo(Info);
		}
	}
	if (!bSucceded)
		pPlayer->EventLoginComplete(false, msg);
}

void UOrionTCPLink::RetrieveCharacterData()
{
	std::list<std::string> Keys;

	Keys = GetCharacterInfo();

	ClientModels::GetUserDataRequest request;

	request.PlayFabId = TCHAR_TO_UTF8(*PlayFabID);
	request.Keys = Keys;

	client.GetUserReadOnlyData(request, OnCharacterData, OnCharacterDataFailed);
}

void UOrionTCPLink::OnCharacterData(ClientModels::GetUserDataResult& result, void* userData)
{
	AOrionPlayerController *pPlayer = PlayerOwner;

	if(pPlayer)
		pPlayer->EventLoginComplete(true, FString("Login Successful!"));

	for(int32 i = 0; i < 4; i++)
		CharacterDatas[i].Reset();

	for(std::map<std::string, ClientModels::UserDataRecord>::iterator it = result.Data.begin(); it!=result.Data.end(); ++it)
	{
		AddToCharacterData(it->first, it->second);
	}

	//tell the player to redraw the character stuff
	if(pPlayer)
		pPlayer->EventDrawCharacterDatas(CharacterDatas[0], CharacterDatas[1], CharacterDatas[2] ,CharacterDatas[3]);
}

void UOrionTCPLink::AddToCharacterData(const std::string first, const ClientModels::UserDataRecord second)
{
	//trim the number from the end of first and that will be our slot index
	std::string title = first.substr(0, first.length()-1);
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
		CharacterDatas[index-1].Weapon = atoi(second.Value.c_str());
}

void UOrionTCPLink::OnCharacterDataFailed(PlayFabError& error, void* userData)
{
}

void UOrionTCPLink::CharacterCreationComplete(bool bSucceded, FString Message)
{
	if (!PlayerOwner)
		return;

	PlayerOwner->EventCharacterCreationComplete(bSucceded, Message);
}

void UOrionTCPLink::AccountCreationComplete(bool bSucceded, FString Message)
{
	if (!PlayerOwner)
		return;

	if(bSucceded)
		RetrieveCharacterData();
	
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
		pPlayer->EventAccountCreationComplete(true, msg);
	if (!bSucceded)
		pPlayer->EventAccountCreationComplete(false, msg);
}

void GlobalErrorHandler(PlayFabError& error, void* userData)
{
	printf("Got a global error\n");
}

void OnLogin(ClientModels::LoginResult& result, void* userData)
{
	UOrionTCPLink::SessionTicket = UTF8_TO_TCHAR(result.SessionTicket.c_str());
	UOrionTCPLink::PlayFabID = UTF8_TO_TCHAR(result.PlayFabId.c_str());
	UOrionTCPLink::NewlyCreated = result.NewlyCreated;

	FString Delim = FString(",");
	if(UOrionTCPLink::connector != nullptr)
		UOrionTCPLink::connector->CloseSockets();
	
	UOrionTCPLink::connector = ConstructObject<UClientConnector>(UClientConnector::StaticClass());

	if (UOrionTCPLink::connector)
	{
		FString Info = FString("CreateAccount") + Delim + UOrionTCPLink::PlayFabID + FString("\r\n\r\n");
		UOrionTCPLink::connector->SendInfo(Info);
	}

	////UOrionTCPLink::LoginComplete(true, 0);
}

void OnLoginError(PlayFabError& error, void* userData)
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

void OnAddUser(ClientModels::RegisterPlayFabUserResult& result, void* userData)
{
	UOrionTCPLink::AddUserComplete(true, "Account Created Successfully!");
}

void OnAddUserError(PlayFabError& error, void* userData)
{
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
	default:
		UOrionTCPLink::AddUserComplete(false, "An Error Has Occured!");
		break;
	}
}
#endif


