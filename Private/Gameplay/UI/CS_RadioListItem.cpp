// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_RadioListItem.h"

#include "Components/Button.h"
#include "Framework/CS_CoreTypes.h"
#include "Components/TextBlock.h"

void UCS_RadioListItem::NativeConstruct()
{
	Super::NativeConstruct();
	check(Title_TextBlock);
	check(Dialog_Button)
		Dialog_Button->OnClicked.AddDynamic(this, &UCS_RadioListItem::ClickOnDialogButton);

	check(AnimationCall);
}

void UCS_RadioListItem::InitRadioList(const FDialogButtonParameters& Params)
{
	DialogParameters = Params;
	Title_TextBlock->SetText(FText::FromString(Params.Description));
}

void UCS_RadioListItem::Calling(bool bCalling)
{
	if (AnimationCall) 
	{
		if (bCalling) 
		{
			PlayAnimation(AnimationCall, 0.0f, 0);
		}
		else
		{
			StopAnimation(AnimationCall);
		}
	}
}

void UCS_RadioListItem::ClickOnDialogButton()
{
	OnClickContactButton.Broadcast(DialogParameters);
}