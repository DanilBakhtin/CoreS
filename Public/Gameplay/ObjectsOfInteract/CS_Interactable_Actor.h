// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "CS_Interact_Interface.h"
#include "GameFramework/Actor.h"
#include "CS_Interactable_Actor.generated.h"

class ACS_Character;

USTRUCT()
struct FCustomDepthComponents
{
	GENERATED_BODY()

	bool bCustomDepth = false;

	UPROPERTY()
	TArray<UStaticMeshComponent*> StaticMeshComponents;
};

UCLASS()
class CORE_SIMULATOR_API ACS_Interactable_Actor : public AActor, public ICS_Interact_Interface
{
	GENERATED_BODY()
	

#pragma region ICS_Interact_Interface

public:

	virtual bool IsUseKey_Implementation(const FString& Key) const override;
	virtual void ActivateObject_Implementation(const FString& Key, const FString& DescriptionObject = "") override;
	virtual void ActivateObjectReactMethod_Implementation(const FString& Key, const FVariables& Variable) override;
	virtual void ActivatePoint_Implementation(const FString& Key, const FParametersObject& Parameters) override;
	virtual void Teleport_Implementation(ACS_PlayerController* PlayerController, const FString& Key) override;
	virtual void DisableObject_Implementation() override;
	virtual void DisablePoint_Implementation() override;
	virtual bool TryToSubTeleport_Implementation(const FString& Key) override;

	//Checking whether we can interact with the object
	virtual void bCheckInteract(ACS_PlayerController* PlayerController, UActorComponent* ActorComponent) override;

	virtual bool Interact() override;

	virtual void StartBlinkingObject(const FString& Key) override;
	virtual void StopBlinkingObject(const FString& Key) override;

#pragma endregion

#pragma region Zoom
public:

	virtual void Zoom(float direction) override {};
	virtual int GetZoomPercent() override { return 100; }
	virtual void ResetZoom() override {};

#pragma endregion

#pragma region FieldsES

protected:

	//List of IDs used in the current actor
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_Interactable_Actor")
	TArray<FString> ActionObjectKeys;
	//List of methods for invoking the ES command
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, bool> MapMethodsCommandES;
	UPROPERTY()
	TMap<FString, UActorComponent*> InteractComponents;

	UPROPERTY()
	TMap<FString, USceneComponent*> SubTeleportComponents;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_Interactable_Actor")
	TArray<FString> MethodNames;

#pragma endregion

public:	

	ACS_Interactable_Actor();

	virtual void Tick(float DeltaTime) override;

public:
	
	//Прекратить прерку объекта
	void StopCheckInteract();
	
	//Прекратить взаимодейсвие с объектом
	virtual void StopInteract();
	//Включить подсветку
	UFUNCTION()
	virtual void StartShowOutline();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "StartShowOutline"))
	void K2_StartShowOutline();
	//Выключить поветку объекта
	UFUNCTION()
	virtual void StopShowOutlineEvent();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "StopShowOutlineEvent"))
	void K2_StopShowOutlineEvent();
	
	//Метод срабатывает в блупренте при старте взаимодейсвия с объектом
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void K2_StartInteract();
	//Метод срабатывает в блуприте при окончании взаимдейсвия с объектом
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void K2_StopInteract();

	//Метод срабатывает в блупренте при старте взаимодейсвия с актором
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void K2_StartInteractActor(const FString& InteractKey);
	//Метод срабатывает в блуприте при окончании взаимдейсвия с актором
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void K2_StopInteractActor();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void K2_StartBlinkingObject(const FString& Key);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void K2_StopBlinkingObject(const FString& Key);
	
	void MakeDescription(const FString& Key, const FString& DescriptionObject = "");
	
	//Телепоритоваться к объекту
	virtual void StartTeleport();

	//Запуск кастомной реакции
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StartReact(const FString& MethodName, const FVariables& Variable);
	virtual void StartReact_Implementation(const FString& MethodName, const FVariables& Variable);

	//Отправка кастомного события
	UFUNCTION(BlueprintCallable)
	void SendCustomAction(const FString& ObjectId, const FString& CustomString);

	UFUNCTION(BlueprintCallable)
	bool GetActorInteractable();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetCanInteractObject();

	UFUNCTION(BlueprintCallable, DisplayName = "Get Current Player Controller")
	ACS_PlayerController* GetCurrentPlayerController() const;

protected:
		
	virtual void BeginPlay() override;

	bool bCanInteractObject = false;
	bool bInteracted = false;
	FString CurrentActionKey;

	UPROPERTY()
	TObjectPtr<ACS_PlayerController> CurrentPlayerController;

	FCustomDepthComponents CustomDepthComponents;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Params")
	FString LocationID = "";

	UPROPERTY(EditAnywhere, Category = "Interaction Params")
	bool bBlueprintOnlyMethodsES = false;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, bool> BlinkingIDs;

};
