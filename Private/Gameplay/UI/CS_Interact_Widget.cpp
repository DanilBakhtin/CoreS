#include "Gameplay/UI/CS_Interact_Widget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Framework/CS_CoreTypes.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UCS_Interact_Widget::NativePreConstruct()
{
	Super::NativePreConstruct();

	check(Container)

	if (WidgetImage == nullptr) {

		WidgetImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Image"));
		FSlateBrush Brush;
		Brush.SetResourceObject(Image);
		WidgetImage->SetBrush(Brush);
		WidgetImage->SetBrushTintColor(DefaultColor);

		UCanvasPanelSlot* SlotAWP = Container->AddChildToCanvas(WidgetImage);

		FAnchors Anchors;
		Anchors.Minimum = FVector2d(0.0f, 0.0f);
		Anchors.Maximum = FVector2d(1.0f, 1.0f);

		SlotAWP->SetAnchors(Anchors);
		SlotAWP->SetOffsets(FMargin(0.0f, 0.0f, 0.0f, 0.0f));
	}
}

void UCS_Interact_Widget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCS_Interact_Widget::ChangeImage(UObject* NewImage)
{
	if (WidgetImage && NewImage) {
		FSlateBrush Brush;
		Brush.SetResourceObject(NewImage);
		WidgetImage->SetBrush(Brush);
		WidgetImage->SetBrushTintColor(DefaultColor);
	}

}

FReply UCS_Interact_Widget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!ActionID.IsEmpty())
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			WidgetInteractPointAction.Broadcast(ActionID, EPointAction::OnMouseDown);
			WidgetImage->SetBrushTintColor(PressedColor);
		}
	}

	return FReply::Handled();
}

FReply UCS_Interact_Widget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (!ActionID.IsEmpty())
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			WidgetInteractPointAction.Broadcast(ActionID, EPointAction::OnDblClick);
			WidgetImage->SetBrushTintColor(PressedColor);
		}
	}

	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

void UCS_Interact_Widget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!ActionID.IsEmpty())
	{
		WidgetInteractPointAction.Broadcast(ActionID, EPointAction::OnMouseOver);
		WidgetImage->SetBrushTintColor(HoveredColor);
	}

	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UCS_Interact_Widget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (!ActionID.IsEmpty())
	{
		WidgetInteractPointAction.Broadcast(ActionID, EPointAction::OnMouseOut);
		WidgetImage->SetBrushTintColor(DefaultColor);
	}

	Super::NativeOnMouseLeave(InMouseEvent);
}

FReply UCS_Interact_Widget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!ActionID.IsEmpty())
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			WidgetInteractPointAction.Broadcast(ActionID, EPointAction::OnMouseUp);
			WidgetInteractPointAction.Broadcast(ActionID, EPointAction::OnClick);
		}
		if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			WidgetInteractPointAction.Broadcast(ActionID, EPointAction::OnRClick);
		}
		WidgetImage->SetBrushTintColor(HoveredColor);
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}
