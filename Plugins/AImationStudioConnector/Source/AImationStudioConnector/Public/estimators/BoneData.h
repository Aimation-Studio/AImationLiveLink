
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
        "Nose",                        // Index 0
        "left_eye_inner",              // Index 1
        "left_eye",                    // Index 2
        "left_eye_outer",              // Index 3
        "right_eye_inner",             // Index 4
        "right_eye",                   // Index 5
        "right_eye_outer",             // Index 6
        "left_ear",                    // Index 7
        "right_ear",                   // Index 8
        "mouth_left",                  // Index 9
        "mouth_right",                 // Index 10

        "left_shoulder",               // Index 11
        "right_shoulder",              // Index 12
        "left_elbow",                  // Index 13
        "right_elbow",                 // Index 14
        "left_wrist",                  // Index 15
        "right_wrist",                 // Index 16

        "left_pinky",                  // Index 17
        "right_pinky",                 // Index 18
        "left_index",                  // Index 19
        "right_index",                 // Index 20
        "left_thumb",                  // Index 21
        "right_thumb",                 // Index 22

        "left_hip",                    // Index 23
        "right_hip",                   // Index 24
        "left_knee",                   // Index 25
        "right_knee",                  // Index 26
        "left_ankle",                  // Index 27
        "right_ankle",                 // Index 28
        "left_heel",                   // Index 29
        "right_heel",                  // Index 30
        "left_foot_index",             // Index 31
        "right_foot_index",            // Index 32

        // Left hand
        "left_wrist",                  // Index 33
        "left_thumb_cmc",              // Index 34
        "left_thumb_mcp",              // Index 35
        "left_thumb_dip",              // Index 36
        "left_thumb_tip",              // Index 37
        "left_index_finger_mcp",       // Index 38
        "left_index_finger_pip",       // Index 39
        "left_index_finger_dip",       // Index 40
        "left_index_finger_tip",       // Index 41
        "left_middle_finger_mcp",      // Index 42
        "left_middle_finger_pip",      // Index 43
        "left_middle_finger_dip",      // Index 44
        "left_middle_finger_tip",      // Index 45
        "left_ring_finger_mcp",        // Index 46
        "left_ring_finger_pip",        // Index 47
        "left_ring_finger_dip",        // Index 48
        "left_ring_finger_tip",        // Index 49
        "left_pinky_mcp",              // Index 50
        "left_pinky_pip",              // Index 51
        "left_pinky_dip",              // Index 52
        "left_pinky_tip",              // Index 53

        // Right hand
        "right_wrist",                 // Index 54
        "right_thumb_cmc",             // Index 55
        "right_thumb_mcp",             // Index 56
        "right_thumb_ip",              // Index 57
        "right_thumb_tip",             // Index 58
        "right_index_finger_mcp",      // Index 59
        "right_index_finger_pip",      // Index 60
        "right_index_finger_dip",      // Index 61
        "right_index_finger_tip",      // Index 62
        "right_middle_finger_mcp",     // Index 63
        "right_middle_finger_pip",     // Index 64
        "right_middle_finger_dip",     // Index 65
        "right_middle_finger_tip",     // Index 66
        "right_ring_finger_mcp",       // Index 67
        "right_ring_finger_pip",       // Index 68
        "right_ring_finger_dip",       // Index 69
        "right_ring_finger_tip",       // Index 70
        "right_pinky_mcp",             // Index 71
        "right_pinky_pip",             // Index 72
        "right_pinky_dip",             // Index 73
        "right_pinky_tip",             // Index 74
    };

private:
    uint32 BaseBoneCount = 33;
    uint32 SingleHandCount = 21;
    uint32 AdvancedBoneCount = BaseBoneCount + SingleHandCount * 2;
    uint32 VirtualBonesCount = 20;
    uint32 VirtualBonesHandCount = 6;
    uint32 BaseVirtualBonesCount = 10;
};
