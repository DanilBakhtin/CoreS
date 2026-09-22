
#include "Gameplay/UI/CS_MeasureDevicesItem.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Framework/CS_CoreTypes.h"
#include "Framework/CS_DeveloperSettings.h"
#include "Framework/CS_SimulatorParamsAsset.h"

void UCS_MeasureDevicesItem::NativeConstruct()
{
	Super::NativeConstruct();

	check(Name_TextBlock);
	check(Icon_Image);
	check(Device_Button);
		Device_Button->OnClicked.AddDynamic(this, &UCS_MeasureDevicesItem::OnClickItem);
	
}

bool UCS_MeasureDevicesItem::InitItem(const FDeviceData& _DeviceData, const TArray<FDeviceData>& _PointsData)
{
	DeviceId = _DeviceData.DeviceId;
	PointsData = _PointsData;
	Name_TextBlock->SetText(FText::FromString(_DeviceData.DeviceName));
	if (UCS_SimulatorParamsAsset* SimulatorParams = GetDefault<UCS_DeveloperSettings>()->SimulatorParamsAsset.LoadSynchronous())
	{
		for (FMeasureDeviceData& DevicesData : SimulatorParams->Params.MeasureDevicesData)
		{
			if (DevicesData.MeasureId == DeviceId)
			{
				Icon_Image->SetBrushFromSoftTexture(DevicesData.MeasureTexture);
				return true;
			}
		}
	}

	return false;
}

void UCS_MeasureDevicesItem::SetHiddenMeasureDevicesList(TFunction<void()> Fun)
{
	HiddenMeasureDevicesListFun = Fun;
}

void UCS_MeasureDevicesItem::SetStartUseMeasureDevice(TFunction<void(const FString& MeasureId, const TArray<FDeviceData>& PointsData)> Fun)
{
	StartUseMeasureDeviceFun = Fun;
}

void UCS_MeasureDevicesItem::OnClickItem()
{
	if(StartUseMeasureDeviceFun)
		StartUseMeasureDeviceFun(DeviceId, PointsData);
	if(HiddenMeasureDevicesListFun)
		HiddenMeasureDevicesListFun();
}
