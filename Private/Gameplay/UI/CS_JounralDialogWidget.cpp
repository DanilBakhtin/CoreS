// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_JounralDialogWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"

void UCS_JounralDialogWidget::InitShowDialog(FString nameTitle, const TArray<FDialogParameters>& Params)
{
	if (bNeedSaveMessage)
		bNeedSaveMessage = false;
	else
		Dialog_WrapBox->ClearChildren();

	Request_WrapBox->ClearChildren();

	TextNameUser->SetText(FText::FromString(nameTitle));

	for (const FDialogParameters& Param : Params)
	{
		if (Param.DialogElement == EDialogElement::Dude)
			AddDudeMSG(Param.Message);
		if (Param.DialogElement == EDialogElement::User)
			AddUserMSG(Param.Message);
		if (Param.DialogElement == EDialogElement::Message)
			AddRequestItem(Param);
	}
	ScrollDown();
}

void UCS_JounralDialogWidget::InitUpdateDialog(const TArray<FDialogParameters>& Params)
{
	if (bNeedSaveMessage)
		bNeedSaveMessage = false;
	else
		Dialog_WrapBox->ClearChildren();

	Request_WrapBox->ClearChildren();

	for (const FDialogParameters Param : Params)
	{
		AddRequestItem(Param);
	}
	ScrollDown();
}

void UCS_JounralDialogWidget::InitAddMessage(const TArray<FDialogParameters>& Params)
{
	for (const FDialogParameters& Param : Params)
	{
		if (Param.DialogElement == EDialogElement::Dude)
		{
			AddDudeMSG(Param.Message);
			bNeedSaveMessage = true;
		}
		if (Param.DialogElement == EDialogElement::User)
			AddUserMSG(Param.Message);
	}
	ScrollDown();
}
