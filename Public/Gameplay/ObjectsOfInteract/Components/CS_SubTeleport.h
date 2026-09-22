// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "CS_SubTeleport.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CORE_SIMULATOR_API UCS_SubTeleport : public UCapsuleComponent
{
	GENERATED_BODY()
	
public:

	UCS_SubTeleport();

	UFUNCTION(BlueprintCallable)
	FString GetLocationID();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Params")
	FString LocationID = "";
};
