
#include "Gameplay/ObjectsOfInteract/CS_Teleport_Actor.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

ACS_Teleport_Actor::ACS_Teleport_Actor()
{
	PrimaryActorTick.bCanEverTick = false;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = Capsule;
	
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(Capsule);
}

void ACS_Teleport_Actor::BeginPlay()
{
	Super::BeginPlay();

	ActionObjectKeys.Empty();


	ActionObjectKeys.Add(ActionObjectKey);
}
