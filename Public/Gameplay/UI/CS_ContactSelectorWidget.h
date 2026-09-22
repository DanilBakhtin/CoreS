// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/UI/CS_UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_ContactSelectorWidget.generated.h"

class UButton;
class UWrapBox;

class UCS_RadioListItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContactSelect, FDialogButtonParameters, Params);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContactClose);

UCLASS()
class CORE_SIMULATOR_API UCS_ContactSelectorWidget : public UCS_UserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	void InitConctactList(const TArray<FDialogButtonParameters>& Parameters);

	UPROPERTY(BlueprintAssignable, Category = "CS_ContactSelectorWidget")
	FOnContactSelect OnContactSelect;

	UPROPERTY(BlueprintAssignable, Category = "CS_ContactSelectorWidget")
	FOnContactClose OnContactClose;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ContactSelectorWidget")
	TSubclassOf<UCS_RadioListItem> ContactListItemClass;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ContactSelectorWidget", meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ContactSelectorWidget", meta = (BindWidget))
	TObjectPtr<UWrapBox> ListContacts_WrapBox;

	UFUNCTION()
	void CloseContacts();

	UFUNCTION()
	void OnClickContact(FDialogButtonParameters Params);

private:

	TArray<UCS_RadioListItem*> CurrentContactItems;
};
