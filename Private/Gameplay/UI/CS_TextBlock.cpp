// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_TextBlock.h"
#include "Framework/CS_UtilityFunctionLibrary.h"

UCS_TextBlock::UCS_TextBlock()
{
}

void UCS_TextBlock::SetText(FText InText)
{	
	FString TempStr = InText.ToString();

	Handle_HTML_tags(TempStr);

	Super::SetText(FText::FromString(TempStr));
}

void UCS_TextBlock::SetTextFromString(FString InString)
{
	SetText(FText::FromString(InString));
}

void UCS_TextBlock::Handle_HTML_tags(FString& SourceString)
{
	SourceString = UCS_UtilityFunctionLibrary::DecodeHtmlEntities(SourceString).ToString();

	SourceString = UCS_UtilityFunctionLibrary::ReplaceSubstring(SourceString, "<br>", "\n");
}
