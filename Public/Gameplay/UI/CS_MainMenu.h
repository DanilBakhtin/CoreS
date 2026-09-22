// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "CS_UserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_MainMenu.generated.h"


class UTextBlock;
class UBackgroundBlur;
class UListView;
class USizeBox;
class UWidgetSwitcher;
class UWrapBox;
class UBorder;
class UVerticalBox;

class UCS_SelectCategoryItem;
class UCS_ShowScheme;
class UCS_ButtonMainMenu;
class UCS_BaseButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowImageViewer, bool, bShow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCheckItem, const FString&, apiId);

UCLASS(Blueprintable)
class CORE_SIMULATOR_API UCS_ListViewItem : public UObject
{
	GENERATED_BODY()

public:
	FAnswerButton AnswerButton = FAnswerButton();
	void Init(FAnswerButton AnswerButton);
	FAnswerButton GetAnswerButtonData(bool& bEquipped);

	UPROPERTY(BlueprintAssignable)
	FUpdateItem UpdateItem;
	UPROPERTY(BlueprintAssignable)
	FUpdateItem Collapse;

	void SetMainMenu(UCS_MainMenu* MainMenu);
	UCS_MainMenu* GetMainMenu() const { return MainMenu; }
	void SetCollapse();
	bool GetIsCollapsed() const { return bIsCollapsed; }

private:
	UPROPERTY()
	TObjectPtr<UCS_MainMenu> MainMenu;

	bool bIsCollapsed;
};

UCLASS()
class CORE_SIMULATOR_API UCS_MainMenu : public UCS_UserWidget
{
	GENERATED_BODY()

public:

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;
	
	//Widgets
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UWidget> Main_SizeBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UWidget> Response_SizeBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UWidget> Quest_SizeBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UWidget> ConfirmExit_SizeBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UCS_ShowScheme> Help_ImageViewer;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UCS_ShowScheme> ImageViewer;

	//Core widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_MainMenu")
	TSubclassOf<UCS_ButtonMainMenu> ButtonMainMenuWidgetClass;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UVerticalBox> ContainerButtons;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UVerticalBox> ContainerDynamicButtons;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Menu_WidgetSwitcher;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton> Exit_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton> CloseMain_Button;

	UPROPERTY(BlueprintAssignable)
	FOnShowImageViewer OnShowImageViewer;

	void InitMainMenu(const FMainMenuParams& _MainMenuParams);
	
	//Reasons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_MainMenu")
	TSubclassOf<UCS_SelectCategoryItem> SelectCategoryItemClass;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton> CloseReasons_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton> MakeReasons_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UWrapBox> CategoryReasons_WrapBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UBorder> Answer_Border;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UListView> Answer_ListView;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UBackgroundBlur> Menu_BackgroundBlur;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UTextBlock> BalanceOfResponses_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton>	QuestBack_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UTextBlock> Quest_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UTextBlock> Quest_Footer_TextBlock;

	//Buttons
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UCS_ButtonMainMenu> Button_Reasons;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UCS_ButtonMainMenu> Button_Quest;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UCS_ButtonMainMenu> Button_Help;

	UPROPERTY(BlueprintAssignable)
	FCheckItem CheckItem;
	UFUNCTION()
	void SelectCategory(const FString& CategoryId, UCS_SelectCategoryItem* SelectCategoryItem, bool IsChecked);
	void MakeItemList(const FString& Category);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_MainMenu")
	FName KeyAlgorithm;

	//Confirm exit
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleTextConfirmExit_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UTextBlock> QuestTextConfirmExit_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UTextBlock> TextConfirmExit_TextBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton>	CloseConfirmExit_Button;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_MainMenu", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton>	ConfirmExit_Button;

	void SetReasonActionEventFun(TFunction<void(const FString& AnswerId)> Fun);
	void SetExitCourseFunc(TFunction<void()> Func);
	void SetCloseMainMenuFunc(TFunction<void()> Func);
	void SetLockFun(TFunction<void()> Func);
	void SetIsLockedMenuFun(TFunction<bool()> Func);
	
	void SetAnswer(const FString& AnswerCategoryId, const FString& AnswerId);

	void ResetMenu();
	
	UFUNCTION()
	void CloseMainMenu();

	void InitQuestInfo(const FShowWelcomeParams& Params);
	
	void InitHelpImage(TArray<TSoftObjectPtr<UTexture2D>> Textures) const;

	UFUNCTION()
	void HideLoadingWidget(bool bHide);
	
protected:
	
	UFUNCTION()
	void Exit();	

	UFUNCTION()
	void ReturnToMainMenu();
	UFUNCTION()
	void MakeReasons();

	UFUNCTION()
	void OpenConfirmExit();

	UFUNCTION()
	void OpenImageWidget(const FName NamePoint);
	UFUNCTION()
	void OpenReasons(const FName NamePoint);
	UFUNCTION()
	void OpenHelp(const FName NamePoint);
	UFUNCTION()
	void OpenQuest(const FName NamePoint);
	
	TFunction<void()> ExitCourseFunc;
	TFunction<void()> CloseMainMenuFunc;
	TFunction<void(const FString& AnswerId)> ReasonActionEventFun;
	TFunction<void()> LockFunc;
	TFunction<bool()> IsLockedMenuFunc;
	
	UPROPERTY()
	TMap<FString, FAnswerButtonArray> Answers;

	UPROPERTY()
	TArray<UCS_ListViewItem*> ListEquipItems;

	FString SelectedAnswerId = "";
	int32 BalanceOfResponses = 2;

	UPROPERTY()
	FMainMenuParams MainMenuParams;
};
