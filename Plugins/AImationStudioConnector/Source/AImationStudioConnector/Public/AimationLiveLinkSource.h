
#pragma once

#include "CoreMinimal.h"
#include "ILiveLinkSource.h"
#include "AimationWebSocket.h"

class AIMATIONSTUDIOCONNECTOR_API AimationLiveLinkSource : public ILiveLinkSource
{
public:
	AimationLiveLinkSource();
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

private:
	UAimationWebSocket m_webSocket;
};
