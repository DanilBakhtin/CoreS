// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ObjectsOfInteract/Components/CS_Interaction_SkeletalMeshComp.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "Gameplay/Player/CS_PlayerController.h"

UCS_Interaction_SkeletalMeshComp::UCS_Interaction_SkeletalMeshComp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCS_Interaction_SkeletalMeshComp::MakeDescription_Implementation(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description)
{
	if (bIsComponentEnable || !IsValid(Interactable_Actor))
		return;

	if (!bInitDescription)
	{
		bInitDescription = true;
		Description = _Description;
		Owner = Interactable_Actor;
	}
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	bIsComponentEnable = true;
}

void UCS_Interaction_SkeletalMeshComp::DisableComponent_Implementation()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Execute_SetOutline(this, false);
	bIsComponentEnable = false;
}

FString UCS_Interaction_SkeletalMeshComp::GetInteractID_Implementation() const
{
	return InteractID;
}

FString UCS_Interaction_SkeletalMeshComp::GetDescription_Implementation() const
{
	return Description;
}

bool UCS_Interaction_SkeletalMeshComp::GetInteractable_Implementation(EInteractMode CurrentMode) const
{
	return bIsComponentEnable && CurrentMode != EInteractMode::MeausureMode;
}

bool UCS_Interaction_SkeletalMeshComp::GetIsEnableComponent_Implementation() const
{
	return bIsComponentEnable;
}

void UCS_Interaction_SkeletalMeshComp::SetIsUsedMeasureDevice_Implementation(bool Used)
{
	bIsUsedMeasureDevise = Used;
}

void UCS_Interaction_SkeletalMeshComp::EnableChangeOutline_Implementation(const bool bEnable)
{
	bChangeOutline = bEnable;
}

void UCS_Interaction_SkeletalMeshComp::SetOutline_Implementation(const bool bEnable)
{
	if (bChangeOutline)
		SetRenderCustomDepth(bEnable);
}

void UCS_Interaction_SkeletalMeshComp::BeginPlay()
{
	Super::BeginPlay();

	//SetCollisionProfileName(FName("InteractObject"), true);

}