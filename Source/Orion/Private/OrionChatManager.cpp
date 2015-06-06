#include "Orion.h"
#include "OrionChatManager.h"

void AOrionChatManager::AddChannel(FString Channel)
{
	//search our chat rooms for an existing entry
	for (int32 i = 0; i < ChatRooms.Num(); i++)
	{
		if (ChatRooms[i].Channel.Equals(Channel, ESearchCase::IgnoreCase))
			return;
	}

	//add a new chat room
	FChatEntry newRoom;
	newRoom.bPrivate = false;
	newRoom.Channel = Channel;
	newRoom.Text.Empty();

	ChatRooms.Add(newRoom);
}

void AOrionChatManager::AddChatEntry(FString Channel, FString Message)
{
	for (int32 i = 0; i < ChatRooms.Num(); i++)
	{
		if (ChatRooms[i].Channel.Equals(Channel, ESearchCase::IgnoreCase))
		{
			//if our log is full, remove the oldest entry
			if (ChatRooms[i].Text.Num() > MAX_CHAT_ENTRIES)
				ChatRooms[i].Text.RemoveAt(0);

			ChatRooms[i].Text.Add(Message);

			//update if chat window is currently visible
			if (pPlayerOwner)
				pPlayerOwner->EventUpdateChatWindows();
			return;
		}
	}
}