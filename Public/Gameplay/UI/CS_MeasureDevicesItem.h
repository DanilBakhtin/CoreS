
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_MeasureDevicesItem.generated.h"

class UButton;
class UImage;
class UTextBlock;

UCLASS()
class CORE_SIMULATOR_API UCS_MeasureDevicesItem : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "CS_MeasureDevicesItem", meta = (BindWidget))
	TObjectPtr<UTextBlock> Name_TextBlock;

	UPROPERTY(BlueprintReadOnly, Category = "CS_MeasureDevicesItem", meta = (BindWidget))
	TObjectPtr<UImage> Icon_Image;

	UPROPERTY(BlueprintReadOnly, Category = "CS_MeasureDevicesItem", meta = (BindWidget))
	TObjectPtr<UButton> Device_Button;

	bool InitItem(const FDeviceData& _DeviceData, const TArray<FDeviceData>& _PointsData);

	void SetHiddenMeasureDevicesList(TFunction<void()> Fun);
	void SetStartUseMeasureDevice(TFunction<void(const FString& MeasureId, const TArray<FDeviceData>& PointsData)> Fun);
protected:

	UFUNCTION()
	void OnClickItem();

	FString DeviceId;
	TArray<FDeviceData> PointsData;

	TFunction<void()> HiddenMeasureDevicesListFun;
	TFunction<void(const FString& MeasureId, const TArray<FDeviceData>& PointsData)> StartUseMeasureDeviceFun;
};
