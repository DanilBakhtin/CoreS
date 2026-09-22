// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "CS_UserWidget.h"
#include "CS_HUDWidget.generated.h"

class UCanvasPanel;
class UHorizontalBox;
class UCS_AWP_Widget;
class UCS_ButtonWidget;
class UCS_CheckWidget;
class UCS_ZoomInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCheckedChangeDynamicButton, FGameplayTag, ActiveTag, bool, bIsChecked);

UCLASS()
class CORE_SIMULATOR_API UCS_HUDWidget : public UCS_UserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUDWidget")
	TSubclassOf<UCS_CheckWidget> CheckWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUDWidget")
	FMargin PaddingBtwHotKeys = FMargin(0.0f, 0.0f, 37.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUDWidget")
	bool bIsTabHidden = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_HUDWidget", meta = (BindWidget))
	TObjectPtr<UCS_ButtonWidget> TabButton;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_HUDWidget", meta = (BindWidget))
	TObjectPtr<UCS_CheckWidget> Quest_HudCheck;

	UPROPERTY(BlueprintReadOnly, Category = "CS_HUDWidget", meta = (BindWidget))
	TObjectPtr<UCS_CheckWidget> Info_HudCheck;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_HUDWidget", meta = (BindWidget))
	TObjectPtr<UCS_CheckWidget> Log_HudCheck;

	UPROPERTY(BlueprintReadOnly, Category = "CS_HUDWidget", meta = (BindWidget))
	TObjectPtr<UCS_CheckWidget> Helper_HudCheck;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_HUDWidget", meta = (BindWidget))
	TObjectPtr<UCS_ButtonWidget> Menu_HudButton;

	UPROPERTY(BlueprintReadOnly, Category = "CS_HUDWidget", meta = (BindWidget))
	TObjectPtr<UHorizontalBox> ContainerHotKeys;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_HUDWidget", meta = (BindWidget))
	TObjectPtr<UCanvasPanel> ContainerAWP;
	
	TArray<UCS_CheckWidget*> HotKeyWidgets;
	
	
	void UpdateCondition() const;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	//Нужно среагировать на нажатие кнопки Tab когад происходит фокусировка на виджете
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUDWidget")
	FKey CheckTabButton = FKey();
	
	virtual void GetUITagContainer(TFunction<FGameplayTagContainer()> Func) override;
	virtual void AddAddTag(TFunction<void(TArray<FGameplayTag> Tags)> Func) override;
	virtual void AddRemoveTag(TFunction<void(TArray<FGameplayTag> Tags)> Func) override;

	UPROPERTY(BlueprintAssignable, Category = "CS_HUDWidget")
	FCheckedChangeDynamicButton OnCheckedChancgeDynamicButton;

	UFUNCTION(BlueprintCallable, Category = "CS_HUDWidget")
	void EnableBlinkingToHotKeys(const int indexHotKey, const bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "CS_HUDWidget")
	void EnableBlinkingToHotKeysByTag(const FGameplayTag ActivatedTag, const bool bEnable);
	
	void AddAWP_Widget(UCS_AWP_Widget* AWP_Widget);
	void ClearAWPContainer();
	void CreateHotKeys();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUDWidget")
	FVector2D SizeAWP = FVector2D(1748.0f, 983.0f);

	UPROPERTY()
	TMap<FGameplayTag, UCS_CheckWidget*> MapCheckWidgets;

private:

	UFUNCTION()
	void OnDynamicButtonCheckedChange(FGameplayTag ActiveTag, bool bIsChecked);
};
