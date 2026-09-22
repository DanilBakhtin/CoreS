// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_EndWorkingAreaWidget.generated.h"

class UCS_LocalizableTextBlock;

/**
 * 
 */
UCLASS()
class CORE_SIMULATOR_API UCS_EndWorkingAreaWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// We use native preconstruct for set localization key of EndWorkingArea_TextBlock.
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

	// Text for display end working zone text. Localize use for default text only!
	UPROPERTY(BlueprintReadOnly, Category = "CS_EndWorkingArea", DisplayName = "End Working Area Text Block", meta = (BindWidget))
	TObjectPtr<UCS_LocalizableTextBlock> EndWorkingArea_TextBlock;

	UFUNCTION(BlueprintCallable, Category = "CS_EndWorkingArea", DisplayName = "End Working Area Set Text")
	virtual void SetText(FText InText);
};
