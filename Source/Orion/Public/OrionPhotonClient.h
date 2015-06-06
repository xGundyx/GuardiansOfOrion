#pragma once

#include "GameFramework/Actor.h"
#include "OrionLoadBalancing.h"
#include "OrionChatListener.h"
#include "OrionPhotonClient.generated.h"

UCLASS()
class ORION_API AOrionPhotonClient : public AActor
{
	GENERATED_BODY()

public:
	AOrionPhotonClient(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	FString serverAddress;
	FString AppID;
	FString ChatAppID;
	FString appVersion;

	void SendChatMessage(FString Channel, FString Message);

	ExitGames::Chat::Client* GetChatClient();

private:
	ExitGames::LoadBalancing::Client* client;
	ExitGames::Chat::Client* chatclient;

	UOrionLoadBalancing* listener;
	UOrionChatListener* chatlistener;
};