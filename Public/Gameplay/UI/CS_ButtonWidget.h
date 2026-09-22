// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "CS_UserWidget.h"
#include "Components/SizeBox.h"
#include "CS_ButtonWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;

UCLASS()
class CORE_SIMULATOR_API UCS_ButtonWidget : public UCS_UserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	void UpdateConstruct() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_CheckWidget")
	float SizeImage = 48.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_CheckWidget")
	FText TitleText = FText::FromString("{Empty}");
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CS_CheckWidget",  meta=(AllowedClasses="/Script/Engine.Texture,/Script/Engine.MaterialInterface,/Script/Engine.SlateTextureAtlasInterface", DisallowedClasses = "/Script/MediaAssets.MediaTexture"))
	TObjectPtr<UObject> IconImage;

	UPROPERTY(BlueprintReadOnly, Category = "CS_CheckWidget", meta = (BindWidget))
	TObjectPtr<UImage> Button_Image;
	UPROPERTY(BlueprintReadOnly, Category = "CS_CheckWidget", meta = (BindWidget))
	TObjectPtr<UTextBlock> Title_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_CheckWidget", meta = (BindWidget))
	TObjectPtr <USizeBox> SizeBoxImage;
	

	
	void UpdateButtonCondition(const FGameplayTagContainer& TagContainer) const;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ButtonWidget", meta = (BindWidget))
	TObjectPtr<UButton> Button;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag Activated;
	
};
