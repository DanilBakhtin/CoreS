
#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "CS_TransferZone_Actor.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS()	
class CORE_SIMULATOR_API ACS_TransferZone_Actor : public ACS_Interactable_Actor
{
	GENERATED_BODY()

public:

	ACS_TransferZone_Actor();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UArrowComponent> Zone_One;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UArrowComponent> Zone_Two;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString EndTween_Zone_One_Id = "";
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString EndTween_Zone_Two_Id = "";
	
protected:

	UFUNCTION()
	void BeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void EndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
