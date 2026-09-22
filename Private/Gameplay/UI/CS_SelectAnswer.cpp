// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_SelectAnswer.h"

#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Gameplay/UI/CS_MainMenu.h"

void UCS_SelectAnswer::NativeConstruct()
{
	Super::NativeConstruct();
	check(Answer_TextBlock);
	check(Answer_UCheckBox);
		Answer_UCheckBox->OnCheckStateChanged.AddDynamic(this, &UCS_SelectAnswer::Check);
	check(Answer_WidgetSwitcher);
	
}

void UCS_SelectAnswer::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if(IsValid(ListViewItem))
	{
		ListViewItem->UpdateItem.RemoveAll(this);
		ListViewItem->Collapse.RemoveAll(this);
	}
	if(UCS_ListViewItem* NewListViewItem = Cast<UCS_ListViewItem>(ListItemObject))
	{
		ListViewItem = NewListViewItem;
		ListViewItem->Collapse.AddDynamic(this, &UCS_SelectAnswer::Collapse);
		ListViewItem->UpdateItem.AddDynamic(this,  &UCS_SelectAnswer::UpdateItem);
		UpdateItem();
	}
}

void UCS_SelectAnswer::NativeOnEntryReleased()
{
	IUserObjectListEntry::NativeOnEntryReleased();

	UpdateItem();
}

void UCS_SelectAnswer::CheckItem(const FString& apiId)
{
	CheckedAnswerId = apiId;
	if(AnswerId !=  CheckedAnswerId)
		Uncheck();
}

void UCS_SelectAnswer::Collapse()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UCS_SelectAnswer::UpdateItem()
{
	bool IsSelected = false;
	FAnswerButton AnswerButtonData = FAnswerButton();
	if(IsValid(ListViewItem))
		AnswerButtonData = ListViewItem->GetAnswerButtonData(IsSelected);
	CategoryID = AnswerButtonData.categoryId;
	if(IsSelected)
	{
		AnswerId = AnswerButtonData.apiId;
		Answer_TextBlock->SetText(FText::FromString(AnswerButtonData.name));
	}
	else
	{
		Answer_TextBlock->SetText(FText::FromString(""));
	}

	if(!IsValid(MainMenuWidget))
	{
		MainMenuWidget = ListViewItem->GetMainMenu();
		MainMenuWidget->CheckItem.AddDynamic(this, &UCS_SelectAnswer::CheckItem);
	}

	if(CheckedAnswerId == AnswerId)
	{
		Answer_UCheckBox->SetCheckedState(ECheckBoxState::Checked);
		Answer_WidgetSwitcher->SetActiveWidgetIndex(0);
	}
	else
	{
		Answer_UCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
		Answer_WidgetSwitcher->SetActiveWidgetIndex(1);
	}

	if(ListViewItem->GetIsCollapsed())
		SetVisibility(ESlateVisibility::Collapsed);
	else
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
}

void UCS_SelectAnswer::Check(bool Check)
{
	if(Check)
	{
		Answer_WidgetSwitcher->SetActiveWidgetIndex(0);
		if(IsValid(MainMenuWidget))
			MainMenuWidget->SetAnswer(CategoryID, AnswerId);
	}
	else
	{
		Answer_WidgetSwitcher->SetActiveWidgetIndex(1);
		if(IsValid(MainMenuWidget))
			MainMenuWidget->SetAnswer(CategoryID, FString());
	}
}

void UCS_SelectAnswer::Uncheck() const
{
	Answer_UCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
	Answer_WidgetSwitcher->SetActiveWidgetIndex(1);
}
