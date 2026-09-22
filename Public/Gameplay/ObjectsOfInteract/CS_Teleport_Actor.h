
#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "CS_Teleport_Actor.generated.h"

class UArrowComponent;
class UCapsuleComponent;

UCLASS()
class CORE_SIMULATOR_API ACS_Teleport_Actor : public ACS_Interactable_Actor
{
	GENERATED_BODY()

public:

	ACS_Teleport_Actor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="CS_Teleport_Actor")
	FString ActionObjectKey;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Mesh")
	TObjectPtr<UArrowComponent> Arrow;

	UPROPERTY(VisibleDefaultsOnly, Category="Mesh")
	TObjectPtr<UCapsuleComponent> Capsule;

	virtual void BeginPlay() override;
};
