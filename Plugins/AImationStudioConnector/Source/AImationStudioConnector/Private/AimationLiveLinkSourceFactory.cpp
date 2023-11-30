
#include "AimationLiveLinkSourceFactory.h"
#include "AimationLiveLinkSource.h"
#include "Slate/AimationStudioLiveLinkConnectionWidget.h"

UCustomLiveLinkSourceFactory::UCustomLiveLinkSourceFactory() : ULiveLinkSourceFactory()
{
}

TSharedPtr<SWidget> UCustomLiveLinkSourceFactory::BuildCreationPanel(FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const
{
    return SNew(SAimationStudioLiveLinkConnectionWidget)
        .OnStartConnectLiveLink(FOnLiveLinkStartConnection::CreateUObject(this, &UCustomLiveLinkSourceFactory::OnLiveLinkSourceStartConnection, OnLiveLinkSourceCreated));
}

TSharedPtr<ILiveLinkSource> UCustomLiveLinkSourceFactory::CreateSource(const FString& ConnectionString) const
{
    FAimationLiveLinkSettings settings;
    if (!ConnectionString.IsEmpty())
        FAimationLiveLinkSettings::StaticStruct()->ImportText(*ConnectionString, &settings, nullptr, PPF_None, GLog, TEXT("ULiveLinkFreeDSourceFactory"));

    return MakeShared<AimationLiveLinkSource>(MoveTemp(settings));
}

void UCustomLiveLinkSourceFactory::OnLiveLinkSourceStartConnection(FAimationLiveLinkSettings settings, FOnLiveLinkSourceCreated source) const
{
    FString ConnectionString;
    FAimationLiveLinkSettings::StaticStruct()->ExportText(ConnectionString, &settings, nullptr, nullptr, PPF_None, nullptr);
    TSharedPtr<AimationLiveLinkSource> aimationClient = MakeShared<AimationLiveLinkSource>(MoveTemp(settings));
    source.ExecuteIfBound(aimationClient, MoveTemp(ConnectionString));
}

