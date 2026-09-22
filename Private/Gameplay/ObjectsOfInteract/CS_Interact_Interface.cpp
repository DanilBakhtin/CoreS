// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ObjectsOfInteract/CS_Interact_Interface.h"
#include "Framework/CS_CoreTypes.h"

// Add default functionality here for any ICS_Interact_Interface functions that are not pure virtual.
void ICS_Interact_Interface::bCheckInteract(ACS_PlayerController* PlayerController, UActorComponent* ActorComponent)
{
}

bool ICS_Interact_Interface::Interact()
{
	return true;
}

bool ICS_Interact_Interface::UseKeys_Implementation(const TArray<FString>& Keys) const
{
	return false;
}

bool ICS_Interact_Interface::IsUseKey_Implementation(const FString& Key) const
{
	return false;
}

void ICS_Interact_Interface::ActivateObject_Implementation(const FString& Key, const FString& DescriptionObject)
{
	
}

void ICS_Interact_Interface::ActivateObjectReactMethod_Implementation(const FString& Key,
	const FVariables& Variable)
{
	
}

void ICS_Interact_Interface::ActivatePoint_Implementation(const FString& Key,
														   const FParametersObject& Parameters)
{
	
}

void ICS_Interact_Interface::UpdateObjectAndPoint_Implementation(const FString& Key,
	const FString& DescriptionObject)
{
	
}


void ICS_Interact_Interface::Teleport_Implementation(ACS_PlayerController* PlayerController,const FString& Key)
{
	
}

void ICS_Interact_Interface::DisableObject_Implementation()
{
	
}

void ICS_Interact_Interface::DisablePoint_Implementation()
{
	
}

bool ICS_Interact_Interface::TryToSubTeleport_Implementation(const FString& Key)
{
	return false;
}

void ICS_Interact_Interface::StartBlinkingObject(const FString& Key)
{

}

void ICS_Interact_Interface::StopBlinkingObject(const FString& Key)
{

}