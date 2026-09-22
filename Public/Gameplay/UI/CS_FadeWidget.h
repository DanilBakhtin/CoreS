// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_FadeWidget.generated.h"

/**
 * 
 */
UCLASS()
class CORE_SIMULATOR_API UCS_FadeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOut;

protected:

	UFUNCTION()
	void FadeOutFinished();
};
