
#include "AimationLiveLinkSource.h"

#include "AImationStudioConnector.h"
#include "AimationWebSocket.h"
#include "Protocol/RegisterEngineConnector.h"
#include <Editor/EditorEngine.h>

AimationLiveLinkSource::AimationLiveLinkSource() : ILiveLinkSource()
, m_webSocket(UAimationWebSocket{})
{
    m_webSocket.RegisterPacketHandler< FRegisterEngineConnectorResponsePacket >(this, &AimationLiveLinkSource::OnRegisterEngineConnectorPacket);
}

AimationLiveLinkSource::~AimationLiveLinkSource()
{
}

void AimationLiveLinkSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
    m_liveLinkClient = InClient;
    m_sourceGuid = InSourceGuid;

    if (m_webSocket.IsConnected())
    {
        FRegisterEngineConnectorPacket packet{};
        packet.ClientName = "Unreal Engine 5.3";

        m_webSocket.SendPacket(packet);
    }
    else if (m_linkConnectionStatus == AImationConnectionStatus::Disconnected)
    {
        Connect();
    }
}

void AimationLiveLinkSource::InitializeSettings(ULiveLinkSourceSettings* Settings)
{

}

void AimationLiveLinkSource::Update()
{
}

bool AimationLiveLinkSource::CanBeDisplayedInUI() const
{
    return true;
}

bool AimationLiveLinkSource::IsSourceStillValid() const
{
    return true;

}

bool AimationLiveLinkSource::RequestSourceShutdown()
{
    if (m_liveLinkClient)
    {
        m_liveLinkClient = nullptr;
        m_sourceGuid.Invalidate();
    }

    if (m_webSocket.IsConnected())
    {
        m_webSocket.OnConnected().RemoveAll(this);
        m_webSocket.OnConnectionError().RemoveAll(this);
        m_webSocket.OnClosed().RemoveAll(this);

        m_webSocket.Disconnect();
        m_linkConnectionStatus = AImationConnectionStatus::Disconnected;
    }

    return true;
}

FText AimationLiveLinkSource::GetSourceType() const
{
    return FText::FromString("AImation Studio");
}

FText AimationLiveLinkSource::GetSourceMachineName() const
{
    return FText::FromString("LocalHost Windows");
}

FText AimationLiveLinkSource::GetSourceStatus() const
{
    if (m_webSocket.IsConnected())
        return FText::FromString("Connected");

    if (m_linkConnectionStatus == AImationConnectionStatus::Connecting)
        return FText::FromString("Connecting");

    if (m_linkConnectionStatus == AImationConnectionStatus::Failed)
        return FText::FromString("Failed: " + m_disconnectedReason);

    return FText::FromString("Disconnected");
}

void AimationLiveLinkSource::OnConnected()
{
    m_linkConnectionStatus = AImationConnectionStatus::Connected;

    // if client exists then we connected after client was set, we need to register with aimation studio now
    if (m_liveLinkClient)
    {
        FRegisterEngineConnectorPacket packet{};
        packet.ClientName = "Unreal Engine 5.3";

        m_webSocket.SendPacket(packet);
    }
}

void AimationLiveLinkSource::OnConnectionError(const FString& Error)
{
    m_linkConnectionStatus = AImationConnectionStatus::Failed;
    m_disconnectedReason = Error;

    if (GEditor->IsTimerManagerValid())
    {
        FTimerDelegate reconnectDelegate{};
        reconnectDelegate.BindRaw(this, &AimationLiveLinkSource::Connect);
        GEditor->GetTimerManager()->SetTimer(m_reconnectTimerHandle, reconnectDelegate, 5.0f, false);
    }
}

void AimationLiveLinkSource::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
    m_linkConnectionStatus = AImationConnectionStatus::Disconnected;
    m_disconnectedReason = Reason;
}

void AimationLiveLinkSource::OnMessage(const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("Received string message on the socket, but string messages are not implemented, aimation connector expects binary messages."), *Message);
}

void AimationLiveLinkSource::Connect()
{
    m_webSocket.Connect(m_socketURL);
    m_webSocket.OnConnected().AddRaw(this, &AimationLiveLinkSource::OnConnected);
    m_webSocket.OnConnectionError().AddRaw(this, &AimationLiveLinkSource::OnConnectionError);
    m_webSocket.OnClosed().AddRaw(this, &AimationLiveLinkSource::OnClosed);

    m_linkConnectionStatus = AImationConnectionStatus::Connecting;

}

void AimationLiveLinkSource::OnRegisterEngineConnectorPacket(const FRegisterEngineConnectorResponsePacket& packet)
{
    m_linkConnectionStatus = AImationConnectionStatus::Connected;
}

