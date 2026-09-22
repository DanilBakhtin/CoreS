// Core Simulator


#include "Framework/CS_DeveloperSettings.h"

UCS_DeveloperSettings::UCS_DeveloperSettings()
{
}

#if WITH_EDITOR
void UCS_DeveloperSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

