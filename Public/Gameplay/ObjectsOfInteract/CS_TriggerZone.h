// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CS_TriggerZone.generated.h"

class ACS_PlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTriggerOverlap, AActor*, ActorOverlap, bool, bStart);

UCLASS()
class CORE_SIMULATOR_API ACS_TriggerZone : public AActor
{
	GENERATED_BODY()
	
public:	
	ACS_TriggerZone();

    UPROPERTY(BlueprintAssignable, Category = "CS_TriggerZone")
    FOnTriggerOverlap OnTriggerOverplap;

protected:

	virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CS_TriggerZone", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> RootSceneComponent;

    UFUNCTION()
    virtual void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    virtual void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);

    UFUNCTION(BlueprintCallable)
    ACS_PlayerController* TryGetPlayerController();

    UPROPERTY()
    TObjectPtr<ACS_PlayerController> CurrentPlayerController;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

    void SubscribeToTriggers();

};
