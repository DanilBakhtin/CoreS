#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_ObjectMenuElement.generated.h"

class ACS_PlayerController;
class UButton;
class UCheckBox;
class UTextBlock;

UCLASS(Abstract)
class CORE_SIMULATOR_API UCS_ObjectMenuElement : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite)
	bool bIsClosed = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_ObjectMenuElement", DisplayName = "Object Menu Element Check Box", meta = (BindWidget))
	TObjectPtr<UCheckBox> ObjectMenuElement_CheckBox;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_ObjectMenuElement", DisplayName = "Object Menu Element Text Block", meta = (BindWidget))
	TObjectPtr<UTextBlock> ObjectMenuElement_TextBlock;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_ObjectMenuElement", DisplayName = "Object Menu Element Button", meta = (BindWidget))
	TObjectPtr<UButton> ObjectMenuElement_Button;

	virtual void InitObjectMenuElement(const FString& NewObjectAPI_Id, const FFullMenuReasons& FullMenuReasons);
	void SetCloseObjectMenuFunction(const TFunction<void()>& NewCloseObjectMenuFunction);
	void SetClickOnObject(const TFunction<void(const FString& CastedToStringAttribute)>& NewClickOnElementFunction);
	void SetPlayerController(ACS_PlayerController* NewPlayerController);

protected:

	// Function that send checked object to AOS
	UFUNCTION()
	virtual void OnClickElement();

	bool bIsChecked = false;
	FString ObjectAPI_Id = "";
	FString ReasonAPI_Id = "";
	
	TFunction<void()> CloseObjectMenuFunction;
	TFunction<void(const FString& CastedToStringAttribute)> ClickOnElementFunction;

	UPROPERTY()
	TWeakObjectPtr<ACS_PlayerController> PlayerController;

private:
	virtual void CallCloseObjectMenu() const;
};
