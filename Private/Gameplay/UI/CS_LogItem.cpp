// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_LogItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Gameplay/UI/CS_ActionLog.h"

void UCS_LogItem::NativeConstruct()
{
	check(IconImage);
	check(BackgroundImage);
	check(LeftLineImage);
	check(TextTime);
	check(TextInfo);

	check(OpenAnimation);
}

void UCS_LogItem::setVisualInfo(const FLogItemVisualInfo& VisualInfo)
{
	if (IsValid(VisualInfo.Icon))
		IconImage->SetBrushFromTexture(VisualInfo.Icon);

	if (BackgroundImage) {
		if (IsValid(VisualInfo.BackgroundMaterial))
		{
			BackgroundImage->SetBrushFromMaterial(VisualInfo.BackgroundMaterial);
		}
	}

	if (LeftLineImage)
		LeftLineImage->SetColorAndOpacity(VisualInfo.ColorLine);
}

void UCS_LogItem::SetTextTime(const FString& NewText) const
{
	TextTime->SetText(FText::FromString(NewText));
}

void UCS_LogItem::SetTextInfo(const FString& NewText) const
{	
	TextInfo->SetText(FText::FromString(NewText));
}

void UCS_LogItem::CollapseImageIcon() const
{
	IconImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UCS_LogItem::PlayOpenAnimation()
{
	if (OpenAnimation)
		PlayAnimation(OpenAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}
