// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CS_GameInstance.h"
#include "GameFramework/GameUserSettings.h"
#include "Gameplay/Utilities/CS_LocalizationManager.h"

void UCS_GameInstance::Init()
{
    Super::Init();

    // Создаем менеджер локализации
    LocalizationManager = NewObject<UCS_LocalizationManager>(this);
    if (LocalizationManager)
    {
        // Инициализируем с языком по умолчанию
        LocalizationManager->Initialize("ru");
    }
}

void UCS_GameInstance::ApplyResolution()
{
    UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();

    if (UserSettings)
    {
        UserSettings->SetFullscreenMode(EWindowMode::Fullscreen);

        UserSettings->SetScreenResolution(UserSettings->GetDesktopResolution());
        UserSettings->ApplyResolutionSettings(false);

        UserSettings->SetOverallScalabilityLevel(2);

        UserSettings->ApplySettings(false);
        UserSettings->SaveSettings();
    }
}

void UCS_GameInstance::Shutdown()
{
    if (LocalizationManager)
    {
        LocalizationManager->ConditionalBeginDestroy();
    }

    Super::Shutdown();
}
