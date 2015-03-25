// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "OrionGameMode.generated.h"

UCLASS(minimalapi)
class AOrionGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AOrionGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	virtual APlayerController* Login(class UPlayer* NewPlayer, const FString& Portal, const FString& Options, const TSharedPtr<class FUniqueNetId>& UniqueId, FString& ErrorMessage);
};



