// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_WindowContainerWidget.generated.h"

class UCanvasPanel;
/**
 * 
 */
UCLASS()
class CORE_SIMULATOR_API UCS_WindowContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "CS_WindowContainerWidget", meta = (BindWidget))
	TObjectPtr<UCanvasPanel> RootCanvasPanel;
	
protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void AddWidgetToPanel(UWidget* Widget);
};
