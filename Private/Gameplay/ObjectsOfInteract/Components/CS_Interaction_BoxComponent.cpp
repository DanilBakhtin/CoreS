// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ObjectsOfInteract/Components/CS_Interaction_BoxComponent.h"

#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "Gameplay/Player/CS_Character.h"
#include "Gameplay/Player/CS_PlayerController.h"

UCS_Interaction_BoxComponent::UCS_Interaction_BoxComponent(const FObjectInitializer& ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCS_Interaction_BoxComponent::MakeDescription_Implementation(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description)
{
	if (bIsComponentEnable || !IsValid(Interactable_Actor))
		return;

	if (!bInitDescription)
	{
		bInitDescription = true;
		Description = _Description;
		Owner = Interactable_Actor;
	}
	bIsComponentEnable = true;
}

void UCS_Interaction_BoxComponent::DisableComponent_Implementation()
{
	bIsComponentEnable = false;
}

FString UCS_Interaction_BoxComponent::GetInteractID_Implementation() const
{
	return InteractID;
}

FString UCS_Interaction_BoxComponent::GetDescription_Implementation() const
{
	return Description;
}

bool UCS_Interaction_BoxComponent::GetIsEnableComponent_Implementation() const
{
	return bIsComponentEnable;
}

void UCS_Interaction_BoxComponent::BeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(Owner))
		return;

	if (ACS_Character* Character = Cast<ACS_Character>(OtherActor))
	{
		if (ICS_Interact_Interface* Interact_Interface = Cast<ICS_Interact_Interface>(Owner))
		{
			Interact_Interface->bCheckInteract(Cast<ACS_PlayerController>(Character->GetController()), this);

			if (!IsValid(Owner->GetCurrentPlayerController()))
				return;

			Owner->GetCurrentPlayerController()->SetInteractableActor(Owner);
			Owner->GetCurrentPlayerController()->EndTween(EndTwinID);
		}
	}
}

void UCS_Interaction_BoxComponent::EndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ACS_Character>(OtherActor))
	{
		if (IsValid(Owner))
		{
			if (!IsValid(Owner->GetCurrentPlayerController()))
				return;

			Owner->GetCurrentPlayerController()->SetInteractableActor(nullptr);
			//Owner->GetCurrentPlayerController()->EndTween(Owner->GetExitEndTwinID());
		}
	}
}

void UCS_Interaction_BoxComponent::BeginPlay()
{
	Super::BeginPlay();

	//SetCollisionProfileName(FName("InteractObject"), true);

	OnComponentBeginOverlap.AddDynamic(this, &UCS_Interaction_BoxComponent::BeginOverLap);
	OnComponentEndOverlap.AddDynamic(this, &UCS_Interaction_BoxComponent::EndOverLap);
}
