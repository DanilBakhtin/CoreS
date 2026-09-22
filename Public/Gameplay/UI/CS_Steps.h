// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_Steps.generated.h"

class UTextBlock;

UCLASS()
class CORE_SIMULATOR_API UCS_Steps : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "CS_Steps", meta = (BindWidget))
	TObjectPtr<UTextBlock> Steps_TextBlock;

	UFUNCTION(Category = "CS_Steps")
	void SetMaxSteps(const int AmountSteps);
	

	UFUNCTION(Category = "CS_Steps")
	void UpdateSteps(const int CurrentSteps);

protected:

	UPROPERTY()
	int MaxSteps = 0;

};
