#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_Interact_Component_Interface.generated.h"

class ACS_Interactable_Actor;
class ACS_PlayerController;

UINTERFACE(Blueprintable, MinimalAPI)
class UCS_Interact_Component_Interface : public UInterface
{
	GENERATED_BODY()
};


class CORE_SIMULATOR_API ICS_Interact_Component_Interface
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact_Component_Interface")
	void MakeDescription(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description);
	virtual void MakeDescription_Implementation(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact_Component_Interface")
	void DisableComponent();
	virtual void DisableComponent_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact_Component_Interface")
	FString GetInteractID() const;
	virtual FString GetInteractID_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact_Component_Interface")
	FString GetDescription() const;
	virtual FString GetDescription_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact_Component_Interface")
	bool GetIsEnableComponent() const;
	virtual bool GetIsEnableComponent_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact_Component_Interface")
	bool GetInteractable(EInteractMode CurrentMode) const;
	virtual bool GetInteractable_Implementation(EInteractMode CurrentMode) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact_Component_Interface")
	void SetIsUsedMeasureDevice(bool Used);
	virtual void SetIsUsedMeasureDevice_Implementation(bool Used);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact_Component_Interface")
	void EnableChangeOutline(const bool bEnable);
	virtual void EnableChangeOutline_Implementation(const bool bEnable);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact_Component_Interface")
	void SetOutline(const bool bEnable);
	virtual void SetOutline_Implementation(const bool bEnable);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact_Component_Interface")
	void RightClickInteract();
	virtual void RightClickInteract_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact_Component_Interface")
	void LeftClickInteract();
	virtual void LeftClickInteract_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact_Component_Interface")
	ETypeInteractPoint GetTypePoint();
	virtual ETypeInteractPoint GetTypePoint_Implementation();
};

