
#include "AimationLiveLinkSource.h"

#include "AImationStudioConnector.h"
#include "AimationWebSocket.h"
#include "Protocol/RegisterBodySubject.h"
#include "Protocol/RegisterEngineConnector.h"
#include <Editor/EditorEngine.h>
#include <ILiveLinkClient.h>
#include <Roles/LiveLinkAnimationRole.h>
#include <Roles/LiveLinkAnimationTypes.h>
#include "estimators/BoneData.h"
#include "Protocol/TrackStreamingPackets.h"

AimationLiveLinkSource::AimationLiveLinkSource(FAimationConnectionSettings&& settings)
    : ILiveLinkSource()
    , m_connectionSettings(settings)
    , m_socket(MakeUnique<UAimationWebSocket>())
{
    m_socket->RegisterPacketHandler< FRegisterEngineConnectorResponsePacket >(this, &AimationLiveLinkSource::OnRegisterEngineResponse);
    m_socket->RegisterPacketHandler< FAimationFrameData >(this, &AimationLiveLinkSource::OnReceiveTrackData);
}

AimationLiveLinkSource::~AimationLiveLinkSource()
{
}

void AimationLiveLinkSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
    m_liveLinkClient = InClient;
    m_sourceGuid = InSourceGuid;

    checkf(!m_isClosed, TEXT("AimationLiveLinkSource ReceiveClient called after source was closed"));
    Connect();
}

void AimationLiveLinkSource::InitializeSettings(ULiveLinkSourceSettings* Settings)
{
    UE_LOG(LogTemp, Warning, TEXT("AimationLiveLinkSource::InitializeSettings"));
    m_liveLinkSettings = Cast<UAimationLiveLinkSettings>(Settings);
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
    return !m_isClosed;
}

bool AimationLiveLinkSource::RequestSourceShutdown()
{
    if (!m_isClosing)
    {
        m_isClosing = true;
        // if we failed to close the socket, we can destroy right away
        if (!m_socket->Disconnect())
        {
            m_isClosed = true;
            return true;
        }
    }

    return m_isClosed;
}

FText AimationLiveLinkSource::GetSourceType() const
{
    return FText::FromString("AImation Studio");
}

FText AimationLiveLinkSource::GetSourceMachineName() const
{
    return FText::FromString(FString::Printf(TEXT("Windows @ Port: %d"), m_connectionSettings.TCPPort));
}

FText AimationLiveLinkSource::GetSourceStatus() const
{
    if (m_isClosed)
        return FText::FromString("Disconnected");
    else if (m_isClosing)
        return FText::FromString("Disconnecting");
    else if (!m_socket->IsConnected())
        return FText::FromString("Connecting");

    return FText::FromString("Connected");
}

void AimationLiveLinkSource::OnConnected()
{
    // we might connect while we were asked to be destroyed, skip if so
    if (m_isClosing)
        return;

    // we will await a response packet from aimation now
    FRegisterEngineConnectorPacket packet{};
    packet.ClientName = "Unreal Engine 5.3";
    m_socket->SendPacket(packet);
}

void AimationLiveLinkSource::OnConnectionError(const FString& Error)
{
    UE_LOG(LogTemp, Warning, TEXT("AimationLiveLinkSource connection error: %s"), *Error);
    OnSocketClose();
}

void AimationLiveLinkSource::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
    UE_LOG(LogTemp, Warning, TEXT("AimationLiveLinkSource socket called OnClose. ( StatusCode: %d, Reason: %s, bWasClean: %d )"), StatusCode, *Reason, bWasClean);
    OnSocketClose();
}

void AimationLiveLinkSource::OnSocketClose()
{
    if (m_isClosed)
        return;

    m_isClosed = true;
    if (!m_isClosing)
    {
        UE_LOG(LogTemp, Warning, TEXT("AimationLiveLinkSource closed unexpectedly."));
        StartReconnectTimer();
    }
}

void AimationLiveLinkSource::OnMessage(const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("Received string message on the socket, but string messages are not implemented, aimation connector expects binary messages."), *Message);
}

void AimationLiveLinkSource::Connect()
{
    if (m_isClosing)
        return;

    if (m_socket->Connect(m_connectionSettings.BuildWebSocketURL()))
    {
        m_isClosed = false;
        // if we are not bound to one, we are not bound to any
        if (!m_socket->OnConnected().IsBoundToObject(this))
        {
            m_socket->OnConnected().AddRaw(this, &AimationLiveLinkSource::OnConnected);
            m_socket->OnConnectionError().AddRaw(this, &AimationLiveLinkSource::OnConnectionError);
            m_socket->OnClosed().AddRaw(this, &AimationLiveLinkSource::OnClosed);
        }
    }
    else
    {
        StartReconnectTimer();
    }
}

void AimationLiveLinkSource::StartReconnectTimer()
{
    if (GEditor->IsTimerManagerValid())
    {
        if (m_reconnectTimerHandle.IsValid())
            GEditor->GetTimerManager()->ClearTimer(m_reconnectTimerHandle);

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
            UE_LOG(LogTemp, Warning, TEXT("Failed to start reconnect timer, timer manager is not valid."));
    }
}

void AimationLiveLinkSource::OnRegisterEngineResponse(const FRegisterEngineConnectorResponsePacket& packet)
{
    auto poseType = packet.AimationPose;
    if (poseType != PoseType::AdvancedHandsPose)
    {
        UE_LOG(LogTemp, Warning, TEXT("Aimation LiveLink only works with advanced pose."), static_cast<int32>(poseType));
        return;
    }

    m_advancedPoseSubjectKey = { m_sourceGuid, m_bodySubjectName };
    FLiveLinkStaticDataStruct advancedPoseData;
    {
        static FAImationBoneData bd{};
        advancedPoseData.InitializeWith(FLiveLinkSkeletonStaticData::StaticStruct(), nullptr);
        FLiveLinkSkeletonStaticData* skeletal = advancedPoseData.Cast<FLiveLinkSkeletonStaticData>();

        skeletal->SetBoneNames( bd.BoneNames );
        skeletal->SetBoneParents(bd.BoneParents);
    }

    m_liveLinkClient->RemoveSubject_AnyThread(m_advancedPoseSubjectKey);
    m_liveLinkClient->PushSubjectStaticData_AnyThread(m_advancedPoseSubjectKey, ULiveLinkAnimationRole::StaticClass(), MoveTemp(advancedPoseData));
}

void AimationLiveLinkSource::OnReceiveTrackData(const FAimationFrameData& packet)
{
    FLiveLinkFrameDataStruct frameData;
    frameData.InitializeWith( FLiveLinkAnimationFrameData::StaticStruct(), nullptr );
    FLiveLinkAnimationFrameData * fdRaw = frameData.Cast<FLiveLinkAnimationFrameData>();
    fdRaw->WorldTime = packet.FrameID;
    fdRaw->Transforms.SetNumUninitialized( FAImationBoneData::BoneCount );
    for ( int32 boneId = 0; boneId < FAImationBoneData::BoneCount; ++boneId )
    {
        if ( boneId >= packet.BoneLocations.Num() || boneId >= packet.BoneRotations.Num() )
            break;
        fdRaw->Transforms[ boneId ].SetComponents( packet.BoneRotations[ boneId ].ToFQuat(), packet.BoneLocations[ boneId ].ToFVector(), FVector3d::OneVector );
    }
    m_liveLinkClient->PushSubjectFrameData_AnyThread( m_advancedPoseSubjectKey, MoveTemp( frameData ) );
}

