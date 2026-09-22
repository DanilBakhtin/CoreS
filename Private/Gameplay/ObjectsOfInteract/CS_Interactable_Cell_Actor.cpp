// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ObjectsOfInteract/CS_Interactable_Cell_Actor.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_Interact_Component_Interface.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_Interaction_CellComp.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Camera/CameraComponent.h"

ACS_Interactable_Cell_Actor::ACS_Interactable_Cell_Actor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	InteractActorCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("InteractActorCameraComponent"));
	InteractActorCameraComponent->SetupAttachment(RootSceneComponent);
}

void ACS_Interactable_Cell_Actor::BeginPlay()
{
    Super::BeginPlay();

	BindToCellSelected();

    ActionObjectKeys.Add(PointID);

    //CellChangeVisability(false);

    GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Cells components: %d"), InteractComponents.Num()));

    DisableOtherComponents(true);
}

void ACS_Interactable_Cell_Actor::CellChangeVisability(bool bIsVisability)
{
    if (RootComponent)
    {
        TArray<USceneComponent*> ChildrenComponents;

        RootComponent->GetChildrenComponents(true, ChildrenComponents);

        for (USceneComponent* Child : ChildrenComponents)
        {
            if (UCS_Interaction_CellComp* Cell = Cast<UCS_Interaction_CellComp>(Child))
            {   
                if (bIsVisability)
                    Cell->SetVisibility(true, true);
                else
                    Cell->Deactivate();

                Cell->SetVisibility(bIsVisability, true);
            }
        }
    }
}

void ACS_Interactable_Cell_Actor::ResetCells()
{
    if (RootComponent)
    {
        TArray<USceneComponent*> ChildrenComponents;

        RootComponent->GetChildrenComponents(true, ChildrenComponents);

        for (USceneComponent* Child : ChildrenComponents)
        {
            if (UCS_Interaction_CellComp* Cell = Cast<UCS_Interaction_CellComp>(Child))
            {
                Cell->ResetCell();
                SelectedCells.Empty();
            }
        }
    }
}

void ACS_Interactable_Cell_Actor::BindToCellSelected()
{
    if (RootComponent)
    {
        TArray<USceneComponent*> ChildrenComponents;

        RootComponent->GetChildrenComponents(true, ChildrenComponents);

        for (USceneComponent* Child : ChildrenComponents)
        {
            if (UCS_Interaction_CellComp* Cell = Cast<UCS_Interaction_CellComp>(Child))
            {
				Cell->OnCellSelected.AddDynamic(this, &ACS_Interactable_Cell_Actor::HandleCellSelected);
                Cell->OnCellPointAction.AddDynamic(this, &ACS_Interactable_Cell_Actor::HandleCellAction);
                CellsMap.Add(Cell->InteractID, Cell);
            }
        }
    }
}

void ACS_Interactable_Cell_Actor::HandleCellSelected(bool bIsSelected, UCS_Interaction_CellComp* CellRef)
{
    if (CellRef->Implements<UCS_Interact_Component_Interface>())
    {
        FString InteractCellID = ICS_Interact_Component_Interface::Execute_GetInteractID(CellRef);

        if (bIsSelected)
        {

            SelectedCells.Add(InteractCellID, CellRef);
            
        }
        else
        {
            SelectedCells.Remove(InteractCellID);
        }
    }
}

const TArray<UCS_Interaction_CellComp*> ACS_Interactable_Cell_Actor::GetSelectedCells() const
{
    TArray<UCS_Interaction_CellComp*> Result;
    SelectedCells.GenerateValueArray(Result);
    return Result;
}

void ACS_Interactable_Cell_Actor::DisableOtherComponents(const bool bDisable)
{

    for (int i = 0; i < ComponentsForDisable.Num(); i++)
        ComponentsForDisable[i]->SetVisibility(!bDisable, true);
}

void ACS_Interactable_Cell_Actor::HandleCellAction(const FString& apiId, const EPointAction& PointAction)
{
    if (!CurrentPlayerController)
    {
        UE_LOG(LogTemp, Display, TEXT("%s: Player controller is invalid."), *GetActorNameOrLabel());
        return;
    }
    CurrentPlayerController->OnPointAction(apiId, PointAction);
}

bool ACS_Interactable_Cell_Actor::Interact()
{	
	if (!CurrentPlayerController)
		return false;

    if (!bInteracted) {

        CurrentPlayerController->StartInteractWithObject(false, false, [&](bool Lock) { SetLockCameraMoveTemp(Lock); });
        CurrentPlayerController->SetViewTargetWithBlend(this, 0.2);
        CurrentPlayerController->SetInteractableActor(this);
        CurrentPlayerController->bEnableMouseOverEvents = true;
        CurrentPlayerController->bEnableClickEvents = true;
        //CellChangeVisability(true);
        DisableOtherComponents(false);
        
        K2_StartInteractActor(FString());

        bInteracted = true;
        return true;
    }
    else
        StopInteract();

    return false;
}

void ACS_Interactable_Cell_Actor::StopInteract()
{   
    CurrentPlayerController->SetViewTargetWithBlend(CurrentPlayerController->GetPawn(), 0.2);
    CurrentPlayerController->SetInteractableActor(nullptr);
	CurrentPlayerController->bEnableMouseOverEvents = false;
	CurrentPlayerController->bEnableClickEvents = false;
    //CellChangeVisability(false);
    DisableOtherComponents(true);
    ResetCells();

    K2_StopInteractActor();

    bInteracted = false;
}

void ACS_Interactable_Cell_Actor::bCheckInteract(ACS_PlayerController* PlayerController, UActorComponent* ActorComponent)
{   
    //if (bCanInteractObject && !bInteracted)
    CurrentPlayerController = PlayerController;
}

void ACS_Interactable_Cell_Actor::RightClick_Implementation(UPrimitiveComponent* HitComp)
{
    if (RootComponent)
    {
        TArray<USceneComponent*> ChildrenComponents;

        RootComponent->GetChildrenComponents(true, ChildrenComponents);

        for (USceneComponent* Child : ChildrenComponents)
        {
            if (UCS_Interaction_CellComp* Cell = Cast<UCS_Interaction_CellComp>(Child))
            {
                if (Cell->GetBoxCollision() == HitComp) {

                    if (Cell->Implements<UCS_Interact_Component_Interface>())
                    {
                        OnCellRightClick.Broadcast(ICS_Interact_Component_Interface::Execute_GetInteractID(Cell));
                        ICS_Interact_Component_Interface::Execute_RightClickInteract(Cell);
                    }
                    return;
                }
            }
        }
    }
}

void ACS_Interactable_Cell_Actor::TrySetViewID_Implementation(const FObjectViewID& StructObjectViewID) {

    Super::ActivateObject_Implementation(StructObjectViewID.PointID, StructObjectViewID.DecsID);

    SetViewForObject(StructObjectViewID);
}

void ACS_Interactable_Cell_Actor::Teleport_Implementation(ACS_PlayerController* PlayerController, const FString& Key)
{
    if (IsValid(PlayerController))
    {
        CurrentActionKey = Key;
        bCanInteractObject = true;
        CurrentPlayerController = PlayerController;
        CurrentPlayerController->UpdateCurrentCellMode(ActiveTag, InteractID);
        StartTeleport();
    }
}

bool ACS_Interactable_Cell_Actor::IsUseKey_Implementation(const FString& Key) const
{
    for (auto& SKey : ActionObjectKeys)
        if (SKey == Key)
            return true;

    if (MapMethodsCommandES.Contains(Key))
        return true;

    if (InteractID == Key)
        return true;

    return false;
}

void ACS_Interactable_Cell_Actor::StartTeleport()
{
    Interact();
}

void ACS_Interactable_Cell_Actor::SetLockCameraMoveTemp(bool Lock)
{
}
