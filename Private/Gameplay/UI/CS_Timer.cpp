// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/UI/CS_Timer.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Framework/CS_GameInstance.h"
#include "Gameplay/Utilities/CS_LocalizationManager.h"

void UCS_Timer::NativeConstruct()
{
	Super::NativePreConstruct();

	InitDynamicMaterial();

	if (TooltipWidgetClass)
	{
		CustomTooltip = CreateWidget<UCS_DescriptionObject>(GetWorld(), TooltipWidgetClass);
		if (CustomTooltip)
		{
			SetToolTip(CustomTooltip);
		}
	}
}

void UCS_Timer::UpdateTime(const FDateTime& DateTo) const
{	
	// ѕолучаем общее количество секунд в DateTo
	int32 totalSeconds = DateTo.GetHour() * 3600 + DateTo.GetMinute() * 60 + DateTo.GetSecond();

	// ¬ычитаем maxTimeTask и убеждаемс€, что врем€ не отрицательное
	float remainingSeconds = FMath::Max(0.0f, maxTimeTask - totalSeconds);

	// ѕреобразуем оставшеес€ врем€ обратно в часы, минуты и секунды
	int32 hours = FMath::FloorToInt(remainingSeconds / 3600.0f);
	remainingSeconds -= hours * 3600;
	int32 minutes = FMath::FloorToInt(remainingSeconds / 60.0f);
	remainingSeconds -= minutes * 60;
	int32 seconds = FMath::FloorToInt(remainingSeconds);

	FString Time;

	if (DateTo.GetHour() > 0)
	{
		Time = FString::Printf(TEXT("%02d:%02d:%02d"),
			DateTo.GetHour(),
			DateTo.GetMinute(),
			DateTo.GetSecond());
	}
	else
	{
		Time = FString::Printf(TEXT("%02d:%02d"),
			DateTo.GetMinute(),
			DateTo.GetSecond());
	}

	if (IsValid(CustomTooltip))
		CustomTooltip->SetDescriptionText(GetStringLeftTime(hours, minutes, seconds));
	

	Time_TextBlock->SetText(FText::FromString(Time));
	Time_TextBlock->SetText(FText::FromString(Time));

	if (IsValid(DynamicTimerMaterial)) {
		if (maxTimeTask > 0) {
			DynamicTimerMaterial->SetScalarParameterValue(FName("LeftPercent"), static_cast<float>(totalSeconds) / maxTimeTask);
		}
		else {
			DynamicTimerMaterial->SetScalarParameterValue(FName("LeftPercent"), 0.0f);
		}
	}

}

void UCS_Timer::InitDynamicMaterial()
{
	if (IsValid(Time_Image))
	{

		if (BaseTimerMaterial) {

			DynamicTimerMaterial = UMaterialInstanceDynamic::Create(BaseTimerMaterial, const_cast<UCS_Timer*>(this));

			if (DynamicTimerMaterial && Time_Image)
			{
				Time_Image->SetBrushFromMaterial(DynamicTimerMaterial);

				DynamicTimerMaterial->SetScalarParameterValue(FName("OptimalPercent"), 0.5f);
				DynamicTimerMaterial->SetScalarParameterValue(FName("LeftPercent"), 0.1f);
			}
		}
	}
}

void UCS_Timer::InitTimerParams(const int optTime, const int maxTime)
{	
	this->maxTimeTask = static_cast<float>(maxTime);

	if (IsValid(DynamicTimerMaterial)) {
		if (optTime > 0 && maxTime > 0) {
			DynamicTimerMaterial->SetScalarParameterValue(FName("OptimalPercent"), static_cast<float>(optTime) / static_cast<float>(maxTime));
			DynamicTimerMaterial->SetScalarParameterValue(FName("LeftPercent"), 0.0f);
		}
		else {
			SetToolTip(nullptr);
			DynamicTimerMaterial->SetScalarParameterValue(FName("OptimalPercent"), 1.0f);
			DynamicTimerMaterial->SetScalarParameterValue(FName("LeftPercent"), 0.0f);
		}
	}
}

FString UCS_Timer::GetStringLeftTime(int32 hours, int32 minutes, int32 seconds) const
{	

	FText LocalizedText;

	if (UCS_GameInstance* GameInstance = Cast<UCS_GameInstance>(GetGameInstance()))
	{
		if (GameInstance->GetLocalizationManager())
		{
			LocalizedText = GameInstance->GetLocalizationManager()->GetText(
				hours > 0 ? "TIMER_LEFT_HOURS" : "TIMER_LEFT_MINUTES");
		}
		else {
			return FString();
		}
	}
	else {
		return FString();
	}

	FText ResultText;

	if (hours > 0)
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("h"), hours);
		Args.Add(TEXT("min"), minutes);
		ResultText = FText::Format(LocalizedText, Args);
	}
	else
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("min"), minutes);
		Args.Add(TEXT("sec"), seconds);
		ResultText = FText::Format(LocalizedText, Args);
	}

	return ResultText.ToString();
}
