// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "Camera/CameraComponent.h"
#include <GameplayTagContainer.h>
#include "CS_Interactable_Cell_Actor.generated.h"

class UArrowComponent;
class UCapsuleComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCellRightClick, const FString&, apiId);

UCLASS()
class CORE_SIMULATOR_API ACS_Interactable_Cell_Actor : public ACS_Interactable_Actor
{
	GENERATED_BODY()
	
public:

	ACS_Interactable_Cell_Actor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CS_Interactable_Cell_Actor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> RootSceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CS_Interactable_Cell_Actor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> InteractActorCameraComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Params")
	FString InteractID = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Params")
	FString PointID = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Params")
	FGameplayTag ActiveTag;

	UPROPERTY(BlueprintAssignable, Category = "Cell Events")
	FOnCellRightClick OnCellRightClick;

	virtual bool Interact() override;
	virtual void StopInteract() override;
	virtual void bCheckInteract(ACS_PlayerController* PlayerController, UActorComponent* ActorComponent) override;
	virtual void RightClick_Implementation(UPrimitiveComponent* HitComp) override;
	virtual void TrySetViewID_Implementation(const FObjectViewID& StructObjectViewID);
	virtual void Teleport_Implementation(ACS_PlayerController* PlayerController, const FString& Key) override;
	virtual bool IsUseKey_Implementation(const FString& Key) const override;

	virtual void StartTeleport() override;

	UFUNCTION()
	void SetLockCameraMoveTemp(bool Lock);

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void CellChangeVisability(bool bIsVisability);

	UFUNCTION(BlueprintCallable, Category = "CS_Interactable_Cell_Actor")
	void ResetCells();

	void BindToCellSelected();

	UFUNCTION()
	void HandleCellSelected(bool bIsSelected, class UCS_Interaction_CellComp* CellRef);

	UFUNCTION(BlueprintCallable, Category = "CS_Interactable_Cell_Actor")
	const TArray<UCS_Interaction_CellComp*> GetSelectedCells() const;

	UPROPERTY(BlueprintReadOnly)
	//TArray<UCS_Interaction_CellComp*> SelectedCells;
	TMap<FString, UCS_Interaction_CellComp*> SelectedCells;

	UPROPERTY(BlueprintReadOnly, Category = "CS_Interactable_Cell_Actor")
	TMap<FString, UCS_Interaction_CellComp*> CellsMap;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_Interactable_Cell_Actor")
	TArray<TObjectPtr<USceneComponent>> ComponentsForDisable;

	UFUNCTION()
	void DisableOtherComponents(const bool bDisable);

	UFUNCTION()
	void HandleCellAction(const FString& apiId, const EPointAction& PointAction);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetViewForObject(const FObjectViewID& StructObjectViewID);

};
