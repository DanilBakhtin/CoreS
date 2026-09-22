
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_MeasureDevices_ActorComponent.generated.h"

class ACS_MeasureDevices_Actor;
class ACS_Interactable_Actor;
class ACS_PlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CORE_SIMULATOR_API UCS_MeasureDevices_ActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCS_MeasureDevices_ActorComponent();

	void ActivateMeasureDevice(ACS_Interactable_Actor* InteractableActor, const FString& MeasureId, const TArray<FDeviceData>& PointsData);
	void DisableMeasureDevice(ACS_Interactable_Actor* InteractableActor);
	void UpdateMeasureDevice(const float& Result) ;
	
	void SetPlayerController(ACS_PlayerController* PlayerController);
protected:

	virtual void BeginPlay() override;

	UPROPERTY()
	TWeakObjectPtr<ACS_PlayerController> PlayerController;
	UPROPERTY()
	TMap<FString, ACS_MeasureDevices_Actor*> MeasureDevices;

	FString CurrentDeviceID = "";
};
