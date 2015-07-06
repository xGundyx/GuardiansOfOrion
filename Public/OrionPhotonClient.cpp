#include "Orion.h"
#include "OrionPhotonClient.h"

class UOrionTCPLink;

AOrionPhotonClient::AOrionPhotonClient(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	serverAddress("ns.exitgames.com:5058"),
	ChatAppID("7ad31ad4-fd07-4343-ab4a-f40570b0dfd6"),
	AppID("899b07e0-1737-4e33-a93d-9bdd550f17ba"),
	appVersion("1.0")
{
	PrimaryActorTick.bCanEverTick = true;
}
#if !IS_SERVER
void AOrionPhotonClient::BeginPlay()
{
	Super::BeginPlay();

	listener = new UOrionLoadBalancing(this);
	client = new ExitGames::LoadBalancing::Client(*listener, *AppID, *appVersion, ExitGames::Common::JString("UR") + rand() % 100);
	listener->setLBC(client);

	//if (client)
	//	client->ConnectToRegionMaster("us");

	//info("Connecting...");
	if(client)
		client->connect(*serverAddress);

	chatlistener = new UOrionChatListener(this);

	FString AuthParams;
	AuthParams = FString("username=") + UOrionTCPLink::PlayFabID + FString("&token=") + UOrionTCPLink::ChatToken;

	ExitGames::Chat::AuthenticationValues *CustomAuthenticationValues = new ExitGames::Chat::AuthenticationValues(ExitGames::Chat::CustomAuthenticationType::CUSTOM, *AuthParams, NULL);
	//CustomAuthenticationValues.SetAuthParameters(*ChatAppID, *UOrionTCPLink::ChatToken);
	//ExitGames::Chat::AuthenticationValues *CustomAuthenticationValues = new ExitGames::Chat::AuthenticationValues("GundyTest", *UOrionTCPLink::ChatToken);

	chatclient = new ExitGames::Chat::Client(*chatlistener, *ChatAppID, *appVersion, ExitGames::Common::JString("TestUser") + rand() % 100, ExitGames::Photon::ConnectionProtocol::TCP, *CustomAuthenticationValues);

	if (chatclient)
	{
		chatclient->connect();

		/*ExitGames::Common::JVector<ExitGames::Common::JString> channels;
		channels.addElement("Global");
		channels.addElement("Trade");
		chatclient->opSubscribe(channels);*/
	}
}

ExitGames::Chat::Client* AOrionPhotonClient::GetChatClient()
{
	return chatclient;
}

void AOrionPhotonClient::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (client) 
	{
		client->service();
		chatclient->service();

		// should be called on props change only but how do one listen to those changes?
		//listener->setAutomove(automove);
		//listener->setUseGroups(useGroups);

		listener->service();
	}
}

void AOrionPhotonClient::SendChatMessage(FString Channel, FString Message)
{
	if (chatclient)
	{
		chatclient->opPublishMessage(*Channel, ExitGames::Common::Helpers::ValueToObject::get(*Message));
	}
}

#endif