#include "Gameplay/UI/CS_TimeAndLocation.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Gameplay/UI/CS_AWP_Widget.h"
#include "Gameplay/UI/CS_ZoomInfo.h"

void UCS_TimeAndLocation::NativeConstruct()
{
	Super::NativeConstruct();

	check(LocationName_SizeBox);
		LocationName_SizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	
	check(LocationName_TextBlock);

	check(TopTimer);
		TopTimer->SetVisibility(ESlateVisibility::Visible);

	check(TopSteps)
		TopSteps->SetVisibility(ESlateVisibility::HitTestInvisible);

	check(DownTimer);
		DownTimer->SetVisibility(ESlateVisibility::Visible);

	check(DownSteps)
		DownSteps->SetVisibility(ESlateVisibility::HitTestInvisible);

	check(Top_TimerSteps)
		Top_TimerSteps->SetVisibility(ESlateVisibility::Visible);

	check(Down_TimerSteps)
		Down_TimerSteps->SetVisibility(ESlateVisibility::Collapsed);

	check(ZoomInfo);
}

void UCS_TimeAndLocation::SetTime(const FDateTime& DateTo) const
{
	TopTimer->UpdateTime(DateTo);
	DownTimer->UpdateTime(DateTo);
}

void UCS_TimeAndLocation::SetSteps(const int CurrentSteps) const
{	
	if (CurrentSteps >= 0) {
		TopSteps->UpdateSteps(CurrentSteps);
		DownSteps->UpdateSteps(CurrentSteps);
	}
}

void UCS_TimeAndLocation::SetLocationName(const FString& LocationName) const
{
	LocationName_TextBlock->SetText(FText::FromString(LocationName));
}

void UCS_TimeAndLocation::SetTimerParameters(const FShowWelcomeParams& ShowWelcomeParams)
{
	TopTimer->InitTimerParams(ShowWelcomeParams.OptTime, ShowWelcomeParams.MaxTime);
	DownTimer->InitTimerParams(ShowWelcomeParams.OptTime, ShowWelcomeParams.MaxTime);
}

void UCS_TimeAndLocation::SetStepsParameters(const FShowWelcomeParams& ShowWelcomeParams)
{	
	if (ShowWelcomeParams.StepsCount == 0) {
		TopSteps->SetVisibility(ESlateVisibility::Collapsed);
		DownSteps->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}


	TopSteps->SetMaxSteps(ShowWelcomeParams.StepsCount);
	TopSteps->UpdateSteps(0);

	DownSteps->SetMaxSteps(ShowWelcomeParams.StepsCount);
	DownSteps->UpdateSteps(0);
}

void UCS_TimeAndLocation::ShowVisibleDownTime(bool Show) const
{
	if(Show)
	{
		Top_TimerSteps->SetVisibility(ESlateVisibility::Collapsed);
		Down_TimerSteps->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Top_TimerSteps->SetVisibility(ESlateVisibility::HitTestInvisible);
		Down_TimerSteps->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCS_TimeAndLocation::SetLocationVisibility(const ESlateVisibility NewVisibility)
{
	LocationName_SizeBox->SetVisibility(NewVisibility);
}

void UCS_TimeAndLocation::UpdateZoomInfo(const int Percent)
{
	if (ZoomInfo)
		ZoomInfo->UpdateZoom(Percent);
}