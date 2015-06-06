// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionLoadBalancing.h"

using namespace ExitGames::Common;
using namespace ExitGames::LoadBalancing;

void UOrionLoadBalancing::createRoom()
{
	std::string name("OrionRoom1");
	Hashtable props;
	props.put("s", gridSize);
	props.put("m", map);
	if (client->opCreateRoom(name.c_str(), true, true, 0, props)) //name, visible, open, maxplayers, hashtable
	{
	}//view->info("Creating room %s", name);
	else
	{
	}//view->error("Can't create room in current state");
}

void UOrionLoadBalancing::leave()
{
	if (!client->opLeaveRoom())
	{
		//view->error("Not in room currently");
	}
}

void UOrionLoadBalancing::service()
{
}

void UOrionLoadBalancing::debugReturn(const JString& string)
{
}

void UOrionLoadBalancing::debugReturn(DebugLevel::DebugLevel debugLevel, const JString& string)
{
}

void UOrionLoadBalancing::connectionErrorReturn(int errorCode)
{
}

void UOrionLoadBalancing::clientErrorReturn(int errorCode)
{
}

void UOrionLoadBalancing::warningReturn(int warningCode)
{
}

void UOrionLoadBalancing::serverErrorReturn(int errorCode)
{
}

void UOrionLoadBalancing::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player)
{
}

void UOrionLoadBalancing::leaveRoomEventAction(int playerNr, bool isInactive)
{
}

void UOrionLoadBalancing::disconnectEventAction(int playerNr)
{
}

void UOrionLoadBalancing::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
{
}

void UOrionLoadBalancing::connectReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
}

void UOrionLoadBalancing::disconnectReturn(void)
{
}

void UOrionLoadBalancing::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString)
{
}

void UOrionLoadBalancing::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString)
{
}

void UOrionLoadBalancing::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString)
{
}

void UOrionLoadBalancing::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
}

void UOrionLoadBalancing::gotQueuedReturn(void)
{
}

void UOrionLoadBalancing::joinLobbyReturn(void)
{
}

void UOrionLoadBalancing::leaveLobbyReturn(void)
{
}

void UOrionLoadBalancing::onRoomListUpdate(void)
{
}

void UOrionLoadBalancing::onLobbyStatsUpdate(const ExitGames::Common::JVector<ExitGames::LoadBalancing::LobbyStats>& lobbyStats)
{
}

void UOrionLoadBalancing::onLobbyStatsResponse(const ExitGames::Common::JVector<ExitGames::LoadBalancing::LobbyStats>& lobbyStats)
{
}

void UOrionLoadBalancing::onRoomPropertiesChange(const ExitGames::Common::Hashtable& changes)
{
}
