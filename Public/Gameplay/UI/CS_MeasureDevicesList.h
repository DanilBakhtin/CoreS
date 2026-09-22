
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_MeasureDevicesList.generated.h"

class UWrapBox;
class UButton;
class UCS_MeasureDevicesItem;

UCLASS()
class CORE_SIMULATOR_API UCS_MeasureDevicesList : public UUserWidget
{
	GENERATED_BODY()


public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_MeasureDevicesList")
	TSubclassOf<UCS_MeasureDevicesItem> MeasureDevicesItemClass;

	UPROPERTY(BlueprintReadOnly, Category = "CS_MeasureDevicesList", meta = (BindWidget))
	TObjectPtr<UWrapBox> List_WrapBox;

	UPROPERTY(BlueprintReadOnly, Category = "CS_MeasureDevicesList", meta = (BindWidget))
	TObjectPtr<UButton> Button_CloseMeasureDevices;
	
	void InitMeasureDevicesList(const FDeviceAndPointsData& DeviceAndPointsData);

	void SetStartUseMeasureDevice(TFunction<void(const FString& MeasureId, const TArray<FDeviceData>& PointsData)> Fun);

	void SetCloseMeasureDevicesFunc(TFunction<void()> Fun);

	void OpenMeasureDevicesList(const FDeviceAndPointsData& DeviceAndPointsData);
	void CloseMeasureDevicesList();

	
protected:
	
	TFunction<void(const FString& MeasureId, const TArray<FDeviceData>& PointsData)> StartUseMeasureDeviceFun;

	TFunction<void()> CloseMeasureDevicesListFunc;

	FDeviceAndPointsData DevicesData;

	UFUNCTION()
	void CloseOnClickButton();
};
