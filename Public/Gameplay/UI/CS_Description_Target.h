// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_Description_Target.generated.h"

class USizeBox;
class UCS_DescriptionObject;

UCLASS()
class CORE_SIMULATOR_API UCS_Description_Target : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_Description_Target", meta = (BindWidget))
	TObjectPtr<USizeBox> Description_SizeBox;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_Description_Target", meta = (BindWidget))
	TObjectPtr<UCS_DescriptionObject> DescriptionObject;

	void ShowTargetDescription(const FString& Description) const;
};
