// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_ActionLog.h"
#include "Gameplay/UI/CS_LogItem.h"
#include "Gameplay/UI/CS_LearningSticker.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Widget.h"
#include "Components/CheckBox.h"
#include "Components/SizeBox.h"
#include "Components/ScrollBox.h"

void UCS_ActionLog::NativeConstruct()
{
	check(LogItemClass);

	check(Content_ScrollBox);
	check(MessageContainer);

	check(LearningStickerBox);
	LearningStickerBox->SetVisibility(ESlateVisibility::Collapsed);

	check(LearningSticker);

	check(btn_CloseLog)
		btn_CloseLog->OnClicked.AddDynamic(this, &UCS_ActionLog::CloseLog);

	check(OpenLearningSticker);
	{
		FWidgetAnimationDynamicEvent OpenFinishedEvent;
		OpenFinishedEvent.BindDynamic(this, &UCS_ActionLog::OnOpenLearningAnimationFinished);
		BindToAnimationFinished(OpenLearningSticker, OpenFinishedEvent);
	}

	check(CloseLearningSticker);
	{
		FWidgetAnimationDynamicEvent CloseFinishedEvent;
		CloseFinishedEvent.BindDynamic(this, &UCS_ActionLog::OnCloseLearningAnimationFinished);
		BindToAnimationFinished(CloseLearningSticker, CloseFinishedEvent);
	}
}

void UCS_ActionLog::SetCloseLogFunc(TFunction<void()> Func)
{
	CloseLogFunc = Func;
}

FString UCS_ActionLog::GetTimeString(const FDateTime& DateTo)
{
	FString StrTime = FString::Printf(TEXT("%02d:%02d:%02d"),
	                                  DateTo.GetHour(),
	                                  DateTo.GetMinute(),
	                                  DateTo.GetSecond());

	return StrTime;
}

void UCS_ActionLog::AddMessageLog(const FLogParams& LogParams)
{	
	if (LogParams.Description.IsEmpty())
		return;

	if (UCS_LogItem* Widget = CreateWidget<UCS_LogItem>(GetWorld(), LogItemClass))
	{	
		AddWidgetToMap(LogParams.LogType, Widget);

		MessageContainer->AddChild(Widget);
		Widget->SetTextInfo(LogParams.Description);
		Widget->SetTextTime(GetTimeString(LogParams.Time));
		Widget->SetPadding(FMargin(0.0f, 0.0f, 0.0f, PaddingBtwItems));
		Widget->SetRenderScale(MessageContainer->GetRenderTransform().Scale);

		if (GetVisibility() != ESlateVisibility::Collapsed)
			Widget->PlayOpenAnimation();
		
		if (VisualItemMap.Contains(LogParams.LogType))
			Widget->setVisualInfo(VisualItemMap[LogParams.LogType]);
		else
			Widget->CollapseImageIcon();
	}
}

void UCS_ActionLog::CloseLog()
{
	if (CloseLogFunc)
		CloseLogFunc();
}

void UCS_ActionLog::AddWidgetToMap(ELogType InputLogType, UUserWidget* WidgetToAdd)
{
	if (!WidgetToAdd)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to add null log widget to map"));
		return;
	}

	if (WidgetMap.Contains(InputLogType))
	{
		TArray<UUserWidget*>& WidgetArray = WidgetMap[InputLogType];
		WidgetArray.Add(WidgetToAdd);
	}
	else
	{
		TArray<UUserWidget*> NewWidgetArray;
		NewWidgetArray.Add(WidgetToAdd);
		WidgetMap.Add(InputLogType, NewWidgetArray);
	}
}

TArray<UUserWidget*> UCS_ActionLog::GetWidgetsByType(ELogType InputLogType) const
{
	if (const TArray<UUserWidget*>* WidgetArray = WidgetMap.Find(InputLogType))
	{
		return *WidgetArray;
	}

	return TArray<UUserWidget*>();
}

void UCS_ActionLog::SetVisabilityLogWidgets(const bool bNewVisability, const TArray<UUserWidget*> InputWidgets)
{
	for (int i = 0; i < InputWidgets.Num(); i++) {
		InputWidgets[i]->SetVisibility(bNewVisability ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UCS_ActionLog::ShowLearningSticker(const FStickerParams& StickerParams) {

	if (!LearningSticker) return;

	if (LearningStickerBox->GetVisibility() == ESlateVisibility::Collapsed) {

		bManualHideSticker = false;

		LearningSticker->InitSticker(StickerParams);

		LearningStickerBox->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (OpenLearningSticker) 
			PlayAnimation(OpenLearningSticker, 0.0f, 1, EUMGSequencePlayMode::Forward, PlayRateLearningSticker);
	}
	else {

		if (GetWorld())
			GetWorld()->GetTimerManager().ClearTimer(ActionLearningStickerTimer);

		HideLearningSticker();

		bManualHideSticker = true;

		CachedStickerParams = StickerParams;
	}
}

void UCS_ActionLog::Reset()
{
	Content_ScrollBox->ScrollToStart();
}

void UCS_ActionLog::OnOpenLearningAnimationFinished()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ActionLearningStickerTimer);

		GetWorld()->GetTimerManager().SetTimer(
			ActionLearningStickerTimer,
			this,
			&UCS_ActionLog::HideLearningSticker,
			TimeLifeLearningSticker,
			false
		);
	}
}

void UCS_ActionLog::OnCloseLearningAnimationFinished()
{
	LearningStickerBox->SetVisibility(ESlateVisibility::Collapsed);

	if (bManualHideSticker) {
		ShowLearningSticker(CachedStickerParams);
	}
}

void UCS_ActionLog::HideLearningSticker()
{
	if (CloseLearningSticker) 
		PlayAnimation(CloseLearningSticker, 0.0f, 1, EUMGSequencePlayMode::Forward, PlayRateLearningSticker);
	else 
		OnCloseLearningAnimationFinished();
	
}
