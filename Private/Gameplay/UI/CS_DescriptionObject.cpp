// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_DescriptionObject.h"

#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void UCS_DescriptionObject::NativeConstruct()
{
	Super::NativeConstruct();

	check(Description_TextBlock);
	check(DescriptionBorder_SizeBox);
}

void UCS_DescriptionObject::SetDescriptionText(const FString& Text) const
{

	Description_TextBlock->SetText(FText::FromString(Text));
	
}
