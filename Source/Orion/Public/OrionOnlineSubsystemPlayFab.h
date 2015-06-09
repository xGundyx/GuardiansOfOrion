// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OnlineSubsystem.h"
#include "OnlineSubsystemImpl.h"
#include "OrionOnlineSubsystemPlayFabPackage.h"

/** Forward declarations of all interface classes */
//typedef TSharedPtr<class FOnlineIdentityFacebook, ESPMode::ThreadSafe> FOnlineIdentityFacebookPtr;
//typedef TSharedPtr<class FOnlineFriendsFacebook, ESPMode::ThreadSafe> FOnlineFriendsFacebookPtr;
//typedef TSharedPtr<class FOnlineSharingFacebook, ESPMode::ThreadSafe> FOnlineSharingFacebookPtr;
//typedef TSharedPtr<class FOnlineUserFacebook, ESPMode::ThreadSafe> FOnlineUserFacebookPtr;

/**
*	OnlineSubsystemFacebook - Implementation of the online subsystem for PlayFab services
*/
class /*ORIONONLINESUBSYSTEMPLAYFAB_API*/ FOrionOnlineSubsystemPlayFab
	: public FOnlineSubsystemImpl
	//, public FTickerObjectBase
{
public:

	// IOnlineSubsystem Interface
	virtual IOnlineSessionPtr GetSessionInterface() const override;
	virtual IOnlineFriendsPtr GetFriendsInterface() const override;
	virtual IOnlineSharedCloudPtr GetSharedCloudInterface() const override;
	virtual IOnlineUserCloudPtr GetUserCloudInterface() const override;
	virtual IOnlineLeaderboardsPtr GetLeaderboardsInterface() const override;
	virtual IOnlineVoicePtr GetVoiceInterface() const override;
	virtual IOnlineExternalUIPtr GetExternalUIInterface() const override;
	virtual IOnlineTimePtr GetTimeInterface() const override;
	virtual IOnlineIdentityPtr GetIdentityInterface() const override;
	virtual IOnlinePartyPtr GetPartyInterface() const override;
	virtual IOnlineTitleFilePtr GetTitleFileInterface() const override;
	virtual IOnlineEntitlementsPtr GetEntitlementsInterface() const override;
	virtual IOnlineStorePtr GetStoreInterface() const override;
	virtual IOnlineEventsPtr GetEventsInterface() const override;
	virtual IOnlineAchievementsPtr GetAchievementsInterface() const override;
	virtual IOnlineSharingPtr GetSharingInterface() const override;
	virtual IOnlineUserPtr GetUserInterface() const override;
	virtual IOnlineMessagePtr GetMessageInterface() const override;
	virtual IOnlinePresencePtr GetPresenceInterface() const override;
	virtual bool Init() override;
	virtual bool Shutdown() override;
	virtual FString GetAppId() const override;
	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	// FTickerBaseObject

	virtual bool Tick(float DeltaTime) override;

	// FOrionOnlineSubsystemPlayFab

	/**
	* Destructor
	*/
	virtual ~FOrionOnlineSubsystemPlayFab();

	/**
	* Is Facebook available for use
	* @return true if Facebook functionality is available, false otherwise
	*/
	bool IsEnabled();

PACKAGE_SCOPE:

	/** Only the factory makes instances */
	FOrionOnlineSubsystemPlayFab();

private:

	/** facebook implementation of identity interface */
	//FOnlineIdentityFacebookPtr FacebookIdentity;

	/** facebook implementation of friends interface */
	//FOnlineFriendsFacebookPtr FacebookFriends;

	/** facebook implementation of sharing interface */
	//FOnlineSharingFacebookPtr FacebookSharing;

	/** facebook implementation of user interface */
	//FOnlineUserFacebookPtr FacebookUser;
};

typedef TSharedPtr<FOrionOnlineSubsystemPlayFab, ESPMode::ThreadSafe> FOrionOnlineSubsystemPlayFabPtr;

