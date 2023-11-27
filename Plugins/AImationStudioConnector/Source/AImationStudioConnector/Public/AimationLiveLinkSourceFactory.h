
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "LiveLinkSourceFactory.h"
#include "Internationalization/Text.h"
#include "Internationalization/Internationalization.h"

#include "AimationLiveLinkSourceFactory.generated.h"

class AimationLiveLinkSource;

UCLASS()
class UCustomLiveLinkSourceFactory : public ULiveLinkSourceFactory
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

    EMenuType GetMenuType() const override { return EMenuType::MenuEntry; }

    TSharedPtr<SWidget> BuildCreationPanel(FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const override
    {
        return TSharedPtr<SWidget>();
    }

    virtual TSharedPtr<ILiveLinkSource> CreateSource(const FString& ConnectionString) const override;

    TSharedPtr< AimationLiveLinkSource > m_aimationSource = nullptr;
};

