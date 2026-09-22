// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_Steps.h"
#include "Components/TextBlock.h"

void UCS_Steps::SetMaxSteps(const int AmountSteps)
{
	MaxSteps = AmountSteps;
}

void UCS_Steps::UpdateSteps(const int CurrentSteps)
{
	if (!IsValid(Steps_TextBlock))
		return;

	FString TempString = FString::Printf(TEXT("%d/%d"), CurrentSteps, MaxSteps);
	Steps_TextBlock->SetText(FText::FromString(TempString));
}


