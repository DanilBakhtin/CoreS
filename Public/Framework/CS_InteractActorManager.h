// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_InteractActorManager.generated.h"

class ACS_PlayerController;

UCLASS()
class CORE_SIMULATOR_API UCS_InteractActorManager : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION()
	TArray<AActor*> GetAllInteractActors();

	// Get actor that implements CS_Location interface
	UFUNCTION()
	TArray<AActor*> GetLocationInteractActors();

	UFUNCTION()
	void TryRegisterActor(AActor* InputActor);

	UFUNCTION()
	void ActivateObjectInActors(const FString& IdObject, const FString& DescriptionObject);

	UFUNCTION()
	void ActivateViewInActors(const FObjectViewID& StructObjectViewID);

	UFUNCTION()
	void ActivateReactMethodInActors(const FString& IdObject, const FVariables& Variables);

	UFUNCTION()
	void DisableActivatedActors();

	UFUNCTION()
	void BlinkingActor(const FString& IdObject, const bool bBlinking);

protected:

private:
	
	UPROPERTY()
	TArray<AActor*> InteractActors;

	UPROPERTY()
	TArray<AActor*> ActivatedInteractActors;

	UPROPERTY()
	TArray<AActor*> BorderInteractActors;

	UPROPERTY()
	bool bFirstDeactivated = true;
};
