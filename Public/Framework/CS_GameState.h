// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CS_GameState_Interface.h"
#include "GameFramework/GameStateBase.h"

#include "CS_GameState.generated.h"

class UCS_InteractActorManager;

UCLASS()
class CORE_SIMULATOR_API ACS_GameState : public AGameStateBase, public ICS_GameState_Interface
{
	GENERATED_BODY()

public:
	
	virtual void BeginPlay() override;

	// ~Begin CS_GameState_Interface
	virtual FDateTime GetCurrentTime_Implementation() const override;
	virtual void SetCurrentTime_Implementation(FDateTime InTime) override;
	virtual TArray<AActor*> GetAllInteractableActors_Implementation() override;
	virtual TArray<AActor*> GetLocationInteractActors_Implementation() override;
	virtual void TryRegisterActor_Implementation(AActor* InputActor) override;
	virtual void ActivateObjectInActors_Implementation(const FString& IdObject, const FString& DescriptionObject) override;
	virtual void ActivateViewInActors_Implementation(const FObjectViewID& StructObjectViewID) override;
	virtual void ActivateReactMethodInActors_Implementation(const FString& IdObject, const FVariables& Variables) override;
	virtual void DisableActivatedActors_Implementation() override;
	virtual void BlinkingActor_Implementation(const FString& IdObject, const bool bBlinking) override;

	virtual bool GetStartingStateInitialized_Implementation() override;
	virtual void SetStartingStateInitialized_Implementation() override;
	// ~End CS_GameState_Interface


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	FDateTime CurrentTime;

	UPROPERTY()
	UCS_InteractActorManager* ActorManager;

	UFUNCTION()
	void InitializeActorsManager();

	UPROPERTY()
	bool bStartingStateInitialized = false;
};
