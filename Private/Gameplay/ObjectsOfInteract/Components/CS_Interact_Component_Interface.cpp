
#include "Gameplay/ObjectsOfInteract/Components/CS_Interact_Component_Interface.h"


void ICS_Interact_Component_Interface::MakeDescription_Implementation(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description)
{
	
}

void ICS_Interact_Component_Interface::DisableComponent_Implementation()
{
	
}

FString ICS_Interact_Component_Interface::GetInteractID_Implementation() const
{
	return "";
}

FString ICS_Interact_Component_Interface::GetDescription_Implementation() const
{
	return "";
}

bool ICS_Interact_Component_Interface::GetIsEnableComponent_Implementation() const
{
	return false;
}

bool ICS_Interact_Component_Interface::GetInteractable_Implementation(EInteractMode CurrentMode) const
{
	return true;
}

void ICS_Interact_Component_Interface::SetIsUsedMeasureDevice_Implementation(bool Used)
{
}

void ICS_Interact_Component_Interface::EnableChangeOutline_Implementation(const bool bEnable)
{
}

void ICS_Interact_Component_Interface::SetOutline_Implementation(const bool bEnable)
{
}

void ICS_Interact_Component_Interface::RightClickInteract_Implementation()
{
}

void ICS_Interact_Component_Interface::LeftClickInteract_Implementation()
{
}

ETypeInteractPoint ICS_Interact_Component_Interface::GetTypePoint_Implementation()
{
	return ETypeInteractPoint::ActionPoint;
}
