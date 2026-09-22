// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CS_GameState.h"

#include "Framework/CS_InteractActorManager.h"

void ACS_GameState::BeginPlay()
{
	Super::BeginPlay();

	InitializeActorsManager();
}

FDateTime ACS_GameState::GetCurrentTime_Implementation() const
{
	return CurrentTime;
}

void ACS_GameState::SetCurrentTime_Implementation(const FDateTime InTime)
{
	CurrentTime = InTime;
}

TArray<AActor*> ACS_GameState::GetAllInteractableActors_Implementation()
{	
	if (ActorManager)
		return ActorManager->GetAllInteractActors();
	
	UE_LOG(LogTemp, Error, TEXT("Failed get all interactable actors. Actor manager is null!"));
	return TArray<AActor*>();
}

TArray<AActor*> ACS_GameState::GetLocationInteractActors_Implementation()
{
	if (ActorManager)
		return ActorManager->GetLocationInteractActors();

	UE_LOG(LogTemp, Error, TEXT("Failed get border interactable actors. Actor manager is null!"));
	return TArray<AActor*>();
}

void ACS_GameState::TryRegisterActor_Implementation(AActor* InputActor)
{
	if (ActorManager)
		ActorManager->TryRegisterActor(InputActor);
	else
		UE_LOG(LogTemp, Error, TEXT("Failed register actors. Actor manager is null!"));
}

void ACS_GameState::ActivateObjectInActors_Implementation(const FString& IdObject, const FString& DescriptionObject)
{
	if (ActorManager)
		ActorManager->ActivateObjectInActors(IdObject, DescriptionObject);
	else
		UE_LOG(LogTemp, Error, TEXT("Failed activated actors. Actor manager is null!"));
}

void ACS_GameState::ActivateViewInActors_Implementation(const FObjectViewID& StructObjectViewID)
{
	if (ActorManager)
		ActorManager->ActivateViewInActors(StructObjectViewID);
	else
		UE_LOG(LogTemp, Error, TEXT("Failed activated view actors. Actor manager is null!"));
}

void ACS_GameState::ActivateReactMethodInActors_Implementation(const FString& IdObject, const FVariables& Variables)
{
	if (ActorManager)
		ActorManager->ActivateReactMethodInActors(IdObject, Variables);
	else
		UE_LOG(LogTemp, Error, TEXT("Failed react method actors. Actor manager is null!"));
}

void ACS_GameState::DisableActivatedActors_Implementation()
{
	if (ActorManager)
		ActorManager->DisableActivatedActors();
	else
		UE_LOG(LogTemp, Error, TEXT("Failed disable actors. Actor manager is null!"));
}

void ACS_GameState::BlinkingActor_Implementation(const FString& IdObject, const bool bBlinking)
{
	if (ActorManager)
		ActorManager->BlinkingActor(IdObject, bBlinking);
}

bool ACS_GameState::GetStartingStateInitialized_Implementation()
{
	return bStartingStateInitialized;
}

void ACS_GameState::SetStartingStateInitialized_Implementation()
{
	bStartingStateInitialized = true;
}

void ACS_GameState::InitializeActorsManager()
{
	ActorManager = NewObject<UCS_InteractActorManager>(this);
	
	if (!ActorManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create Actor manager!"));
	}
}
