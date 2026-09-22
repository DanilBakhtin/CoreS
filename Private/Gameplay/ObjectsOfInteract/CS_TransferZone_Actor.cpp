// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ObjectsOfInteract/CS_TransferZone_Actor.h"

#include "Components/BoxComponent.h"
#include "Gameplay/Player/CS_Character.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

ACS_TransferZone_Actor::ACS_TransferZone_Actor()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);
	//BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACS_TransferZone_Actor::BeginOverLap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ACS_TransferZone_Actor::EndOverLap);

	Zone_One = CreateDefaultSubobject<UArrowComponent>(TEXT("Zone_One"));
	Zone_One->SetupAttachment(BoxComponent);
	Zone_Two = CreateDefaultSubobject<UArrowComponent>(TEXT("Zone_Two"));
	Zone_Two->SetupAttachment(BoxComponent);
	
}

void ACS_TransferZone_Actor::BeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ACS_TransferZone_Actor::EndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(ACS_Character* Character = Cast<ACS_Character>(OtherActor))
	{
		if(!IsValid(CurrentPlayerController))
			CurrentPlayerController = Cast<ACS_PlayerController>(Character->GetController());

		if(!IsValid(CurrentPlayerController))
			return;
		
		const FVector CharacterLocation = Character->GetActorLocation();
		const FVector Zone_One_Location = Zone_One->GetComponentLocation();
		const FVector Zone_Two_Location = Zone_Two->GetComponentLocation();

		if(UKismetMathLibrary::Vector_Distance(CharacterLocation, Zone_One_Location) < UKismetMathLibrary::Vector_Distance(CharacterLocation, Zone_Two_Location))
		{
			CurrentPlayerController->EndTween(EndTween_Zone_One_Id);
		}
		else
		{
			CurrentPlayerController->EndTween(EndTween_Zone_Two_Id);
		}
	}	
}
