#pragma once

#include "GameFramework/Actor.h"
#include "OrionChatManager.generated.h"

class AOrionPlayerController;

//max entries of text per category
#define MAX_CHAT_ENTRIES 1000

USTRUCT(BlueprintType)
struct FChatEntry
{
	GENERATED_USTRUCT_BODY()

	TArray<FString> Text;		//array of text entries
	FString Channel;			//name of channel these belong to
	FColor Color;				//color to display this channel
	bool bPrivate;				//is private?
};

UCLASS()
class ORION_API AOrionChatManager : public AActor
{
	GENERATED_BODY()

public:
	//subscribe to a channel
	void AddChannel(FString Channel);

	//add an entry to an existing chat room
	void AddChatEntry(FString Channel, FString Message);

	AOrionPlayerController *pPlayerOwner;

private:
	TArray<FChatEntry> ChatRooms;
};