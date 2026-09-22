// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_DialogSendMessage.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickSendMessageButton, FDialogParameters, Params);

UCLASS()
class CORE_SIMULATOR_API UCS_DialogSendMessage : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "CS_DialogSendMessage", meta = (BindWidget))
	TObjectPtr<UTextBlock> Message_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_DialogSendMessage", meta = (BindWidget))
	TObjectPtr<UButton> Message_Button;

	UPROPERTY(BlueprintAssignable, Category = "CS_DialogSendMessage")
	FOnClickSendMessageButton OnClickSendMessageButton;

	void InitDialogMessage(const FDialogParameters& Params);
	
protected:

	FDialogParameters ButtonParameters = FDialogParameters();
	
	UFUNCTION()
	void ClickOnDialogButton();
};
