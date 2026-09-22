// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "CS_UserWidget.generated.h"


UCLASS()
class CORE_SIMULATOR_API UCS_UserWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;

	virtual void GetUITagContainer(TFunction<FGameplayTagContainer()> Func);
	virtual void AddAddTag(TFunction<void(TArray<FGameplayTag> Tags)> Func);
	virtual void AddRemoveTag(TFunction<void(TArray<FGameplayTag> Tags)> Func);
	
protected:

	TFunction<FGameplayTagContainer()> GetTagContainerFun;
	TFunction<void(TArray<FGameplayTag> Tags)> AddTagFun;
	TFunction<void(TArray<FGameplayTag> Tags)> RemoveTagFun;
	
};
