
#include "Gameplay/ObjectsOfInteract/Components/CS_Interaction_StaticMeshComp.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "Gameplay/Player/CS_PlayerController.h"

UCS_Interaction_StaticMeshComp::UCS_Interaction_StaticMeshComp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

	bChangeOutline = true;

	SetCustomDepthStencilValue(DefaultStencilValue);
}

void UCS_Interaction_StaticMeshComp::MakeDescription_Implementation(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description)
{
	if (!bActive)
		return;

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

void UCS_Interaction_StaticMeshComp::DisableComponent_Implementation()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Execute_SetOutline(this, false);
	bIsComponentEnable = false;
}

FString UCS_Interaction_StaticMeshComp::GetInteractID_Implementation() const
{
	return InteractID;
}

FString UCS_Interaction_StaticMeshComp::GetDescription_Implementation() const
{
	return Description;
}

bool UCS_Interaction_StaticMeshComp::GetIsEnableComponent_Implementation() const
{
	return bIsComponentEnable;
}

bool UCS_Interaction_StaticMeshComp::GetInteractable_Implementation(EInteractMode CurrentMode) const
{
	return bIsComponentEnable && CurrentMode != EInteractMode::MeausureMode;
}

void UCS_Interaction_StaticMeshComp::SetIsUsedMeasureDevice_Implementation(bool Used)
{
	bIsUsedMeasureDevise = Used;
}

void UCS_Interaction_StaticMeshComp::EnableChangeOutline_Implementation(const bool bEnable)
{
	bChangeOutline = bEnable;
}

void UCS_Interaction_StaticMeshComp::SetOutline_Implementation(const bool bEnable)
{
	if (bChangeOutline) {
		SetRenderCustomDepth(bEnable);
	}

	if (bActiveChangeStencil)
		SetCustomDepthStencilValue(bEnable ? PointingStencilValue : DefaultStencilValue);
}

void UCS_Interaction_StaticMeshComp::ActiveStaticMeshComp(bool bEnable)
{
	SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	bActive = bEnable;
}

void UCS_Interaction_StaticMeshComp::SetDefaultCustomStencil(int StencilValue)
{
	DefaultStencilValue = StencilValue;
}

void UCS_Interaction_StaticMeshComp::SetPointingCustomStencil(int StencilValue)
{
	PointingStencilValue = StencilValue;
}

bool UCS_Interaction_StaticMeshComp::GetAOSEnable()
{
	return bIsComponentEnable;
}

void UCS_Interaction_StaticMeshComp::BeginPlay()
{
	Super::BeginPlay();

	SetCollisionProfileName(FName("InteractObject"), true);
}
