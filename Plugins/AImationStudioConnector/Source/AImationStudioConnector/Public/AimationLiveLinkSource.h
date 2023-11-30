
#pragma once

#include "CoreMinimal.h"
#include "ILiveLinkSource.h"
#include "AimationWebSocket.h"
#include "AimationLiveLinkSettings.h"

struct FRegisterEngineConnectorResponsePacket;

enum class AImationConnectionStatus : uint8
{
    Disconnected,
    Connecting,
    Connected,
    Failed
};

class AIMATIONSTUDIOCONNECTOR_API AimationLiveLinkSource : public ILiveLinkSource
{
public:
    AimationLiveLinkSource(FAimationLiveLinkSettings&& settings);
    ~AimationLiveLinkSource();

    void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;
    void InitializeSettings(ULiveLinkSourceSettings* Settings) override;
    void Update() override;
    bool CanBeDisplayedInUI() const override;
    bool IsSourceStillValid() const override;
    bool RequestSourceShutdown() override;
    FText GetSourceType() const override;
    FText GetSourceMachineName() const override;
    FText GetSourceStatus() const override;

protected:
    void OnConnected();
    void OnConnectionError(const FString& Error);
    void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
    void OnMessage(const FString& Message);

    void OnRegisterEngineConnectorPacket(const FRegisterEngineConnectorResponsePacket& packet);
private:
    void Connect();
    void StartReconnectTimer();

    FAimationLiveLinkSettings m_connectionSettings;
    UAimationWebSocket m_webSocket;

    AImationConnectionStatus m_linkConnectionStatus = AImationConnectionStatus::Disconnected;
    FString m_disconnectedReason{ };
    FTimerHandle m_reconnectTimerHandle;

    ILiveLinkClient* m_liveLinkClient{ nullptr };
    FGuid m_sourceGuid{ };

    void CreateAndAddAimationSubject();
    FName const m_bodySubjectName{ "Body" };
    FName const m_rightHandSubjectName{ "Left Hand" };
    FName const m_leftHandSubjectName{ "Right Hand" };
    FLiveLinkSubjectKey m_bodySubjectKey{ };
    FLiveLinkSubjectKey m_leftHandSubjectKey{ };
    FLiveLinkSubjectKey m_rightHandSubjectKey{ };
};
