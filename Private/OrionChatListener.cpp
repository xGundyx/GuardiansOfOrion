#include "Orion.h"
#if !IS_SERVER
#include "OrionChatListener.h"

using namespace ExitGames::Common;
using namespace ExitGames::Chat;

class UOrionTCPLink;

void UOrionChatListener::debugReturn(DebugLevel::DebugLevel debugLevel, const JString& string)
{
	//UE_LOG(LogTemp, Warning, TEXT("DebugReturn %s"), string);
}

void UOrionChatListener::onStateChange(int state)
{
	switch (state)
	{
	case ExitGames::Chat::ClientState::ConnectedToFrontEnd:
		ExitGames::Common::JVector<ExitGames::Common::JString> channels;
		channels.addElement("Global");
		channels.addElement("Trade");
		if (view && view->GetChatClient())
			view->GetChatClient()->opSubscribe(channels);
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("OnStateChanged %i"), state);
}

void UOrionChatListener::connectionErrorReturn(int errorCode)
{
	UE_LOG(LogTemp, Warning, TEXT("ConnectionErrorReturn %i"), errorCode);
}

void UOrionChatListener::clientErrorReturn(int errorCode)
{
	UE_LOG(LogTemp, Warning, TEXT("ClientErrorReturn %i"), errorCode);
}

void UOrionChatListener::warningReturn(int warningCode)
{
	UE_LOG(LogTemp, Warning, TEXT("WarningReturn %i"), warningCode);
}

void UOrionChatListener::serverErrorReturn(int errorCode)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerErrorReturn %i"), errorCode);
}

void UOrionChatListener::connectReturn(int errorCode, const JString& errorString)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerErrorReturn %i"), errorCode);
}

void UOrionChatListener::disconnectReturn(void)
{
	UE_LOG(LogTemp, Warning, TEXT("DisconnectReturn"));
}

void UOrionChatListener::subscribeReturn(const JVector<JString>& channels, const JVector<bool>& results)
{
	UE_LOG(LogTemp, Warning, TEXT("SubscribeReturn"));
}

void UOrionChatListener::unsubscribeReturn(const JVector<JString>& channels)
{
	UE_LOG(LogTemp, Warning, TEXT("UnSubscribeReturn"));
}

void UOrionChatListener::onStatusUpdate(const JString& user, int status, bool gotMessage, const Object& message)
{
}

void UOrionChatListener::onGetMessages(const JString& channelName, const JVector<JString>& senders, const JVector<Object>& messages)
{
	//get the message and put it in our chat log
	TArray<FString> strMessages;

	JString str;
	FString chn;

	for (unsigned int i = 0; i < messages.getSize(); i++)
	{
		str = messages[i].toString(str);
		strMessages.Add(UTF8_TO_TCHAR(str.UTF8Representation()));
	}

	chn = UTF8_TO_TCHAR(channelName.UTF8Representation());

	UOrionTCPLink::ChatMessageReceived(chn, strMessages);
}

void UOrionChatListener::onPrivateMessage(const JString& sender, const Object& message, const JString& channelName)
{
}

#endif