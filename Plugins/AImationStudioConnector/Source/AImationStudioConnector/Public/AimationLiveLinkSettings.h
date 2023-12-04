
#pragma once

#include <Containers/UnrealString.h>
#include <LiveLinkSourceSettings.h>
#include "AimationLiveLinkSettings.generated.h"

USTRUCT()
struct AIMATIONSTUDIOCONNECTOR_API FAimationConnectionSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "AImation|Connection")
        FString IPAddress = TEXT("127.0.0.1");

    UPROPERTY(EditAnywhere, Category = "AImation|Connection")
        uint16 TCPPort = 52693;

    UPROPERTY(EditAnywhere, Category = "AImation|Connection")
        FString EngineName = TEXT("Unreal Engine 5.3");

    FString BuildWebSocketURL() const
    {
        return FString::Printf(TEXT("ws://%s:%d"), *IPAddress, TCPPort);
    }
};

UCLASS()
class AIMATIONSTUDIOCONNECTOR_API UAimationLiveLinkSettings : public ULiveLinkSourceSettings
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "AImation|Settings")
        bool PointlessCheckboxForTestingPurposes = false;
};
