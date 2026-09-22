// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Utilities/CS_CheckingLoadingOfWorldPartitionChunks.h"

#include "WorldPartition/WorldPartitionSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogCheckingLoadingOfWorldPartitionChunks, All, All);

UCS_CheckingLoadingOfWorldPartitionChunks::UCS_CheckingLoadingOfWorldPartitionChunks()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCS_CheckingLoadingOfWorldPartitionChunks::StartCheckForActiveLoadingsInSteaming()
{
	UE_LOG(LogCheckingLoadingOfWorldPartitionChunks, Display, TEXT("StartCheckForActiveLoadingsInSteaming"));
	CurrentWaitingTime = WaitingTime;
	CheckForActiveLoadingsInSteaming();
}

void UCS_CheckingLoadingOfWorldPartitionChunks::StopCheckForActiveLoadingsInSteaming()
{
	UE_LOG(LogCheckingLoadingOfWorldPartitionChunks, Display, TEXT("StopCheckForActiveLoadingsInSteaming"));
	GetWorld()->GetTimerManager().ClearTimer(TimeTimerHandle);
	CurrentWaitingTime = 0.f;
	OnStreamingCompleted.Broadcast(false);
}

// Called when the game starts
void UCS_CheckingLoadingOfWorldPartitionChunks::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCS_CheckingLoadingOfWorldPartitionChunks::CheckForActiveLoadingsInSteaming()
{
	if (const UWorldPartitionSubsystem* WorldPartitionSubsystem = UWorld::GetSubsystem<UWorldPartitionSubsystem>(GetWorld()))
	{
		if(	WorldPartitionSubsystem->IsStreamingCompleted())
		{
			OnStreamingCompleted.Broadcast(true);
		}
		else
		{
			if(CurrentWaitingTime <= 0.f && WaitingTime > 0.f)
			{
				OnStreamingCompleted.Broadcast(false);
			}
			else
			{
				UE_LOG(LogCheckingLoadingOfWorldPartitionChunks, Display, TEXT("CheckForActiveLoadingsInSteaming"));
				CurrentWaitingTime -= VerificationPeriod;
				GetWorld()->GetTimerManager().SetTimer(TimeTimerHandle, this, &ThisClass::CheckForActiveLoadingsInSteaming, VerificationPeriod, false);
			}
			
		}
	}
}
