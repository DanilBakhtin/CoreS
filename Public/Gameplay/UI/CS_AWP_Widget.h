#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_AWP_Widget.generated.h"

class UCS_Interact_Widget;
class UCanvasPanelSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWidgetPointAction, const FString&, apiId, const EPointAction&,
											 PointAction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitWidgets);

/**
 * Automated workplace (AWP) widget that implements the OS interface and the program installed in it.
 * Use CS_Interact_Button for interaction.
 */
UCLASS(Blueprintable, Abstract)
class CORE_SIMULATOR_API UCS_AWP_Widget : public UUserWidget
{
	GENERATED_BODY()

public:	
	virtual void NativePreConstruct() override {};
	
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintAssignable)
	FWidgetPointAction WidgetPointAction;

	UPROPERTY(BlueprintAssignable)
	FOnInitWidgets OnInitWidgets;
	
	UFUNCTION()
	bool UpdateActiveKey(const FObjectViewID& StructObjectViewID);

	UFUNCTION()
	void HandleButtonAction(const FString& apiId, const EPointAction& PointAction);

	UFUNCTION()
	void HideAllInteractWidgets();

	UFUNCTION()
	TArray<FString> GetViewIDs();

	UFUNCTION()
	void InitSize(const FVector2D TargetSize);

protected:
	UPROPERTY()
	TMap<FString, UCS_Interact_Widget*> InteractWidgetsMap;

	UPROPERTY()
	FVector2D RatioSize = FVector2D(1.0f, 1.0f);

	UPROPERTY()
	int MaxZOrder = -1;

	UFUNCTION()
	void UpdateWidgetSlot(UCanvasPanelSlot* ContextObjectSlot, const FVector2D TargetSize, const FVector2D TargetPosition, const int ZOrder);

	UFUNCTION()
	void PostZOrderWidgets();

private:
	UPROPERTY()
	FVector2D BaseSize = FVector2D(1920.0f, 1200.0f);

	UPROPERTY()
	bool bInitSize = false;
};
