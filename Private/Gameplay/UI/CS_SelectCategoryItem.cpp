
#include "Gameplay/UI/CS_SelectCategoryItem.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void UCS_SelectCategoryItem::NativeConstruct()
{
	Super::NativeConstruct();

	check(Description_TextBlock);
	check(Category_Button)
		Category_Button->OnClicked.AddDynamic(this, & UCS_SelectCategoryItem::SelectCategory);
	check(SelectedIcon_Image)
		//SelectedIcon_Image->SetVisibility(ESlateVisibility::Collapsed);
	check(Background_Border)
		Background_Border->SetVisibility(ESlateVisibility::Collapsed);
	check(Select_WidgetSwitcher);
	
}

void UCS_SelectCategoryItem::InitCategory(const FString& _Id, const FString& _Description)
{
	Id = _Id;
	Description_TextBlock->SetText(FText::FromString(_Description));
}

void UCS_SelectCategoryItem::CheckMarker(const FString& IdForCheck) const
{
	if(Id == IdForCheck)
		Select_WidgetSwitcher->SetActiveWidgetIndex(1);
	else
		Select_WidgetSwitcher->SetActiveWidgetIndex(0);
	
}

void UCS_SelectCategoryItem::Uncheck()
{
	IsChecked = false;
	//SelectedIcon_Image->SetVisibility(ESlateVisibility::Collapsed);
	Background_Border->SetVisibility(ESlateVisibility::Collapsed);
}

void UCS_SelectCategoryItem::SelectCategory()
{
	IsChecked = true;
	OnSelectCategoryItem.Broadcast(Id, this, IsChecked);
	SelectedIcon_Image->SetVisibility(ESlateVisibility::HitTestInvisible);
	Background_Border->SetVisibility(ESlateVisibility::HitTestInvisible);
}
