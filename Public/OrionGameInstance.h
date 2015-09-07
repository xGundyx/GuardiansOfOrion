// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "OrionGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UOrionGameInstance(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(BlueprintReadWrite, Category = PlayFab)
		FString PlayFabID;
	UPROPERTY(BlueprintReadWrite, Category = PlayFab)
		FString SessionTicket;
	UPROPERTY(BlueprintReadWrite, Category = PlayFab)
		FString CharacterID;
	UPROPERTY(BlueprintReadWrite, Category = PlayFab)
		FString PlayFabName;
	UPROPERTY(BlueprintReadWrite, Category = PlayFab)
		FString ChatToken;
};
