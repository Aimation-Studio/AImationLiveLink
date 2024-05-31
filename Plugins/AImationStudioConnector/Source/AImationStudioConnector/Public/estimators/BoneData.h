
#pragma once

#include "CoreMinimal.h"
#include "BoneData.generated.h"

USTRUCT()
struct FAImationBoneData
{
    GENERATED_BODY();
public:

    UPROPERTY()
    TArray<FName> BoneNames =
    {
        "nose", "lEye2", "lEye3", "lEye1", "rEye2", "rEye3", "rEye1", "lEar", "rEar", "lLip", "rLip",
        "lShoulder", "rShoulder", "lElbow", "rElbow", "lWrist", "rWrist",
        "lHand1", "rHand1", "lHand2", "rHand2", "lHand3", "rHand3",
        "lHip", "rHip", "lKnee", "rKnee",
        "lFoot1", "rFoot1", "lFoot2", "rFoot2", "lFoot3", "rFoot3",
        "Midhip", "Neck"
    };

    static const uint32 BoneCount = 35;


    UPROPERTY()
    TArray<int32> BoneParents =
    {
        34,0,1,1,0,4,4,1,4,0,
        0,34,34,11,12,13,14,15,16,15,
        16,15,16,33,33,23,24,25,26,27,
        28,27,28,-1,33
    };
};
