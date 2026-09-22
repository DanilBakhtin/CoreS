
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_ContextItemButton.generated.h"

class UButton;
class UImage;
class UTextBlock;

UCLASS()
class CORE_SIMULATOR_API UCS_ContextItemButton : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_ContextItemButton", meta = (BindWidget))
	TObjectPtr<UImage> Context_Image;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ContextItemButton", meta = (BindWidget))
	TObjectPtr<UTextBlock> Context_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ContextItemButton", meta = (BindWidget))
	TObjectPtr<UButton> Context_Button;

	void InitContextMenuItem(const FContextMenuItem& ContextMenuItem);
	void SetCloseContextMenuFun(TFunction<void()> CloseContextMenuFun);
	void SetClickOnObject(TFunction<void(const FString& Id, const FString& Type)> ClickOnItemFun);

	
protected:

	UFUNCTION()
	void OnClickItem();
	
	FString Id = "";
	FString Type = "";
	
	TFunction<void()> CloseContextMenuFun;
	TFunction<void(const FString& Id, const FString& Type)> ClickOnItemFun;
};
