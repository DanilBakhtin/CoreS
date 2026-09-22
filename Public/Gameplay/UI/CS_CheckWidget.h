// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "CS_UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_CheckWidget.generated.h"

class UTextBlock;
class UImage;
class UCheckBox;
class USizeBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCheckedChangeActive, FGameplayTag, ActiveTag, bool, bIsChecked);

UCLASS()
class CORE_SIMULATOR_API UCS_CheckWidget : public UCS_UserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	void UpdateConstruct() const;
	
	//Materials for Text
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_CheckWidget")
	TObjectPtr<UMaterialInterface> CheckedTextMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_CheckWidget")
	TObjectPtr<UMaterialInterface> UncheckedTextMaterial;

	//Class material for Icon Image
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_CheckWidget")
	TObjectPtr<UMaterialInterface> IconMaterialClass;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_CheckWidget")
	FText TitleText = FText::FromString("{Empty}");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_CheckWidget",
		meta=(AllowedClasses="/Script/Engine.Texture,/Script/Engine.MaterialInterface,/Script/Engine.SlateTextureAtlasInterface", DisallowedClasses = "/Script/MediaAssets.MediaTexture"))
	TObjectPtr<UObject> IconImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_CheckWidget")
	float SizeImage = 48.0f;

	UPROPERTY(BlueprintReadOnly, Category = "CS_CheckWidget", meta = (BindWidget))
	TObjectPtr<UImage> Button_Image;
	UPROPERTY(BlueprintReadOnly, Category = "CS_CheckWidget", meta = (BindWidget))
	TObjectPtr<UTextBlock> Title_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_CheckWidget", meta = (BindWidget))
	TObjectPtr<UCheckBox> CheckBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_CheckWidget", meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBoxImage;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag ActivatedTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FGameplayTag> UsedArray;

	void SetIsEnabledCheckBoxBox(bool bDisabled) const;
	bool GetIsEnabledCheckBoxBox(bool bDisabled) const;

	UFUNCTION()
	void CheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void UpdateButtonCondition(const FGameplayTagContainer& TagContainer) const;

	UFUNCTION()
	void Init(UObject* Icon, const FString& InputTitleText, const FHotkeySlot& HotKeysStruct);

	UPROPERTY(BlueprintAssignable, Category = "CS_CheckWidget")
	FCheckedChangeActive OnCheckedChangeWithActiveTag;

	UFUNCTION(BlueprintCallable, Category = "CS_CheckWidget")
	void EnableBlinking(const bool bEnable) const;

protected:
	
	UFUNCTION()
	void UpdateTextAndIcon(bool bIsChecked) const;

private:
	UPROPERTY()
	mutable TObjectPtr<UMaterialInstanceDynamic> DynamicIconMaterial;
};
