#pragma once

#include "Object.h"
#include "OrionPhotonImport.h"

class AOrionPhotonClient;

class UOrionChatListener : public ExitGames::Chat::Listener
{
public:
	UOrionChatListener(AOrionPhotonClient *newView){ view = newView; }
	~UOrionChatListener(){}

	AOrionPhotonClient *view;

	void debugReturn(ExitGames::Common::DebugLevel::DebugLevel debugLevel, const ExitGames::Common::JString& string);
	void onStateChange(int state);
	void connectionErrorReturn(int errorCode);
	void clientErrorReturn(int errorCode);
	void warningReturn(int warningCode);
	void serverErrorReturn(int errorCode);
	void connectReturn(int errorCode, const ExitGames::Common::JString& errorString);
	void disconnectReturn(void);
	void subscribeReturn(const ExitGames::Common::JVector<ExitGames::Common::JString>& channels, const ExitGames::Common::JVector<bool>& results);
	void unsubscribeReturn(const ExitGames::Common::JVector<ExitGames::Common::JString>& channels);
	void onStatusUpdate(const ExitGames::Common::JString& user, int status, bool gotMessage, const ExitGames::Common::Object& message); 
	void onGetMessages(const ExitGames::Common::JString& channelName, const ExitGames::Common::JVector<ExitGames::Common::JString>& senders, const ExitGames::Common::JVector<ExitGames::Common::Object>& messages);
	void onPrivateMessage(const ExitGames::Common::JString& sender, const ExitGames::Common::Object& message, const ExitGames::Common::JString& channelName);
};