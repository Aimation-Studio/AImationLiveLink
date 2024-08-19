
#pragma once

#include "CoreMinimal.h"
#include "TrackStreamingPackets.generated.h"

USTRUCT()
struct FAimationVector3
{
    GENERATED_BODY()
public:

    UPROPERTY()
    float x = 0.0f;

    UPROPERTY()
    float y = 0.0f;

    UPROPERTY()
    float z = 0.0f;

    FVector ToFVector() const
    {
        return FVector(x, y, z);
    }
};

USTRUCT()
struct FAimationQuaternion
{
    GENERATED_BODY()
public:

    UPROPERTY()
    float x = 0.0f;

    UPROPERTY()
    float y = 0.0f;

    UPROPERTY()
    float z = 0.0f;

    UPROPERTY()
    float w = 0.0f;

    FQuat ToFQuat() const
    {
        return FQuat(x, y, z, w);
    }
};

USTRUCT()
struct FAimationFrameData
{
    GENERATED_BODY()
public:

    UPROPERTY()
    uint32 HandlerID = 17;

    UPROPERTY()
    uint32 FrameID = 0;

    UPROPERTY()
    uint32 BoneCount = 0;

    UPROPERTY()
    double WorldTimeInSeconds = 0.0;

    UPROPERTY(SkipSerialization) // appended binary data, transform them manually ( origin is vec3 of floats )
    TArray<FAimationVector3> BoneLocations;

    UPROPERTY(SkipSerialization) // appended binary data, transform them manually ( origin is vec4 of floats )
    TArray<FAimationQuaternion> BoneRotations;
};
