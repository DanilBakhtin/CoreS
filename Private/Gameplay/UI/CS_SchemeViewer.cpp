// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_SchemeViewer.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScaleBox.h"

void UCS_SchemeViewer::NativeConstruct()
{
	Super::NativeConstruct();

	MarkerSizeMultiplier = 1.f;

	check(SchemeImage);
}

void UCS_SchemeViewer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	ZoomSchemeOnMouseWheel(MyGeometry, InDeltaTime);
}

FReply UCS_SchemeViewer::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	bLeftMouseButtonPressed = (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton);

	return bLeftMouseButtonPressed ? FReply::Handled() : FReply::Unhandled();
}

void UCS_SchemeViewer::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	bLeftMouseButtonPressed = false;
}

FReply UCS_SchemeViewer::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	bLeftMouseButtonPressed = !(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton);

	return bLeftMouseButtonPressed? FReply::Unhandled() : FReply::Handled();
	
}

FReply UCS_SchemeViewer::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
	
	if (bZoomEnable)
		StartZoomingSchemeOnMouseWheel(InMouseEvent.GetWheelDelta());

	return FReply::Handled();
}

FReply UCS_SchemeViewer::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	MovingSchemeOnMouseMove(InGeometry, InMouseEvent);

	return FReply::Handled();
}

FVector2D UCS_SchemeViewer::ClampSchemePosition(FVector2D NewPosition)
{
	const float PosX = SchemeImage->GetDesiredSize().X * (CurrentScale - 1) / 2;
	const float PosY = SchemeImage->GetDesiredSize().Y * (CurrentScale - 1) / 2;

	float X = FMath::Clamp(NewPosition.X, FMath::Min(PosX * -1.0f, PosX), FMath::Max(PosX * -1.0f, PosX));
	float Y = FMath::Clamp(NewPosition.Y, FMath::Min(PosY * -1.0f, PosY), FMath::Max(PosY * -1.0f, PosY));

	return FVector2D(X,Y);
}

void UCS_SchemeViewer::ZoomSchemeOnMouseWheel(const FGeometry& InGeometry, float InDeltaTime)
{
	if (bZoomingMap)
	{
		const float InterpedNewScaleSize = FMath::FInterpTo(CurrentScale, TargetScale, InDeltaTime, ZoomSpeed);
		SchemeImage->SetRenderScale(FVector2D(InterpedNewScaleSize, InterpedNewScaleSize));

		CurrentScale = InterpedNewScaleSize;

		SchemeImage->SetRenderTranslation(ClampSchemePosition(SchemeImage->GetRenderTransform().Translation));

		if (InterpedNewScaleSize == TargetScale)
		{
			bZoomingMap = false;
		}
	}
}

void UCS_SchemeViewer::StartZoomingSchemeOnMouseWheel(float MouseWheelDelta)
{
	TargetScale = FMath::Clamp(CurrentScale + MouseWheelDelta * ZoomStep, MinScale, MaxScale);

	OnZoomChanged.Broadcast(FMath::RoundToInt(TargetScale * 100.0f));

	bZoomingMap = true;
}

void UCS_SchemeViewer::MovingSchemeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bLeftMouseButtonPressed)
	{
		const FVector2D NewMapPosition = SchemeImage->GetRenderTransform().Translation + InMouseEvent.GetCursorDelta();

		SchemeImage->SetRenderTranslation(ClampSchemePosition(NewMapPosition));
	}
}

void UCS_SchemeViewer::SetTextureByImage(TSoftObjectPtr<UTexture2D> Texture)
{
	if (!Texture.IsNull())
	{
		UTexture2D* ImageTexture = Texture.LoadSynchronous();

		FSlateBrush Brush;
		Brush.SetResourceObject(ImageTexture);
		//Brush.ImageSize = FVector2D(ImageTexture->GetImportedSize().X, ImageTexture->GetImportedSize().Y);
		Brush.ImageSize = FVector2D(ImageTexture->GetPlatformData()->SizeX, ImageTexture->GetPlatformData()->SizeY);
		Brush.DrawAs = ESlateBrushDrawType::Image;

		SchemeImage->SetBrush(Brush);
		SchemeImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
		SchemeImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UCS_SchemeViewer::ReturnBaseSize()
{	
	TargetScale = 1.0f;
	CurrentScale = 1.0f;
	SchemeImageScale->SetRenderScale(FVector2D(1.0f, 1.0f));
	bZoomingMap = true;

	OnZoomChanged.Broadcast(FMath::RoundToInt(CurrentScale * 100.0f));
}

int UCS_SchemeViewer::GetCurrentZoomValue()
{
	return FMath::RoundToInt(TargetScale * 100.0f);
}
