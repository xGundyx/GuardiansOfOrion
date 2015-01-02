// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Networking.h"
#include "ClientConnector.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UClientConnector : public UObject
{
	GENERATED_BODY()
public:
	UClientConnector(const FObjectInitializer& ObejctInitializer);

#if !IS_SERVER
	static bool CreateSenderSocket();
	static void SendInfo(FString Info);
	static void Update();
	static void CloseSockets();

	static bool IsSuccess(FString Message, FString &ClientMessage);

private:
	static FSocket *LoginSenderSocket;
	static FSocket *LoginReceiverSocket;
	static FIPv4Endpoint LoginServerAddress;
#endif
};

