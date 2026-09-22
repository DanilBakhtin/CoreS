// Core Simulator

#include "Gameplay/ObjectsOfInteract/Components/CS_SubTeleport.h"

UCS_SubTeleport::UCS_SubTeleport()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FString UCS_SubTeleport::GetLocationID()
{
	return LocationID;
}
