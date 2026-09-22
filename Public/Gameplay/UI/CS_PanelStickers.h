// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_PanelStickers.generated.h"

class USizeBox;
class UWrapBox;

class UCS_LearningSticker;
class UCS_ShowReaction;

UCLASS(Blueprintable)
class CORE_SIMULATOR_API UCS_PanelStickers : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	virtual void NativePreConstruct() override;

	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "CS_PanelStickers", meta = (BindWidget))
	TObjectPtr<USizeBox> LearningStickerBox;

	UPROPERTY(BlueprintReadOnly, Category = "CS_PanelStickers", meta = (BindWidget))
	TObjectPtr<USizeBox> ReactionsBox;

	UPROPERTY(BlueprintReadOnly, Category = "CS_PanelStickers", meta = (BindWidget))
	TObjectPtr<UWrapBox> MainContainer;

	UPROPERTY(BlueprintReadOnly, Category = "CS_PanelStickers", meta = (BindWidget))
	TObjectPtr<UWrapBox> ReactionContainer;

	UPROPERTY(BlueprintReadOnly, Category = "CS_PanelStickers", meta = (BindWidget))
	TObjectPtr<UCS_LearningSticker> LearningSticker;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* OpenLearningSticker;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* CloseLearningSticker;



	UPROPERTY(EditAnywhere, Category = "CS_PanelStickers")
	float TimeLifeLearningSticker = 3.0f;

	UPROPERTY(EditAnywhere, Category = "CS_PanelStickers")
	float PlayRateLearningAnimation = 2.0f;

	UPROPERTY(EditAnywhere, Category = "CS_PanelStickers")
	float MinTimeLifeReaction = 2.0f;

	UPROPERTY(EditAnywhere, Category = "CS_PanelStickers")
	float MaxTimeLifeReaction = 10.0f;

	UPROPERTY(EditAnywhere, Category = "CS_PanelStickers")
	int MinReactionText = 20;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_PanelStickers")
	float BaseTopPadding = 76.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_PanelStickers")
	float ArmTopPadding = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_PanelStickers")
	float ReactionBoxBaseHeight = 829.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UCS_ShowReaction> ShowReactionClass;

	UFUNCTION()
	void SetIsArm(bool bArmActive);

	UFUNCTION()
	void UpdateCondition();

	UFUNCTION()
	void ShowLearningSticker(const FStickerParams& StickerParams);
	
	void SetCloseStickerFun(TFunction<void()> InputFun);

	UFUNCTION()
	void AddShowReaction(const FString& Description);

	void SetCloseReactionFun(TFunction<void()> InputFun);

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_PanelStickers")
	bool bIsArm = false;

	UFUNCTION()
	void StartTimerCloseSticker();

	UFUNCTION()
	void HideLearningSticker();

	UFUNCTION()
	void CloseSticker();

	UFUNCTION()
	void PlayStickerAnimation(UWidgetAnimation* Animation);

	UFUNCTION()
	void OnOpenLearningAnimationFinished();

	UFUNCTION()
	void OnCloseLearningAnimationFinished();

	TFunction<void()> CloseStickerFun;

	UPROPERTY()
	FTimerHandle LearningStickerTimer;

	UPROPERTY()
	FStickerParams CachedStickerParams;

	UPROPERTY()
	bool bManualHideSticker = false;

	TFunction<void()> CloseReactionFun;

	UFUNCTION()
	void OnReactionRemoved(UCS_ShowReaction* ReactionWidget);

};
