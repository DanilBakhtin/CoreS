#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_ObjectMenu.generated.h"

class UButton;
class UCanvasPanel;
class USizeBox;
class UTextBlock;
class UVerticalBox;

class UCS_ObjectMenuElement;

class ACS_PlayerController;

UCLASS(Abstract)
class CORE_SIMULATOR_API UCS_ObjectMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ObjectMenu", DisplayName = "Object Menu Element Class")
	TSubclassOf<UCS_ObjectMenuElement> ObjectMenuElementClass;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_ObjectMenu", DisplayName = "Object Menu Canvas Panel", meta = (BindWidget))
	TObjectPtr<UCanvasPanel> ObjectMenu_CanvasPanel;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ObjectMenuElement", DisplayName = "Object Menu Text Block Name", meta = (BindWidget))
	TObjectPtr<UTextBlock> ObjectMenuElement_TextBlock_Name;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ObjectMenuElement", DisplayName = "Object Menu Text Block", meta = (BindWidget))
	TObjectPtr<UTextBlock> ObjectMenuElement_TextBlock;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ObjectMenu", DisplayName = "Object Menu Background Button", meta = (BindWidget))
	TObjectPtr<UButton> ObjectMenu_BackgroundButton;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_ObjectMenu", DisplayName = "Object Menu Clear Button", meta = (BindWidget))
	TObjectPtr<UButton> ObjectMenu_ClearButton;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ObjectMenu", DisplayName = "Object Menu Size Box", meta = (BindWidget))
	TObjectPtr<USizeBox> ObjectMenu_SizeBox;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ObjectMenu", DisplayName = "Object Menu Wrap Box", meta = (BindWidget))
	TObjectPtr<UVerticalBox> ObjectMenu_VerticalBox;

	UPROPERTY(BlueprintReadWrite)
	bool bIsClosed = false;
	
	void SetPlayerController(ACS_PlayerController* NewPlayerController);
	void SetCloseObjectMenuFun(const TFunction<void()>& NewCloseObjectMenuFunction);
	void SetReleasedOnObject(
		const TFunction<void(const FString& CastedToStringAttribute)>& NewClickOnElementFunction);

	// Initialize object menu function
	virtual void InitObjectMenuElements(const FFullMenuReasons& FullMenuReasons);
	
protected:
	bool bIsNotChosenDefect = true;
	FString ObjectAPI_Id = "";
	
	TFunction<void()> CloseObjectMenuFunction;
	TFunction<void(const FString& CastedToStringAttribute)> ClickOnElementFunction;
	
	UPROPERTY()
	TWeakObjectPtr<ACS_PlayerController> PlayerController;
	
	// Function that close menu
	UFUNCTION()
	virtual void OnReleasedEmptyPlace();

	// Function that clear object 
	UFUNCTION()
	virtual void OnReleasedClearButton();

	//~ Begin initialize object menu functions.
	virtual void CreateChildren(const FFullMenuReasons& ObjectReason);
	virtual void SetWidgetViewportPosition();
	//~ End initialize object menu functions.
};
