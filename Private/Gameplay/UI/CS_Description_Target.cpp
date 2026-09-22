// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_Description_Target.h"

#include "Components/SizeBox.h"
#include "Gameplay/UI/CS_DescriptionObject.h"

void UCS_Description_Target::NativeConstruct()
{
	check(DescriptionObject);
	check(Description_SizeBox);
}

void UCS_Description_Target::ShowTargetDescription(const FString& Description) const
{
	if(Description != "")
	{
		Description_SizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		DescriptionObject->SetDescriptionText(Description);
	}
	else
	{
		Description_SizeBox->SetVisibility(ESlateVisibility::Collapsed);
	}
		
}
