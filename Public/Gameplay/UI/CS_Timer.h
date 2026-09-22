// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_DescriptionObject.h"
#include "CS_Timer.generated.h"

class USizeBox;
class UTextBlock;
class UImage;
class UCS_DescriptionObject;

UCLASS()
class CORE_SIMULATOR_API UCS_Timer : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_Timer")
	TObjectPtr<UMaterialInterface> BaseTimerMaterial;

	UPROPERTY(BlueprintReadOnly, Category = "CS_Timer", meta = (BindWidget))
	TObjectPtr<UTextBlock> Time_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_Timer", meta = (BindWidget))
	TObjectPtr<UImage> Time_Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_Timer")
	TSubclassOf<UCS_DescriptionObject> TooltipWidgetClass;

	UFUNCTION(Category = "CS_Timer")
	void UpdateTime(const FDateTime& DateTo) const;
	UFUNCTION(Category = "CS_Timer")
	void InitDynamicMaterial();
	UFUNCTION(Category = "CS_Timer")
	void InitTimerParams(const int optTime, const int maxTime);

	UFUNCTION(Category = "CS_Timer")
	FString GetStringLeftTime(int32 hours, int32 minutes, int32 seconds) const;

private:
	// Динамический материал для иконки таймера
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicTimerMaterial;

	UPROPERTY()
	TObjectPtr<UCS_DescriptionObject> CustomTooltip;
	
	float maxTimeTask;
};
