
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_RadioListItem.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickContactButton, FDialogButtonParameters, Params);

UCLASS()
class CORE_SIMULATOR_API UCS_RadioListItem : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_RadioListItem")
	TObjectPtr<UTextBlock> Title_TextBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_RadioListItem")
	TObjectPtr<UButton> Dialog_Button;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* AnimationCall;
	
	UPROPERTY(BlueprintAssignable, Category = "CS_RadioListItem")
	FOnClickContactButton OnClickContactButton;

	void InitRadioList(const FDialogButtonParameters& Params);

	void Calling(bool bCalling);

	
protected:

	FDialogButtonParameters DialogParameters = FDialogButtonParameters();

	UFUNCTION()
	void ClickOnDialogButton();
};
