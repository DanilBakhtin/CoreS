
#include "Gameplay/ObjectsOfInteract/CS_Interactable_3dPoint_Actor.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_Interaction_StaticMeshComp.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_Interaction_SkeletalMeshComp.h"
#include "Gameplay/Player/CS_PlayerController.h"

ACS_Interactable_3dPoint_Actor::ACS_Interactable_3dPoint_Actor()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
}

bool ACS_Interactable_3dPoint_Actor::Interact()
{
	if(!bCanInteractObject)
		return false;
	
	CurrentPlayerController->SetInteractableActor(this);
	if(UCS_Interaction_StaticMeshComp* StaticMeshComp = Cast<UCS_Interaction_StaticMeshComp>(CurrentPlayerController->GetLastInteractActorComp()))
	{
		FIntVector2 ViewportSize = FIntVector2();
		CurrentPlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);
		CurrentPlayerController->SetMouseLocation(ViewportSize.X/2, ViewportSize.Y/2);
		CurrentPlayerController->OnClickObject(StaticMeshComp->InteractID, "");
	}

	if (UCS_Interaction_SkeletalMeshComp* SkeletalMeshComp = Cast<UCS_Interaction_SkeletalMeshComp>(CurrentPlayerController->GetLastInteractActorComp()))
	{
		FIntVector2 ViewportSize = FIntVector2();
		CurrentPlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);
		CurrentPlayerController->SetMouseLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
		CurrentPlayerController->OnClickObject(SkeletalMeshComp->InteractID, "");
	}

	return false;
}

void ACS_Interactable_3dPoint_Actor::StopInteract()
{
	CurrentPlayerController->SetInteractableActor(nullptr);
	bInteracted = false;
}
