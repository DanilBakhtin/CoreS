// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CS_GameInstance.generated.h"


class UCS_LocalizationManager;

UCLASS()
class CORE_SIMULATOR_API UCS_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    virtual void Init() override;
    virtual void Shutdown() override;

    UFUNCTION(BlueprintCallable, Category = "Localization")
    UCS_LocalizationManager* GetLocalizationManager() const { return LocalizationManager; }

    UFUNCTION(BlueprintCallable, Category = "UCS_GameInstance")
    void ApplyResolution();

private:
    UPROPERTY()
    UCS_LocalizationManager* LocalizationManager;
};
