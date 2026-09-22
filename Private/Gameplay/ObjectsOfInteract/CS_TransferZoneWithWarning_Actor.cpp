// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ObjectsOfInteract/CS_TransferZoneWithWarning_Actor.h"
#include "Components/BoxComponent.h"
#include "Gameplay/Player/CS_Character.h"
#include "Gameplay/Player/CS_PlayerController.h"

ACS_TransferZoneWithWarning_Actor::ACS_TransferZoneWithWarning_Actor() : Super()
{
	WarningTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WarningBoxComponent"));
	WarningTriggerBox->SetupAttachment(BoxComponent);
	WarningTriggerBox->SetBoxExtent(BoxComponent->GetUnscaledBoxExtent() + 5.f);
	
	WarningTriggerBox->OnComponentBeginOverlap.Clear();
	WarningTriggerBox->OnComponentEndOverlap.Clear();
	WarningTriggerBox->OnComponentBeginOverlap.AddDynamic(this,	&ACS_TransferZoneWithWarning_Actor::BeginOverlapTriggerBox);
	WarningTriggerBox->OnComponentEndOverlap.AddDynamic(this, &ACS_TransferZoneWithWarning_Actor::EndOverlapTriggerBox);
	WarningTriggerBox->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

void ACS_TransferZoneWithWarning_Actor::BeginPlay()
{
	Super::BeginPlay();
}

void ACS_TransferZoneWithWarning_Actor::SetLockLocation_Implementation(const FString& LocationAPI_Id,
	const bool bIsBlockedLocation)
{
	if (LocationAPI_Id.Equals(EndTween_Zone_One_Id) || LocationAPI_Id.Equals(EndTween_Zone_Two_Id))
	{
		if (!bIsBlocked && bIsBlockedLocation)
		{
			BoxComponent->SetCollisionProfileName(FName("BlockAll"), true);
			WarningTriggerBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
			bIsBlocked = true;
		}
		if (bIsBlocked && !bIsBlockedLocation)
		{
			BoxComponent->SetCollisionProfileName(FName("OverlapAllDynamic"), true);
			WarningTriggerBox->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
			bIsBlocked = false;
		}
	}
}

bool ACS_TransferZoneWithWarning_Actor::IsPlayerOverlapped()
{
	return bIsPlayerOverlapped;
}

void ACS_TransferZoneWithWarning_Actor::BeginOverlapTriggerBox_Implementation(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
	if(const ACS_Character* Character = Cast<ACS_Character>(OtherActor))
	{
		if(!CurrentPlayerController && Character->GetPlayerController())
		{
			CurrentPlayerController = Character->GetPlayerController();
		}
		bIsPlayerOverlapped = true;
		CurrentPlayerController->TryShowLockedBoundary();
	}
}

void ACS_TransferZoneWithWarning_Actor::EndOverlapTriggerBox_Implementation(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(const ACS_Character* Character = Cast<ACS_Character>(OtherActor))
	{
		if(!CurrentPlayerController && Character->GetPlayerController())
		{
			CurrentPlayerController = Character->GetPlayerController();
		}
		bIsPlayerOverlapped = false;
		CurrentPlayerController->TryHideLockedBoundary();
		// TODO: Add a call to hide display info in player controller.
	}
}
