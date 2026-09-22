#include "Gameplay/UI/CS_ResultCategory.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"

void UCS_ResultCategory::NativeConstruct()
{
	Super::NativeConstruct();

	check(Category_Button);
		Category_Button->OnClicked.AddDynamic(this, &UCS_ResultCategory::ClickOnCategory);
	check(Category_Border);
	check(Category_WidgetSwitcher);
	check(Category_TextBlock);
	check(Category_Image);
}

void UCS_ResultCategory::Init(const FResultParams& _ResultParams)
{
	ResultParams = _ResultParams;
	Category_WidgetSwitcher->SetActiveWidgetIndex(ResultParams.Penalty);
	Category_TextBlock->SetText(FText::FromString(ResultParams.Name));
	if(ResultParams.bIsHasMessage)
	{
		Category_Button->SetVisibility(ESlateVisibility::Visible);
		Category_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		Category_Button->SetVisibility(ESlateVisibility::HitTestInvisible);
		Category_Image->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCS_ResultCategory::SetShowDescriptionFun(TFunction<void(UCS_ResultCategory* ResultCategory)> Fun)
{
	ShowDescriptionFun  = Fun;
	
}

void UCS_ResultCategory::Check() const
{
	Category_Border->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UCS_ResultCategory::Uncheck() const
{
	Category_Border->SetVisibility(ESlateVisibility::Collapsed);
}

FResultParams UCS_ResultCategory::GetResultParams() const
{
	return ResultParams;
}

void UCS_ResultCategory::ClickOnCategory()
{
	if(ShowDescriptionFun)
		ShowDescriptionFun(this);
}
