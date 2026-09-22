
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_MeasureDevices_Actor.generated.h"

class ACS_PlayerController;
class ACS_Interactable_Actor;

UCLASS()
class CORE_SIMULATOR_API ACS_MeasureDevices_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACS_MeasureDevices_Actor();

	virtual void ClickOnMeasurePoint(const FString& InteractID, const FTransform& PositionMeasure);
protected:
	
	virtual void AttachMeasureDeviceToPrev(UActorComponent* PrevComponent);
	
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void ActivateMeasureDevice(ACS_Interactable_Actor* InteractableActor, const FString& MeasureId, const TArray<FDeviceData>& PointsData);

	virtual void SetPlayerController(ACS_PlayerController* PlayerController);

	virtual void UpdateMeasureDevice(const float& Result);
	
	virtual void ResetDevice();
	
protected:

	FString IdDevice;
	
	UPROPERTY()
	TWeakObjectPtr<ACS_PlayerController> PlayerController;
};
