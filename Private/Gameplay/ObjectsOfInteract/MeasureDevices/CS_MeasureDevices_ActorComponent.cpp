
#include "Gameplay/ObjectsOfInteract/MeasureDevices/CS_MeasureDevices_ActorComponent.h"

#include "Components/CapsuleComponent.h"
#include "Framework/CS_DeveloperSettings.h"
#include "Framework/CS_SimulatorParamsAsset.h"
#include "Gameplay/ObjectsOfInteract/MeasureDevices/CS_C43101.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "Framework/CS_CoreTypes.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_Interact_Component_Interface.h"
#include "Gameplay/Player/CS_Character.h"
#include "Gameplay/Player/CS_PlayerController.h"

UCS_MeasureDevices_ActorComponent::UCS_MeasureDevices_ActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCS_MeasureDevices_ActorComponent::ActivateMeasureDevice(ACS_Interactable_Actor* InteractableActor, const FString& MeasureId, const TArray<FDeviceData>& PointsData)
{
	if(!IsValid(InteractableActor))
		return;

	if(ACS_MeasureDevices_Actor* MeasureDeviceActor = *MeasureDevices.Find(MeasureId))
	{
		CurrentDeviceID = MeasureId;
		MeasureDeviceActor->ActivateMeasureDevice(InteractableActor, MeasureId, PointsData);
		MeasureDeviceActor->SetActorTickEnabled(true);
	}
}

void UCS_MeasureDevices_ActorComponent::DisableMeasureDevice(ACS_Interactable_Actor* InteractableActor)
{
	CurrentDeviceID = "";

	ACS_Character* Character = Cast<ACS_Character>(GetOwner());
	
	for(const TTuple<FString, ACS_MeasureDevices_Actor*>& Devise : MeasureDevices)
	{
		Devise.Value->AttachToComponent(Character->GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);
		Devise.Value->ResetDevice();
		Devise.Value->SetActorRelativeLocation(FVector());
		Devise.Value->SetActorRelativeRotation(FRotator());
		Devise.Value->SetActorHiddenInGame(true);
		Devise.Value->SetActorEnableCollision(false);
		Devise.Value->SetActorTickEnabled(false);
	}
	TArray<UActorComponent*> Components;
	InteractableActor->GetComponents(Components);
	
	for (UActorComponent*& Component : Components)
	{
		if (Component->Implements<UCS_Interact_Component_Interface>())
		{
			ICS_Interact_Component_Interface::Execute_SetIsUsedMeasureDevice(Component, false);
		}
	}
}

void UCS_MeasureDevices_ActorComponent::UpdateMeasureDevice(const float& Result)
{
	if (!CurrentDeviceID.IsEmpty()) {
		if (MeasureDevices.Contains(CurrentDeviceID))
		{
			if (MeasureDevices[CurrentDeviceID])
			{
				MeasureDevices[CurrentDeviceID]->UpdateMeasureDevice(Result);
			}
		}
	}
	
}

void UCS_MeasureDevices_ActorComponent::SetPlayerController(ACS_PlayerController* _PlayerController)
{
	PlayerController = _PlayerController;

	ACS_Character* Character = Cast<ACS_Character>(GetOwner());
	if(UCS_SimulatorParamsAsset* SimulatorParams = GetDefault<UCS_DeveloperSettings>()->SimulatorParamsAsset.LoadSynchronous())
	{
		for(const FMeasureDeviceData& MeasureDevicesData : SimulatorParams->Params.MeasureDevicesData)
		{
			if(TSubclassOf<ACS_MeasureDevices_Actor> MeasureDevice = MeasureDevicesData.MeasureActor)
			{
				ACS_MeasureDevices_Actor* MeasureDeviceActor = GetWorld()->SpawnActor<ACS_MeasureDevices_Actor>(MeasureDevice, FVector(), FRotator());
				MeasureDeviceActor->AttachToComponent(Character->GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);
				MeasureDeviceActor->SetHidden(false);
				MeasureDeviceActor->SetActorEnableCollision(false);
				MeasureDeviceActor->SetPlayerController(PlayerController.Get());
				MeasureDevices.Add(MeasureDevicesData.MeasureId, MeasureDeviceActor);
				
			}
		}	
	}
}

void UCS_MeasureDevices_ActorComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


