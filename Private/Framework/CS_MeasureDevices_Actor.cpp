
#include "Framework/CS_MeasureDevices_Actor.h"
#include "Framework/CS_CoreTypes.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_Interact_Component_Interface.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_MeasurePoint.h"
#include "Gameplay/Player/CS_PlayerController.h"

ACS_MeasureDevices_Actor::ACS_MeasureDevices_Actor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ACS_MeasureDevices_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACS_MeasureDevices_Actor::ClickOnMeasurePoint(const FString& InteractID, const FTransform& PositionMeasure)
{
	
}

void ACS_MeasureDevices_Actor::AttachMeasureDeviceToPrev(UActorComponent* PrevComponent)
{
	
}

void ACS_MeasureDevices_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACS_MeasureDevices_Actor::ActivateMeasureDevice(ACS_Interactable_Actor* InteractableActor, const FString& MeasureId, const TArray<FDeviceData>& PointsData)
{
	TArray<UActorComponent*> Components;
	InteractableActor->GetComponents(Components);
	IdDevice = MeasureId;

	for(auto& Component : Components)
	{
		AttachMeasureDeviceToPrev(Component);
		
		if(Component->Implements<UCS_Interact_Component_Interface>())
		{
			if (Component->Implements<UCS_Interact_Component_Interface>())
			{
				ICS_Interact_Component_Interface::Execute_SetIsUsedMeasureDevice(Component, true);
				for(auto& PointData : PointsData)
				{
					for(auto& Point : PointData.Param)
					{
						if(ICS_Interact_Component_Interface::Execute_GetInteractID(Component) == Point.Id)
						{
							ICS_Interact_Component_Interface::Execute_MakeDescription(Component, InteractableActor, Point.Name);
							if(UCS_MeasurePoint* MeasurePoint = Cast<UCS_MeasurePoint>(Component))
							{
								MeasurePoint->SetClickOnMeasuringPointFun([&](const FString& InteractID, const FTransform& PositionMeasure)
								{
									ClickOnMeasurePoint(InteractID, PositionMeasure);
								});
							}
						}
					}
				}
			}
		}
	}
}

void ACS_MeasureDevices_Actor::SetPlayerController(ACS_PlayerController* _PlayerController)
{
	PlayerController = _PlayerController;
}

void ACS_MeasureDevices_Actor::UpdateMeasureDevice(const float& Result)
{
	
}

void ACS_MeasureDevices_Actor::ResetDevice()
{
}
