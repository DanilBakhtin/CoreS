// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Utilities/CS_LocalizationManager.h"

FString UCS_LocalizationManager::NameFolder = TEXT("Locales");
FString UCS_LocalizationManager::NameFileExtend = TEXT("_extend");

UCS_LocalizationManager::UCS_LocalizationManager()
{
    CurrentLanguage = "ru";
}

bool UCS_LocalizationManager::Initialize(const FString& DefaultLanguage)
{
    return ChangeLanguage(DefaultLanguage);
}

bool UCS_LocalizationManager::ChangeLanguage(const FString& LanguageCode)
{
    if (LoadLanguageFile(LanguageCode))
    {
        CurrentLanguage = LanguageCode;
        OnLanguageChanged.Broadcast();
        return true;
    }
    return false;
}

bool UCS_LocalizationManager::LoadLanguageFile(const FString& LanguageCode)
{
    Translations.Empty();

    // Load main file
    FString MainFilePath = FPaths::ProjectContentDir() / NameFolder / (LanguageCode + ".json");
    bool bMainFileLoaded = LoadAndMergeJsonFile(MainFilePath);

    // Load extend file
    FString ExtendFilePath = FPaths::ProjectContentDir() / NameFolder / (LanguageCode + NameFileExtend + ".json");
    bool bExtendFileLoaded = LoadAndMergeJsonFile(ExtendFilePath);

    if (!bMainFileLoaded && !bExtendFileLoaded)
    {
        UE_LOG(LogTemp, Error, TEXT("No localization files found for language: %s"), *LanguageCode);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("Loaded language: %s with %d total translations (Main: %s, Extend: %s)"),
        *LanguageCode, Translations.Num(),
        bMainFileLoaded ? TEXT("Yes") : TEXT("No"),
        bExtendFileLoaded ? TEXT("Yes") : TEXT("No"));

    return true;
}

bool UCS_LocalizationManager::LoadAndMergeJsonFile(const FString& FilePath)
{
    if (!FPaths::FileExists(FilePath))
    {
        return false;
    }

    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load file: %s"), *FilePath);
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON: %s"), *FilePath);
        return false;
    }

    const TSharedPtr<FJsonObject>* TranslationsObject;
    if (JsonObject->TryGetObjectField(TEXT("translations"), TranslationsObject))
    {
        int32 LoadedCount = 0;
        for (const TTuple<FString, TSharedPtr<FJsonValue>>& Pair : (*TranslationsObject)->Values)
        {
            FString Value;
            if (Pair.Value->TryGetString(Value))
            {
                Translations.Add(Pair.Key, Value);
                LoadedCount++;
            }
        }

        UE_LOG(LogTemp, Verbose, TEXT("Loaded %d translations from: %s"), LoadedCount, *FilePath);
        return true;
    }

    return false;
}

FText UCS_LocalizationManager::GetText(const FString& Key) const
{
    if (const FString* Translation = Translations.Find(Key))
    {
        return FText::FromString(*Translation);
    }

    UE_LOG(LogTemp, Warning, TEXT("Translation key not found: %s"), *Key);

    return FText::FromString("MISSING " + CurrentLanguage + ' ' + ':' + Key);
}

FString UCS_LocalizationManager::GetCurrentLanguage() const
{
    return CurrentLanguage;
}

TArray<FString> UCS_LocalizationManager::GetAvailableLanguages() const
{
    TArray<FString> Languages;

    FString LocalesDir = FPaths::ProjectContentDir() / "Locales";
    TArray<FString> Files;

    IFileManager::Get().FindFiles(Files, *(LocalesDir / "*.json"), true, false);

    for (const FString& File : Files)
    {
        Languages.Add(FPaths::GetBaseFilename(File));
    }

    return Languages;
}