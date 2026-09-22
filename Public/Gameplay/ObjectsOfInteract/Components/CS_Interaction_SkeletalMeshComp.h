// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "CS_Interact_Component_Interface.h"
#include "CS_Interaction_SkeletalMeshComp.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CORE_SIMULATOR_API UCS_Interaction_SkeletalMeshComp : public USkeletalMeshComponent, public ICS_Interact_Component_Interface
{
	GENERATED_BODY()

public:
	UCS_Interaction_SkeletalMeshComp(const FObjectInitializer& ObjectInitializer);

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Params")
	FString InteractID = "";

	FString Description = "";

	bool bInitDescription = false;

#pragma region InterfaceMethods

	virtual void MakeDescription_Implementation(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description) override;
	virtual void DisableComponent_Implementation() override;
	UFUNCTION(BlueprintCallable)
	virtual bool GetIsEnableComponent_Implementation() const override;

	virtual FString GetInteractID_Implementation() const override;
	virtual FString GetDescription_Implementation() const override;
	virtual bool GetInteractable_Implementation(EInteractMode CurrentMode) const override;
	virtual void SetIsUsedMeasureDevice_Implementation(bool Used) override;

	UFUNCTION(BlueprintCallable)
	virtual void EnableChangeOutline_Implementation(const bool bEnable) override;
	UFUNCTION(BlueprintCallable)
	virtual void SetOutline_Implementation(const bool bEnable) override;

#pragma endregion

protected:

	bool bIsComponentEnable = false;
	bool bIsUsedMeasureDevise = false;

	UPROPERTY()
	TObjectPtr<ACS_Interactable_Actor> Owner;

	UPROPERTY()
	bool bChangeOutline = true;

	virtual void BeginPlay() override;
	
};
