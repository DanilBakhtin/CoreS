
#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "CS_C43101_Prev_StaticMeshComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CORE_SIMULATOR_API UCS_C43101_Prev_StaticMeshComp : public UStaticMeshComponent
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
};
