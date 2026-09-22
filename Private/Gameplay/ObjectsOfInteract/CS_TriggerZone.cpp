// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ObjectsOfInteract/CS_TriggerZone.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Gameplay/Player/CS_Character.h"

ACS_TriggerZone::ACS_TriggerZone()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
}

void ACS_TriggerZone::BeginPlay()
{
	Super::BeginPlay();
	
	SubscribeToTriggers();
}

void ACS_TriggerZone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    TArray<UPrimitiveComponent*> PrimitiveComponents;
    GetComponents<UPrimitiveComponent>(PrimitiveComponents);

    for (UPrimitiveComponent* PrimComp : PrimitiveComponents)
    {
        if (PrimComp)
        {
            PrimComp->OnComponentBeginOverlap.RemoveAll(this);
            PrimComp->OnComponentEndOverlap.RemoveAll(this);
        }
    }

    Super::EndPlay(EndPlayReason);
}

void ACS_TriggerZone::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{   
    OnTriggerOverplap.Broadcast(OtherActor, true);

    if (Cast<ACS_Character>(OtherActor)) {

        if (TryGetPlayerController())
            TryGetPlayerController()->TryShowEndZone();
    }

    UE_LOG(LogTemp, Warning, TEXT("Begin Overlap: Component %s with Actor %s"),
        *OverlappedComponent->GetName(), *OtherActor->GetName());
}

void ACS_TriggerZone::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{   
    OnTriggerOverplap.Broadcast(OtherActor, false);

    if (Cast<ACS_Character>(OtherActor)) {
        if (TryGetPlayerController())
            TryGetPlayerController()->TryCloseEndZone();
    }

    UE_LOG(LogTemp, Warning, TEXT("End Overlap: Component %s with Actor %s"),
        *OverlappedComponent->GetName(), *OtherActor->GetName());
}

void ACS_TriggerZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACS_TriggerZone::SubscribeToTriggers()
{
    TArray<UPrimitiveComponent*> PrimitiveComponents;
    GetComponents<UPrimitiveComponent>(PrimitiveComponents);

    for (UPrimitiveComponent* PrimComp : PrimitiveComponents)
    {
        if (PrimComp && PrimComp != RootSceneComponent)
        {
            if (PrimComp->IsA<UBoxComponent>() || PrimComp->IsA<USphereComponent>() ||
                PrimComp->IsA<UCapsuleComponent>() || PrimComp->IsA<UStaticMeshComponent>())
            {
                PrimComp->OnComponentBeginOverlap.AddDynamic(this, &ACS_TriggerZone::OnTriggerBeginOverlap);
                PrimComp->OnComponentEndOverlap.AddDynamic(this, &ACS_TriggerZone::OnTriggerEndOverlap);
                PrimComp->SetGenerateOverlapEvents(true);
            }
        }
    }
}

ACS_PlayerController* ACS_TriggerZone::TryGetPlayerController() {

    if (IsValid(CurrentPlayerController))
        return CurrentPlayerController;
    else {

        ACS_PlayerController* TempPlayerController = Cast<ACS_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

        if (IsValid(TempPlayerController)) {
            CurrentPlayerController = TempPlayerController;
            return CurrentPlayerController;
        }
    }

    return nullptr;
}

