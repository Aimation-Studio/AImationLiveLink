
#include "AimationLiveLinkSourceFactory.h"
#include "AimationLiveLinkSource.h"

UCustomLiveLinkSourceFactory::UCustomLiveLinkSourceFactory() : ULiveLinkSourceFactory(), m_aimationSource(nullptr)
{
    m_aimationSource = MakeShared<AimationLiveLinkSource>();
}

TSharedPtr<ILiveLinkSource> UCustomLiveLinkSourceFactory::CreateSource(const FString& ConnectionString) const
{
    return m_aimationSource;
}

