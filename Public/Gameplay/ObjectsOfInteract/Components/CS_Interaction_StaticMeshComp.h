
#pragma once

#include "CoreMinimal.h"
#include "CS_Interact_Component_Interface.h"
#include "Components/StaticMeshComponent.h"
#include "CS_Interaction_StaticMeshComp.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CORE_SIMULATOR_API UCS_Interaction_StaticMeshComp : public UStaticMeshComponent, public ICS_Interact_Component_Interface
{
	GENERATED_BODY()

public:

	UCS_Interaction_StaticMeshComp(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Params")
	FString InteractID = "";

	FString Description = "";

	bool bInitDescription = false;

#pragma region InterfaceMethods

	virtual void MakeDescription_Implementation(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description) override;
	virtual void DisableComponent_Implementation() override;
	virtual FString GetInteractID_Implementation() const override;
	virtual FString GetDescription_Implementation() const override;
	virtual bool GetIsEnableComponent_Implementation() const override;
	virtual bool GetInteractable_Implementation(EInteractMode CurrentMode) const override;
	virtual void SetIsUsedMeasureDevice_Implementation(bool Used) override;
	UFUNCTION(BlueprintCallable)
	virtual void EnableChangeOutline_Implementation(const bool bEnable) override;
	UFUNCTION(BlueprintCallable)
	virtual void SetOutline_Implementation(const bool bEnable) override;

#pragma endregion

	UFUNCTION(BlueprintCallable)
	void ActiveStaticMeshComp(bool bEnable);

	UFUNCTION(BlueprintCallable)
	void SetDefaultCustomStencil(int StencilValue);

	UFUNCTION(BlueprintCallable)
	void SetPointingCustomStencil(int StencilValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetAOSEnable();

	//Temp for truba
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Params")
	bool bActiveChangeStencil = false;

protected:

	bool bIsComponentEnable = false;
	bool bIsUsedMeasureDevise = false;

	UPROPERTY()
	TObjectPtr<ACS_Interactable_Actor> Owner;

	UPROPERTY()
	bool bChangeOutline = true;

	UPROPERTY()
	bool bActive = true;

	virtual void BeginPlay() override;

	UPROPERTY()
	int DefaultStencilValue = 0;

	UPROPERTY()
	int PointingStencilValue = 0;

};
