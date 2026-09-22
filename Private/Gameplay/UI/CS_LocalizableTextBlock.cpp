// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_LocalizableTextBlock.h"
#include "Framework/CS_GameInstance.h"
#include "Gameplay/Utilities/CS_LocalizationManager.h"

UCS_LocalizableTextBlock::UCS_LocalizableTextBlock()
{
	LocalizationKey = TEXT("");
}

void UCS_LocalizableTextBlock::SetText(FText InText)
{
    UpdateText();
}

void UCS_LocalizableTextBlock::SetPreviewText(FText InText)
{
    Super::SetText(InText);
}

void UCS_LocalizableTextBlock::OnWidgetRebuilt()
{
    Super::OnWidgetRebuilt();

    if (bInitialized) return;

    InitializeLocalization();
}

void UCS_LocalizableTextBlock::BeginDestroy()
{
    if (UCS_GameInstance* GameInstance = Cast<UCS_GameInstance>(GetGameInstance()))
    {
        if (GameInstance->GetLocalizationManager())
        {
            GameInstance->GetLocalizationManager()->OnLanguageChanged.RemoveDynamic(this, &UCS_LocalizableTextBlock::OnLanguageChanged);
        }
    }

    Super::BeginDestroy();
}

void UCS_LocalizableTextBlock::SetKey(const FString InputKey) {

    LocalizationKey = InputKey;
    InitializeLocalization();
}

void UCS_LocalizableTextBlock::InitializeLocalization()
{
    if (bInitialized) return;

    // Подписываемся на смену языка
    if (UCS_GameInstance* GameInstance = Cast<UCS_GameInstance>(GetGameInstance()))
    {
        if (GameInstance->GetLocalizationManager())
        {
            bInitialized = true;
            GameInstance->GetLocalizationManager()->OnLanguageChanged.AddDynamic(this, &UCS_LocalizableTextBlock::OnLanguageChanged);
            UpdateText();
        }
    }
}

void UCS_LocalizableTextBlock::UpdateText()
{
    if (UCS_GameInstance* GameInstance = Cast<UCS_GameInstance>(GetGameInstance()))
    {
        if (GameInstance->GetLocalizationManager() && !LocalizationKey.IsEmpty())
        {
            FText LocalizedText = GameInstance->GetLocalizationManager()->GetText(LocalizationKey);
            Super::SetText(LocalizedText);
        }
    }
}

void UCS_LocalizableTextBlock::OnLanguageChanged()
{
    UpdateText();
}
