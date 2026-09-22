#include "Gameplay/Player/CS_PlayerState.h"

FString ACS_PlayerState::GetCurrentLocationName() const
{
	return CurrentLocationName;
}

void ACS_PlayerState::SetCurrentLocationName(const FString& NewLocationName)
{
	this->CurrentLocationName = NewLocationName;
}

FString ACS_PlayerState::GetCurrentLocationAPI_Id() const
{
	return CurrentLocationAPI_Id;
}

void ACS_PlayerState::SetCurrentLocationAPI_Id(const FString& NewLocationAPI_Id)
{
	this->CurrentLocationAPI_Id = NewLocationAPI_Id;
	OnLocationUpdated.Broadcast(NewLocationAPI_Id);
}

void ACS_PlayerState::GetCurrentLocation(FString& LocationName, FString& LocationAPI_Id)
{
	LocationName = CurrentLocationName;
	LocationAPI_Id = CurrentLocationAPI_Id;
}

void ACS_PlayerState::SetCurrentLocation(const FString& NewLocationName, const FString& NewLocationAPI_Id)
{
	this->CurrentLocationName = NewLocationName;
	this->CurrentLocationAPI_Id = NewLocationAPI_Id;
	OnLocationUpdated.Broadcast(NewLocationAPI_Id);
}

EInteractMode ACS_PlayerState::GetCurrentInteractMode()
{
	return CurrentInteractMode;
}

void ACS_PlayerState::SetCurrentInteractMode(EInteractMode NewInteractMode)
{
	CurrentInteractMode = NewInteractMode;

	OnChangeInteractMode.Broadcast(CurrentInteractMode);
}
