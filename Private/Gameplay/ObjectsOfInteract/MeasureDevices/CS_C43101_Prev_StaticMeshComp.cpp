
#include "Gameplay/ObjectsOfInteract/MeasureDevices/CS_C43101_Prev_StaticMeshComp.h"

void UCS_C43101_Prev_StaticMeshComp::BeginPlay()
{
	Super::BeginPlay();

	SetHiddenInGame(true);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
