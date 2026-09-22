#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Framework/CS_CoreTypes.h"
#include "GameplayTagContainer.h"
#include "CS_SimulatorParamsAsset.generated.h"

class ACS_MeasureDevices_Actor;

USTRUCT(BlueprintType)
struct FMeasureDeviceData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString MeasureId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UTexture2D> MeasureTexture;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ACS_MeasureDevices_Actor> MeasureActor;
};

USTRUCT(BlueprintType)
struct FSimulatorParams  
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = HUD, 
		meta = (ToolTip = "The subkey is a part of the ID from the AES, the content of which uses the dialog type specified in the value."))
	TMap<FString, ETypeDialog> MapDialogSubKeys;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = HUD)
	TArray<TSoftObjectPtr<UTexture2D>> HelpTexture;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FMeasureDeviceData> MeasureDevicesData;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UWorld> MainCourseMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta = (DisplayName = "Helper is available for this course"))
	bool bIsHelperAvailable = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (DisplayName = "Player crouch possibility on start course"))
	bool bIsCanCrouch = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (DisplayName = "Player sprint possibility on start course"))
	bool bIsCanSprint = true;
};

UCLASS()
class CORE_SIMULATOR_API UCS_SimulatorParamsAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSimulatorParams Params;

	//Max size - 10
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = "0", ClampMax = "10"))
	TArray<FHotkeySlot> HotKeys;
};
