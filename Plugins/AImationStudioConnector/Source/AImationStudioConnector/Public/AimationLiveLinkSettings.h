
#pragma once

#include <Containers/UnrealString.h>
#include "AimationLiveLinkSettings.generated.h"

USTRUCT()
struct AIMATIONSTUDIOCONNECTOR_API FAimationLiveLinkSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
        FString IPAddress = TEXT("127.0.0.1");

    UPROPERTY(EditAnywhere)
        uint16 TCPPort = 52693;

    FString BuildWebSocketURL() const
    {
        return FString::Printf(TEXT("ws://%s:%d"), *IPAddress, TCPPort);
    }
};
