// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_GameState_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, MinimalAPI)
class UCS_GameState_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CORE_SIMULATOR_API ICS_GameState_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CS_GameState")
	FDateTime GetCurrentTime() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CS_GameState")
	void SetCurrentTime(const FDateTime InTime);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CS_GameState_ActorManager")
	TArray<AActor*> GetAllInteractableActors();

	// Get actor that implements CS_Location interface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CS_GameState_ActorManager")
	TArray<AActor*> GetLocationInteractActors();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CS_GameState_ActorManager")
	void TryRegisterActor(AActor* InputActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CS_GameState_ActorManager")
	void ActivateObjectInActors(const FString& IdObject, const FString& DescriptionObject);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CS_GameState_ActorManager")
	void ActivateViewInActors(const FObjectViewID& StructObjectViewID);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CS_GameState_ActorManager")
	void ActivateReactMethodInActors(const FString& IdObject, const FVariables& Variables);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CS_GameState_ActorManager")
	void DisableActivatedActors();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CS_GameState_ActorManager")
	void BlinkingActor(const FString& IdObject, const bool bBlinking);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CS_GameState_ActorManager")
	bool GetStartingStateInitialized();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CS_GameState_ActorManager")
	void SetStartingStateInitialized();
	
};
