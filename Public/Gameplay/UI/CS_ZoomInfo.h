// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_ZoomInfo.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class CORE_SIMULATOR_API UCS_ZoomInfo : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	void UpdateZoom(int ZoomPercent);

	UPROPERTY(BlueprintReadOnly, Category = "CS_ZoomInfo", meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ZoomPercent;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ZoomInfo", meta = (BindWidget))
	TObjectPtr<UButton> ButtonZoom;

protected:

	UFUNCTION()
	void ResetZoom();
};
