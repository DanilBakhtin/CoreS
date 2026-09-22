// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/UI/CS_EndWorkingAreaWidget.h"

#include "Framework/CS_UtilityFunctionLibrary.h"
#include "Gameplay/UI/CS_LocalizableTextBlock.h"

void UCS_EndWorkingAreaWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (EndWorkingArea_TextBlock)
	{
		EndWorkingArea_TextBlock->LocalizationKey = "ENDZONE_TITLE";
		EndWorkingArea_TextBlock->SetPreviewText(FText::FromString("Working area boundary"));
		EndWorkingArea_TextBlock->SetText(FText::FromString("Working area boundary"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EndWorkingAreaWidget: EndWorkingArea_TextBlock is not valid!"))
	}
}

void UCS_EndWorkingAreaWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(EndWorkingArea_TextBlock);
}

void UCS_EndWorkingAreaWidget::SetText(FText InText)
{
	if (InText.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("EndWorkingAreaWidget: SetText get empty text parameter!"))
		return;
	}
	
	if (EndWorkingArea_TextBlock)
	{
		const FString NewText = UCS_UtilityFunctionLibrary::ReplaceSubstring(
			InText.ToString(), "<br>", "\n");
		EndWorkingArea_TextBlock->SetPreviewText(FText::FromString(NewText));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EndWorkingAreaWidget: EndWorkingArea_TextBlock is not valid!"))
	}
}
