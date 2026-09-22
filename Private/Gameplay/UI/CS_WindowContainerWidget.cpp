// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/UI/CS_WindowContainerWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UCS_WindowContainerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(RootCanvasPanel);
}

void UCS_WindowContainerWidget::AddWidgetToPanel(UWidget* Widget)
{
	if (!RootCanvasPanel)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Canvas wasn't created! Aboarting add to panel."), *GetName());
		return;
	}

	if (UCanvasPanelSlot* CanvasSlot = RootCanvasPanel->AddChildToCanvas(Widget))
	{
		CanvasSlot->SetPosition(FVector2D(0));
		CanvasSlot->SetSize(FVector2D(0));
		CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));

		UE_LOG(LogTemp, Log, TEXT("%s: Widget %s added to canvas."), *GetName(), *Widget->GetName());
	}
}
