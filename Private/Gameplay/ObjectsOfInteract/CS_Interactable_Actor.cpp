
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_Interaction_BoxComponent.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_Interact_Component_Interface.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_SubTeleport.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Framework/CS_UtilityFunctionLibrary.h"
#include "Components/SceneComponent.h"

ACS_Interactable_Actor::ACS_Interactable_Actor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACS_Interactable_Actor::BeginPlay()
{
	Super::BeginPlay();

	InteractComponents.Empty();

	TArray<UActorComponent*> Components;
	GetComponents(Components);
	
	TArray<FString> TempMethods = UCS_UtilityFunctionLibrary::GetCallableFunctionNames(this, bBlueprintOnlyMethodsES);

	for (FString& MethodName : TempMethods)
	{
		MapMethodsCommandES.Add(MethodName, false);
	}

	for (FString& MethodName : MethodNames)
	{
		MapMethodsCommandES.Add(MethodName, false);
	}

	for (UActorComponent*& Component : Components)
	{
		if (Component->Implements<UCS_Interact_Component_Interface>())
		{
			FString InteractID = ICS_Interact_Component_Interface::Execute_GetInteractID(Component);
			if (InteractID != "")
			{
				FString FinalKey = InteractID;

				// Рекурсивно добавляем "/" пока ключ не станет уникальным
				while (InteractComponents.Contains(FinalKey))
				{
					FinalKey += TEXT("/");
				}

				InteractComponents.Add(FinalKey, Component);
				ActionObjectKeys.Add(FinalKey);
			}
		}
		if (UStaticMeshComponent* SM = Cast<UStaticMeshComponent>(Component))
		{
			CustomDepthComponents.StaticMeshComponents.Add(SM);
		}

		if (UCS_SubTeleport* SubTeleport = Cast<UCS_SubTeleport>(Component))
		{
			FString TeleportLocationID = SubTeleport->GetLocationID();

			if (!TeleportLocationID.IsEmpty())
			{
				SubTeleportComponents.Add(TeleportLocationID, SubTeleport);
			}
		}
	}
}

void ACS_Interactable_Actor::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

}

void ACS_Interactable_Actor::bCheckInteract(ACS_PlayerController* PlayerController, UActorComponent* ActorComponent)
{
	StartShowOutline();

	if (bCanInteractObject && !bInteracted)
	{
		CurrentPlayerController = PlayerController;

		if (InteractComponents.IsEmpty())
			return;

		if (ActorComponent->Implements<UCS_Interact_Component_Interface>())
		{
			if (!CustomDepthComponents.bCustomDepth)
			{
				ICS_Interact_Component_Interface::Execute_SetOutline(ActorComponent, true);
				CustomDepthComponents.bCustomDepth = true;
			}

			if (IsValid(CurrentPlayerController) && !Cast<UCS_Interaction_BoxComponent>(ActorComponent))
			{	
				CurrentPlayerController->ShowTargetDescription(ICS_Interact_Component_Interface::Execute_GetDescription(ActorComponent));
				CurrentPlayerController->SetLastInteractActorComp(ActorComponent);
			}
		}
	}
}

bool ACS_Interactable_Actor::Interact()
{
	if (!bCanInteractObject)
		return false;

	if (!bInteracted)
	{
		CurrentPlayerController->SetInteractableActor(this);
		K2_StartInteract();
		bInteracted = true;
		return true;
	}
	return false;
}

void ACS_Interactable_Actor::StopInteract()
{
	bInteracted = false;
	CurrentPlayerController->SetInteractableActor(nullptr);
}

void ACS_Interactable_Actor::StopCheckInteract()
{
	StopShowOutlineEvent();
	if (InteractComponents.IsEmpty())
		return;
	
	for (const TTuple<FString, UActorComponent*>& Component : InteractComponents)
	{
		if (Component.Value->Implements<UCS_Interact_Component_Interface>())
		{
			if (ICS_Interact_Component_Interface::Execute_GetIsEnableComponent(Component.Value))
			{
				if (CustomDepthComponents.bCustomDepth)
				{
					ICS_Interact_Component_Interface::Execute_SetOutline(Component.Value, true);
					CustomDepthComponents.bCustomDepth = false;
				}
				ICS_Interact_Component_Interface::Execute_SetOutline(Component.Value, false);
				if (IsValid(CurrentPlayerController)) {
					CurrentPlayerController->ShowTargetDescription(FString());
					CurrentPlayerController->SetLastInteractActorComp(nullptr);
				}
			}
		}
	}
}

void ACS_Interactable_Actor::StartShowOutline()
{
	K2_StartShowOutline();
}

void ACS_Interactable_Actor::StopShowOutlineEvent()
{
	K2_StopShowOutlineEvent();
}

void ACS_Interactable_Actor::MakeDescription(const FString& BaseKey, const FString& DescriptionObject)
{
	if (InteractComponents.IsEmpty())
		return;

	
	for (TTuple<FString, UActorComponent*>& Pair : InteractComponents)
	{
		if (Pair.Key.StartsWith(BaseKey))
		{
			if (Pair.Key.Len() == BaseKey.Len() || Pair.Key[BaseKey.Len()] == static_cast<TCHAR>('/'))
			{
				if (UActorComponent*& Component = Pair.Value)
				{
					if (Component->Implements<UCS_Interact_Component_Interface>())
					{
						ICS_Interact_Component_Interface::Execute_MakeDescription(Component,this, DescriptionObject);
					}
				}
			}
		}
	}
}

void ACS_Interactable_Actor::StartTeleport()
{
	if (IsValid(CurrentPlayerController) && IsValid(CurrentPlayerController->GetPawn()))
	{
		CurrentPlayerController->GetPawn()->SetActorLocation(GetActorLocation());
		CurrentPlayerController->SetControlRotation(GetActorRotation());
	}
}

void ACS_Interactable_Actor::StartReact_Implementation(const FString& MethodName, const FVariables& Variable)
{
	UCS_UtilityFunctionLibrary::CallFunctionByName(this, MethodName, Variable);
}

void ACS_Interactable_Actor::SendCustomAction(const FString& ObjectId, const FString& CustomString)
{	

	if (IsValid(CurrentPlayerController))
		CurrentPlayerController->OnCustomEvent(ObjectId, CustomString);
	else {

		ACS_PlayerController* TempPlayerController = Cast<ACS_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		if (IsValid(TempPlayerController)) {
			CurrentPlayerController = TempPlayerController;
			CurrentPlayerController->OnCustomEvent(ObjectId, CustomString);
		}
	}
}

bool ACS_Interactable_Actor::GetActorInteractable()
{
	return bInteracted;
}

bool ACS_Interactable_Actor::GetCanInteractObject()
{
	return bCanInteractObject;
}

bool ACS_Interactable_Actor::IsUseKey_Implementation(const FString& Key) const
{
	for (const FString& SKey : ActionObjectKeys)
		if (SKey == Key)
			return true;

	if (MapMethodsCommandES.Contains(Key))
		return true;

	return false;
}

void ACS_Interactable_Actor::ActivateObject_Implementation(const FString& Key,
	const FString& _DescriptionObject)
{
	CurrentActionKey = Key;
	bCanInteractObject = true;
	MakeDescription(Key, _DescriptionObject);
}

void ACS_Interactable_Actor::ActivateObjectReactMethod_Implementation(const FString& Key,
	const FVariables& Variable)
{
	StartReact(Key, Variable);
}

void ACS_Interactable_Actor::ActivatePoint_Implementation(const FString& Key,
	const FParametersObject& Parameters)
{

}

void ACS_Interactable_Actor::Teleport_Implementation(ACS_PlayerController* PlayerController, const FString& Key)
{
	if (IsValid(PlayerController))
	{
		CurrentPlayerController = PlayerController;
		CurrentActionKey = Key;
		bCanInteractObject = true;
		StartTeleport();
	}
}

void ACS_Interactable_Actor::DisableObject_Implementation()
{
	bCanInteractObject = false;

	TArray<FString> Keys;
	BlinkingIDs.GenerateKeyArray(Keys);

	for (const FString& BlinkID : Keys) {
		StopBlinkingObject(BlinkID);
	}	
	
	for (const TTuple<FString, UActorComponent*>& Component : InteractComponents)
	{
		if (Component.Value->Implements<UCS_Interact_Component_Interface>())
		{
			ICS_Interact_Component_Interface::Execute_DisableComponent(Component.Value);
		}
	}
}

void ACS_Interactable_Actor::DisablePoint_Implementation()
{

}

void ACS_Interactable_Actor::StartBlinkingObject(const FString& Key)
{
	BlinkingIDs.Add(Key, true);
	K2_StartBlinkingObject(Key);
}

void ACS_Interactable_Actor::StopBlinkingObject(const FString& Key)
{
	if (!BlinkingIDs.Contains(Key)) return;

	BlinkingIDs.Remove(Key);
	K2_StopBlinkingObject(Key);
}

bool ACS_Interactable_Actor::TryToSubTeleport_Implementation(const FString& Key)
{
	if (SubTeleportComponents.Contains(Key))
	{
		if (IsValid(CurrentPlayerController) && IsValid(CurrentPlayerController->GetPawn()))
		{
			CurrentPlayerController->GetPawn()->SetActorLocation(SubTeleportComponents[Key]->GetComponentLocation());
			CurrentPlayerController->SetControlRotation(SubTeleportComponents[Key]->GetComponentRotation());

			return true;
		}
	}

	return false;
}

ACS_PlayerController* ACS_Interactable_Actor::GetCurrentPlayerController() const
{	
	return CurrentPlayerController;
}
