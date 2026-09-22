// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/UI/CS_ActionLog.h"
#include "CS_LogItem.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class CORE_SIMULATOR_API UCS_LogItem : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	 
	UPROPERTY(BlueprintReadOnly, Category = "CS_LogItem", meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(BlueprintReadOnly, Category = "CS_LogItem", meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(BlueprintReadOnly, Category = "CS_LogItem", meta = (BindWidget))
	TObjectPtr<UImage> LeftLineImage;

	UPROPERTY(BlueprintReadOnly, Category = "CS_LogItem", meta = (BindWidget))
	TObjectPtr<UTextBlock> TextTime;

	UPROPERTY(BlueprintReadOnly, Category = "CS_LogItem", meta = (BindWidget))
	TObjectPtr<UTextBlock> TextInfo;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* OpenAnimation;

	void setVisualInfo(const FLogItemVisualInfo& VisualInfo);

	void SetTextTime(const FString& NewText) const;

	void SetTextInfo(const FString& NewText) const;

	void CollapseImageIcon() const;

	void PlayOpenAnimation();
};
