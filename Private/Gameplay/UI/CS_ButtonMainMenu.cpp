// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_ButtonMainMenu.h"
#include "Framework/CS_CoreTypes.h"
#include "Gameplay/UI/CS_LocalizableTextBlock.h"

#include "Components/SizeBox.h"
#include "Components/Button.h"

void UCS_ButtonMainMenu::NativePreConstruct()
{
	InitPreviewWidget(ButtonData);
}

void UCS_ButtonMainMenu::NativeConstruct()
{
	check(SizeBox)

	check(Button)
	if (IsValid(Button))
		Button->OnClicked.AddDynamic(this, &UCS_ButtonMainMenu::OnClickButton);

	check(LocalizableTextBlock)

	InitWidget(ButtonData);
	LocalizableTextBlock->InitializeLocalization();
}

void UCS_ButtonMainMenu::SetSizeWidget(float Width, float Height)
{
	if (IsValid(SizeBox))
	{
		SizeBox->SetWidthOverride(Width);
		SizeBox->SetHeightOverride(Height);
	}
}

void UCS_ButtonMainMenu::InitWidget(const FButtonMainMenu& Data)
{
	ActionName = Data.ActionName;

	if (IsValid(LocalizableTextBlock))
	{	
		LocalizableTextBlock->SetPreviewText(Data.PreviewText);
		LocalizableTextBlock->LocalizationKey = Data.LocalizationKey;
		LocalizableTextBlock->SetText(FText());
	}
}

void UCS_ButtonMainMenu::InitPreviewWidget(const FButtonMainMenu& Data)
{
	if (IsValid(LocalizableTextBlock))
	{
		LocalizableTextBlock->SetPreviewText(Data.PreviewText);
	}
}

void UCS_ButtonMainMenu::OnClickButton()
{
	OnClickActionName.Broadcast(ActionName);
}
