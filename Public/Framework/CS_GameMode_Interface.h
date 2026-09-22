// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_GameMode_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, MinimalAPI)
class UCS_GameMode_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CORE_SIMULATOR_API ICS_GameMode_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CS_GameMode")
	ESimulatorMode GetSimulatorMode() const;
};
