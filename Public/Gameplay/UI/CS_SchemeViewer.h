// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_SchemeViewer.generated.h"

class UCanvasPanel;
class UScaleBox;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImageZoomChanged, int, ZoomValue);

UCLASS(Abstract)
class CORE_SIMULATOR_API UCS_SchemeViewer : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	bool bLeftMouseButtonPressed;

	bool bZoomingMap;
	FVector2D TargetMapSize;

	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent);
	
	FVector2D ClampSchemePosition(FVector2D NewPosition);
	void ZoomSchemeOnMouseWheel(const FGeometry& InGeometry, float InDeltaTime);
	void StartZoomingSchemeOnMouseWheel(float MouseWheelDelta);
	void MovingSchemeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	
public:
	
	UPROPERTY(BlueprintReadOnly, Category = SchemeWidget, meta = (BindWidget))
	TObjectPtr<UScaleBox> SchemeImageScale;

	UPROPERTY(BlueprintReadOnly, Category = SchemeWidget, meta = (BindWidget))
	TObjectPtr<UImage> SchemeImage;
	
	/**If true, marker positions and a path will be updated every frame */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapSettings)
	bool bUpdateMapEveryFrame;

	/**This variable is responsible for the speed at which the map moves when the left mouse button is pressed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapSettings)
	float MoveRate = 1.f;

	/**This is the map zoom step */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapSettings)
	float ZoomStep = 0.25f;

	/**This variable is used when interpolating the map size when zooming. The smaller the value, the smoother the zoom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapSettings)
	float ZoomSpeed = 15.f;

	/**The maximum map size. Zooming changes the size of the map */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapSettings)
	float MaxScale = 4.0f;

	/**The minimum map size. Zooming changes the size of the map */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapSettings)
	float MinScale = 1.0f;

	/**Multiply the size of the marker by this value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapSettings)
	float MarkerSizeMultiplier;

	UFUNCTION(BlueprintCallable)
	void SetTextureByImage(TSoftObjectPtr<UTexture2D> Texture);


	UFUNCTION(BlueprintCallable)
	void ReturnBaseSize();

	UFUNCTION()
	int GetCurrentZoomValue();

	UPROPERTY()
	float CurrentScale = 1.0f;

	UPROPERTY()
	float TargetScale = 1.0f;

	UPROPERTY()
	bool bZoomEnable = true;

	UPROPERTY(BlueprintAssignable, Category = "CS_SchemeViewer")
	FOnImageZoomChanged OnZoomChanged;

};
