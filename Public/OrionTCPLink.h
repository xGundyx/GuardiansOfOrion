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

	//void GlobalErrorHandler(PlayFabError& error, void* userData);

	static void LoginComplete(bool bSuccess, int32 error);

	static void Update();

private:
	static AOrionPlayerController *PlayerOwner;
	static PlayFabClientAPI client;
	static bool bShouldUpdate;
};
