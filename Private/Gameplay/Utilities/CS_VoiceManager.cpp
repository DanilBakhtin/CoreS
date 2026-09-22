// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Utilities/CS_VoiceManager.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

// Sets default values
ACS_VoiceManager::ACS_VoiceManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Создаем AudioComponent
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

	 RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	 AudioComponent->SetupAttachment(RootComponent);

	// Отключаем пространственные эффекты
	AudioComponent->bAllowSpatialization = false;
	AudioComponent->bOverrideAttenuation = false;
	AudioComponent->AttenuationSettings = nullptr;

	// Делаем звук глобальным (2D)
	AudioComponent->SetWorldLocation(FVector::ZeroVector); // или оставляем на месте

    AudioComponent->OnAudioFinished.AddDynamic(this, &ACS_VoiceManager::OnSoundFinished);

}

// Called when the game starts or when spawned
void ACS_VoiceManager::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ACS_VoiceManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACS_VoiceManager::PlaySound(const FString& ID_Sound)
{
    if (!AudioComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("AudioComponent is null!"));
        return;
    }

    bool bSoundFound = false;

    FString TargetSoundName = ID_Sound;

    TargetSoundName.RemoveFromStart(TEXT("s_"));
    TargetSoundName.RemoveFromStart(TEXT("S_"));
    TargetSoundName.RemoveFromStart(TEXT("Sound_"));

    for (USoundBase* Sound : SoundArray)
    {
        if (!Sound) continue;

        FString SoundName = Sound->GetName();

        SoundName.RemoveFromStart(TEXT("s_"));
        SoundName.RemoveFromStart(TEXT("S_"));
        SoundName.RemoveFromStart(TEXT("Sound_"));

        if (SoundName == TargetSoundName)
        {   
            bIsPlayingQueueSound = false;

            AudioComponent->SetSound(Sound);
            AudioComponent->FadeIn(0.3f, 1.0f, 0.0f, EAudioFaderCurve::Linear);
            bSoundFound = true;
            UE_LOG(LogTemp, Log, TEXT("Playing sound: %s"), *SoundName);

            float SoundDuration = Sound->GetDuration();
            if (SoundDuration > 0.0f)
            {
                GetWorld()->GetTimerManager().SetTimer(
                    SoundTimerHandle,
                    this,
                    &ACS_VoiceManager::OnSoundFinished,
                    SoundDuration,
                    false
                );
                UE_LOG(LogTemp, Log, TEXT("Timer set for sound duration: %f seconds"), SoundDuration);
            }

            break;
        }
    }

    if (!bSoundFound)
    {
        AudioComponent->SetSound(nullptr);
        AudioComponent->FadeOut(0.1f, 0.0f, EAudioFaderCurve::Linear);

        UE_LOG(LogTemp, Warning, TEXT("Sound not found: %s"), *ID_Sound);
    }

}

void ACS_VoiceManager::AddQueueSounds(const TArray<FString> IDSounds)
{   

    //PlaySound(IDSounds[0]);

    //The queue is temporarily disabled, the code is working

    for (const FString& SoundID : IDSounds)
    {
        SoundQueue.Add(SoundID);
        UE_LOG(LogTemp, Log, TEXT("Sound added to queue: %s"), *SoundID);
    }

    if (!bIsPlayingQueueSound && !SoundQueue.IsEmpty())
    {
        PlayNextInQueue();
    }
}

void ACS_VoiceManager::OnSoundFinished()
{   
    if (!bIsPlayingQueueSound) return;

    bIsPlayingQueueSound = false;
    UE_LOG(LogTemp, Log, TEXT("Sound finished playing"));

    // После завершения звука, проигрываем следующий из очереди
    PlayNextInQueue();
}

void ACS_VoiceManager::PlayNextInQueue()
{
    if (SoundQueue.Num() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Sound queue is empty"));
        return;
    }

    // Берем первый звук из очереди и удаляем его
    FString NextSoundID = SoundQueue[0];
    SoundQueue.RemoveAt(0);

    UE_LOG(LogTemp, Log, TEXT("Playing next sound from queue: %s"), *NextSoundID);
    PlaySoundInQueue(NextSoundID);
}

void ACS_VoiceManager::PlaySoundInQueue(const FString& ID_Sound)
{
    if (!AudioComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("AudioComponent is null!"));
        return;
    }

    bool bSoundFound = false;

    FString TargetSoundName = ID_Sound;

    TargetSoundName.RemoveFromStart(TEXT("S_"));
    TargetSoundName.RemoveFromStart(TEXT("s_"));
    TargetSoundName.RemoveFromStart(TEXT("Sound_"));

    for (USoundBase* Sound : SoundArray)
    {
        if (!Sound) continue;

        FString SoundName = Sound->GetName();
        

        SoundName.RemoveFromStart(TEXT("S_"));
        SoundName.RemoveFromStart(TEXT("s_"));
        SoundName.RemoveFromStart(TEXT("Sound_"));

        if (SoundName == TargetSoundName)
        {
            AudioComponent->SetSound(Sound);
            AudioComponent->FadeIn(0.3f, 1.0f, 0.0f, EAudioFaderCurve::Linear);
            bSoundFound = true;
            bIsPlayingQueueSound = true;

            UE_LOG(LogTemp, Log, TEXT("Playing sound from queue: %s"), *SoundName);

            float SoundDuration = Sound->GetDuration();
            if (SoundDuration > 0.0f)
            {
                GetWorld()->GetTimerManager().SetTimer(
                    SoundTimerHandle,
                    this,
                    &ACS_VoiceManager::OnSoundFinished,
                    SoundDuration,
                    false
                );
                UE_LOG(LogTemp, Log, TEXT("Timer set for sound duration: %f seconds"), SoundDuration);
            }
            break;
        }
    }

    if (!bSoundFound)
    {
        AudioComponent->SetSound(nullptr);
        AudioComponent->FadeOut(0.1f, 0.0f, EAudioFaderCurve::Linear);
        bIsPlayingQueueSound = false;

        UE_LOG(LogTemp, Warning, TEXT("Sound not found in queue: %s"), *ID_Sound);

        // Если есть очередь, пробуем проиграть следующий звук
        PlayNextInQueue();
    }
}

void ACS_VoiceManager::RemoveAllSoundsInQueue()
{   
    if (AudioComponent)
    {
        AudioComponent->Stop();
        AudioComponent->SetSound(nullptr);
    }

    SoundQueue.Empty();

    bIsPlayingQueueSound = false;
}

void ACS_VoiceManager::PauseSounds(bool Pause)
{
    if (!AudioComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("AudioComponent is null in PauseSounds!"));
        return;
    }

    if ((Pause && bIsPaused) || (!Pause && !bIsPaused))
    {
        UE_LOG(LogTemp, Log, TEXT("Sound already in desired state (Paused: %s)"), bIsPaused ? TEXT("true") : TEXT("false"));
        return;
    }

    if (Pause)
    {
        if (AudioComponent->IsPlaying())
        {
            AudioComponent->SetPaused(true);

            GetWorld()->GetTimerManager().PauseTimer(SoundTimerHandle);

            bIsPaused = true;

            UE_LOG(LogTemp, Log, TEXT("VoiceManager: Sound paused"));
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("VoiceManager: No sound is playing to pause"));
        }
    }
    else
    {
        if (AudioComponent->bIsPaused || bIsPaused)
        {
            AudioComponent->SetPaused(false);

            GetWorld()->GetTimerManager().UnPauseTimer(SoundTimerHandle);

            bIsPaused = false;

            UE_LOG(LogTemp, Log, TEXT("VoiceManager: Sound resumed"));
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("VoiceManager: Sound is not paused"));
        }
    }
}
