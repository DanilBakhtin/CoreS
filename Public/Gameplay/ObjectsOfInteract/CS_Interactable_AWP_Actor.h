#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "CS_Interactable_AWP_Actor.generated.h"

enum class EPointAction : uint8;
class UCS_AWP_Widget;
class UWidgetComponent;
class UTextureRenderTarget2D;

/**
 * Automated workplace actor (AWP) is a widget-oriented actor that
 * implements the OS interface and the program installed in it.
 */
UCLASS()
class CORE_SIMULATOR_API ACS_Interactable_AWP_Actor : public ACS_Interactable_Actor
{
	GENERATED_BODY()
public:
	ACS_Interactable_AWP_Actor();

	virtual void BeginPlay() override;
	virtual bool Interact() override;
	virtual void StopInteract() override;
	virtual void ActivateObject_Implementation(const FString& Key, const FString& DescriptionObject = "") override;
	virtual void DisableObject_Implementation() override;
	virtual void TrySetViewID_Implementation(const FObjectViewID& StructObjectViewID);
	virtual bool IsUseKey_Implementation(const FString& Key) const override;
	virtual void Teleport_Implementation(ACS_PlayerController* PlayerController, const FString& Key) override;
	virtual void StartTeleport() override;
	
	UPROPERTY()
	TObjectPtr<USceneComponent> RootSceneComponent;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CS_AWP)
	TObjectPtr<UCameraComponent> ActorCameraComponent;
	
protected:
	/** Class for AWP widget. Use for create widget in AWP instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CS_AWP)
	TSubclassOf<UCS_AWP_Widget> WidgetClass;
	
	/** Widget for AWP. Delegate buttons reacts to PC. Create in BeginPlay. */
	UPROPERTY(BlueprintReadOnly, Category = CS_AWP)
	TObjectPtr<UCS_AWP_Widget> AWP_Widget;

	UPROPERTY(BlueprintReadOnly, Category = CS_AWP)
	UWidgetComponent* WidgetComponent;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "UpdatedWidgetAWP"))
	void K2_UpdatedWidgetAWP(UTextureRenderTarget2D* WidgetRenderTarget);

private:
	/** Function call where AWP widget triggered by buttons delegate. */
	UFUNCTION()
	void HandlePointAction(const FString& apiId, const EPointAction& PointAction);

	UFUNCTION()
	void InitWidgetsArray();
};
