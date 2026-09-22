
#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "CS_Interactable_3dPoint_Actor.generated.h"

class USceneComponent;

UCLASS()
class CORE_SIMULATOR_API ACS_Interactable_3dPoint_Actor : public ACS_Interactable_Actor
{
	GENERATED_BODY()

public:

	ACS_Interactable_3dPoint_Actor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CS_Interactable_2d_Actor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> RootSceneComponent;
	
	virtual bool Interact() override;
	virtual void StopInteract() override;
};
