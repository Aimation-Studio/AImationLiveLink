
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



USTRUCT()
struct FAImationAdvancedBoneData
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
        "Midhip", "Neck",
        "WRIST_L", "THUMB_CMC_L", "THUMB_MCP_L", "THUMB_IP_L", "THUMB_TIP_L", "INDEX_FINGER_MCP_L", "INDEX_FINGER_PIP_L",
        "INDEX_FINGER_DIP_L", "INDEX_FINGER_TIP_L", "MIDDLE_FINGER_MCP_L", "MIDDLE_FINGER_PIP_L", "MIDDLE_FINGER_DIP_L",
        "MIDDLE_FINGER_TIP_L", "RING_FINGER_MCP_L", "RING_FINGER_PIP_L", "RING_FINGER_DIP_L", "RING_FINGER_TIP_L",
        "PINKY_MCP_L", "PINKY_PIP_L", "PINKY_DIP_L", "PINKY_TIP_L",
        "WRIST_R", "THUMB_CMC_R", "THUMB_MCP_R", "THUMB_IP_R", "THUMB_TIP_R", "INDEX_FINGER_MCP_R", "INDEX_FINGER_PIP_R",
        "INDEX_FINGER_DIP_R", "INDEX_FINGER_TIP_R", "MIDDLE_FINGER_MCP_R", "MIDDLE_FINGER_PIP_R", "MIDDLE_FINGER_DIP_R",
        "MIDDLE_FINGER_TIP_R", "RING_FINGER_MCP_R", "RING_FINGER_PIP_R", "RING_FINGER_DIP_R", "RING_FINGER_TIP_R",
        "PINKY_MCP_R", "PINKY_PIP_R", "PINKY_DIP_R", "PINKY_TIP_R",

    };

    static const uint32 BoneCount = 77;


    UPROPERTY()
        TArray<int32> BoneParents =
    {
        34,0,1,1,0,4,4,1,4,0,
        0,34,34,11,12,13,14,15,16,15,
        16,15,16,33,33,23,24,25,26,27,
        28,27,28,-1,33,
        15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
        16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16
    };
};