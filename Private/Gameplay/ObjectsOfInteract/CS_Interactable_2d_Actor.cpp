#include "Gameplay/ObjectsOfInteract/CS_Interactable_2d_Actor.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_Interact_Component_Interface.h"
#include "Camera/CameraComponent.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Math/UnrealMath.h"

ACS_Interactable_2d_Actor::ACS_Interactable_2d_Actor()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	InteractActorCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("InteractActorCameraComponent"));
	InteractActorCameraComponent->SetupAttachment(RootSceneComponent);
}

void ACS_Interactable_2d_Actor::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = false;

	// Initialize camera locks and positions
	StartCameraLockWithMaxZoom = CameraLockWithMaxZoom;
	StartCameraLock = CameraLock;
	StartZoomDistance = ZoomDistance;

	StartCameraPosition = InteractActorCameraComponent->GetRelativeLocation();
	CurrentStartCameraPosition = StartCameraPosition;
	StartCameraRotator = InteractActorCameraComponent->GetComponentRotation();

	// Initialize zoom parameters
	MinZoomDistance = CurrentStartCameraPosition.X;
	TargetZoomDistance = MinZoomDistance;
	CurrentZoomDistance = MinZoomDistance;
	MaxZoomDistance = CurrentStartCameraPosition.X + ZoomDistance;

	CalcStartLockCamera();
}

void ACS_Interactable_2d_Actor::CalcStartLockCamera()
{
	// Configure horizontal movement locks
	if (!bUseLeftRightMoveZone)
	{
		CameraLockWithMinZoom.ViewportRightLock = 0.0f;
		CameraLockWithMinZoom.ViewportLeftLock = 0.0f;
	}
	else
	{
		CameraLockWithMinZoom.ViewportRightLock = CameraLock.ViewportRightLock;
		CameraLockWithMinZoom.ViewportLeftLock = CameraLock.ViewportLeftLock;
	}

	// Configure vertical movement locks
	if (!bUseUpDownMoveZone)
	{
		CameraLockWithMinZoom.ViewportTopLock = 0.0f;
		CameraLockWithMinZoom.ViewportDownLock = 0.0f;
	}
	else
	{
		CameraLockWithMinZoom.ViewportTopLock = CameraLock.ViewportTopLock;
		CameraLockWithMinZoom.ViewportDownLock = CameraLock.ViewportDownLock;
	}

	UpdateLockCamera();
}

void ACS_Interactable_2d_Actor::Zoom(float direction)
{
	if (bPauseMovingCamera) return;

	if (direction > 0.0f)
		TargetZoomDistance = FMath::Clamp(TargetZoomDistance + ZoomSpeed, MinZoomDistance, MaxZoomDistance);
	else if (direction < 0.0f)
		TargetZoomDistance = FMath::Clamp(TargetZoomDistance - ZoomSpeed, MinZoomDistance, MaxZoomDistance);
	else
		return;

	//OnZoomUpdated.Broadcast((TargetZoomDistance - MinZoomDistance) / (MaxZoomDistance - MinZoomDistance));

	UpdateLockCamera(true);
}

int ACS_Interactable_2d_Actor::GetZoomPercent()
{
	if (ZoomDistance == 0.0f)
	{
		return 100;
	}
	else
	{
		float normalPercent = (TargetZoomDistance - MinZoomDistance) / (MaxZoomDistance - MinZoomDistance) + 1.0f;
		return FMath::RoundToInt(normalPercent * 100.0f);
	}
}

void ACS_Interactable_2d_Actor::ResetZoom()
{
	TargetZoomDistance = FMath::Clamp(MinZoomDistance, MinZoomDistance, MaxZoomDistance);

	UpdateLockCamera(true);
}

void ACS_Interactable_2d_Actor::Teleport_Implementation(ACS_PlayerController* PlayerController, const FString& Key)
{
	if (IsValid(PlayerController))
	{
		CurrentPlayerController = PlayerController;

		CurrentActionKey = Key;
		bCanInteractObject = true;
		StartTeleport();
	}
}

void ACS_Interactable_2d_Actor::StartTeleport()
{
	Interact();
}

bool ACS_Interactable_2d_Actor::IsUseKey_Implementation(const FString& Key) const
{
	for (auto& SKey : ActionObjectKeys)
		if (SKey == Key)
			return true;

	if (MapMethodsCommandES.Contains(Key))
		return true;

	if (LocationID == Key)
		return true;

	return false;
}

void ACS_Interactable_2d_Actor::UpdateLockCamera(bool bUpdateMoveArrows)
{
	CurrentCameraLock.ViewportRightLock
		= FMath::Lerp(CameraLockWithMinZoom.ViewportRightLock, CameraLockWithMaxZoom.ViewportRightLock, ZoomLevel - 1.0f);

	CurrentCameraLock.ViewportLeftLock
		= FMath::Lerp(CameraLockWithMinZoom.ViewportLeftLock, CameraLockWithMaxZoom.ViewportLeftLock, ZoomLevel - 1.0f);

	CurrentCameraLock.ViewportTopLock
		= FMath::Lerp(CameraLockWithMinZoom.ViewportTopLock, CameraLockWithMaxZoom.ViewportTopLock, ZoomLevel - 1.0f);

	CurrentCameraLock.ViewportDownLock
		= FMath::Lerp(CameraLockWithMinZoom.ViewportDownLock, CameraLockWithMaxZoom.ViewportDownLock, ZoomLevel - 1.0f);

	// Update movement zone flags
	bUseLeftRightMoveZone = !(CurrentCameraLock.ViewportRightLock == 0.0f && CurrentCameraLock.ViewportLeftLock == 0.0f);
	bUseUpDownMoveZone = !(CurrentCameraLock.ViewportTopLock == 0.0f && CurrentCameraLock.ViewportDownLock == 0.0f);
}

void ACS_Interactable_2d_Actor::UpdateArrows()
{
	if (bPauseMovingCamera || !IsValid(CurrentPlayerController)) {
		if (IsValid(CurrentPlayerController))
			CurrentPlayerController->UpdateMoveZones(false, false, false, false);
		return;
	}
	const float Epsilon = 0.001f;

	const bool bCanMoveLeft = bUseLeftRightMoveZone && (CurrentRightOffset > CurrentCameraLock.ViewportLeftLock + Epsilon);
	const bool bCanMoveRight = bUseLeftRightMoveZone && (CurrentRightOffset < CurrentCameraLock.ViewportRightLock - Epsilon);
	const bool bCanMoveDown = bUseUpDownMoveZone && (CurrentTopOffset > CurrentCameraLock.ViewportDownLock + Epsilon);
	const bool bCanMoveUp = bUseUpDownMoveZone && (CurrentTopOffset < CurrentCameraLock.ViewportTopLock - Epsilon);

	CurrentPlayerController->UpdateMoveZones(bCanMoveUp, bCanMoveDown, bCanMoveLeft, bCanMoveRight);
}

void ACS_Interactable_2d_Actor::CheckCameraMoveFromViewport()
{
	if (!IsValid(CurrentPlayerController) || !bInteracted || bIsLockedCameraMove)
		return;

	int32 ViewportX = 0;
	int32 ViewportY = 0;
	CurrentPlayerController->GetViewportSize(ViewportX, ViewportY);

	FVector2D MoveDirection = FVector2D();
	FVector2D MousePosition;
	CurrentPlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);

	constexpr float BorderThreshold = 50.0f;

	// Check horizontal movement
	if (bUseLeftRightMoveZone)
	{
		if (MousePosition.X <= BorderThreshold && CurrentRightOffset > CurrentCameraLock.ViewportLeftLock)
		{
			MoveDirection.X = -1.f;
		}
		else if (MousePosition.X >= ViewportX - BorderThreshold && CurrentRightOffset < CurrentCameraLock.ViewportRightLock)
		{
			MoveDirection.X = 1.f;
		}
	}

	// Check vertical movement
	if (bUseUpDownMoveZone)
	{
		if (MousePosition.Y <= BorderThreshold && CurrentTopOffset < CurrentCameraLock.ViewportTopLock)
		{
			MoveDirection.Y = 1.f;
		}
		else if (MousePosition.Y >= ViewportY - BorderThreshold && CurrentTopOffset > CurrentCameraLock.ViewportDownLock)
		{
			MoveDirection.Y = -1.f;
		}
	}

	MoveCamera(MoveDirection);
}

void ACS_Interactable_2d_Actor::SetPauseCameraMove(bool bPause)
{
	bPauseMovingCamera = bPause;
}

void ACS_Interactable_2d_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInteracted)
	{	
		if (!bPauseMovingCamera) {
			// Zoom interpolation
			float OldZoomDistance = CurrentZoomDistance;
			CurrentZoomDistance = FMath::FInterpTo(CurrentZoomDistance, TargetZoomDistance, DeltaTime, LerpZoomSpeed);
			float DeltaZoomDistance = CurrentZoomDistance - OldZoomDistance;

			InteractActorCameraComponent->AddLocalOffset(FVector(DeltaZoomDistance, 0.0f, 0.0f));

			if (ZoomLevel != (CurrentZoomDistance - MinZoomDistance) / (MaxZoomDistance - MinZoomDistance) + 1.0f)
				OnZoomUpdated.Broadcast((CurrentZoomDistance - MinZoomDistance) / (MaxZoomDistance - MinZoomDistance));

			// Update zoom level
			ZoomLevel = (CurrentZoomDistance - MinZoomDistance) / (MaxZoomDistance - MinZoomDistance) + 1.0f;

			UpdateLockCamera(true);
			CheckCameraMoveFromViewport();
		}

		
		UpdateArrows();
	}
}

bool ACS_Interactable_2d_Actor::Interact()
{
	if (!bCanInteractObject || !CurrentPlayerController)
		return false;

	if (!bInteracted)
	{
		// Setup 2D interaction
		CurrentPlayerController->SetViewTargetWithBlend(this, 0.2f, EViewTargetBlendFunction::VTBlend_Linear);
		CurrentPlayerController->StartInteractWithObject(bUseUpDownMoveZone, bUseLeftRightMoveZone, [&](bool Lock) { SetLockCameraMove(Lock); });
		CurrentPlayerController->SetInteractableActor(this);
		CurrentPlayerController->bEnableMouseOverEvents = true;
		CurrentPlayerController->bEnableClickEvents = true;

		/*CurrentPlayerController->GetPawn()->SetActorLocation(GetActorLocation());
		CurrentPlayerController->SetControlRotation(GetActorRotation());*/

		// Initialize camera state
		InteractActorCameraComponent->SetRelativeLocation(CurrentStartCameraPosition);

		// Reset zoom parameters
		MinZoomDistance = CurrentStartCameraPosition.X;
		TargetZoomDistance = MinZoomDistance;
		CurrentZoomDistance = MinZoomDistance;
		MaxZoomDistance = CurrentStartCameraPosition.X + ZoomDistance;
		CalcStartLockCamera();

		bInteracted = true;
		SetActorTickEnabled(true);

		K2_StartInteractActor(CurrentActionKey);

		return true;
	}

	return false;
}

void ACS_Interactable_2d_Actor::StopInteract()
{
	SetActorTickEnabled(false);
	bInteracted = false;

	// Reset player controller state
	CurrentPlayerController->SetViewTargetWithBlend(CurrentPlayerController->GetPawn(), 0.2f);
	CurrentPlayerController->SetInteractableActor(nullptr);
	CurrentPlayerController->bEnableMouseOverEvents = false;
	CurrentPlayerController->bEnableClickEvents = false;

	// Reset camera state
	InteractActorCameraComponent->SetRelativeLocation(StartCameraPosition);
	ZoomLevel = 1.0f;
	CurrentZoomDistance = MinZoomDistance;
	TargetZoomDistance = MinZoomDistance;
	CurrentStartCameraPosition = StartCameraPosition;

	// Restore initial values
	CameraLockWithMaxZoom = StartCameraLockWithMaxZoom;
	CameraLock = StartCameraLock;
	ZoomDistance = StartZoomDistance;
	CurrentRightOffset = 0.0f;
	CurrentTopOffset = 0.0f;

	UpdateLockCamera();

	K2_StopInteractActor();
}

void ACS_Interactable_2d_Actor::MoveCamera(const FVector2D& Direction)
{
	float EffectiveMoveSpeed = (MoveSpeed / (ZoomLevel * 2.0f)) * GetWorld()->GetDeltaSeconds();
	float MoveY = 0.f;
	float MoveZ = 0.f;

	if (bUseLeftRightMoveZone)
		MoveY = Direction.X * EffectiveMoveSpeed;
	if (bUseUpDownMoveZone)
		MoveZ = Direction.Y * EffectiveMoveSpeed;

	// Apply horizontal movement
	if (bUseLeftRightMoveZone)
	{
		float PrevRightOffset = CurrentRightOffset;
		CurrentRightOffset = FMath::Clamp(CurrentRightOffset + MoveY, CurrentCameraLock.ViewportLeftLock, CurrentCameraLock.ViewportRightLock);
		float DeltaRightOffset = CurrentRightOffset - PrevRightOffset;

		//InteractActorCameraComponent->AddLocalOffset(FVector(0.0f, DeltaRightOffset, 0.0f));
		FVector WorldOffset = StartCameraRotator.RotateVector(FVector(0.0f, DeltaRightOffset, 0.0f));
		InteractActorCameraComponent->AddWorldOffset(WorldOffset);
	}

	// Apply vertical movement
	if (bUseUpDownMoveZone)
	{
		float PrevTopOffset = CurrentTopOffset;
		CurrentTopOffset = FMath::Clamp(CurrentTopOffset + MoveZ, CurrentCameraLock.ViewportDownLock, CurrentCameraLock.ViewportTopLock);
		float DeltaTopOffset = CurrentTopOffset - PrevTopOffset;

		FVector WorldOffset = StartCameraRotator.RotateVector(FVector(0.0f, 0.0f, DeltaTopOffset));
		InteractActorCameraComponent->AddWorldOffset(WorldOffset);
	}
}

void ACS_Interactable_2d_Actor::CameraPanning(const FVector2D& Direction)
{
	MoveCamera(Direction);
}

void ACS_Interactable_2d_Actor::SetLockCameraMove(bool Lock)
{
	bIsLockedCameraMove = Lock;
}