
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
#include "Logging/StructuredLog.h"
#include "Misc/QualifiedFrameTime.h"

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
    if (GEditor->IsTimerManagerValid())
    {
        if (m_reconnectTimerHandle.IsValid())
            GEditor->GetTimerManager()->ClearTimer(m_reconnectTimerHandle);
    }

    m_reconnectTimerHandle.Invalidate();
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
    m_requestedPose = Cast<UAimationLiveLinkSettings>(Settings)->DataForRetargetting ? PoseType::WorldCoordsPose : PoseType::LocalCoordsPose;
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

void AimationLiveLinkSource::OnSettingsChanged( ULiveLinkSourceSettings * Settings, const FPropertyChangedEvent & PropertyChangedEvent )
{
    auto newRequest = Cast<UAimationLiveLinkSettings>( Settings )->DataForRetargetting ? PoseType::WorldCoordsPose : PoseType::LocalCoordsPose;
    if ( newRequest != m_requestedPose )
    {
        m_requestedPose = newRequest;
        // repeat register packet
        OnConnected();
    }
}

void AimationLiveLinkSource::OnConnected()
{
    // we might connect while we were asked to be destroyed, skip if so
    if (m_isClosing)
        return;

    // we will await a response packet from aimation now
    FRegisterEngineConnectorPacket packet{};
    packet.ClientName = m_connectionSettings.EngineName;
    
    packet.RequestedPoseType = static_cast<std::underlying_type_t<PoseType>>(m_requestedPose);
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
        m_isClosed = true;
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

void AimationLiveLinkSource::OnRegisterEngineResponse( const FRegisterEngineConnectorResponsePacket & packet, const AppendedBinaryStore & appendedBinaryData )
{
    auto poseType = packet.AimationPose;
    if ( poseType != m_requestedPose )
    {
        UE_LOG( LogTemp, Warning, TEXT( "Aimation LiveLink received other pose type than requested." ) );
        return;
    }

    m_basePoseSubjectKey = { m_sourceGuid, m_bodySubjectName };
    m_advancedPoseSubjectKey = { m_sourceGuid, m_bodyHandsSubjectName };
    FLiveLinkStaticDataStruct advancedPoseData, basePoseData;
    {
        static FAImationBoneData bd{};
        basePoseData.InitializeWith( FLiveLinkSkeletonStaticData::StaticStruct(), nullptr );
        FLiveLinkSkeletonStaticData * skeletal = basePoseData.Cast<FLiveLinkSkeletonStaticData>();
        skeletal->SetBoneNames( bd.BoneNames );
        skeletal->SetBoneParents( bd.BoneParents );
    }
    {
        static FAImationAdvancedBoneData abd{};
        advancedPoseData.InitializeWith( FLiveLinkSkeletonStaticData::StaticStruct(), nullptr );
        FLiveLinkSkeletonStaticData * skeletalAdv = advancedPoseData.Cast<FLiveLinkSkeletonStaticData>();
        skeletalAdv->SetBoneNames( abd.BoneNames );
        skeletalAdv->SetBoneParents( abd.BoneParents );
    }

    m_liveLinkClient->RemoveSubject_AnyThread( m_advancedPoseSubjectKey );
    m_liveLinkClient->PushSubjectStaticData_AnyThread( m_advancedPoseSubjectKey, ULiveLinkAnimationRole::StaticClass(), MoveTemp( advancedPoseData ) );
    m_liveLinkClient->RemoveSubject_AnyThread( m_basePoseSubjectKey );
    m_liveLinkClient->PushSubjectStaticData_AnyThread( m_basePoseSubjectKey, ULiveLinkAnimationRole::StaticClass(), MoveTemp( basePoseData ) );
}

TArray<FAimationVector3> DeserializeLocations(const aimation::BinaryBlobData& BinaryData)
{
    TArray<FAimationVector3> Result;

    FMemoryReader Reader(BinaryData.data, true);
    Reader.Seek(0);

    Result.Reserve(BinaryData.header.Size);
    for (size_t i = 0; i < BinaryData.header.Size; ++i)
    {
        size_t sizeOfThreeFloats = 3 * sizeof(float);
        float X, Y, Z = 0.f;
        Reader << X;
        Reader << Y;
        Reader << Z;

        // check if we had an error at reading
        if (Reader.IsError())
        {
            UE_LOG(LogTemp, Error, TEXT("Error reading binary blob data, skipping the rest of the data."));
            checkf(false, TEXT("Error reading binary blob data, skipping the rest of the data."));
            break;
        }

        Result.Add(FAimationVector3(X, Y, Z));
    }

    return Result;
}

TArray<FAimationQuaternion> DeserializeRotations(const aimation::BinaryBlobData& binData)
{
    TArray<FAimationQuaternion> Result;

    FMemoryReader Reader(binData.data, true);
    Reader.Seek(0);

    Result.Reserve(binData.header.Size);
    for (size_t i = 0; i < binData.header.Size; ++i)
    {
        float X, Y, Z, W;
        Reader << X << Y << Z << W;
        Result.Add(FAimationQuaternion(X, Y, Z, W));
    }

    return Result;
}

void AimationLiveLinkSource::OnReceiveTrackData(const FAimationFrameData& constPkt, const AppendedBinaryStore& appendedBinaryData)
{
    UE_LOGFMT(LogTemp, Verbose, "Received track data, appended bin data size {BinDataSize}", appendedBinaryData.Num());
    FLiveLinkFrameDataStruct frameData;
    frameData.InitializeWith(FLiveLinkAnimationFrameData::StaticStruct(), nullptr);

    auto boneLocations = DeserializeLocations(appendedBinaryData[0]);
    auto boneRots = DeserializeRotations(appendedBinaryData[1]);

    FAimationFrameData packet = constPkt;
    packet.BoneLocations = MoveTemp(boneLocations);
    packet.BoneRotations = MoveTemp(boneRots);

    FLiveLinkAnimationFrameData* fdRaw = frameData.Cast<FLiveLinkAnimationFrameData>();
    fdRaw->WorldTime = constPkt.WorldTimeInSeconds;
    FQualifiedFrameTime qft{ };
    qft.Rate = { 60, 1 };
    qft.Time = FFrameTime{ static_cast<int32>(constPkt.FrameID) };
    fdRaw->MetaData.SceneTime = MoveTemp(qft);

    //fdRaw->MetaData.SceneTime = FQualifiedFrameTime{ constPkt.FrameID, { 60, 1 } };

    int32 totalCount = packet.BoneLocations.Num() >= FAImationAdvancedBoneData::BoneCount ? FAImationAdvancedBoneData::BoneCount : FAImationBoneData::BoneCount;

    fdRaw->Transforms.SetNumUninitialized( totalCount );
    for (int32 boneId = 0; boneId < totalCount; ++boneId)
    {
        if ( boneId >= packet.BoneLocations.Num() || boneId >= packet.BoneRotations.Num() )
        {
            UE_LOGFMT( LogTemp, Log, "Received track data, bone {0} missing", boneId );
            break;
        }

        fdRaw->Transforms[boneId].SetComponents(packet.BoneRotations[boneId].ToFQuat(), packet.BoneLocations[boneId].ToFVector(), FVector3d::OneVector);
        if ( fdRaw->Transforms[ boneId ].ContainsNaN() )
        {
            UE_LOG( LogTemp, Log, TEXT( "received data contains NaN" ) );
        }
    }

    UE_LOGFMT(LogTemp, Verbose, "Received track data, bone locations size {BoneLocSize}, bone rotations size {BoneRotationsSize}", boneLocations.Num(), boneRots.Num());
    m_liveLinkClient->PushSubjectFrameData_AnyThread( packet.BoneLocations.Num() >= FAImationAdvancedBoneData::BoneCount ? m_advancedPoseSubjectKey : m_basePoseSubjectKey, MoveTemp(frameData));
}

