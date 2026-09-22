// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "Math/Color.h"
#include "CS_ActionLog.generated.h"

class UCS_LogItem;
class UCS_LearningSticker;
class UVerticalBox;
class UWidgetSwitcher;
class UButton;
class UScrollBox;
class USizeBox;

USTRUCT(BlueprintType)
struct FLogItemVisualInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* BackgroundMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor ColorLine;

	FLogItemVisualInfo()
		: Icon(nullptr)
		, BackgroundMaterial(nullptr)
		, ColorLine()
	{
	}
};

UCLASS()
class CORE_SIMULATOR_API UCS_ActionLog : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ActionLog")
	TMap<ELogType, FLogItemVisualInfo> VisualItemMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ActionLog")
	float PaddingBtwItems = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ActionLog")
	TSubclassOf<UCS_LogItem> LogItemClass;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ActionLog", meta = (BindWidget))
	TObjectPtr<UButton> btn_CloseLog;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ActionLog", meta = (BindWidget))
	TObjectPtr<UVerticalBox> MessageContainer;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ActionLog", meta = (BindWidget))
	TObjectPtr<UScrollBox> Content_ScrollBox;

	//Sticker
	UPROPERTY(BlueprintReadOnly, Category = "CS_PanelStickers", meta = (BindWidget))
	TObjectPtr<USizeBox> LearningStickerBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_PanelStickers", meta = (BindWidget))
	TObjectPtr<UCS_LearningSticker> LearningSticker;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* OpenLearningSticker;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* CloseLearningSticker;

	void SetCloseLogFunc(TFunction<void()> Func);

	void AddMessageLog(const FLogParams& LogParams);

	UFUNCTION()
	void ShowLearningSticker(const FStickerParams& StickerParams);

	UFUNCTION()
	void Reset();

protected:

	UPROPERTY(EditAnywhere, Category = "CS_ActionLog")
	float TimeLifeLearningSticker = 3.0f;

	UPROPERTY(EditAnywhere, Category = "CS_ActionLog")
	float PlayRateLearningSticker = 2.0f;

	TFunction<void()> CloseLogFunc;

	TMap<ELogType, TArray<UUserWidget*>> WidgetMap;

	UPROPERTY()
	FTimerHandle ActionLearningStickerTimer;

	UPROPERTY()
	FStickerParams CachedStickerParams;

	UPROPERTY()
	bool bManualHideSticker = false;

	UFUNCTION(BlueprintCallable, Category = "CS_ActionLog")
	void AddWidgetToMap(ELogType InputLogType, UUserWidget* WidgetToAdd);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CS_ActionLog")
	TArray<UUserWidget*> GetWidgetsByType(ELogType InputLogType) const;

	UFUNCTION()
	void SetVisabilityLogWidgets(const bool bNewVisability, const TArray<UUserWidget*> InputWidgets);

	//Вынести в статический класс
	UFUNCTION()
	FString GetTimeString(const FDateTime& DateTo);

	UFUNCTION()
	void CloseLog();

	UFUNCTION()
	void OnOpenLearningAnimationFinished();

	UFUNCTION()
	void OnCloseLearningAnimationFinished();

	UFUNCTION()
	void HideLearningSticker();
};
