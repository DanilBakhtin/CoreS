
#pragma once

#include "CoreMinimal.h"
#include "CS_Interact_Component_Interface.h"
#include "Components/StaticMeshComponent.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_MeasurePoint.generated.h"

class UMaterialInstanceDynamic;

UCLASS()
class CORE_SIMULATOR_API UCS_MeasurePoint : public UStaticMeshComponent, public ICS_Interact_Component_Interface
{
	GENERATED_BODY()

public:

	UCS_MeasurePoint(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_MeasurePoint")
	FString InteractID = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_MeasurePoint")
	FTransform PositionMeasure = FTransform();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_MeasurePoint")
	FLinearColor ColorUnhovered = FLinearColor(0.590619f, 0.590619f, 0.590619f, 1.f);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_MeasurePoint")
	FLinearColor ColorHovered = FLinearColor(0.0865f, 0.47932f, 0.309469f, 1.f);

	FString Description = "";

	bool bInitDescription = false;

	virtual void MakeDescription_Implementation(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description) override;
	virtual void DisableComponent_Implementation() override;
	virtual FString GetInteractID_Implementation() const override;
	virtual FString GetDescription_Implementation() const override;
	virtual bool GetInteractable_Implementation(EInteractMode CurrentMode) const override;
	virtual bool GetIsEnableComponent_Implementation() const override;
	virtual void LeftClickInteract_Implementation() override;
	virtual ETypeInteractPoint GetTypePoint_Implementation() override;

	UFUNCTION(BlueprintCallable)
	virtual void SetOutline_Implementation(const bool bEnable) override;

	virtual void SetIsUsedMeasureDevice_Implementation(bool Used) override;

	void SetClickOnMeasuringPointFun(TFunction<void(const FString& InteractID, const FTransform& PositionMeasure)> Fun);

protected:

	bool bIsComponentEnable = false;

	bool bIsUsedMeasureDevice = false;

	UPROPERTY()
	TObjectPtr<ACS_Interactable_Actor> Owner;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> PointMaterial;

	TFunction<void(const FString& InteractID, const FTransform& PositionMeasure)> ClickOnMeasuringPointFun;
	virtual void BeginPlay() override;

};
