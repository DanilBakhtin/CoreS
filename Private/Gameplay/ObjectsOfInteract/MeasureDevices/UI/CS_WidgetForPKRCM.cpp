#include <Gameplay/ObjectsOfInteract/MeasureDevices/UI/CS_WidgetForPKRCM.h>
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCS_WidgetForPKRCM::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCS_WidgetForPKRCM::SetMultimeterState(bool bOn)
{
	if (!ImgMultimeter) return;
	if (bOn && Icon_Multimeter_On)
		ImgMultimeter->SetBrushFromTexture(Icon_Multimeter_On, true);
	else if (!bOn && Icon_Multimeter_Off)
		ImgMultimeter->SetBrushFromTexture(Icon_Multimeter_Off, true);
}

void UCS_WidgetForPKRCM::SetSwitchType(EPKCurrentType Type)
{
	if (!ImgSwitchType) return;

	switch (Type)
	{
	case EPKCurrentType::Direct:
		if (Icon_Switch_Direct) ImgSwitchType->SetBrushFromTexture(Icon_Switch_Direct, true);
		break;
	case EPKCurrentType::Resistance:
		if (Icon_Switch_Resistance) ImgSwitchType->SetBrushFromTexture(Icon_Switch_Resistance, true);
		break;
	case EPKCurrentType::Alternate:
		if (Icon_Switch_Alternate) ImgSwitchType->SetBrushFromTexture(Icon_Switch_Alternate, true);
		break;
	}
}

void UCS_WidgetForPKRCM::SetSwitchVoltage(EPKCurrentVoltage Voltage)
{
	if (!ImgSwitchVoltage) return;

	switch (Voltage)
	{
	case EPKCurrentVoltage::A:
		if (Icon_Switch_A) ImgSwitchVoltage->SetBrushFromTexture(Icon_Switch_A, true);
		if (Icon_BackGround_For_R_voltage) ImgBackGround->SetBrushFromTexture(Icon_BackGround_For_U_I_voltage);
		if (ImgMultimeter) ImgMultimeter->SetVisibility(ESlateVisibility::Visible);
		if (ImgSwitchType) ImgSwitchType->SetVisibility(ESlateVisibility::Visible);
		if (TxtMinVoltage) TxtMinVoltage->SetVisibility(ESlateVisibility::Visible);
		if (TxtMaxVoltage) TxtMaxVoltage->SetVisibility(ESlateVisibility::Visible);
		break;
	case EPKCurrentVoltage::R:
		if (Icon_Switch_R) ImgSwitchVoltage->SetBrushFromTexture(Icon_Switch_R, true);
		if (Icon_BackGround_For_R_voltage) ImgBackGround->SetBrushFromTexture(Icon_BackGround_For_R_voltage);
		if (ImgMultimeter) ImgMultimeter->SetVisibility(ESlateVisibility::Hidden);
		/*if (ImgSwitchType) ImgSwitchType->SetVisibility(ESlateVisibility::Hidden);*/
		if (TxtMinVoltage) TxtMinVoltage->SetVisibility(ESlateVisibility::Hidden);
		if (TxtMaxVoltage) TxtMaxVoltage->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EPKCurrentVoltage::V:
		if (Icon_Switch_V) ImgSwitchVoltage->SetBrushFromTexture(Icon_Switch_V, true);
		if (Icon_BackGround_For_R_voltage) ImgBackGround->SetBrushFromTexture(Icon_BackGround_For_U_I_voltage);
		if (ImgMultimeter) ImgMultimeter->SetVisibility(ESlateVisibility::Visible);
		if (ImgSwitchType) ImgSwitchType->SetVisibility(ESlateVisibility::Visible);
		if (TxtMinVoltage) TxtMinVoltage->SetVisibility(ESlateVisibility::Visible);
		if (TxtMaxVoltage) TxtMaxVoltage->SetVisibility(ESlateVisibility::Visible);
		break;
	}
}

void UCS_WidgetForPKRCM::SetStartStopState(bool bMeasuring)
{
	if (!ImgStartStop) return;
	if (!bMeasuring && Icon_Start)
		ImgStartStop->SetBrushFromTexture(Icon_Start, true);
	else if (bMeasuring && Icon_Stop)
		ImgStartStop->SetBrushFromTexture(Icon_Stop, true);
}

void UCS_WidgetForPKRCM::SetMeasurementState(EMeasurementState State)
{
	if (!IconCurrentStatus) return;

	switch (State)
	{
	case EMeasurementState::Idle:
		IconCurrentStatus->SetBrushFromTexture(ImgReady, true);
		break;
	case EMeasurementState::Measuring:
		IconCurrentStatus->SetBrushFromTexture(ImgMeasurement, true);
		break;
	case EMeasurementState::Done:
		IconCurrentStatus->SetBrushFromTexture(ImgMeasurementDone, true);
		break;
	}
}

void UCS_WidgetForPKRCM::SetDisplayVoltageText(const FText& MinValue, const FText& MaxValue)
{
	if (TxtMinVoltage) TxtMinVoltage->SetText(MinValue);
	if (TxtMaxVoltage) TxtMaxVoltage->SetText(MaxValue);
}

void UCS_WidgetForPKRCM::SetDisplayText(const FText& InValue, const FText& InText)
{
	if (TxtValue) TxtValue->SetText(InValue);
	if (TxtDisplay) TxtDisplay->SetText(InText);
}
