// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_Description_Mouse.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Gameplay/UI/CS_DescriptionObject.h"

void UCS_Description_Mouse::NativeConstruct()
{
	Super::NativeConstruct();

	check(Description_CanvasPanel);
	check(DescriptionObjectWidget);
}

void UCS_Description_Mouse::SetPlayerController(ACS_PlayerController* _PlayerController)
{
	PlayerController = _PlayerController;
}

void UCS_Description_Mouse::UpdateDescriptionPosition() const
{
	if(!PlayerController.IsValid())
		return;
	FIntVector2 ViewportSize = FIntVector2();
	PlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);
	FVector2D MousePosition = FVector2D();
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);

	bool OnLeftSide = ViewportSize.X <= MousePosition.X + DescriptionObjectWidget->GetDesiredSize().X + 16;
	bool OnTopSide = ViewportSize.Y <= MousePosition.Y + DescriptionObjectWidget->GetDesiredSize().Y + 16;
	
	float XPosition;
	float YPosition;
	
	if(OnLeftSide)
		XPosition =  MousePosition.X/ViewportSize.X;
	else
		XPosition =  (MousePosition.X + 16.f)/ViewportSize.X;
	
	if(OnTopSide)
		YPosition =  MousePosition.Y /ViewportSize.Y;
	else
		YPosition =  (MousePosition.Y + 16.f)/ViewportSize.Y;

	const float AlignmentX = OnLeftSide >= true ? 1.f : 0.f;
	const float AlignmentY = OnTopSide >= true ? 1.f : 0.f;
	
	if(UCanvasPanelSlot* DescriptionObjectSlot = Cast<UCanvasPanelSlot>(DescriptionObjectWidget->Slot))
	{
		DescriptionObjectSlot->SetAlignment(FVector2D(AlignmentX, AlignmentY));
		DescriptionObjectSlot->SetAnchors(FAnchors(XPosition, YPosition, XPosition, YPosition));
		DescriptionObjectSlot->SetPosition(FVector2D());
	}
}

void UCS_Description_Mouse::ShowTargetDescription(const FString& ObjectId, const FString& Description) 
{

	if(Description != "")
	{
		DescriptionObjectWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		DescriptionObjectWidget->SetDescriptionText(Description);
		CurrentDescriptionObjectId = ObjectId;
	}
	if(CurrentDescriptionObjectId == ObjectId && Description == "")
	{
		DescriptionObjectWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCS_Description_Mouse::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(!DescriptionObjectWidget->IsVisible())
		return;

	UpdateDescriptionPosition();
}
