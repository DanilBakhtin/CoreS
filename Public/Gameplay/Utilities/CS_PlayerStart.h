
#pragma once

#include "CoreMinimal.h"
#include "CS_CheckingLoadingOfWorldPartitionChunks.h"
#include "GameFramework/PlayerStart.h"
#include "CS_PlayerStart.generated.h"


UCLASS(Blueprintable)
class CORE_SIMULATOR_API ACS_PlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:

	ACS_PlayerStart(const FObjectInitializer& ObjectInitializer);

	UCS_CheckingLoadingOfWorldPartitionChunks* GetWorldPartitionSteamingChecker();
	
protected:
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TObjectPtr<UCS_CheckingLoadingOfWorldPartitionChunks> WorldPartitionSteamingChecker;

	UFUNCTION()
	void MainCourseMapLoadingFinish(bool WorldIsLoad);
};
