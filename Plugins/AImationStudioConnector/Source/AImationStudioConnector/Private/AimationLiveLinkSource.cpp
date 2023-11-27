
#include "AimationLiveLinkSource.h"

#include "AImationStudioConnector.h"
#include "AimationWebSocket.h"

AimationLiveLinkSource::AimationLiveLinkSource() : ILiveLinkSource()
, m_webSocket(UAimationWebSocket{})
{
}

AimationLiveLinkSource::~AimationLiveLinkSource()
{
}

void AimationLiveLinkSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
    if (!m_webSocket.IsConnected())
        m_webSocket.Connect("ws://127.0.0.1:52693");
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
    if (m_webSocket.IsConnected())
        m_webSocket.Disconnect();

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
    {
        return FText::FromString("Connected");
    }
    else
    {
        return FText::FromString("Connecting");
    }

    return FText::FromString("Not Connected");
}
