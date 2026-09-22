#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_PlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewLocation, FString, LocationID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeInteractMode, EInteractMode, NewInteractMode);

UCLASS()
class CORE_SIMULATOR_API ACS_PlayerState : public APlayerState
{
public:
	GENERATED_BODY()

protected:

	FString CurrentLocationName;
	FString CurrentLocationAPI_Id;
	EInteractMode CurrentInteractMode = EInteractMode::Normal;

public:

	UFUNCTION(BlueprintCallable)
	FString GetCurrentLocationName() const;
	UFUNCTION(BlueprintCallable)
	void SetCurrentLocationName(const FString& CurrentLocationName);
	UFUNCTION(BlueprintCallable)
	FString GetCurrentLocationAPI_Id() const;
	UFUNCTION(BlueprintCallable)
	void SetCurrentLocationAPI_Id(const FString& CurrentLocationAPI_Id);

	UFUNCTION(BlueprintCallable)
	void GetCurrentLocation(FString& CurrentLocationName, FString& CurrentLocationAPI_Id);
	UFUNCTION(BlueprintCallable)
	void SetCurrentLocation(const FString& CurrentLocationName, const FString& CurrentLocationAPI_Id);

	UFUNCTION(BlueprintCallable)
	EInteractMode GetCurrentInteractMode();
	UFUNCTION(BlueprintCallable)
	void SetCurrentInteractMode(EInteractMode NewInteractMode);

	UPROPERTY(BlueprintAssignable, Category = "CS_PlayerState")
	FOnNewLocation OnLocationUpdated;

	UPROPERTY(BlueprintAssignable, Category = "CS_PlayerState")
	FOnChangeInteractMode OnChangeInteractMode;
};
