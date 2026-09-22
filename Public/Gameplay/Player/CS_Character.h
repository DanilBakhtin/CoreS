#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_Character.generated.h"

class USpringArmComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;

struct FInputActionValue;
class UCS_CheckingLoadingOfWorldPartitionChunks;
class ACS_Interactable_Actor;
class ACS_PlayerController;
class UCS_MeasureDevices_ActorComponent;
class ACS_LocationZone;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoomChangedSignature, int, ZoomValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPointingComponent, UActorComponent*, Component, FString, PointID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNonInteractComp);

UCLASS(config = Game)
class CORE_SIMULATOR_API ACS_Character : public ACharacter
{
	GENERATED_BODY()

public:
	ACS_Character();

#pragma region BaseComponents

	// Pawn SkeletalMesh for first person (like hands, etc.)
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> TraceForLocationComponent;

#pragma endregion

#pragma region InputActions
public:
	// Default mapping context for actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	// Move Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	// Look Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	// Crouch Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CrouchAction;

	// Sprint Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SprintAction;

	// Interact Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;

	// Zoom Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ZoomAction;
	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Input")
	bool GetIsCanCrouch() const;
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetIsCanCrouch(const bool bNewCanCrouch);

	UFUNCTION(BlueprintCallable, Category = "Input")
	bool GetIsCanSprint() const;
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetIsCanSprint(const bool bNewCanCrouch);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float CrouchedHalfHeight = 40.0f;

	// Default walk speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float WalkSpeed = 200;

	// Max added speed for sprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float SprintDeltaSpeed = 300;
	
	UPROPERTY(EditAnywhere, Category = "Zoom Camera")
	float ZoomMin = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Zoom Camera")
	float ZoomMax = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Zoom Camera")
	float ZoomInterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Zoom Camera")
	float ZoomSpeedPercent = 20.0f;

	UPROPERTY()
	float CurrentZoom = 1.0f;

	UPROPERTY()
	float TargetFOV;

	UPROPERTY()
	float BaseFOV;
	
	/** Вызывается при десвие передвижении */
	void Move(const FInputActionValue& Value);

	/** Вызывается при дейсвие поворота камеры*/
	void Look(const FInputActionValue& Value);

	/** Вызывается при старте приседания*/
	void StartCrouch();

	/** Вызывается при окончание приседания*/
	void StopCrouch();

	void Sprint(const FInputActionValue& Value);

	void ZoomCamera(const FInputActionValue& Value);

	void Zooming(float DeltaTime);

	bool bIsZooming = false;

	void Interact();

	void InitMovementComponent();

private:
	bool bIsCanCrouch = true;
	bool bIsCanSprint = true;
	
	UPROPERTY(EditAnywhere, Category = "Camera")
	float MouseSensitivityX = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MouseSensitivityY = 1.0f;
	
#pragma endregion
	
public:
	/** Компонент для активации измерительных приборов */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UCS_MeasureDevices_ActorComponent> MeasureDevices;

	// Диспетчер событий для изменения зума
	UPROPERTY(BlueprintAssignable, Category = "Zoom Camera")
	FOnZoomChangedSignature OnZoomChanged;

	UPROPERTY(BlueprintAssignable)
	FPointingComponent OnPointingComponent;

	UPROPERTY(BlueprintAssignable)
	FNonInteractComp OnNonInteractComponent;

	UFUNCTION(BlueprintCallable)
	void ZoomReset();

protected:
	virtual void BeginPlay() override;

public:
	//Зациклить появления луча для нахождения объекта для взаимодейсвия
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact Trace")
	bool bUseTrace = true;
	//Интенсивность с какой будет появлять лучь для поиска объекта взаимодейсвия
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact Trace")
	float TraceTimerPeriod = 0.1f;
	//Длина луча для нахождения объекта для взаимодейсвия
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact Trace")
	float TraceDistance = 500.f;
	//Debug показать лучь который ищет объект для взаимодейсвия
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact Trace")
	bool bShowTrace = false;
	//Debug показать информацию, с каким объектом для взаимодейстия пересекя лучь
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact Trace")
	bool bShowInteractObject;

	void StartUseMeasureDevice(TWeakObjectPtr<ACS_Interactable_Actor> InteractableActor, const FString& MeasureId, const TArray<FDeviceData>& PointsData) const;
	void StopUseMeasureDevice(TWeakObjectPtr<ACS_Interactable_Actor> InteractableActor) const;
	void UpdateMeasureDevice(const float& Result) const;

	int GetZoomPercent();

	void ClearHitActor();

protected:
	UFUNCTION()
	void AfterTeleport();

	UFUNCTION()
	void SetCameraLagSpeedToDefault();
	
	//Создаем лучь для нахождкния объекта для взаимодейсвия
	void MakeTrace();
	//Обработка трейса
	virtual void TraceProcessing(FHitResult OutHit);

	void MakeTraceForLocation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Interact Trace", meta=(DisplayName = "Trace Processing"))
	void K2_TraceProcessing(FHitResult OutHit);

	//Последний объект для взаимодейсвия с которым мы взаимодейсвовали
	UPROPERTY()
	TWeakObjectPtr<ACS_Interactable_Actor> LastInteractActor;

	TWeakObjectPtr<UActorComponent> LastInteractComponent;

	UPROPERTY()
	TObjectPtr<ACS_PlayerController> PlayerController;

	void HideDescription();

	UPROPERTY()
	TObjectPtr<ACS_LocationZone> LastLocationZone;

public:
	virtual void Tick(float DeltaTime) override;

	/** Getter Mesh1P  **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Getter FirstPersonCameraComponent **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UCS_MeasureDevices_ActorComponent* GetMeasureDevices() const;

	/** Метод вызывается во время OnPossess пешки*/
	void StartUsingPawn(APlayerController* Controller);

	ACS_PlayerController* GetPlayerController() const;
};
