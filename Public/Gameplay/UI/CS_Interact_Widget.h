#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_Interact_Widget.generated.h"

class UImage;
class UCanvasPanel;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWidgetInteractPointAction, const FString&, apiId, const EPointAction&,
	PointAction);

UCLASS(Blueprintable, Abstract)
class CORE_SIMULATOR_API UCS_Interact_Widget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintAssignable)
	FWidgetInteractPointAction WidgetInteractPointAction;

	UPROPERTY()
	FObjectViewID ViewStruct;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "CS_TimeAndLocation", meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Container;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CS_Interact)
	UObject* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CS_Interact)
	FLinearColor DefaultColor = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CS_Interact)
	FLinearColor HoveredColor = FColor(185, 185, 185, 255);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CS_Interact)
	FLinearColor PressedColor = FColor(185, 185, 185, 255);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CS_Interact)
	FString ActionID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CS_Interact)
	FString ViewID;

	UFUNCTION(BlueprintCallable)
	void ChangeImage(UObject* NewImage);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY()
	UImage* WidgetImage = nullptr;


public:
	[[nodiscard]] FString GetViewID() const
	{
		return ViewID;
	}

	[[nodiscard]] FString GetActionID() const
	{
		return ActionID;
	}
};
