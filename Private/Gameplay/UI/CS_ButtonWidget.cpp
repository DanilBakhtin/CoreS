// Core Simulator


#include "Gameplay/UI/CS_ButtonWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCS_ButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateConstruct();
}

void UCS_ButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	UpdateConstruct();
}

void UCS_ButtonWidget::UpdateConstruct() const
{
	if(IsValid(Button_Image))
	{
		FSlateBrush SlateBrush = FSlateBrush();
		SlateBrush.SetResourceObject(IconImage);
		Button_Image->SetBrush(SlateBrush);
	}

	if(IsValid(Title_TextBlock))
	{
		Title_TextBlock->SetText(TitleText);
	}	

	if (IsValid(SizeBoxImage))
	{
		SizeBoxImage->SetWidthOverride(SizeImage);
		SizeBoxImage->SetHeightOverride(SizeImage);
	}
}

void UCS_ButtonWidget::UpdateButtonCondition(const FGameplayTagContainer& TagContainer) const
{
	if(IsValid(Button))
		Button->SetIsEnabled(TagContainer.HasTag(Activated));
}
