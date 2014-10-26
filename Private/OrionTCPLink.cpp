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

void OnLogin(ClientModels::LoginResult& result, void* userData);
void OnLoginError(PlayFabError& error, void* userData);

AOrionPlayerController *UOrionTCPLink::PlayerOwner = NULL;
PlayFabClientAPI UOrionTCPLink::client;
bool UOrionTCPLink::bShouldUpdate = false;

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

	ClientModels::RegisterPlayFabUserRequest request;
	request.TitleId = "93F0";
	request.Username = "Grundyman";
	request.Password = "dumbweb";
	request.Email = "gundy84@hotmail.com";

	bShouldUpdate = true;
	client.RegisterPlayFabUser(request, OnAddUser, OnAddUserError);

	return true;
}

void UOrionTCPLink::Update()
{
	if (bShouldUpdate)
		client.Update();
}

void UOrionTCPLink::Login(FString UserName, FString Password)
{
	bShouldUpdate = true;

	PlayFabClientAPI client;

	ClientModels::LoginWithPlayFabRequest request;
	request.Username = TCHAR_TO_UTF8(&UserName);
	request.Password = TCHAR_TO_UTF8(&Password);

	client.LoginWithPlayFab(request, OnLogin,OnLoginError);
}

void UOrionTCPLink::LoginComplete(bool bSucceded, int32 error)
{
	bShouldUpdate = false;

	if (PlayerOwner)
		PlayerOwner->OpenInventory();

	if (bSucceded)
		UE_LOG(LogClass, Log, _T("SUCCESS"));
	if (!bSucceded)
		UE_LOG(LogClass, Log, _T("FAILURE %i"), error);
}

void GlobalErrorHandler(PlayFabError& error, void* userData)
{
	printf("Got a global error\n");
}

void OnLogin(ClientModels::LoginResult& result, void* userData)
{
	UOrionTCPLink::LoginComplete(true, 0);
}

void OnLoginError(PlayFabError& error, void* userData)
{
	UOrionTCPLink::LoginComplete(false, 1);
}

void OnAddUser(ClientModels::RegisterPlayFabUserResult& result, void* userData)
{
	UOrionTCPLink::LoginComplete(true, 0);
}

void OnAddUserError(PlayFabError& error, void* userData)
{
	UOrionTCPLink::LoginComplete(false, error.ErrorCode);

}


