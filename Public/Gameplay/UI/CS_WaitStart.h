// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_WaitStart.generated.h"

class UCS_BaseButton;

UCLASS()
class CORE_SIMULATOR_API UCS_WaitStart : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_WaitStart", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton> Exit_Button;

	UFUNCTION()
	void ClickExit();
	
	void SetExitFun(TFunction<void()> Fun);
	
protected:

	TFunction<void()> ExitFun;
};
