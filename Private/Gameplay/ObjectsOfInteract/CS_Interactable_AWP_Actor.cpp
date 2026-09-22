#include "Gameplay/ObjectsOfInteract/CS_Interactable_AWP_Actor.h"

#include "Camera/CameraComponent.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Gameplay/UI/CS_AWP_Widget.h"
#include "Components/WidgetComponent.h"

ACS_Interactable_AWP_Actor::ACS_Interactable_AWP_Actor()
{
	// Create Default scene root
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	// Create Camera component
	ActorCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ActorCameraComponent->SetupAttachment(RootSceneComponent);

	//Create WidgetComponent
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootSceneComponent);
	WidgetComponent->bRenderInMainPass = false;
	WidgetComponent->bRenderInDepthPass = false;

}

void ACS_Interactable_AWP_Actor::BeginPlay()
{
	SetActorTickEnabled(false);

	// Create AWP widget by instanced class
	if (WidgetClass)
	{
		if ((AWP_Widget = CreateWidget<UCS_AWP_Widget>(GetWorld(), WidgetClass)))
		{	

			InitWidgetsArray();

			AWP_Widget->WidgetPointAction.AddDynamic(this, &ACS_Interactable_AWP_Actor::HandlePointAction);

			AWP_Widget->OnInitWidgets.AddDynamic(this, &ACS_Interactable_AWP_Actor::InitWidgetsArray);

			/*if (WidgetComponent)
			{
				WidgetComponent->SetWidget(AWP_Widget);
			}*/

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s: Something went wrong while creating the AWP widget!"), *GetActorNameOrLabel());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: AWP widget class was not set!"), *GetActorNameOrLabel());
	}

	Super::BeginPlay();
}

bool ACS_Interactable_AWP_Actor::Interact()
{
#pragma region Checks
	if (!Super::Interact())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Trying interact with non-interactable object."), *GetActorNameOrLabel());
		return false;
	}

	if (!CurrentPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Player controller is invalid"
			" or it is not actual interact actor."), *GetActorNameOrLabel());
		return false;
	}

	if (!AWP_Widget)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: AWP widget was not create!"), *GetActorNameOrLabel());
		return false;
	}
#pragma endregion

	CurrentPlayerController->SetViewTargetWithBlend(this, 0.2);
	CurrentPlayerController->SetInteractableActor(this);
	CurrentPlayerController->ShowAWP(AWP_Widget);

	SetActorTickEnabled(true);

	return true;
}

void ACS_Interactable_AWP_Actor::StopInteract()
{
	CurrentPlayerController->SetViewTargetWithBlend(CurrentPlayerController->GetPawn(), 0.2f);
	CurrentPlayerController->HideAWP();

	SetActorTickEnabled(false);

	K2_StopInteractActor();

	Super::StopInteract();
}

void ACS_Interactable_AWP_Actor::ActivateObject_Implementation(const FString& Key, const FString& DescriptionObject)
{
	Super::ActivateObject_Implementation(Key, DescriptionObject);
}

void ACS_Interactable_AWP_Actor::DisableObject_Implementation()
{
	Super::DisableObject_Implementation();
	
	if (AWP_Widget)
		AWP_Widget->HideAllInteractWidgets();
}

void ACS_Interactable_AWP_Actor::TrySetViewID_Implementation(const FObjectViewID& StructObjectViewID)
{	
	ActivateObject_Implementation(StructObjectViewID.PointID, StructObjectViewID.DecsID);
	ActivateObject_Implementation(StructObjectViewID.ViewID, StructObjectViewID.DecsID);

	if (AWP_Widget)
	{
		if (AWP_Widget->UpdateActiveKey(StructObjectViewID))
		{
			if (WidgetComponent)
			{
				K2_UpdatedWidgetAWP(WidgetComponent->GetRenderTarget());
			}
		}
	}

	UE_LOG(LogTemp, Display, TEXT("%s: View ID"), *StructObjectViewID.ViewID);
}

bool ACS_Interactable_AWP_Actor::IsUseKey_Implementation(const FString& Key) const
{
	for (const FString& SKey : ActionObjectKeys)
		if (SKey == Key)
			return true;

	if (MapMethodsCommandES.Contains(Key))
		return true;

	if (AWP_Widget)
		for (const FString& SKey : AWP_Widget->GetViewIDs())
			if (SKey == Key)
				return true;

	if (LocationID == Key)
		return true;

	return false;
}

void ACS_Interactable_AWP_Actor::Teleport_Implementation(ACS_PlayerController* PlayerController, const FString& Key)
{
	if (IsValid(PlayerController))
	{
		CurrentPlayerController = PlayerController;

		CurrentActionKey = Key;
		bCanInteractObject = true;
		StartTeleport();
	}
}

void ACS_Interactable_AWP_Actor::StartTeleport()
{
	Interact();
}

void ACS_Interactable_AWP_Actor::HandlePointAction(const FString& apiId, const EPointAction& PointAction)
{
	if (!CurrentPlayerController)
	{
		UE_LOG(LogTemp, Display, TEXT("%s: Player controller is invalid."), *GetActorNameOrLabel());
		return;
	}
	CurrentPlayerController->OnPointAction(apiId, PointAction);
}

void ACS_Interactable_AWP_Actor::InitWidgetsArray()
{
	TArray<FString> Keys = AWP_Widget->GetViewIDs();

	for (int i = 0; i < Keys.Num(); i++) {

		ActionObjectKeys.AddUnique(Keys[i]);
	}
}
