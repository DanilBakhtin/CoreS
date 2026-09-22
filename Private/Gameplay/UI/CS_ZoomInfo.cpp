// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_ZoomInfo.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Gameplay/Player/CS_PlayerController.h"

void UCS_ZoomInfo::NativeConstruct()
{
	check(TextBlock_ZoomPercent)

	SetVisibility(ESlateVisibility::Collapsed);

	check(ButtonZoom)
		ButtonZoom->OnClicked.AddDynamic(this, &UCS_ZoomInfo::ResetZoom);
}

void UCS_ZoomInfo::UpdateZoom(int ZoomPercent)
{
	if (ZoomPercent == 100)
		SetVisibility(ESlateVisibility::Collapsed);
	else {
		SetVisibility(ESlateVisibility::Visible);

		FString Temp = FString::Printf(TEXT("%d%%"), ZoomPercent);
		TextBlock_ZoomPercent->SetText(FText::FromString(Temp));
	}
}

void UCS_ZoomInfo::ResetZoom()
{
	if (GetWorld()->GetFirstPlayerController())
	{
		if (ACS_PlayerController* PlayerController = Cast<ACS_PlayerController>(GetWorld()->GetFirstPlayerController()))
			PlayerController->ResetZoom();
	}
}
