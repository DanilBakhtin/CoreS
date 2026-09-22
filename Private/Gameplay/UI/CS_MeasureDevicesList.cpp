
#include "Gameplay/UI/CS_MeasureDevicesList.h"
#include "Framework/CS_CoreTypes.h"
#include "Components/WrapBox.h"
#include "Components/Button.h"
#include "Gameplay/UI/CS_MeasureDevicesItem.h"


void UCS_MeasureDevicesList::NativeConstruct()
{
	Super::NativeConstruct();

	check(List_WrapBox);

	check(Button_CloseMeasureDevices);
	Button_CloseMeasureDevices->OnClicked.AddDynamic(this, &UCS_MeasureDevicesList::CloseOnClickButton);
}

void UCS_MeasureDevicesList::InitMeasureDevicesList(const FDeviceAndPointsData& _DeviceAndPointsData)
{
	DevicesData = _DeviceAndPointsData;
}

void UCS_MeasureDevicesList::SetStartUseMeasureDevice(TFunction<void(const FString& MeasureId, const TArray<FDeviceData>& PointsData)> Fun)
{
	StartUseMeasureDeviceFun = Fun;
}

void UCS_MeasureDevicesList::SetCloseMeasureDevicesFunc(TFunction<void()> Fun)
{
	CloseMeasureDevicesListFunc = Fun;
}

void UCS_MeasureDevicesList::OpenMeasureDevicesList(const FDeviceAndPointsData& DeviceAndPointsData)
{
	for(const FDeviceData& DeviceData : DeviceAndPointsData.DevicesData)
	{
		if(UCS_MeasureDevicesItem* Widget = CreateWidget<UCS_MeasureDevicesItem>(GetWorld(), MeasureDevicesItemClass))
		{
			
			if (Widget->InitItem(DeviceData, DevicesData.PointsData)) {
				List_WrapBox->AddChildToWrapBox(Widget);
				Widget->SetStartUseMeasureDevice(StartUseMeasureDeviceFun);
				Widget->SetHiddenMeasureDevicesList([&]() {CloseMeasureDevicesList(); });
			}
			else
				Widget->RemoveFromParent();
		}
	}
}

void UCS_MeasureDevicesList::CloseMeasureDevicesList()
{
	List_WrapBox->ClearChildren();
}

void UCS_MeasureDevicesList::CloseOnClickButton()
{
	if (CloseMeasureDevicesListFunc)
		CloseMeasureDevicesListFunc();
}
