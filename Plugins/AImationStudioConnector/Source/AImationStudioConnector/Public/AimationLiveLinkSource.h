
#pragma once

#include "CoreMinimal.h"
#include "ILiveLinkSource.h"
#include "AimationWebSocket.h"
#include "AimationLiveLinkSettings.h"
#include "Protocol/RegisterEngineConnector.h"

struct FRegisterEngineConnectorResponsePacket;
struct FRegisterBodySubjectServerPacket;
struct FAimationFrameData;

// A aimation live link source creates a websocket connection to the aimation studio server and sends/receives data using binary protocol, please note aimation studio documentation
// for more information on the protocol visit AImation website.
// This class is responsible for creating the connection and sending/receiving data, it also handles the registration process with aimation studio, and populate virtual subject list
// Thread Unsafe! Underlying WebSocket runs on a thread, so make sure to synchronize properly when pushing data to unreal engine
//
// The connection process starts when the client is set by the engine and goes as follows:
// Send FRegisterEngineConnectorPacket
// AImation will respond with FRegisterEngineConnectorResponsePacket which will contain feature list available in the studio
// based on the feature list we will build subject list available for live link
// await animation start/stop packets TODO
class AIMATIONSTUDIOCONNECTOR_API AimationLiveLinkSource : public ILiveLinkSource
{
    using AppendedBinaryStore = TArray<aimation::BinaryBlobData>;
public:
    AimationLiveLinkSource(FAimationConnectionSettings&& settings);
    virtual ~AimationLiveLinkSource() override;

    // Initializes the websocket connection and registers for socket events
    void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;
    void InitializeSettings(ULiveLinkSourceSettings* Settings) override;
    void Update() override;
    bool CanBeDisplayedInUI() const override;
    bool IsSourceStillValid() const override;
    bool RequestSourceShutdown() override;
    FText GetSourceType() const override;
    FText GetSourceMachineName() const override;
    FText GetSourceStatus() const override;
    virtual TSubclassOf<ULiveLinkSourceSettings> GetSettingsClass() const override {
        return UAimationLiveLinkSettings::StaticClass();
    }
    virtual void OnSettingsChanged( ULiveLinkSourceSettings * Settings, const FPropertyChangedEvent & PropertyChangedEvent ) override;

protected:
    void OnConnected();
    void OnConnectionError(const FString& Error);
    void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
    // can be called from either OnConnectionError or OnClosed
    void OnSocketClose();
    void OnMessage(const FString& Message);

    void OnRegisterEngineResponse(const FRegisterEngineConnectorResponsePacket& packet, const AppendedBinaryStore& appendedBinaryData);
    void OnReceiveTrackData(const FAimationFrameData& packet, const AppendedBinaryStore& appendedBinaryData);
private:
    void Connect();
    void StartReconnectTimer();

    FAimationConnectionSettings m_connectionSettings;
    PoseType m_requestedPose;
    TUniquePtr<UAimationWebSocket> m_socket;
    FTimerHandle m_reconnectTimerHandle;
    FThreadSafeBool m_isClosing = false;
    FThreadSafeBool m_isClosed = false;

    ILiveLinkClient* m_liveLinkClient{ nullptr };
    FGuid m_sourceGuid{ };
    FName const m_bodySubjectName{ "Body" };
    FName const m_bodyHandsSubjectName{ "BodyHands" };
    FLiveLinkSubjectKey m_basePoseSubjectKey{ };
    FLiveLinkSubjectKey m_advancedPoseSubjectKey{ };
};
