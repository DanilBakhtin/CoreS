// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_FadeWidget.h"

void UCS_FadeWidget::NativeConstruct()
{
	check(FadeOut);
	{
		FWidgetAnimationDynamicEvent FadeOutEvent;
		FadeOutEvent.BindDynamic(this, &UCS_FadeWidget::FadeOutFinished);
		BindToAnimationFinished(FadeOut, FadeOutEvent);
	}

	if (FadeOut)
		PlayAnimation(FadeOut);
}

void UCS_FadeWidget::FadeOutFinished()
{
	RemoveFromParent();
}
