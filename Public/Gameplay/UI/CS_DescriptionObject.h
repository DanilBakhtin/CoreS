// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_DescriptionObject.generated.h"

class UTextBlock;
class USizeBox;

UCLASS()
class CORE_SIMULATOR_API UCS_DescriptionObject : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_InfoQues", meta = (BindWidget))
	TObjectPtr<UTextBlock> Description_TextBlock;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_InfoQues", meta = (BindWidget))
	TObjectPtr<USizeBox> DescriptionBorder_SizeBox;
			
	UFUNCTION()
	void SetDescriptionText(const FString& Text) const;

};
