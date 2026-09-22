// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/UI/CS_UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_DialogWidget.generated.h"

class UButton;
class UWrapBox;
class UScrollBox;
class UTextBlock;
class UImage;

class UCS_DialogSendMessage;
class UCS_ReceivedMessage;
class UCS_SendedMessage;
class UCS_NestedItemDialog;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSendMessage, FDialogParameters, Params);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogWidgetClose);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReturnToDialogs);

UCLASS()
class CORE_SIMULATOR_API UCS_DialogWidget : public UCS_UserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	virtual void InitShowDialog(FString nameTitle, const TArray<FDialogParameters>& Params);
	virtual void InitAddMessage(const TArray<FDialogParameters>& Params);
	virtual void InitUpdateDialog(const TArray<FDialogParameters>& Params);
	void UpdateNestingList(const TArray<FString>& NameList);
	void ClearNestingList();
	void UpdateIcon(UTexture2D* NewTexture);

	UPROPERTY(BlueprintAssignable, Category = "CS_DialogWidget")
	FOnSendMessage OnSendMessage;

	UPROPERTY(BlueprintAssignable, Category = "CS_DialogWidget")
	FOnDialogWidgetClose OnDialogWidgetClose;

	UPROPERTY(BlueprintAssignable, Category = "CS_DialogWidget")
	FOnReturnToDialogs OnReturnToDialogs;

	UFUNCTION()
	void ShowReturnToContactsButton(bool bShow);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_DialogWidget")
	TSubclassOf<UCS_DialogSendMessage> DialogSendMessage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_DialogWidget")
	TSubclassOf<UCS_ReceivedMessage> ReceivedMessageClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_DialogWidget")
	TSubclassOf<UCS_SendedMessage> SendedMessageClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_DialogWidget")
	TSubclassOf<UCS_NestedItemDialog> NestedItemClass;

	UPROPERTY(BlueprintReadOnly, Category = "CS_DialogWidget", meta = (BindWidget))
	TObjectPtr<UButton> ReturnToContactsButton;
	UPROPERTY(BlueprintReadOnly, Category = "CS_DialogWidget", meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	UPROPERTY(BlueprintReadOnly, Category = "CS_DialogWidget", meta = (BindWidget))
	TObjectPtr<UTextBlock> TextNameUser;

	UPROPERTY(BlueprintReadOnly, Category = "CS_DialogWidget", meta = (BindWidget))
	TObjectPtr<UWrapBox> Dialog_WrapBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_DialogWidget", meta = (BindWidget))
	TObjectPtr<UScrollBox> Dialog_ScrollBox;

	UPROPERTY(BlueprintReadOnly, Category = "CS_DialogWidget", meta = (BindWidget))
	TObjectPtr<UWrapBox> Request_WrapBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_DialogWidget", meta = (BindWidget))
	TObjectPtr<UScrollBox> Request_ScrollBox;

	UPROPERTY(BlueprintReadOnly, Category = "CS_DialogWidget", meta = (BindWidget))
	TObjectPtr<UWrapBox> NestedList_WrapBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_DialogWidget", meta = (BindWidget))
	TObjectPtr<UImage> Icon_Image;

	UPROPERTY()
	TArray<UCS_DialogSendMessage*> DialogSendItems;

	void AddDudeMSG(const FString& Text) const;
	void AddUserMSG(const FString& Text) const;
	void AddRequestItem(const FDialogParameters& Params);

	void ScrollDown() const;

	UFUNCTION()
	void ReturnToContacts();

	UFUNCTION()
	void CloseDialog();

	UFUNCTION()
	void OnClickSendMessage(FDialogParameters Params);

	UPROPERTY()
	UTexture* DefaultIconTexture;
};
