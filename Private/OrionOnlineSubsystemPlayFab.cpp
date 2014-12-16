// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionOnlineSubsystemPlayFab.h"

//#include "OnlineSubsystemFacebookPrivatePCH.h"
//#include "OnlineIdentityFacebook.h"
//#include "OnlineFriendsFacebook.h"


IOnlineSessionPtr FOrionOnlineSubsystemPlayFab::GetSessionInterface() const
{
	return NULL;
}


IOnlineFriendsPtr FOrionOnlineSubsystemPlayFab::GetFriendsInterface() const
{
	return NULL;
}


IOnlineSharedCloudPtr FOrionOnlineSubsystemPlayFab::GetSharedCloudInterface() const
{
	return NULL;
}


IOnlineUserCloudPtr FOrionOnlineSubsystemPlayFab::GetUserCloudInterface() const
{
	return NULL;
}


IOnlineLeaderboardsPtr FOrionOnlineSubsystemPlayFab::GetLeaderboardsInterface() const
{
	return NULL;
}

IOnlineVoicePtr FOrionOnlineSubsystemPlayFab::GetVoiceInterface() const
{
	return NULL;
}


IOnlineExternalUIPtr FOrionOnlineSubsystemPlayFab::GetExternalUIInterface() const
{
	return NULL;
}


IOnlineTimePtr FOrionOnlineSubsystemPlayFab::GetTimeInterface() const
{
	return NULL;
}


IOnlineIdentityPtr FOrionOnlineSubsystemPlayFab::GetIdentityInterface() const
{
	return NULL;
}


IOnlineTitleFilePtr FOrionOnlineSubsystemPlayFab::GetTitleFileInterface() const
{
	return NULL;
}


IOnlineEntitlementsPtr FOrionOnlineSubsystemPlayFab::GetEntitlementsInterface() const
{
	return NULL;
}


IOnlineStorePtr FOrionOnlineSubsystemPlayFab::GetStoreInterface() const
{
	return NULL;
}

IOnlineEventsPtr FOrionOnlineSubsystemPlayFab::GetEventsInterface() const
{
	return NULL;
}

IOnlineAchievementsPtr FOrionOnlineSubsystemPlayFab::GetAchievementsInterface() const
{
	return NULL;
}


IOnlineSharingPtr FOrionOnlineSubsystemPlayFab::GetSharingInterface() const
{
	return NULL;
}


IOnlineUserPtr FOrionOnlineSubsystemPlayFab::GetUserInterface() const
{
	return NULL;
}

IOnlineMessagePtr FOrionOnlineSubsystemPlayFab::GetMessageInterface() const
{
	return NULL;
}

IOnlinePresencePtr FOrionOnlineSubsystemPlayFab::GetPresenceInterface() const
{
	return NULL;
}

IOnlinePartyPtr FOrionOnlineSubsystemPlayFab::GetPartyInterface() const
{
	return NULL;
}

bool FOrionOnlineSubsystemPlayFab::Tick(float DeltaTime)
{
	/*if (FacebookIdentity.IsValid())
	{
		FacebookIdentity->Tick(DeltaTime);
	}*/
	return true;
}

bool FOrionOnlineSubsystemPlayFab::Init()
{
	//FacebookIdentity = MakeShareable(new FOnlineIdentityFacebook());
	//FacebookFriends = MakeShareable(new FOnlineFriendsFacebook(this));
	return true;
}

bool FOrionOnlineSubsystemPlayFab::Shutdown()
{
	UE_LOG(LogOnline, Display, TEXT("FOrionOnlineSubsystemPlayFab::Shutdown()"));

	//FacebookIdentity = NULL;
	//FacebookFriends = NULL;
	return true;
}

FString FOrionOnlineSubsystemPlayFab::GetAppId() const
{
	return TEXT("PlayFab");
}

bool FOrionOnlineSubsystemPlayFab::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

bool FOrionOnlineSubsystemPlayFab::IsEnabled()
{
	return true;
}

FOrionOnlineSubsystemPlayFab::FOrionOnlineSubsystemPlayFab()
	//: FacebookIdentity(NULL)
{

}

FOrionOnlineSubsystemPlayFab::~FOrionOnlineSubsystemPlayFab()
{

}
