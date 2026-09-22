// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/UI/CS_DialogWidget.h"
#include "CS_JounralDialogWidget.generated.h"

/**
 * 
 */
UCLASS()
class CORE_SIMULATOR_API UCS_JounralDialogWidget : public UCS_DialogWidget
{
	GENERATED_BODY()
	
public:

	virtual void InitShowDialog(FString nameTitle, const TArray<FDialogParameters>& Params) override;
	virtual void InitUpdateDialog(const TArray<FDialogParameters>& Params) override;
	virtual void InitAddMessage(const TArray<FDialogParameters>& Params) override;

private:
	 
	bool bNeedSaveMessage = false;
};
