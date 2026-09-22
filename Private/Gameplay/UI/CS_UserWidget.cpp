// Core Simulator


#include "Gameplay/UI/CS_UserWidget.h"

void UCS_UserWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCS_UserWidget::GetUITagContainer(TFunction<FGameplayTagContainer()> Func)
{
	GetTagContainerFun = Func;
}

void UCS_UserWidget::AddAddTag(TFunction<void(TArray<FGameplayTag> Tags)> Func)
{
	AddTagFun = Func;
}

void UCS_UserWidget::AddRemoveTag(TFunction<void(TArray<FGameplayTag> Tags)> Func)
{
	RemoveTagFun = Func;
}
