// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/CS_InteractActorManager.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/ObjectsOfInteract/CS_Interact_Interface.h"
#include "Gameplay/ObjectsOfInteract/CS_Location_Interface.h"

TArray<AActor*> UCS_InteractActorManager::GetAllInteractActors()
{
	if (InteractActors.Num() == 0) 
		if (GEngine)
			UGameplayStatics::GetAllActorsWithInterface(GEngine->GameViewport->GetWorld(), UCS_Interact_Interface::StaticClass(), InteractActors);

	return InteractActors;
}

TArray<AActor*> UCS_InteractActorManager::GetLocationInteractActors()
{
	if (BorderInteractActors.Num() == 0)
		if (GEngine)
			UGameplayStatics::GetAllActorsWithInterface(GEngine->GameViewport->GetWorld(), UCS_Location_Interface::StaticClass(), BorderInteractActors);

	return BorderInteractActors;
}

void UCS_InteractActorManager::TryRegisterActor(AActor* InputActor)
{
	if (InputActor->Implements<UCS_Interact_Interface>()) {
		InteractActors.AddUnique(InputActor);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Actor %s failed register, when is not implemented CS_Interact_Interface"), *InputActor->GetName());
	}
}

void UCS_InteractActorManager::ActivateObjectInActors(const FString& IdObject, const FString& DescriptionObject)
{	
	GetAllInteractActors();

	for (AActor*& Actor : InteractActors){
		if (ICS_Interact_Interface::Execute_IsUseKey(Actor, IdObject)) {
			ICS_Interact_Interface::Execute_ActivateObject(Actor, IdObject, DescriptionObject);
			ActivatedInteractActors.Add(Actor);
		}
	}

}

void UCS_InteractActorManager::ActivateViewInActors(const FObjectViewID& StructObjectViewID)
{
	GetAllInteractActors();

	for (AActor*& Actor : InteractActors) {
		if (ICS_Interact_Interface::Execute_IsUseKey(Actor, StructObjectViewID.ViewID) || ICS_Interact_Interface::Execute_IsUseKey(Actor, StructObjectViewID.PointID)) {
			ICS_Interact_Interface::Execute_TrySetViewID(Actor, StructObjectViewID);
			ActivatedInteractActors.Add(Actor);
		}
	}
}

void UCS_InteractActorManager::ActivateReactMethodInActors(const FString& IdObject, const FVariables& Variables)
{
	GetAllInteractActors();

	for (AActor*& Actor : InteractActors) {
		if (ICS_Interact_Interface::Execute_IsUseKey(Actor, IdObject)) {
			ICS_Interact_Interface::Execute_ActivateObjectReactMethod(Actor, IdObject, Variables);
		}
	}
}

void UCS_InteractActorManager::DisableActivatedActors()
{	
	GetAllInteractActors();

	if (bFirstDeactivated) {

		for (AActor*& Actor : InteractActors)
			ICS_Interact_Interface::Execute_DisableObject(Actor);

		bFirstDeactivated = false;
	}
	else {
		for (AActor*& Actor : ActivatedInteractActors)
			ICS_Interact_Interface::Execute_DisableObject(Actor);

		ActivatedInteractActors.Empty();
	}
}

void UCS_InteractActorManager::BlinkingActor(const FString& IdObject, const bool bBlinking)
{
	for (AActor*& Actor : InteractActors) {
		if (ICS_Interact_Interface::Execute_IsUseKey(Actor, IdObject)) {

			ICS_Interact_Interface* InteractInterface = Cast<ICS_Interact_Interface>(Actor);
			if (InteractInterface) {
				if (bBlinking) {
					InteractInterface->StartBlinkingObject(IdObject);
				}
				else {
					InteractInterface->StopBlinkingObject(IdObject);
				}
			}
		}
	}
}
