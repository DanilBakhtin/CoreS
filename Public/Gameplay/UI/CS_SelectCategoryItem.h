
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_SelectCategoryItem.generated.h"

class UWidgetSwitcher;
class UButton;
class UTextBlock;
class UImage;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSelectCategoryItem, const FString&, Id, UCS_SelectCategoryItem*, SelectCategoryItem, bool, IsChecked);

UCLASS()
class CORE_SIMULATOR_API UCS_SelectCategoryItem : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "CS_SelectCategoryItem", meta = (BindWidget))
	TObjectPtr<UTextBlock> Description_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_SelectCategoryItem", meta = (BindWidget))
	TObjectPtr<UButton> Category_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_SelectCategoryItem", meta = (BindWidget))
	TObjectPtr<UImage> SelectedIcon_Image;
	UPROPERTY(BlueprintReadOnly, Category = "CS_SelectCategoryItem", meta = (BindWidget))
	TObjectPtr<UImage> Background_Border;
	UPROPERTY(BlueprintReadOnly, Category = "CS_SelectCategoryItem", meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Select_WidgetSwitcher;
	
	void InitCategory(const FString& Id, const FString& Description);

	UPROPERTY(BlueprintAssignable)
	FOnSelectCategoryItem OnSelectCategoryItem;

	void CheckMarker(const FString& IdForCheck) const;
	void Uncheck();
protected:

	UFUNCTION()
	void SelectCategory();

	bool IsChecked = false;
	FString Id = "";
};
