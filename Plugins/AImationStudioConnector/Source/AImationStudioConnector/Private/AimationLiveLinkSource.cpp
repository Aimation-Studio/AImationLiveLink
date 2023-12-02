
#include "AimationLiveLinkSource.h"

#include "AImationStudioConnector.h"
#include "AimationWebSocket.h"
#include "Protocol/RegisterEngineConnector.h"
#include <Editor/EditorEngine.h>
#include <Roles/LiveLinkAnimationTypes.h>
#include <Roles/LiveLinkAnimationRole.h>
#include <ILiveLinkClient.h>
#include "Protocol/RegisterBodySubject.h"

AimationLiveLinkSource::AimationLiveLinkSource( FAimationLiveLinkSettings && settings )
    : ILiveLinkSource()
    , m_connectionSettings(settings)
    , m_webSocket(UAimationWebSocket{})
{
    m_webSocket.RegisterPacketHandler< FRegisterEngineConnectorResponsePacket >(this, &AimationLiveLinkSource::OnRegisterEngineResponse);
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
    FString IP = m_connectionSettings.IPAddress;
    FString FirstThreeDigits = IP.Left(3);
    return FText::FromString(FString::Printf(TEXT("IP: %s... Port: %d"), *FirstThreeDigits, m_connectionSettings.TCPPort));
}

FText AimationLiveLinkSource::GetSourceStatus() const
{
    if (m_linkConnectionStatus == AImationConnectionStatus::Connecting)
        return FText::FromString("Connecting");

    if (m_webSocket.IsConnected())
        return FText::FromString("Connected");

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

    StartReconnectTimer();
}

void AimationLiveLinkSource::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
    if (m_liveLinkClient)
    {
        m_liveLinkClient->RemoveSubject_AnyThread(m_bodySubjectKey);
        m_liveLinkClient->RemoveSubject_AnyThread(m_leftHandSubjectKey);
        m_liveLinkClient->RemoveSubject_AnyThread(m_rightHandSubjectKey);
    }

    m_linkConnectionStatus = AImationConnectionStatus::Disconnected;
    m_disconnectedReason = Reason;

    StartReconnectTimer();
}

void AimationLiveLinkSource::OnMessage(const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("Received string message on the socket, but string messages are not implemented, aimation connector expects binary messages."), *Message);
}

void AimationLiveLinkSource::Connect()
{
    m_reconnectTimerHandle.Invalidate();

    m_webSocket.Connect(m_connectionSettings.BuildWebSocketURL());
    m_webSocket.OnConnected().AddRaw(this, &AimationLiveLinkSource::OnConnected);
    m_webSocket.OnConnectionError().AddRaw(this, &AimationLiveLinkSource::OnConnectionError);
    m_webSocket.OnClosed().AddRaw(this, &AimationLiveLinkSource::OnClosed);

    m_linkConnectionStatus = AImationConnectionStatus::Connecting;
}

void AimationLiveLinkSource::StartReconnectTimer()
{
    if (GEditor->IsTimerManagerValid() && !m_reconnectTimerHandle.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Starting reconnect to Aimation Studio, connect will fire in 5 seconds"));
        FTimerDelegate reconnectDelegate{};
        reconnectDelegate.BindRaw(this, &AimationLiveLinkSource::Connect);
        GEditor->GetTimerManager()->SetTimer(m_reconnectTimerHandle, reconnectDelegate, 5.0f, false);
    }
    else
    {
        if (m_reconnectTimerHandle.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to start reconnect timer, reconnect timer is already valid."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to start reconnect timer, timer manager is not valid."));
        }
    }
}

void AimationLiveLinkSource::OnRegisterEngineResponse(const FRegisterEngineConnectorResponsePacket& packet)
{
    m_bodySubjectKey = { m_sourceGuid, m_bodySubjectName };
    m_leftHandSubjectKey = { m_sourceGuid, m_rightHandSubjectName };
    m_rightHandSubjectKey = { m_sourceGuid, m_leftHandSubjectName };

    auto poseType = packet.AimationPose;

    FLiveLinkStaticDataStruct baseData;
    baseData.InitializeWith(FLiveLinkSkeletonStaticData::StaticStruct(), nullptr);
    FLiveLinkSkeletonStaticData* skeletal = baseData.Cast<FLiveLinkSkeletonStaticData>();
    check(skeletal);
    // TODO: add bone names

    FLiveLinkStaticDataStruct bodyData;
    FLiveLinkStaticDataStruct leftHandData;
    FLiveLinkStaticDataStruct rightHandData;
    bodyData.InitializeWith(baseData);
    leftHandData.InitializeWith(baseData);
    rightHandData.InitializeWith(baseData);

    m_liveLinkClient->RemoveSubject_AnyThread(m_bodySubjectKey);
    m_liveLinkClient->RemoveSubject_AnyThread(m_leftHandSubjectKey);
    m_liveLinkClient->RemoveSubject_AnyThread(m_rightHandSubjectKey);

    m_linkConnectionStatus = AImationConnectionStatus::Connected;
    m_liveLinkClient->PushSubjectStaticData_AnyThread(m_bodySubjectKey, ULiveLinkAnimationRole::StaticClass(), MoveTemp(bodyData));
    if (poseType != PoseType::BasePose)
    {
        m_liveLinkClient->PushSubjectStaticData_AnyThread(m_leftHandSubjectKey, ULiveLinkAnimationRole::StaticClass(), MoveTemp(leftHandData));
        m_liveLinkClient->PushSubjectStaticData_AnyThread(m_rightHandSubjectKey, ULiveLinkAnimationRole::StaticClass(), MoveTemp(rightHandData));
    }
}

