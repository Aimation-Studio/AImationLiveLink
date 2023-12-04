
#pragma once

#include "CoreMinimal.h"
#include "AimationLiveLinkSettings.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include <Containers/UnrealString.h>

DECLARE_DELEGATE_OneParam(FOnLiveLinkStartConnection, FAimationConnectionSettings);

/**
 * Widget for Animation Studio Live Link Connection settings.
 */
class AIMATIONSTUDIOCONNECTOR_API SAimationStudioLiveLinkConnectionWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SAimationStudioLiveLinkConnectionWidget)
    {}
        SLATE_EVENT(FOnLiveLinkStartConnection, OnStartConnectLiveLink)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FAimationConnectionSettings ConnectionSettings;
    FOnLiveLinkStartConnection OnStartConnectLiveLink;

    TSharedPtr<SEditableTextBox> EngineNameInputBox;
    TSharedPtr<SEditableTextBox> IPEditableTextBox;
    TSharedPtr<SNumericEntryBox<uint32>> PortNumericEntryBox;
    TSharedPtr<SButton> ConnectButton;

    FReply OnConnectClicked();
    void UpdateConnectButton();
    void OnIPTextChanged(const FText& InText, ETextCommit::Type CommitType);
    void OnPortValueChanged(uint32 NewValue, ETextCommit::Type CommitType);
    void OnEngineNameTextChanged(const FText& InText, ETextCommit::Type CommitType);
    bool IsIPValid(const FString& IPAddress);
};
