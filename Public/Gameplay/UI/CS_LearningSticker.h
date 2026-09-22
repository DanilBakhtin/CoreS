// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_LearningSticker.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class CORE_SIMULATOR_API UCS_LearningSticker : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_LearningSticker")
	TObjectPtr<UMaterialInterface> RightMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_LearningSticker")
	TObjectPtr<UMaterialInterface> NotRightMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_LearningSticker")
	TSoftObjectPtr<UTexture2D> CheckAnswerIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_LearningSticker")
	TSoftObjectPtr<UTexture2D> UncheckAnswerIcon;

	UPROPERTY(BlueprintReadOnly, Category = "CS_LearningSticker", meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(BlueprintReadOnly, Category = "CS_LearningSticker", meta = (BindWidget))
	TObjectPtr<UImage> ImageAnswer;

	UPROPERTY(BlueprintReadOnly, Category = "CS_LearningSticker", meta = (BindWidget))
	TObjectPtr<UTextBlock> Sticker_MainTextBlock;

	UPROPERTY(BlueprintReadOnly, Category = "CS_LearningSticker", meta = (BindWidget))
	TObjectPtr<UTextBlock> Sticker_AdditionalTextBlock;

	void InitSticker(const FStickerParams& StickerParams);

protected:

	UFUNCTION()
	void ApplyStickerParams(const FStickerParams& StickerParams);

	UPROPERTY()
	FStickerParams CurrentStickerParams;

};
