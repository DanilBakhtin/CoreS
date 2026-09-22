// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/UI/CS_TextBlock.h"
#include "CS_LocalizableTextBlock.generated.h"

UCLASS()
class CORE_SIMULATOR_API UCS_LocalizableTextBlock : public UCS_TextBlock
{
	GENERATED_BODY()

public:
    UCS_LocalizableTextBlock();
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString LocalizationKey;

    virtual void SetText(FText InText) override;

    UFUNCTION(BlueprintCallable)
    void SetPreviewText(FText InText);

    void Init();

    UFUNCTION(BlueprintCallable)
    void SetKey(const FString InputKey);
    
    void InitializeLocalization();

protected:

    virtual void OnWidgetRebuilt() override;

    virtual void BeginDestroy() override;

private:
    UFUNCTION()
    void OnLanguageChanged();

    UFUNCTION()
    void UpdateText();

    UPROPERTY()
    bool bInitialized = false;
	
};
