// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionTCPLink.h"
#include "ClientConnector.h"

#if !IS_SERVER
FSocket * UClientConnector::LoginSenderSocket;
FSocket *UClientConnector::LoginReceiverSocket;
FIPv4Endpoint UClientConnector::LoginServerAddress;
#endif

UClientConnector::UClientConnector(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if !IS_SERVER

bool UClientConnector::IsSuccess(FString Message, FString &ClientMessage)
{
	int32 pos = Message.Find("::ERROR::");

	if (pos != INDEX_NONE)
	{
		ClientMessage = Message.Mid(pos + 9);
		return false;
	}
	else
	{
		pos = Message.Find("::SUCCESS::");
		if (pos != INDEX_NONE)
		{
			ClientMessage = Message.Mid(pos + 12);
			return true;
		}
	}

	return false;
}

void UClientConnector::Update()
{
	//check to see if we have any messages to receive
	uint32 bytes_to_read = 0;
	TArray<uint8> ReceivedData;
	if (LoginSenderSocket && LoginSenderSocket->HasPendingData(bytes_to_read))
	{
		ReceivedData.Init(FMath::Min(bytes_to_read, 65507u));

		int32 Read = 0;
		if(LoginSenderSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read))
		{
			//convert the data into a string
			const std::string cstr(reinterpret_cast<const char*>(ReceivedData.GetData()), ReceivedData.Num());

			//FString can take in the c_str() of a std::string
			FString Message = FString(cstr.c_str());

			//strip our message header
			int32 pos = Message.Find("::");
			if (pos != INDEX_NONE)
			{
				FString ClientMessage;
				FString header = Message.Mid(0, pos);
				bool bSuccess = IsSuccess(Message, ClientMessage);
				if (header == "CREATECHARACTER")
					UOrionTCPLink::CharacterCreationComplete(bSuccess, ClientMessage);
				else if (header == "CREATEACCOUNT")
					UOrionTCPLink::AccountCreationComplete(bSuccess, ClientMessage);
			}

			//send this message to our ui
			//UOrionTCPLink::CharacterCreationComplete(true, Message);
		}
	}
}

bool UClientConnector::CreateSenderSocket()
{
	//we already have a socket
	if (LoginSenderSocket)
		return true;

	FIPv4Endpoint Endpoint(FIPv4Address(206,75,103,142), 7890);
	LoginSenderSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("LoginSocket"), false);

	if (LoginSenderSocket)
	{
		//connect us
		FString address = TEXT("192.168.1.66");
		int32 port = 7890;
		FIPv4Address ip;
		FIPv4Address::Parse(address, ip);

		TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		addr->SetIp(ip.GetValue());
		addr->SetPort(port);
		if (LoginSenderSocket->Connect(*addr) == false)
		{
			//connect us
			address = TEXT("206.75.103.142");
			FIPv4Address::Parse(address, ip);

			addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
			addr->SetIp(ip.GetValue());
			addr->SetPort(port);
			if (LoginSenderSocket->Connect(*addr) == false)
			{
				UOrionTCPLink::PlayerOwner->ReceiveErrorMessage(TEXT("Unable to connect to backend servers.  Maybe they're down:("));
				return false;
			}
		}
	}
	else
	{
		UOrionTCPLink::PlayerOwner->ReceiveErrorMessage(TEXT("Unable to create socket."));
		return false;
	}

	return true;
}

void UClientConnector::SendInfo(FString Info)
{
	//attempt to create a socket if we don't have one
	if (!CreateSenderSocket())
		return;

	int32 BytesSent;
	TCHAR *serializedChar = Info.GetCharArray().GetData();
	LoginSenderSocket->Send((uint8*)TCHAR_TO_UTF8(serializedChar), FCString::Strlen(serializedChar), BytesSent);
}

void UClientConnector::CloseSockets()
{
	if (LoginSenderSocket)
	{
		LoginSenderSocket->Close();
		LoginSenderSocket = nullptr;
	}
}

#endif


