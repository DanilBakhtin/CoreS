// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "Framework/CS_CoreTypes.h"
#include "GameFramework/PlayerController.h"
#include "WebSocketServer/WebSocketServer.h"
#include <Gameplay/ObjectsOfInteract/Components/CS_Interact_Component_Interface.h>
#include "CS_PlayerController.generated.h"

struct FInputActionInstance;
class ACS_PlayerState;
class ACS_HUD;
class UInputAction;
class UInputMappingContext;
class ACS_PlayerStart;
class ACS_Interactable_Actor;
class ACS_Character;
class UCS_AWP_Widget;
class UCS_WebSocketSender;

struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientBattleDataInitialized, class ACS_PlayerController*, PlayerController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLearningStickerShow, int32, penaltyValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableActorUpdated, ACS_Interactable_Actor*, InteractableActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartedCourse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeleport);

UCLASS()
class CORE_SIMULATOR_API ACS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ACS_PlayerController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;
	/** Метод для привязки действий и методов которые должны вызываться этими действиями*/
	virtual void SetupInputComponent() override;

	/** Контекс клавиш и дейсвий не привязанных к пешке*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> ControllerMappingContext;

	/** Действие при нажатии ESC*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ESCAction;

	/** Действие клика правой кнопкой мыши*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LeftClickAction;

	/** Действие клика правой кнопкой мыши*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> RightClickAction;
	/** Действие открытия/закрытия подсказки */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> QuestAction;
	/** Действие открытия/закрытия информации с заданием*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InfoAction;
	/** Действие открытия/закрытия измерительного прибора*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LogAction;
	/** Call Helper action. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> HelperAction;
	/** Действие обратного дейсвия */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> BackAction;
	/** Зум*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ZoomAction;
	/** Действие перемещение камеры в 2D режиме */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CameraPanAction;
	/** Действие дельта мыши */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MouseDeltaAction;

	/** Клавиши от 0 до 1*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TArray<UInputAction*> HotKeysAction;

	/** Метод привязанный к действию ESCAction*/
	UFUNCTION()
	void EscLogic();

	/** Метод привязанный к действию QuestAction*/
	UFUNCTION()
	void ShowQuest();
	/** Метод привязанный к действию InfoAction*/
	UFUNCTION()
	void ShowInfo();
	/** Метод привязанный к действию BackAction*/
	UFUNCTION(BlueprintCallable)
	void BackButton();

	UFUNCTION()
	void SendBack();

	UFUNCTION()
	void SetActivateBackButton(bool bIsActive);

	/** Метод для показа описания объекта при наведении прицела*/
	void ShowTargetDescription(const FString& Description);
	/** Метод для показа описания объекта при наведении курсора*/
	void ShowMouseDescription(const FString& ObjectId, const FString& Description);
	/** Метод привязанный к действию LogAction*/
	UFUNCTION()
	void ShowLogAction();
	UFUNCTION()
	virtual void CallHelperAction();

	UFUNCTION()
	void ShowWelcome(const FShowWelcomeParams& ShowWelcomeParams);
	//Этот метод нужен для того что бы нам пришел Show Welcome
	void SendStartLocation() const;

	UFUNCTION(BlueprintCallable)
	void OnCustomEvent(const FString& ObjectId, const FString& CustomString);

	//Методы на отпавку информации клиенту
	UFUNCTION(BlueprintCallable)
	void OnMenuEvent(const FString CastedToStringAttribute = "");
	UFUNCTION(BlueprintCallable)
	void NavStart();
	UFUNCTION(BlueprintCallable)
	void NavActionFinish();
	UFUNCTION(BlueprintCallable)
	void NavActionExit();
	UFUNCTION(BlueprintCallable)

	void NavActionCustom(const FString& String);
	UFUNCTION(BlueprintCallable)
	void OnClickObject(const FString& ObjectId, const FString& String);
	UFUNCTION(BlueprintCallable)
	void OnPointAction(const FString& ObjectId, const EPointAction& ActionType);
	//Метод сообщающий что мы в процессе взаимодейсвия с объектом или находимся в локации, если объект является локацией
	UFUNCTION(BlueprintCallable)
	void EndTween(const FString& Point);
	//Метод сообщает что мы находимся в нескольких локациях
	UFUNCTION(BlueprintCallable)
	void EndTweens(const TArray<FString>& Points);
	UFUNCTION(BlueprintCallable)
	void OnDialogPoint(const FString& String);

	UFUNCTION(BlueprintCallable)
	ETypeDialog GetCurrentTypeDialog();

	UFUNCTION(BlueprintCallable)
	void ReasonsActionEvent(const FString& ObjectId);
	//Взаимодейсвие с точкой измерения
	UFUNCTION(BlueprintCallable)
	void OnMeasureActionEvent(const FString& Attribute);
	// TODO: Clear plugin code from comments in Russian.
	// Trying to exit from locked location.
	// #VSP_only
	UFUNCTION(BlueprintCallable)
	void ExitPlace();
	
	ACS_HUD* GetPlayerHUD();

	void SetWebSocketServer(AWebSocketServer* r_WebSocketServer);

	void SetInteractableActor(ACS_Interactable_Actor* Interactable_Actor);

	void SetTime(const FDateTime& DateTo);
	void SetLocationName(const FString& LocationName);

	UFUNCTION()
	void ActivateHotKey(bool bEnable, FGameplayTag ActivatedTag, TArray<FGameplayTag> UsedTags);

	//Radio
	void InitShowDialog(FString API_Id, FString nameTitle, const TArray<FDialogParameters>& Params, ETypeDialog TypeDialog);
	void InitAddMessage(const TArray<FDialogParameters>& Params);
	void InitUpdateDialog(const FString& NewAPI_Id, const TArray<FDialogParameters>& Params);
	void ShowMeasureDevicesList(const FDeviceAndPointsData& DeviceAndPointsData);

	//Main Menu
	void InitMainMenu(const FMainMenuParams& MainMenuParams);
	//ShowMessage
	void ShowMessage(const FShowMessageParams& ShowMessageParams);
	//ShowResult
	void ShowResult(const FShowResultParams& ShowResultParams);
	//Взаимодейсвие с объектом
	void StartInteractWithObject(const bool& bUseUpDownMoveZone, const bool& bUseLeftRightMoveZone, TFunction<void(bool Lock)> LockCameraMoveFun);
	//Показать Контекстное меню
	void ShowContextMenu(FString ApiID, const  TArray<FContextMenuItem>& ContextMenuItems);
	//Показать реакцию
	UFUNCTION(BlueprintCallable, Category = "CS_PlayerController", DisplayName = "Show reaction")
	void ShowReaction(const FString& Description, const FString& Action = "next");
	//Начало взаимодейсвие с выбранным измерительным прибором
	void StartUseMeasureDevice(const FString& MeasureId, const TArray<FDeviceData>& PointsData);
	void StopUseMeasureDevice();
	void UpdateMeasureDevice(const float& Result) const;
	//Обновление количества шагов
	void UpdateSteps(const int CountSteps);
	void ShowLearningSticker(const FStickerParams& StickerParams);

	UFUNCTION()
	void UpdateSchemes(const TArray<FLoadingImageInfo>& SchemesParams);

	void ShowAWP(UCS_AWP_Widget* AWP_Widget);
	void HideAWP();

	UFUNCTION()
	void ShowDialogContacts(FString API_Id, const TArray<FDialogButtonParameters>& PointsDialog, ETypeDialog TypeDialog);

	UFUNCTION(BlueprintCallable, Category = "CS_PlayerController", DisplayName = "Add message to log")
	void AddMessageLog(const FLogParams& LogParams);

	void HandleZoomInput(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void ApplyZoom(const float Direction);

	void UpdateMoveZones(const bool& bUseUpMoveZone, const bool& bUseDownMoveZone, const bool& bUseLeftMoveZone, const bool& bUseRightMoveZone);

	void TryPlaySound(const FString& ID_Sound);

	void InitQueueSoundsDialog(const TArray<FString> QueueSounds);

	void InitAddQueueSoundsToDialog(const TArray<FString> QueueSounds);

	void RemoveSoundsInQueue();

	void StopVoiceManager(bool bPause);

	// ~Begin #VSP_only
	void ShowObjectMenu(const FFullMenuReasons& FullMenuReasons);
	void SetLockedBoundaryWidgetText(const FString& InText);
	// ~End VSP_only

	UFUNCTION()
	void ShowZoomToInteractObject();

#pragma region CameraPanning

	bool bIsCameraPanning = false;
	FVector2D LastMousePosition = FVector2D(0.f);

	void StartCameraPan();
	void StopCameraPan();
	void HandleCameraPan(const FInputActionValue& Value);

	void Lock2DZoom(const bool bIsLocking);

	void CenterMouseCursor();

#pragma endregion

	UPROPERTY(BlueprintAssignable, Category = "CS_PlayerController")
	FOnStartedCourse OnStartedCourse;
	UPROPERTY(BlueprintAssignable, Category = "CS_PlayerController")
	FOnTeleport OnTeleport;

	UPROPERTY(BlueprintAssignable, Category = "CS_PlayerController")
	FLearningStickerShow OnLearingnStickerShow;
	UPROPERTY(BlueprintAssignable, Category = "CS_PlayerController")
	FOnInteractableActorUpdated OnInteractableActorUpdated;
	
	UFUNCTION()
	void CallStartCourse();

	UFUNCTION()
	void CallTeleport();

	UFUNCTION()
	void ResetZoom();

	UFUNCTION()
	void TryShowEndZone();
	UFUNCTION()
	void ShowEndZone();
	UFUNCTION()
	void TryCloseEndZone();
	UFUNCTION()
	void CloseEndZone();

	// ~Begin #VSP_only
	UFUNCTION()
	void TryShowLockedBoundary();
	UFUNCTION()
	void ShowLockedBoundary();
	UFUNCTION()
	void TryHideLockedBoundary();
	UFUNCTION()
	void HideLockedBoundary();
	// ~End VSP_only

	UFUNCTION()
	void CheckAllBoundaryWarnings();

	bool bCourseStarted = false;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "ClickObject"))
	void K2_ClickObject(const FString& ApiID);

	UFUNCTION(BlueprintCallable)
	void EnableBlinkingToHotKeysByTag(const FString SubKey, const FGameplayTag ActivatedTag, const bool bEnable);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "BlinkingSpecialPoint"))
	void K2_BlinkingSpecialPoint(const FString& SubKey, const FGameplayTag Tag, const bool bBlinking);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "ClickedHotKey"))
	void K2_ClickedHotKey(const FGameplayTag ActiveTag, const FString& ApiID, const bool bUsed);

	UFUNCTION(BlueprintCallable)
	void HotKeyObjectClick(FGameplayTag InputActiveTag);

	FDeviceAndPointsData CurrentDeviceAndPointsData;

protected:

	UPROPERTY()
	bool bBackButtonActive = false;

	//Ссылка на испорльзуемый HUD 
	UPROPERTY()
	TObjectPtr<ACS_HUD> PlayerHUD;

	UPROPERTY()
	TObjectPtr<AWebSocketServer> WebSocketServer;

	UPROPERTY()
	TObjectPtr<UCS_WebSocketSender> WebSocketSender;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<ACS_Interactable_Actor> CurrentInteractableObject;
	//Последний компонент статик меша с которым которым мы взаимодейсвовал луч
	TWeakObjectPtr<UActorComponent> LastInteractActorComp;

	void StartCheckMapLoaded();
	bool bLevelStreamingLoaded = false;
	TFunction<bool()> GetMainCourseMapLoadedFun;
	void FindPlayerStart();
	UFUNCTION()
	void MainCourseMapLoadingFinish(bool WorldIsLoad);

#pragma region HotKeys

	UFUNCTION()
	void BindToHotKeys();

	UFUNCTION()
	void OnAnyHotKey(const FInputActionInstance& Instance);

	UPROPERTY()
	TArray<FHotkeySlot> HotKeysStruct;

	UPROPERTY()
	TMap<const UInputAction*, FGameplayTag> MapHotKeysTag;

	UPROPERTY()
	TMap<FGameplayTag, FString> MapActiveTagToApiID;

	TMap<FGameplayTag, TArray<FGameplayTag>> MapActiveTagToUsingTags;

	UPROPERTY()
	TMap<FGameplayTag, bool> MapActiveTagUsing;

	UPROPERTY()
	TMap<FGameplayTag, bool> MapActiveTagCellMode;

	FGameplayTag CurrentActiveCellModeTag;
#pragma endregion

	UPROPERTY(BlueprintReadWrite)
	bool bIsLockZoom = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_PlayerController")
	bool bPossiblityEnableBackButtonIn3D = false;

	UFUNCTION()
	void CheckUnderCursorRigthClick();

	UFUNCTION()
	void TryLeftClick();

	UFUNCTION()
	void TraceMouseCursor();

	UFUNCTION()
	void ClearCurrentComponent();

	UPROPERTY()
	bool bEnableRightClick = true;

	UPROPERTY()
	bool bEnableTraceMouseCursor = false;

	UPROPERTY()
	UActorComponent* CurrentComponent;

	TSoftObjectPtr<ACS_PlayerState> PlayerState;

	// ~Begin #VSP_only
	UPROPERTY()
	bool bIsExitPlaceCooldown = false;
	
	UPROPERTY()
	int CounterEndZone = 0;

	UPROPERTY()
	int CounterLockedBoundary = 0;

	UPROPERTY()
	FTimerHandle LockedBoundaryHideTimerHandle;
	// ~End VSP_only

	TMap<FString, TArray<FDialogButtonParameters>> MapPhoneDialogs;

public:

	UFUNCTION()
	void SendDialogClose();

	void SetGetMainCourseMapLoadedFun(TFunction<bool()> Func);

	void StartCourse();

	void SetLastInteractActorComp(UActorComponent* ActorComponent);
	UActorComponent* GetLastInteractActorComp() const;
	
	UFUNCTION(BlueprintCallable, Category = "CS_PlayerController", DisplayName = "Get Core Simulator Player State")
	ACS_PlayerState* GetCS_PlayerState() const;

	UFUNCTION()
	void ZoomChanged(int PercentZoom);

	UFUNCTION(BlueprintCallable, Category = "CS_PlayerController", DisplayName = "Get Core Simulator Player State")
	virtual void SetMouseToCenter();

	UFUNCTION()
	void SetIsExitPlaceCooldown(const bool IsExitPlaceCooldown);
	UFUNCTION()
	void SetIsExitPlaceCooldownFalse();

	UFUNCTION()
	void EnableRightClick();

	UFUNCTION()
	void DisableRightClick();

	UFUNCTION()
	void LogWebSocketSender(const FString& Message);

	UFUNCTION()
	void ClearPointingUpdatePlace();

	UFUNCTION()
	void TryClickedDialog();

#pragma region HotKeys
	UFUNCTION(BlueprintCallable)
	void AddActiveTagToApiID(FGameplayTag ActiveTag, FString ApiID);

	UFUNCTION(BlueprintCallable)
	void RemoveActiveTag(FGameplayTag ActiveTag);

	UFUNCTION()
	void ClearActiveTagToApiID();

	UFUNCTION()
	void ClearActiveTagUsing();

	UFUNCTION()
	void ApplyUsingTags(FGameplayTag ActiveTag, bool bUsing);

	UFUNCTION()
	void UpdateCurrentCellMode(FGameplayTag ActiveTagCellMode, FString ApiID);

	UFUNCTION()
	bool Teleport(FString LocationID);
#pragma endregion

protected:

	mutable ACS_Character* PlayerCharacter;

	UFUNCTION(BlueprintCallable)
	ACS_Character* GetPlayerCharacter() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FPSProjectLimit = 60;

	void SetFPSLimit(int32 FPSLimit = 60);
};
