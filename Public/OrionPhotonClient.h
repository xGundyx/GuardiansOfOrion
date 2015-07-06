#pragma once

//#if !IS_SERVER

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
#if !IS_SERVER
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	void SendChatMessage(FString Channel, FString Message);

	ExitGames::Chat::Client* GetChatClient();

private:
	ExitGames::LoadBalancing::Client* client;
	ExitGames::Chat::Client* chatclient;

	UOrionLoadBalancing* listener;
	UOrionChatListener* chatlistener;
#endif
public:
	FString serverAddress;
	FString AppID;
	FString ChatAppID;
	FString appVersion;
};