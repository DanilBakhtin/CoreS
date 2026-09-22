// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ObjectsOfInteract/CS_TransferZone_Actor.h"
#include "Gameplay/ObjectsOfInteract/CS_Location_Interface.h"
#include "CS_TransferZoneWithWarning_Actor.generated.h"

class UCS_EndWorkingAreaWidget;

/**
 * The class implements standard transfer between locations, combined with blocking.
 * Blocking is currently only realized in VSP Study mode (Delete this line if something changed).
 * #VSP_only
 * TODO: Discuss creating plugin with VSP-related logic only.
 */
UCLASS()
class CORE_SIMULATOR_API ACS_TransferZoneWithWarning_Actor : public ACS_TransferZone_Actor, public ICS_Location_Interface
{
	GENERATED_BODY()

public:
	ACS_TransferZoneWithWarning_Actor();

	// ~Begin AActor virtual function.
	virtual void BeginPlay() override;
	// ~End AActor virtual function.

	// ~Begin CS_Location_Interface.
	void SetLockLocation_Implementation(const FString& LocationAPI_Id, const bool bIsBlockedLocation) override;
	// ~End CS_Location_Interface.

	// Class for end working area widget. Use for create widget to warning player about end of working area.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_TransferZoneWithWarning|Widget")
	TObjectPtr<UBoxComponent> WarningTriggerBox;

	UFUNCTION(BlueprintCallable)
	bool IsPlayerOverlapped();
	
protected:
	bool bIsBlocked = false;
	bool bIsPlayerOverlapped = false;

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger")
	void BeginOverlapTriggerBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Trigger")
	void EndOverlapTriggerBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
};
