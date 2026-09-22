// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "CS_UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_ShowMessage.generated.h"

class USizeBox;
class UTextBlock;
class UWidgetSwitcher;

class UCS_BaseButton;

UCLASS()
class CORE_SIMULATOR_API UCS_ShowMessage : public UCS_UserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ShowMessage")
	TObjectPtr<USizeBox> ShowMessageIcon_SizeBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ShowMessage")
	TObjectPtr<UWidgetSwitcher> ShowMessage_WidgetSwitcher;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ShowMessage", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton>	Close_Button;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ShowMessage")
	TObjectPtr<UTextBlock> Title_TextBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ShowMessage")
	TObjectPtr<UTextBlock> Text_TextBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ShowMessage")
	TObjectPtr<UTextBlock> Footer_TextBlock;

	
	void SetCloseShowMessageFunc(TFunction<void()> Func);

	void InitParams(const FShowMessageParams& ShowMessageParams) const;
protected:

	UFUNCTION()
	void CloseShowMessage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ShowMessage")
	FText BaseTitleText;
	
	TFunction<void()> CloseShowMessageFunc;
};
