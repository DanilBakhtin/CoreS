// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_WaitStart.h"
#include "Gameplay/UI/CS_BaseButton.h"

void UCS_WaitStart::NativeConstruct()
{
	Super::NativeConstruct();

	check(Exit_Button)
		Exit_Button->OnReleased.AddDynamic(this, &UCS_WaitStart::ClickExit);
}

void UCS_WaitStart::ClickExit()
{
	if(ExitFun)
		ExitFun();
}

void UCS_WaitStart::SetExitFun(TFunction<void()> Fun)
{
	ExitFun = Fun;
}
