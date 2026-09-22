// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_NestedItemDialog.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class CORE_SIMULATOR_API UCS_NestedItemDialog : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetDescriptionText(const FString& Description);

	UFUNCTION()
	void SetShowArrowImage(bool bShow);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "CS_NestedItemDialog", meta = (BindWidget))
	TObjectPtr<UImage> ArrowImage;

	UPROPERTY(BlueprintReadOnly, Category = "CS_NestedItemDialog", meta = (BindWidget))
	TObjectPtr<UTextBlock> TextDescription;
};
