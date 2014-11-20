// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "playfab/PlayFabClientAPI.h"
#include "OrionTCPLink.generated.h"

using namespace PlayFab;

/**
 * 
 */
UCLASS()
class ORION_API UOrionTCPLink : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	static bool Init(AOrionPlayerController *Owner);
	static void Login(FString UserName, FString Password);
	static void CreateAccount(FString UserName, FString Password, FString EMail);
	static void CreateCharacter(FString UserName, FString Gender, FString BaseColor);

	//void GlobalErrorHandler(PlayFabError& error, void* userData);

	static void LoginComplete(bool bSuccess, int32 error);
	static void AddUserComplete(bool bSuccess, int32 error);
	static void CharacterCreationComplete(bool bSuccess, int32 error);

	static void Update();

	static AOrionPlayerController *PlayerOwner;

	static FString SessionTicket;
	static FString PlayFabID;
	static bool NewlyCreated;

private:
	static PlayFabClientAPI client;
};
