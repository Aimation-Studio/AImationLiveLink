
#include "Slate/AimationStudioLiveLinkConnectionWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include <Interfaces/IPv4/IPv4Address.h>

void SAimationStudioLiveLinkConnectionWidget::Construct(const FArguments& InArgs)
{
    OnStartConnectLiveLink = InArgs._OnStartConnectLiveLink;

    ChildSlot
    [
        SNew(SBox)
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            [
                SNew(STextBlock)
                .Text(FText::FromString("AImation-Studio connection settings"))
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Fill)
            [
                SNew(SBorder)
                .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
                [
                    SNew(SVerticalBox)

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Fill)
                    [
                        SAssignNew(EngineNameInputBox, SEditableTextBox)
                        .ToolTipText(FText::FromString("Enter display name that will be shown in AImation-Studio"))
                        .OnTextCommitted(this, &SAimationStudioLiveLinkConnectionWidget::OnEngineNameTextChanged)
                        .IsCaretMovedWhenGainFocus(true)
                        .SelectAllTextWhenFocused(true)
                        .Text(FText::FromString(ConnectionSettings.EngineName))
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Fill)
                    [
                        SNew(SBorder)
                        .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
                        [
                            SNew(SSeparator)
                            .Orientation(Orient_Horizontal)
                        ]
                    ]

                    + SVerticalBox::Slot()
                    .FillHeight(1.0f)
                    .Padding(0, 10)
                    [
                        SNew(SUniformGridPanel)
                        // IP Input
                        + SUniformGridPanel::Slot(0, 0)
                        .HAlign(HAlign_Center)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString("IP address:"))
                        ]

                        + SUniformGridPanel::Slot(0, 1)
                        .HAlign(HAlign_Center)
                        [
                            SAssignNew(IPEditableTextBox, SEditableTextBox)
                            .MinDesiredWidth(100.f)
                            .ToolTipText(FText::FromString("Enter IP address"))
                            .OnTextCommitted(this, &SAimationStudioLiveLinkConnectionWidget::OnIPTextChanged)
                            .IsCaretMovedWhenGainFocus(true)
                            .SelectAllTextWhenFocused(true)
                            .Text( FText::FromString(ConnectionSettings.IPAddress) )
                        ]

                        // Aimation port input
                        + SUniformGridPanel::Slot(1, 0)
                        .HAlign(HAlign_Center)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString("Port:"))
                        ]
                        + SUniformGridPanel::Slot(1, 1)
                        .HAlign(HAlign_Center)
                        [
                            SAssignNew(PortNumericEntryBox, SNumericEntryBox<uint32>)
                            .MinDesiredValueWidth(100.f)
                            .ToolTipText(FText::FromString("Enter port"))
                            .OnValueCommitted(this, &SAimationStudioLiveLinkConnectionWidget::OnPortValueChanged)
                            .MinSliderValue(0)
                            .MaxSliderValue(65535)
                            .Value_Lambda( [this]() { return ConnectionSettings.TCPPort; })
                        ]
                    ]

                    // Connect to aimation button
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    [
                        SAssignNew(ConnectButton, SButton)
                        .Text(FText::FromString("Connect"))
                        .IsEnabled_Lambda([this]() { return !ConnectionSettings.IPAddress.IsEmpty() && ConnectionSettings.TCPPort != 0; })
                        .OnClicked(this, &SAimationStudioLiveLinkConnectionWidget::OnConnectClicked)
                    ]
                ]
            ]
        ]
    ];
}

void SAimationStudioLiveLinkConnectionWidget::UpdateConnectButton()
{
    ConnectButton->SetEnabled(!ConnectionSettings.IPAddress.IsEmpty() && ConnectionSettings.TCPPort != 0);
}

FReply SAimationStudioLiveLinkConnectionWidget::OnConnectClicked()
{
    OnStartConnectLiveLink.ExecuteIfBound(ConnectionSettings);
    return FReply::Handled();
}

void SAimationStudioLiveLinkConnectionWidget::OnIPTextChanged(const FText& InText, ETextCommit::Type CommitType)
{
    FString IP = InText.ToString();
    if (!IsIPValid(IP))
    {
        IPEditableTextBox->SetError(FText::FromString("Invalid IP address"));
        return;
    }

    ConnectionSettings.IPAddress = IP;
}

void SAimationStudioLiveLinkConnectionWidget::OnPortValueChanged(uint32 NewValue, ETextCommit::Type type)
{
    ConnectionSettings.TCPPort = NewValue;
}

void SAimationStudioLiveLinkConnectionWidget::OnEngineNameTextChanged(const FText& InText, ETextCommit::Type CommitType)
{
    ConnectionSettings.EngineName = InText.ToString();
}

bool SAimationStudioLiveLinkConnectionWidget::IsIPValid(const FString& IPAddress)
{
    FIPv4Address IP;
    bool bIsValid = FIPv4Address::Parse(IPAddress, IP);
    return bIsValid;
}
