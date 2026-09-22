// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Math/MathFwd.h"
#include "CS_Interact_Interface.generated.h"

class ACS_Character;
class ACS_PlayerController;

UINTERFACE(MinimalAPI)
class UCS_Interact_Interface : public UInterface
{
	GENERATED_BODY()
};

class CORE_SIMULATOR_API ICS_Interact_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void bCheckInteract(ACS_PlayerController* PlayerController, UActorComponent* ActorComponent);
	virtual bool Interact();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool UseKeys(const TArray<FString>& Keys) const;
	virtual bool UseKeys_Implementation(const TArray<FString>& Keys) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool IsUseKey(const FString& Key);
	virtual bool IsUseKey_Implementation(const FString& Key) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void ActivateObject(const FString& Key, const FString& DescriptionObject = "" );
	virtual void ActivateObject_Implementation(const FString& Key, const FString& DescriptionObject = "");

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void ActivateObjectReactMethod(const FString& Key, const FVariables& Variable);
	virtual void ActivateObjectReactMethod_Implementation(const FString& Key, const FVariables& Variable);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void ActivatePoint(const FString& Key, const FParametersObject& Parameters);
	virtual void ActivatePoint_Implementation(const FString& Key, const FParametersObject& Parameters);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void UpdateObjectAndPoint(const FString& Key, const FString& DescriptionObject = "" );
	virtual void UpdateObjectAndPoint_Implementation(const FString& Key, const FString& DescriptionObject = "");
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void Teleport(ACS_PlayerController* PlayerController, const FString& Key);
	virtual void Teleport_Implementation(ACS_PlayerController* PlayerController, const FString& Key);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void DisableObject();
	virtual void DisableObject_Implementation();


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void TrySetViewID(const FObjectViewID& StructObjectViewID);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void RightClick(UPrimitiveComponent* HitComp);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void DisablePoint();
	virtual void DisablePoint_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool TryToSubTeleport(const FString& Key);
	virtual bool TryToSubTeleport_Implementation(const FString& Key);

	virtual void Zoom(float direction) {};
	virtual int GetZoomPercent() { return 100; }
	virtual void ResetZoom() {};

	virtual void CameraPanning(const FVector2D& Direction) {};

	virtual void StartBlinkingObject(const FString& Key);
	virtual void StopBlinkingObject(const FString& Key);

};
