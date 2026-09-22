// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "CS_LocationZone.generated.h"

/**
 * For using this class, you need to add second user trace channel profile. 
 */
UCLASS()
class CORE_SIMULATOR_API ACS_LocationZone : public ACS_Interactable_Actor
{
	GENERATED_BODY()
	
public:	
	ACS_LocationZone();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	int32 GetPriorityNumber() { return Priority; }

	UFUNCTION()
	FString GetLocationID() { return LocationID; }

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	int32 Priority = 0;

private:
};
