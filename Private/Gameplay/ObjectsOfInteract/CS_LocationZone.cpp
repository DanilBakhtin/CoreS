#include "Gameplay/ObjectsOfInteract/CS_LocationZone.h"

ACS_LocationZone::ACS_LocationZone()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ACS_LocationZone::BeginPlay()
{
	Super::BeginPlay();
}

void ACS_LocationZone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACS_LocationZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

