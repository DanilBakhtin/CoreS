
#include "Gameplay/ObjectsOfInteract/Components/CS_MeasurePoint.h"

#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "Gameplay/Player/CS_PlayerController.h"

UCS_MeasurePoint::UCS_MeasurePoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCS_MeasurePoint::MakeDescription_Implementation(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description)
{
	if (bIsComponentEnable || !IsValid(Interactable_Actor))
		return;

	if (!bInitDescription)
	{
		bInitDescription = true;
		Description = _Description;
		Owner = Interactable_Actor;
	}
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetHiddenInGame(false);
	bIsComponentEnable = true;
}

void UCS_MeasurePoint::DisableComponent_Implementation()
{
	if (bIsUsedMeasureDevice) return;

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRenderCustomDepth(false);
	bIsComponentEnable = false;
	SetHiddenInGame(true);
	Execute_SetOutline(this, false);
}

FString UCS_MeasurePoint::GetInteractID_Implementation() const
{
	return InteractID;
}

FString UCS_MeasurePoint::GetDescription_Implementation() const
{
	return Description;
}

bool UCS_MeasurePoint::GetInteractable_Implementation(EInteractMode CurrentMode) const
{
	return CurrentMode == EInteractMode::MeausureMode;
}

bool UCS_MeasurePoint::GetIsEnableComponent_Implementation() const
{
	return bIsComponentEnable;
}

void UCS_MeasurePoint::LeftClickInteract_Implementation()
{
	if (ClickOnMeasuringPointFun) {

		FTransform TempTransform = GetComponentTransform();

		FQuat CurrentRotation = TempTransform.GetRotation();
		FQuat LocalRotation = FQuat(FVector(1.0f, 0.0f, 0.0f), FMath::DegreesToRadians(90.0f));
		FQuat NewRotation = CurrentRotation * LocalRotation;

		TempTransform.SetRotation(NewRotation);
		TempTransform.SetScale3D(FVector(1.0f));

		ClickOnMeasuringPointFun(InteractID, TempTransform);
	}
}

ETypeInteractPoint UCS_MeasurePoint::GetTypePoint_Implementation()
{
	return ETypeInteractPoint::MeausurePoint;
}

void UCS_MeasurePoint::SetOutline_Implementation(const bool bEnable)
{
	if (IsValid(PointMaterial)) 
	{
		PointMaterial->SetVectorParameterValue(TEXT("Color2"), bEnable? ColorHovered : ColorUnhovered);
	}
}

void UCS_MeasurePoint::SetIsUsedMeasureDevice_Implementation(bool Used)
{
	bIsUsedMeasureDevice = Used;

	if (!bIsUsedMeasureDevice)
	{
		Execute_DisableComponent(this);
		ClickOnMeasuringPointFun = nullptr;
	}
}

void UCS_MeasurePoint::SetClickOnMeasuringPointFun(
	TFunction<void(const FString& InteractID, const FTransform& PositionMeasure)> Fun)
{
	ClickOnMeasuringPointFun = Fun;
}


void UCS_MeasurePoint::BeginPlay()
{
	Super::BeginPlay();

	SetHiddenInGame(true);
	SetCollisionProfileName(FName("InteractObject"), true);

	PointMaterial = CreateDynamicMaterialInstance(0, GetMaterial(0));
}
