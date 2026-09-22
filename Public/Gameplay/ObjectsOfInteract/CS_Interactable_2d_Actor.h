#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_Interactable_2d_Actor.generated.h"

class UCameraComponent;
class USceneComponent;
class UActorComponent;
class UCameraComponent;
class UCS_Interaction_StaticMeshComp;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoomUpdated, float, ZoomAlpha);

UCLASS()

class CORE_SIMULATOR_API ACS_Interactable_2d_Actor : public ACS_Interactable_Actor
{
	GENERATED_BODY()

public:
	ACS_Interactable_2d_Actor();

	virtual void Tick(float DeltaTime) override;
	virtual bool Interact() override;
	virtual void StopInteract() override;
	virtual void CameraPanning(const FVector2D& Direction);
	virtual void Zoom(float direction) override;
	virtual int GetZoomPercent() override;
	virtual void ResetZoom() override;

	virtual void Teleport_Implementation(ACS_PlayerController* PlayerController, const FString& Key) override;
	virtual void StartTeleport() override;
	virtual bool IsUseKey_Implementation(const FString& Key) const override;

	void MoveCamera(const FVector2D& Direction);

	UFUNCTION()
	void SetLockCameraMove(bool Lock);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void CalcStartLockCamera();

	void UpdateLockCamera(bool bUpdateMoveArrows = false);
	void UpdateArrows();
	void CheckCameraMoveFromViewport();

	UFUNCTION(BlueprintCallable)
	void SetPauseCameraMove(bool bPause);

#pragma region COMPONENTS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CS_Interactable_2d_Actor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CS_Interactable_2d_Actor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> InteractActorCameraComponent;


#pragma endregion COMPONENTS

#pragma region CAMERA_SETTINGS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Params|Camera")
	bool bUseUpDownMoveZone = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Params|Camera")
	bool bUseLeftRightMoveZone = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Params|Camera")
	float MoveSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction Params|Camera")
	float MouseMoveSensitivity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction Params|Camera")
	FCameraLock CameraLock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction Params|Camera")
	FCameraLock CameraLockWithMaxZoom;
#pragma endregion CAMERA_SETTINGS

#pragma region ZOOM_SETTINGS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Params|Camera|Zoom")
	float ZoomDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Params|Camera|Zoom")
	float ZoomSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Params|Camera|Zoom")
	float LerpZoomSpeed = 10.0f;
#pragma endregion ZOOM_SETTINGS

	UPROPERTY(BlueprintReadOnly, Category = "CS_Interactable_2d_Actor")
	FRotator StartCameraRotator;

private:
#pragma region CAMERA_STATE
	UPROPERTY()
	bool bIsLockedCameraMove = false;

	UPROPERTY()
	FVector StartCameraPosition;

	UPROPERTY()
	FVector CurrentStartCameraPosition;

	UPROPERTY()
	float CurrentRightOffset = 0.0f;

	UPROPERTY()
	float CurrentTopOffset = 0.0f;
#pragma endregion CAMERA_STATE

#pragma region ZOOM_STATE
	UPROPERTY()
	float ZoomLevel = 1.0f;

	UPROPERTY()
	float TargetZoomDistance;

	UPROPERTY()
	float CurrentZoomDistance;

	UPROPERTY()
	float MinZoomDistance;

	UPROPERTY()
	float MaxZoomDistance;

	UPROPERTY(BlueprintAssignable, Category = "CS_Interactable_2d_Actor")
	FOnZoomUpdated OnZoomUpdated;
#pragma endregion ZOOM_STATE

	#pragma region CAMERA_LOCKS
		UPROPERTY()
		FCameraLock CurrentCameraLock;

		UPROPERTY()
		FCameraLock CameraLockWithMinZoom;

		UPROPERTY()
		FCameraLock StartCameraLockWithMaxZoom;

		UPROPERTY()
		FCameraLock StartCameraLock;
	#pragma endregion CAMERA_LOCKS

	bool bPauseMovingCamera = false;

#pragma region INITIAL_VALUES
	UPROPERTY()
	float StartZoomDistance;
#pragma endregion INITIAL_VALUES
};