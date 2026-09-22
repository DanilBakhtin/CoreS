// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_ButtonMainMenu.generated.h"

class UCS_LocalizableTextBlock;
class USizeBox;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNameDispatherOpenWidget, FName, ReceivedName);

UCLASS()
class CORE_SIMULATOR_API UCS_ButtonMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ButtonMainMenu", meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ButtonMainMenu", meta = (BindWidget))
	TObjectPtr<UButton> Button;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ButtonMainMenu", meta = (BindWidget))
	TObjectPtr<UCS_LocalizableTextBlock> LocalizableTextBlock;

	UPROPERTY(BlueprintAssignable, Category = "CS_ButtonMainMenu")
	FNameDispatherOpenWidget OnClickActionName;

	UPROPERTY(EditAnywhere, Category = "CS_ButtonMainMenu")
	FButtonMainMenu ButtonData;

	void SetSizeWidget(float Width, float Height);

	void InitWidget(const FButtonMainMenu& Data);

	void InitPreviewWidget(const FButtonMainMenu& Data);

	UFUNCTION()
	void OnClickButton();

protected:

	UPROPERTY()
	FName ActionName;
};
