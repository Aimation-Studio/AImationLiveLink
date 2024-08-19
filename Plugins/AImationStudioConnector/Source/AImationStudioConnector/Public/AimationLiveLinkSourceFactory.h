
#pragma once
#include "CoreMinimal.h"
#include "LiveLinkSourceFactory.h"
#include "AimationLiveLinkSettings.h"

#include "AimationLiveLinkSourceFactory.generated.h"

class AimationLiveLinkSource;

UCLASS()
class AIMATIONSTUDIOCONNECTOR_API UCustomLiveLinkSourceFactory : public ULiveLinkSourceFactory
{
    GENERATED_BODY()

public:
    UCustomLiveLinkSourceFactory();

    // Override functions required by ULiveLinkSourceFactory
    FText GetSourceDisplayName() const override
    {
        return FText::FromString("AImation Studio");
    }

    FText GetSourceTooltip() const override
    {
        return FText::FromString("Connects to AImation Studio.");
    }

    EMenuType GetMenuType() const override { return EMenuType::SubPanel; }

    TSharedPtr<SWidget> BuildCreationPanel(FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const override;

    virtual TSharedPtr<ILiveLinkSource> CreateSource(const FString& ConnectionString) const override;

private:
    void OnLiveLinkSourceStartConnection(FAimationConnectionSettings, FOnLiveLinkSourceCreated) const;
};

