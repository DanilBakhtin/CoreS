// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CS_CheckingLoadingOfWorldPartitionChunks.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStreamingCompleted, bool, IsStreamingCompleted);

//ActorComponent который проверяет находятся ли в загрузки в потоке 
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CORE_SIMULATOR_API UCS_CheckingLoadingOfWorldPartitionChunks : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UCS_CheckingLoadingOfWorldPartitionChunks();

	UPROPERTY(BlueprintAssignable)
	FOnStreamingCompleted OnStreamingCompleted;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float WaitingTime = -1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float VerificationPeriod = 0.5f;

	UFUNCTION(BlueprintCallable)
	void StartCheckForActiveLoadingsInSteaming();

	UFUNCTION(BlueprintCallable)
	void StopCheckForActiveLoadingsInSteaming();

protected:
	
	virtual void BeginPlay() override;
	
	void CheckForActiveLoadingsInSteaming();

	FTimerHandle TimeTimerHandle;
	
	float CurrentWaitingTime = 0.f;
};
