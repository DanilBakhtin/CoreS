// Core Simulator


#include "Gameplay/Player/CS_Character.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Framework/CS_CoreTypes.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "Gameplay/ObjectsOfInteract/CS_Interact_Interface.h"
#include "Gameplay/ObjectsOfInteract/MeasureDevices/CS_MeasureDevices_ActorComponent.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include <Gameplay/ObjectsOfInteract/Components/CS_Interact_Component_Interface.h>
#include "Gameplay/ObjectsOfInteract/CS_LocationZone.h"

#include "Framework/CS_DeveloperSettings.h"
#include "Framework/CS_SimulatorParamsAsset.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ACS_Character::ACS_Character()
{
	// Будет ли отрабатывать Tick() в каждом кадре.
	PrimaryActorTick.bCanEverTick = false;

	// Задание размеров Капсулы коллизии
	GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetCapsuleComponent());
	SpringArmComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 0.f;
	SpringArmComponent->TargetArmLength = 0.f;
	
	// Создание CameraComponent
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(SpringArmComponent);
	//FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	
	// Создание mesh component который будет отображаться в виде от первого лица на локальной машине
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	
	MeasureDevices = CreateDefaultSubobject<UCS_MeasureDevices_ActorComponent>(TEXT("MeasureDevices_ActorComponent"));
	
	CurrentZoom = 1.0f;
	BaseEyeHeight = 60.f;
	CrouchedEyeHeight = 32.f;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->CastShadow = false;
	GetMesh()->SetHiddenInGame(true);
	GetMesh()->SetRenderInMainPass(false);
	GetMesh()->SetRenderInDepthPass(false);

	TraceForLocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TraceForLocationComponent"));
	TraceForLocationComponent->SetupAttachment(GetCapsuleComponent());
	TraceForLocationComponent->SetRelativeLocation(FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()/2));
}

void ACS_Character::ZoomReset()
{
	CurrentZoom = ZoomMin;

	if (FirstPersonCameraComponent)
	{
		TargetFOV = BaseFOV / CurrentZoom;
		TargetFOV = FMath::RoundToFloat(TargetFOV);
		OnZoomChanged.Broadcast(GetZoomPercent());
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("ZoomReset failed: FirstPersonCameraComponent is invalid for %s."), *GetNameSafe(this));
	}
}

void ACS_Character::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(MeasureDevices))
	{

	}

	if (FirstPersonCameraComponent) {
		BaseFOV = FirstPersonCameraComponent->FieldOfView;
		TargetFOV = BaseFOV / CurrentZoom;
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("FirstPersonCameraComponent is not valid in BeginPlay for %s."), *GetNameSafe(this));
	}

	InitMovementComponent();
}

UCS_MeasureDevices_ActorComponent* ACS_Character::GetMeasureDevices() const
{
	return MeasureDevices;
}

void ACS_Character::StartUsingPawn(APlayerController* _Controller)
{
	// Add Input Mapping Context
	if (IsValid(_Controller))
	{
		PlayerController = Cast<ACS_PlayerController>(_Controller);
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_Controller->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		PlayerController->OnTeleport.AddDynamic(this, &ACS_Character::AfterTeleport);
		MeasureDevices->SetPlayerController(PlayerController);
	}
}

ACS_PlayerController* ACS_Character::GetPlayerController() const
{
	return PlayerController;
}

void ACS_Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ACS_Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X * MouseSensitivityX);
		AddControllerPitchInput(LookAxisVector.Y * MouseSensitivityY);
	}
}

void ACS_Character::StartCrouch()
{
	if (GetIsCanCrouch())
	{
		Crouch(false);
	}
	
}

void ACS_Character::StopCrouch()
{
	UnCrouch(false);
}

void ACS_Character::Sprint(const FInputActionValue& Value)
{
	if (GetIsCanSprint() && !GetCharacterMovement()->bWantsToCrouch)
	{
		const float EffortValue = Value.Get<float>();

		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed + (SprintDeltaSpeed * EffortValue);
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void ACS_Character::HideDescription()
{
	if (LastInteractComponent.IsValid())
	{
		OnNonInteractComponent.Broadcast();

		if (LastInteractActor.IsValid()) LastInteractActor->StopCheckInteract();

		LastInteractComponent = nullptr;

		if (!LastInteractActor->GetActorInteractable())
			LastInteractActor = nullptr;
	}
	else
	{
		if (IsValid(PlayerController)) PlayerController->ShowTargetDescription(FString());
	}
}

void ACS_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MakeTrace();
	MakeTraceForLocation();

	Zooming(DeltaTime);
}

// Called to bind functionality to input
void ACS_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movement
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACS_Character::Move);

		// Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACS_Character::Look);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ACS_Character::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACS_Character::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Canceled, this, &ACS_Character::Sprint);

		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ACS_Character::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ACS_Character::StopCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Canceled, this, &ACS_Character::StopCrouch);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ACS_Character::Interact);

		// Zoom
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ACS_Character::ZoomCamera);
		
		

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}

bool ACS_Character::GetIsCanCrouch() const
{
	return bIsCanCrouch;
}

void ACS_Character::SetIsCanCrouch(const bool bNewCanCrouch)
{
	bIsCanCrouch = bNewCanCrouch;
}

bool ACS_Character::GetIsCanSprint() const
{
	return bIsCanSprint;
}

void ACS_Character::SetIsCanSprint(const bool bNewCanCrouch)
{
	bIsCanSprint = bNewCanCrouch;
}

void ACS_Character::ZoomCamera(const FInputActionValue& Value)
{
	if (IsValid(PlayerController) && this->InputEnabled()) {
		if (!PlayerController->bCourseStarted)
		{
			return;
		}
	}
	else
	{
		return;
	}


	float ZoomValue = Value.Get<float>();
	float ZoomChange = ZoomValue * ZoomSpeedPercent / 100.0f;

	CurrentZoom = FMath::Clamp(CurrentZoom + ZoomChange, ZoomMin, ZoomMax);
	TargetFOV = BaseFOV / CurrentZoom;
	TargetFOV = FMath::RoundToFloat(TargetFOV);

	bIsZooming = true;

	OnZoomChanged.Broadcast(GetZoomPercent());
}

void ACS_Character::Zooming(float DeltaTime)
{
	if (FirstPersonCameraComponent && bIsZooming)
	{
		float CurrentFOV = FirstPersonCameraComponent->FieldOfView;
		float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, ZoomInterpSpeed);
		FirstPersonCameraComponent->SetFieldOfView(NewFOV);

		const float Tolerance = 0.01f;
		if (FMath::IsNearlyEqual(NewFOV, TargetFOV, Tolerance))
		{
			FirstPersonCameraComponent->SetFieldOfView(TargetFOV);
			bIsZooming = false;
		}
	}
}

int ACS_Character::GetZoomPercent()
{
	return FMath::RoundToInt(CurrentZoom * 100.0f);
}

void ACS_Character::ClearHitActor()
{
	LastInteractActor = nullptr;
	LastInteractComponent = nullptr;
}

void ACS_Character::AfterTeleport()
{
	if (SpringArmComponent->CameraLagSpeed == 0.f)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ACS_Character::SetCameraLagSpeedToDefault);
	}
}

void ACS_Character::SetCameraLagSpeedToDefault()
{
	SpringArmComponent->CameraLagSpeed = 10.f;
}

void ACS_Character::StartUseMeasureDevice(TWeakObjectPtr<ACS_Interactable_Actor> InteractableActor, const FString& MeasureId, const TArray<FDeviceData>& PointsData) const
{
	if (InteractableActor.IsValid())
	{
		MeasureDevices->ActivateMeasureDevice(InteractableActor.Get(), MeasureId, PointsData);
		//PlayerController->Lock2DZoom(true);
	}
}

void ACS_Character::StopUseMeasureDevice(TWeakObjectPtr<ACS_Interactable_Actor> InteractableActor) const
{
	if (InteractableActor.IsValid())
	{
		MeasureDevices->DisableMeasureDevice(InteractableActor.Get());
		//PlayerController->Lock2DZoom(false);
	}
}

void ACS_Character::UpdateMeasureDevice(const float& Result) const
{
	MeasureDevices->UpdateMeasureDevice(Result);
}

void ACS_Character::Interact()
{
	if (!LastInteractActor.IsValid())
		return;

	PlayerController->SetMouseToCenter();

	if (LastInteractComponent.IsValid())
	{
		if (LastInteractComponent->Implements<UCS_Interact_Component_Interface>())
		{
			PlayerController->OnClickObject(ICS_Interact_Component_Interface::Execute_GetInteractID(LastInteractComponent.Get()), FString());
		}
	}
}

void ACS_Character::InitMovementComponent()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->SetCrouchedHalfHeight(CrouchedHalfHeight);
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	if (UCS_SimulatorParamsAsset* SimulatorParams = GetDefault<UCS_DeveloperSettings>()->SimulatorParamsAsset.LoadSynchronous())
	{
		SetIsCanCrouch(SimulatorParams->Params.bIsCanCrouch);
		SetIsCanSprint(SimulatorParams->Params.bIsCanSprint);
	}
}

void ACS_Character::MakeTrace()
{
	if (!bUseTrace) {

		HideDescription();
		return;
	}

	FHitResult OutHit;
	FVector Start = FirstPersonCameraComponent ? FirstPersonCameraComponent->GetComponentLocation() : GetActorLocation();
	FVector ForwardVector = FirstPersonCameraComponent ? FirstPersonCameraComponent->GetForwardVector() : GetActorForwardVector();
	FVector End = ((ForwardVector * TraceDistance) + Start);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (bShowInteractObject)
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, SIM_INTERACT, CollisionParams))
	{
#if WITH_EDITOR
		if (bShowTrace)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *OutHit.GetActor()->GetName()));
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Impact Point: %s"), *OutHit.ImpactPoint.ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Normal Point: %s"), *OutHit.ImpactNormal.ToString()));
		}
#endif

		TraceProcessing(OutHit);
	}
	else
	{
		HideDescription();
	}
}

void ACS_Character::TraceProcessing(FHitResult hit)
{
	ICS_Interact_Interface* InteractInterface = Cast<ICS_Interact_Interface>(hit.GetActor());
	UActorComponent* HitComponent = hit.GetComponent();

	if (InteractInterface)
	{
		if (LastInteractComponent.Get() != HitComponent)
		{
			if (LastInteractComponent.IsValid())
			{
				if (LastInteractActor.IsValid()) {
					LastInteractActor->StopCheckInteract();
				}
			}

			LastInteractActor = Cast<ACS_Interactable_Actor>(hit.GetActor());
			LastInteractComponent = HitComponent;

			InteractInterface->bCheckInteract(PlayerController, HitComponent);

			if (HitComponent->Implements<UCS_Interact_Component_Interface>())
			{
				OnPointingComponent.Broadcast(HitComponent, ICS_Interact_Component_Interface::Execute_GetInteractID(HitComponent));
			}
		}
	}
	else
	{
		HideDescription();
	}
}

void ACS_Character::MakeTraceForLocation()
{
	if (!bUseTrace) {
		LastLocationZone = nullptr;
		return;
	}

	TArray<FHitResult> OutHit;
	FVector Start = TraceForLocationComponent ? TraceForLocationComponent->GetComponentLocation() : GetActorLocation();
	FVector ForwardVector = TraceForLocationComponent ? TraceForLocationComponent->GetUpVector() : GetActorUpVector();
	FVector End = Start + FVector(5.0f, 5.0f, -0.1f);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	int32 MaxPriority = -1;
	FString MaxLocationID = "";

	ACS_LocationZone* CurrentLocationZone = nullptr;

	if (GetWorld()->LineTraceMultiByChannel(OutHit, Start, End, SIM_LOCATION, CollisionParams))
	{
		for (FHitResult& HitEl : OutHit)
		{
			if (ACS_LocationZone* LocationZone = Cast<ACS_LocationZone>(HitEl.GetActor()))
			{
				if (MaxPriority < LocationZone->GetPriorityNumber())
				{
					MaxPriority = LocationZone->GetPriorityNumber();
					MaxLocationID = LocationZone->GetLocationID();
					CurrentLocationZone = LocationZone;
				}
			}
		}
	}

	if (!MaxLocationID.IsEmpty() && MaxPriority >= 0 && IsValid(PlayerController))
	{
		if (CurrentLocationZone != LastLocationZone)
		{
			LastLocationZone = CurrentLocationZone;
			PlayerController->EndTween(MaxLocationID);
		}
	}
}
