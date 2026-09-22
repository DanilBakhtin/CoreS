
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_ResultCategory.generated.h"


class UWidgetSwitcher;
class UBorder;
class UTextBlock;
class UButton;
class UImage;

UCLASS()
class CORE_SIMULATOR_API UCS_ResultCategory : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ResultCategory", meta = (BindWidget))
	TObjectPtr<UButton> Category_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ResultCategory", meta = (BindWidget))
	TObjectPtr<UBorder>	Category_Border;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ResultCategory", meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Category_WidgetSwitcher;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ResultCategory", meta = (BindWidget))
	TObjectPtr<UTextBlock> Category_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ResultCategory", meta = (BindWidget))
	TObjectPtr<UImage> Category_Image;
	
	void Init(const FResultParams& ResultParams);

	void SetShowDescriptionFun(TFunction<void(UCS_ResultCategory* ResultCategory)> Fun);

	void Check() const;
	void Uncheck() const;

	FResultParams GetResultParams() const;
protected:

	UFUNCTION()
	void ClickOnCategory();
	
	FResultParams ResultParams;

	TFunction<void(UCS_ResultCategory* ResultCategory)> ShowDescriptionFun;
};
