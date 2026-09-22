#include "Gameplay/UI/CS_AWP_Widget.h"

#include "Components/PanelWidget.h" // For UPanelWidget
#include "Blueprint/WidgetTree.h"
#include "Gameplay/UI/CS_Interact_Widget.h"
#include "Components/CanvasPanelSlot.h"

void UCS_AWP_Widget::NativeConstruct()
{
	Super::NativeConstruct();
	
	TArray<UWidget*> AllWidgetsInTree;
	WidgetTree->GetAllWidgets(AllWidgetsInTree); // Gathers all widgets in the tree recursively

	for (UWidget* Widget : AllWidgetsInTree)
	{
		if (UCS_Interact_Widget* InteractWidget = Cast<UCS_Interact_Widget>(Widget))
		{
			InteractWidget->WidgetInteractPointAction.Clear();
			InteractWidget->WidgetInteractPointAction.AddDynamic(this, &UCS_AWP_Widget::HandleButtonAction);
			//UE_LOG(LogTemp, Log, TEXT("Found button with ID: %s"), *Button->GetActionID());
			InteractWidgetsMap.Add(InteractWidget->GetViewID(), InteractWidget);
		}
	}

	OnInitWidgets.Broadcast();

	HideAllInteractWidgets();
}

bool UCS_AWP_Widget::UpdateActiveKey(const FObjectViewID& StructObjectViewID)
{	
	UCS_Interact_Widget* Element = nullptr;

	if (UCS_Interact_Widget** Found = InteractWidgetsMap.Find(StructObjectViewID.ViewID))
		Element = *Found;

	if (UCS_Interact_Widget** Found = InteractWidgetsMap.Find(StructObjectViewID.PointID))
		Element = *Found;

	if (!Element)
		return false;

	const bool bAction = Element->GetActionID().IsEmpty();
	Element->SetVisibility(bAction ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Visible);

	UCanvasPanelSlot* ContextObjectSlot = Cast<UCanvasPanelSlot>(Element->Slot);
	if (!ContextObjectSlot)
		return false;

	UpdateWidgetSlot(ContextObjectSlot,
		FVector2D(StructObjectViewID.SizeX, StructObjectViewID.SizeY),
		FVector2D(StructObjectViewID.OffsetX, StructObjectViewID.OffsetY),
		StructObjectViewID.ZOrder);

	if (StructObjectViewID.ZOrder > MaxZOrder)
		MaxZOrder = StructObjectViewID.ZOrder;

	Element->ViewStruct = StructObjectViewID;
	PostZOrderWidgets();

	UE_LOG(LogTemp, Display, TEXT("%s z: %d"), *Element->GetViewID(), ContextObjectSlot->GetZOrder());

	return true;
}

void UCS_AWP_Widget::HideAllInteractWidgets()
{

	MaxZOrder = -1;

	for (TTuple<FString, UCS_Interact_Widget*> Element : InteractWidgetsMap)
	{
		Element.Value->SetVisibility(ESlateVisibility::Collapsed);
	}
}

TArray<FString> UCS_AWP_Widget::GetViewIDs()
{	
	TArray<FString> DirectKeys;
	InteractWidgetsMap.GetKeys(DirectKeys);

	return DirectKeys;
}

void UCS_AWP_Widget::InitSize(const FVector2D TargetSize)
{	
	if (bInitSize) return;

	bInitSize = true;

	RatioSize = FVector2D(TargetSize.X / BaseSize.X, TargetSize.Y / BaseSize.Y);


	TArray<UCS_Interact_Widget*> WidgetArray;
	InteractWidgetsMap.GenerateValueArray(WidgetArray);

	for (UCS_Interact_Widget* Widget : WidgetArray)
	{
		if (Widget)
		{
			UCanvasPanelSlot* ContextObjectSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
			UpdateWidgetSlot(ContextObjectSlot, ContextObjectSlot->GetSize(), ContextObjectSlot->GetPosition(), ContextObjectSlot->GetZOrder());

			UE_LOG(LogTemp, Display, TEXT("init %s z: %d"), *Widget->GetViewID(), ContextObjectSlot->GetZOrder());
		}
	}
}

void UCS_AWP_Widget::UpdateWidgetSlot(UCanvasPanelSlot* ContextObjectSlot, const FVector2D TargetSize, const FVector2D TargetPosition, const int ZOrder)
{
	if (!ContextObjectSlot)  return;

	ContextObjectSlot->SetSize(FVector2D(TargetSize.X * RatioSize.X, TargetSize.Y * RatioSize.Y));
	ContextObjectSlot->SetPosition(FVector2D(TargetPosition.X * RatioSize.X, TargetPosition.Y * RatioSize.Y));
	ContextObjectSlot->SetAlignment(FVector2D(0.0f, 0.0f));
	ContextObjectSlot->SetMinimum(FVector2D(0.0f, 0.0f));
	ContextObjectSlot->SetMaximum(FVector2D(0.0f, 0.0f));
	ContextObjectSlot->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
	ContextObjectSlot->SetZOrder(ZOrder);
}

void UCS_AWP_Widget::PostZOrderWidgets()
{
	for (TTuple<FString, UCS_Interact_Widget*> Element : InteractWidgetsMap)
	{
		if (Element.Value->GetVisibility() != ESlateVisibility::Collapsed) {

			UCanvasPanelSlot* ContextObjectSlot = Cast<UCanvasPanelSlot>(Element.Value->Slot);
			if (!ContextObjectSlot) continue;

			if (Element.Value->ViewStruct.Z_CSS > 0) {
				ContextObjectSlot->SetZOrder(Element.Value->ViewStruct.Z_CSS + Element.Value->ViewStruct.ZOrder + MaxZOrder);
			}
		}
	}
}


void UCS_AWP_Widget::HandleButtonAction(const FString& apiId, const EPointAction& PointAction)
{
	WidgetPointAction.Broadcast(apiId, PointAction);
}
