// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "CS_LocalizationManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLanguageChangedDelegate);

UCLASS()
class CORE_SIMULATOR_API UCS_LocalizationManager : public UObject
{
	GENERATED_BODY()
	
public:
    UCS_LocalizationManager();

    UPROPERTY(BlueprintAssignable, Category = "Localization")
    FLanguageChangedDelegate OnLanguageChanged;

    UFUNCTION(BlueprintCallable, Category = "Localization")
    bool Initialize(const FString& DefaultLanguage = "en");

    UFUNCTION(BlueprintCallable, Category = "Localization")
    bool ChangeLanguage(const FString& LanguageCode);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Localization")
    FText GetText(const FString& Key) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Localization")
    FString GetCurrentLanguage() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Localization")
    TArray<FString> GetAvailableLanguages() const;

private:

    bool LoadLanguageFile(const FString& LanguageCode);

    bool LoadAndMergeJsonFile(const FString& FilePath);

    UPROPERTY()
    TMap<FString, FString> Translations;

    FString CurrentLanguage;

    static FString NameFolder;

    static FString NameFileExtend;
};
