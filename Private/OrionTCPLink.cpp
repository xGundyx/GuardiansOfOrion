// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "SocketSubsystem.h"
#include "Networking.h"
#include "SharedPointer.h"
#include "OrionTCPLink.h"
#include "StringConv.h"

void GlobalErrorHandler(PlayFabError& error, void* userData);

void OnAddUser(ClientModels::RegisterPlayFabUserResult& result, void* userData);
void OnAddUserError(PlayFabError& error, void* userData);

void OnCreateCharacter(ClientModels::UpdateUserDataResult& result, void* userData);
void OnCreateCharacterError(PlayFabError& error, void* userData);

void OnLogin(ClientModels::LoginResult& result, void* userData);
void OnLoginError(PlayFabError& error, void* userData);

AOrionPlayerController *UOrionTCPLink::PlayerOwner = NULL;
PlayFabClientAPI UOrionTCPLink::client;
FString UOrionTCPLink::SessionTicket = FString("");
FString UOrionTCPLink::PlayFabID = FString("");
bool UOrionTCPLink::NewlyCreated = false;

UOrionTCPLink::UOrionTCPLink(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

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
	ClientModels::UpdateUserDataRequest request;

	request.Permission = ClientModels::UserDataPermission::UserDataPermissionPublic;

	std::map<std::string, std::string> newData;

	newData["CharacterName"] = TCHAR_TO_UTF8(*UserName);
	newData["CharacterGender"] = TCHAR_TO_UTF8(*Gender);
	newData["CharacterColor"] = TCHAR_TO_UTF8(*BaseColor);
	request.Data = newData;

	client.UpdateUserData(request, OnCreateCharacter, OnAddUserError);
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

void UOrionTCPLink::LoginComplete(bool bSucceded, int32 error)
{
	if (!PlayerOwner)
		return;

	AOrionPlayerController *pPlayer = PlayerOwner;

	if (bSucceded)
		pPlayer->EventLoginComplete(true, FString(""), FString(""));
	if (!bSucceded)
		pPlayer->EventLoginComplete(false, FString(""), FString(""));
}

void UOrionTCPLink::CharacterCreationComplete(bool bSucceded, int32 error)
{
	if (!PlayerOwner)
		return;

	AOrionPlayerController *pPlayer = PlayerOwner;

	if (bSucceded)
		pPlayer->EventCharacterCreationComplete(true, FString(""));
	if (!bSucceded)
		pPlayer->EventCharacterCreationComplete(false, FString(""));
}

void UOrionTCPLink::AddUserComplete(bool bSucceded, int32 error)
{
	if (!PlayerOwner)
		return;

	AOrionPlayerController *pPlayer = PlayerOwner;

	if (bSucceded)
		pPlayer->EventAccountCreationComplete(true, FString(""));
	if (!bSucceded)
		pPlayer->EventAccountCreationComplete(false, FString(""));
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

	UOrionTCPLink::LoginComplete(true, 0);
}

void OnLoginError(PlayFabError& error, void* userData)
{
	UOrionTCPLink::LoginComplete(false, 1);
}

void OnAddUser(ClientModels::RegisterPlayFabUserResult& result, void* userData)
{
	UOrionTCPLink::AddUserComplete(true, 0);
}

void OnAddUserError(PlayFabError& error, void* userData)
{
	UOrionTCPLink::AddUserComplete(false, error.ErrorCode);
}

void OnCreateCharacter(ClientModels::UpdateUserDataResult& result, void* userData)
{
	UOrionTCPLink::CharacterCreationComplete(true, 0);
}

void OnCreateCharacterError(PlayFabError& error, void* userData)
{
	UOrionTCPLink::CharacterCreationComplete(false, 1);
}


