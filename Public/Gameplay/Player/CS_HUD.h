// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "GameFramework/HUD.h"
#include "Framework/CS_CoreTypes.h"
#include "Gameplay/UI/CS_WindowContainerWidget.h"
#include "CS_HUD.generated.h"

class ACS_Character;
class ACS_PlayerController;
class UCS_MoveZone;
class UCS_HUDWidget;
class UCS_MainMenu;
class UCS_ShowMessage;
class UCS_ShowScheme;
class UCS_ShowQuest;
class UCS_ShowInfo;
class UCS_WaitStart;
class UCS_ShowWelcome;
class UCS_TimeAndLocation;
class UCS_ShowMessage;
class UCS_ShowResult;
class UCS_Description_Target;
class UCS_Description_Mouse;
class UCS_ContextMenu;
class UCS_ShowReaction;
class UCS_MeasureDevicesList;
class UCS_LearningSticker;
class UCS_ActionLog;
class UCS_ZoomInfo;
class UCS_HelpCellMode;
class UCS_AWP_Widget;
class UCS_MoveZone;
class UCS_ObjectMenu;
class UCS_PanelStickers;
class UCS_EndWorkingAreaWidget;
class UCS_ContactSelectorWidget;
class UCS_DialogWidget;
class UCS_DialogContainerWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateButtonCondition);

USTRUCT(BlueprintType)
struct FDialogWidgetStructure
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UCS_ContactSelectorWidget> ContactWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UCS_DialogWidget> DialogWidgetClass;

	FDialogWidgetStructure() {}
};

UENUM(BlueprintType)
enum class EDialogActionType : uint8
{
	None,
	OnClick,
	OnDialogPoint
};

USTRUCT(BlueprintType)
struct FDialogActionQueue
{
	GENERATED_BODY()

	FString API_Id = "";
	EDialogActionType Action = EDialogActionType::None;
	FString Description = "";

	FDialogActionQueue(const FString& _API_Id, const EDialogActionType& _Action)
	{
		API_Id = _API_Id;
		Action = _Action;
	}
	FDialogActionQueue(const FString& _API_Id, const EDialogActionType& _Action, const FString& _Description)
	{
		API_Id = _API_Id;
		Action = _Action;
		Description = _Description;
	}

	FDialogActionQueue() {};
};


UCLASS()
class CORE_SIMULATOR_API ACS_HUD : public AHUD
{
	GENERATED_BODY()

#pragma region Widgets

public:

#pragma region WidgetsClasses
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig HUDWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig MainMenuClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig ShowMessageClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig ShowSchemeClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig MeasureDevicesListClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig ShowQuestClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig ShowInfoClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig WaitStartClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig ShowWelcomeClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig TimeAndLocationClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig ShowResultClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig DescriptionTargetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig DescriptionMouseClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig ContextMenuClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig MoveZoneClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig PanelStickersClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig ActionLogClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig FadeWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig HelpCellModeWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig ObjectMenuWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig EndZoneWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig LockedBoundaryWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FWidgetConfig DialogContainerWidgetClass;
#pragma endregion
protected:

#pragma region WidgetObjects
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCS_HUDWidget> HUDWidget;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCS_MainMenu> MainMenuWidget;
	UPROPERTY()
	TObjectPtr<UCS_ShowScheme> SchemeWidget;
	UPROPERTY()
	TObjectPtr<UCS_MeasureDevicesList> MeasureDevicesListWidget;
	UPROPERTY()
	TObjectPtr<UCS_ShowQuest> QuestWidget;
	UPROPERTY()
	TObjectPtr<UCS_ShowInfo> InfoWidget;
	UPROPERTY()
	TObjectPtr<UCS_WaitStart> WaitStartWidget;
	UPROPERTY()
	TObjectPtr<UCS_ShowWelcome> ShowWelcomeWidget;
	UPROPERTY()
	TObjectPtr<UCS_TimeAndLocation> TimeAndLocationWidget;
	UPROPERTY()
	TObjectPtr<UCS_ShowMessage> ShowMessageWidget;
	UPROPERTY()
	TObjectPtr<UCS_ShowResult> ShowResultWidget;
	UPROPERTY()
	TObjectPtr<UCS_Description_Target> DescriptionTargetWidget;
	UPROPERTY()
	TObjectPtr<UCS_Description_Mouse> DescriptionMouseWidget;
	UPROPERTY()
	TObjectPtr<UCS_ContextMenu> ContextMenuWidget;
	UPROPERTY()
	TObjectPtr<UCS_MoveZone> MoveZoneWidget;
	UPROPERTY()
	TObjectPtr<UCS_PanelStickers> PanelStickersWidget;
	UPROPERTY()
	TObjectPtr<UCS_ActionLog> ActionLogWidget;
	UPROPERTY(BlueprintReadOnly, Category = "CS_HUD")
	TObjectPtr<UCS_HelpCellMode> HelpCellModeWidget;
	UPROPERTY()
	TObjectPtr<UCS_AWP_Widget> AWP_Widget;
	UPROPERTY()
	TObjectPtr<UCS_ObjectMenu> ObjectMenuWidget;
	UPROPERTY()
	TObjectPtr<UCS_EndWorkingAreaWidget> EndZoneWidget;
	UPROPERTY()
	TObjectPtr<UCS_EndWorkingAreaWidget> LockedBoundaryWidget;

	UPROPERTY()
	TObjectPtr<UCS_DialogContainerWidget> DialogContainerWidget;
#pragma endregion

#pragma endregion

#pragma region WindowsSystem

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FGameplayTagContainer UIWindowsTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FGameplayTagContainer UIFullscreenTags;

	UFUNCTION(BlueprintCallable)
	bool IsShowingFullscreenWidget();

	UFUNCTION(BlueprintCallable)
	bool IsShowingFullscreenWidgetByTag(FGameplayTag UsedTag);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	TSubclassOf<UCS_WindowContainerWidget> WindowContainerClass;

	UPROPERTY()
	int32 WindowZOrder = 200;

	UPROPERTY()
	TObjectPtr<UCS_WindowContainerWidget> WindowContainerWidget;

	UPROPERTY()
	TMap<FGameplayTag, UUserWidget*> TagWidgetMap;

	UPROPERTY()
	TMap<UUserWidget*, FGameplayTag> WidgetTagMap;

	UPROPERTY()
	TArray<FGameplayTag> WindowsArray;

	UPROPERTY()
	FGameplayTag LastWindowTagBeforeShowMessage;

	UFUNCTION(BlueprintCallable)
	virtual void InitCustomWidgetInWindowsSystem(bool bFullScreen, UUserWidget* Widget, FGameplayTag UsedTag);

	UFUNCTION()
	bool TryShowWidget(bool UpdatedByCheckbox, UUserWidget* Widget, const FGameplayTag& UsedTag, bool Checked = false);

	UFUNCTION()
	bool TryShowBorderWidget(const bool bIsShow, UUserWidget* Widget, const FGameplayTag& UsedTag);

	UFUNCTION()
	void TryActivateWidget(bool bEnable, UUserWidget* Widget, const FGameplayTag& UsedTag, const FGameplayTag& ActivateTag);

	UFUNCTION(BlueprintCallable)
	void TryUseWindowSystem(const FGameplayTag& UsedTag, const bool bIsOpenWidget = false);

#pragma endregion

#pragma region Dialogs

public:

	void InitShowDialog(FString API_Id, FString nameTitle, const TArray<FDialogParameters>& Params, ETypeDialog TypeDialog);
	void ShowDialogContacts(FString API_Id, const TArray<FDialogButtonParameters>& PointsDialog, ETypeDialog TypeDialog);
	void InitAddMessage(const TArray<FDialogParameters>& Params);
	void InitUpdateDialog(const FString& NewAPI_Id, const TArray<FDialogParameters>& Params);
	FString GetLastDialogID();

	void TrySwitchDialogHotKey(FGameplayTag NewActivatedDialogTag);

	UFUNCTION()
	void RemoveAllSoundsInQueue();

	UFUNCTION()
	ETypeDialog GetCurrentTypeDialog();

protected:

	void UpdateDialogActions();

	void BackDialog();

	UFUNCTION(BlueprintCallable)
	void DialogClose();

	UFUNCTION()
	void HotKeyDialogClose();

	UPROPERTY()
	FGameplayTag CurrentDialogTag;

	UPROPERTY()
	ETypeDialog CurrentTypeDialog = ETypeDialog::None;

	UPROPERTY()
	TArray<FDialogActionQueue> DialogActionList;

	UPROPERTY()
	FDialogActionQueue LastDialogAction;

	UPROPERTY()
	FDialogActionQueue CurrentDialogAction;

#pragma endregion

#pragma region HotKeys

public:

	UFUNCTION(BlueprintCallable)
	void EnableBlinkingToHotKeysByTag(const FGameplayTag ActivatedTag, const bool bEnable);

	UFUNCTION()
	void ActivateHotKey(bool bEnable, FGameplayTag ActivatedTag, TArray<FGameplayTag> UsedTags);

	UFUNCTION()
	void TryCloseHotKeyWidget(FGameplayTag ActivatedTag);

	UFUNCTION(BlueprintCallable)
	void ChangeActivatedTag(FGameplayTag ActivatedTag, bool bAdded);

	UFUNCTION(BlueprintCallable)
	void ChangeUsingTags(TArray<FGameplayTag> UsingTags, bool bAdded);

protected:

	UPROPERTY()
	TMap<FGameplayTag, UUserWidget*> TagWidgetHotKeysMap;

	UFUNCTION()
	void InitWidgetsForHotKeys();

	UFUNCTION(BlueprintCallable)
	void AddWidgetsForHotKeys(const FGameplayTag ActivatedTag, UUserWidget* Widget);

	UFUNCTION()
	void OnCheckedDynamicButton(FGameplayTag ActiveTag, bool bIsChecked);

#pragma endregion

#pragma region WidgetInteractionWithEditorStep

public:

	//Scheme
	UPROPERTY()
	bool bSchemeInit = false;
	UFUNCTION()
	void UpdateSchemes(const TArray<FLoadingImageInfo>& SchemesParams);
	UFUNCTION()
	void CloseScheme();

	//MeasureDevicesList
	TFunction<void(const FString& MeasureId, const TArray<FDeviceData>& PointsData)> StartUseMeasureDeviceFun;
	void InitMeasureDevicesList(const FDeviceAndPointsData& DevicesData) const;
	UFUNCTION()
	void ShowTool(const FDeviceAndPointsData& DevicesData);

	//Object Menu
	void ShowObjectMenu(FFullMenuReasons FullMenuReasons);
	void CloseObjectMenu();

	//ShowResult
	void ShowResult(const FShowResultParams& ShowResultParams);

	//ShowMessage
	void ShowMessage(const FShowMessageParams& ShowMessageParams);
	void CloseShowMessage();

	//Reaction
	void ShowReaction(const FString& Description);
	void CloseShowReaction();

	//Stickers
	void ShowSticker(const FStickerParams& StickerParams);
	void CloseSticker();

	//Other
	void ShowWelcome(const FShowWelcomeParams& ShowWelcomeParams);
	void UpdateSteps(const int CountSteps);
	void SetTime(const FDateTime& DateTo) const;
	void SetLocationName(const FString& LocationName) const;

#pragma endregion

#pragma region WidgetsRelated_To_The_F_keys

public:

	//Help
	UFUNCTION()
	void ShowQuest(bool UpdatedByCheckbox = false, bool Checked = false);
	UFUNCTION()
	void QuestCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void HelperCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	virtual bool CallHelper(bool UpdatedByCheckbox = false, bool bIsChecked = false);

	//Info
	UFUNCTION()
	void ShowInfo(bool UpdatedByCheckbox = false, bool Checked = false);
	UFUNCTION()
	void InfoCheckStateChanged(bool bIsChecked);

	//Log
	void ActivateShowLog(bool Enable);
	UFUNCTION()
	void ShowLogs(bool UpdatedByCheckbox = false, bool Checked = false);
	UFUNCTION()
	void LogCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void AddMessageLog(const FLogParams& LogParams);

#pragma endregion

#pragma region BackButton

public:

	UFUNCTION(BlueprintCallable)
	void BackButton();
	void BackButtonLogic();
	UFUNCTION()
	void ActivateBackButton(bool bActivate);
	UFUNCTION()
	bool GetBackButtonActive();

#pragma endregion

#pragma region User

protected:

	UPROPERTY()
	TWeakObjectPtr<ACS_PlayerController> PlayerController;

	UPROPERTY()
	TWeakObjectPtr<ACS_Character> Character;

#pragma endregion

#pragma region TagsLogic

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FGameplayTagContainer UITags;
	FGameplayTagContainer GetUITagContainer() const;

	UFUNCTION(BlueprintCallable)
	void UITagsAddTag(TArray<FGameplayTag> UsedTags);
	UFUNCTION(BlueprintCallable)
	void UITagsRemoveTag(TArray<FGameplayTag> UsedTags);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FGameplayTagContainer TagsForDisableInputForCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FGameplayTagContainer TagsForDisableCenterPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FGameplayTagContainer TagsDialog;

protected:

	//Using this function?
	UFUNCTION()
	void UpdateTagsWidget(FGameplayTagContainer& TagContainer) const;

#pragma endregion

#pragma region Course

public:

	void StartCourse();
	void FinishCourse() const;
	void ExitCourse() const;

#pragma endregion

#pragma region MainMenu

public:

	void InitMainMenu(const FMainMenuParams& MainMenuParams) const;

	UFUNCTION(BlueprintCallable)
	void ShowMainMenu();

	bool IsLockedMenu() const;

#pragma endregion

#pragma region ContextMenu

public:

	void ShowContextMenu(FString ApiID, const  TArray<FContextMenuItem>& ContextMenuItems);
	void CloseContextMenu();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FGameplayTagContainer UIOverlapingContextMenuTags;

	UFUNCTION()
	void UpdateContextMenu();

	UFUNCTION()
	void InitOverlapContextMenu();

	UFUNCTION(BlueprintCallable)
	void AddWidgetForOverlapContextMenu(UUserWidget* Widget);

#pragma endregion

public:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void EscButton();


	void ShowAWP(UCS_AWP_Widget* NewAWP_Widget);
	void HideAWP();

	void ShowTargetDescription(const FString& Description) const;
	void ShowMouseDescription(const FString& ObjectId, const FString& Description) const;


	void DisableInputForCharacter();

	UFUNCTION()
	void MenuCheckStateChanged();

	//Начало взаимодейсвия с объектом
	void StartInteractWithObject(const bool& bUseUpDownMoveZone, const bool& bUseLeftRightMoveZone, TFunction<void(bool Lock)> SetLockCameraMoveFun);
	virtual void StopInteractWithObject();

	UFUNCTION()
	void ZoomChanged(int PercentZoom);

	void UpdateArrowsHUD(const bool& bUseUpMoveZone, const bool& bUseDownMoveZone, const bool& bUseLeftMoveZone, const bool& bUseRightMoveZone);

	void SpawnFadeWidget();

	UFUNCTION()
	void StopAllSoundInGame(bool bPause);

	UFUNCTION()
	bool ResetZoomInUI();

	UFUNCTION()
	void ShowEndZone(const bool bShow);

	UFUNCTION()
	void ShowLockedBoundaryWidget(const bool bIsShow);
	void SetLockedBoundaryWidgetText(const FString& InText) const;

	UFUNCTION(BlueprintCallable)
	void ActionToShowImageViewer(bool bHide);

	UFUNCTION(BlueprintCallable)
	virtual void UpdateCondition();


#pragma region Utilities
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "CS_HUD|Utilities")
	void CollapseAllWidgets();
	UFUNCTION()
	bool IsMouseOverAnyWidget();

#pragma endregion

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void K2_DisableInputCharacter();

#pragma region Utilities

	template<typename T>
	T* CreateAndInitWidget(const FWidgetConfig& Config, TFunction<void(T*)> InitFunc = nullptr);

#pragma endregion

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FGameplayTagContainer TagsForDisableZoomCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_HUD")
	FGameplayTagContainer TagsForDisableMoveCamera2D;

	UFUNCTION()
	void CheckDisableZoomCamera();

};

template<typename T>
inline T* ACS_HUD::CreateAndInitWidget(const FWidgetConfig& Config, TFunction<void(T*)> InitFunc)
{
	check(Config.WidgetClass)

		if (!Config.WidgetClass.Get()) return nullptr;

	if (T* Widget = CreateWidget<T>(GetWorld(), Config.WidgetClass))
	{
		Widget->SetVisibility(ESlateVisibility::Collapsed);

		if (Config.UsedGameplayTag != Config.UsedGameplayTag.EmptyTag
			&& (UIWindowsTags.HasTag(Config.UsedGameplayTag) || UIFullscreenTags.HasTag(Config.UsedGameplayTag)))
		{
			TagWidgetMap.Add(Config.UsedGameplayTag, Widget);
			WidgetTagMap.Add(Widget, Config.UsedGameplayTag);
			WindowContainerWidget->AddWidgetToPanel(Widget);
		}
		else
		{
			Widget->AddToViewport(Config.ZOrder);
		}

		if (InitFunc)
		{
			InitFunc(Widget);
		}

		return Widget;
	}
	return nullptr;
}
