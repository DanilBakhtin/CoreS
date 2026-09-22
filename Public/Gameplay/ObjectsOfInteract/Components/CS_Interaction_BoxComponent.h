
#pragma once

#include "CoreMinimal.h"
#include "CS_Interact_Component_Interface.h"
#include "Components/BoxComponent.h"
#include "CS_Interaction_BoxComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CORE_SIMULATOR_API UCS_Interaction_BoxComponent : public UBoxComponent, public ICS_Interact_Component_Interface
{
	GENERATED_BODY()

public:

	UCS_Interaction_BoxComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Params")
	FString InteractID = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Params")
	FString EndTwinID = "";

	FString Description = "";

	bool bInitDescription = false;

	virtual void MakeDescription_Implementation(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description) override;
	virtual void DisableComponent_Implementation() override;
	virtual FString GetInteractID_Implementation() const override;
	virtual FString GetDescription_Implementation() const override;
	virtual bool GetIsEnableComponent_Implementation() const override;

protected:

	bool bIsComponentEnable = false;

	UPROPERTY()
	TObjectPtr<ACS_Interactable_Actor> Owner;

	UFUNCTION()
	void BeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginPlay() override;


};
