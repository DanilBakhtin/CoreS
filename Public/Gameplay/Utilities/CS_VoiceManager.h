// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "CS_VoiceManager.generated.h"

UCLASS()
class CORE_SIMULATOR_API ACS_VoiceManager : public AActor
{
	GENERATED_BODY()
	
public:	

	ACS_VoiceManager();

	virtual void Tick(float DeltaTime) override;

	void PlaySound(const FString& ID_Sound);

	void AddQueueSounds(const TArray<FString> IDSounds);

	void RemoveAllSoundsInQueue();

	void PauseSounds(bool Pause);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CS_VoiceManager ")
	UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TArray<USoundBase*> SoundArray;

	TArray<FString> SoundQueue;

	UPROPERTY()
	bool bIsPlayingQueueSound;

	UPROPERTY()
	float PausedPlaybackTime;

	UPROPERTY()
	bool bIsPaused;

	UFUNCTION()
	void OnSoundFinished();

	void PlayNextInQueue();

	void PlaySoundInQueue(const FString& ID_Sound);

	FTimerHandle SoundTimerHandle;
};
