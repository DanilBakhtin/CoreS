// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_DeveloperSettings.generated.h"

class UCS_SimulatorParamsAsset;

UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Core Simulator Settings"))
class CORE_SIMULATOR_API UCS_DeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	UCS_DeveloperSettings();

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Core Simulator Settings")
	FString StartLocation;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Core Simulator Settings")
	int32 CourseVersion = 3;
	UPROPERTY(EditAnywhere, config, BlueprintReadOnly, Category = "Core Simulator Settings")
	TSoftObjectPtr<UCS_SimulatorParamsAsset> SimulatorParamsAsset;
	UPROPERTY(EditAnywhere, config, BlueprintReadOnly, Category = "Core Simulator Settings")
	TArray<FContextMenuIconItem> ContextMenuIconItems;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	static constexpr int32 MaxHotKeys = 10;
};
