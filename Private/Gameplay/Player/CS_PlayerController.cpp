// Core Simulator

#include "Gameplay/Player/CS_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Framework/CS_UtilityFunctionLibrary.h"
#include "Framework/CS_DeveloperSettings.h"
#include "Framework/CS_SimulatorParamsAsset.h"
#include "Framework/CS_WebSocketSender.h"
#include "Framework/CS_GameState_Interface.h"
#include "Framework/CS_CoreTypes.h"

#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_2d_Actor.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Cell_Actor.h"

#include "Gameplay/Player/CS_Character.h"
#include "Gameplay/Player/CS_HUD.h"
#include "Gameplay/Utilities/CS_VoiceManager.h"
#include "Gameplay/Utilities/CS_PlayerStart.h"
#include "Gameplay/Player/CS_PlayerState.h"

#include "WebSocketServer/WebSocketServer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameViewportClient.h"
#include "Containers/Ticker.h"


DEFINE_LOG_CATEGORY_STATIC(LogACS_PlayerController, All, All);

ACS_PlayerController::ACS_PlayerController()
{
}

void ACS_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	WebSocketSender = NewObject<UCS_WebSocketSender>(this);
	WebSocketSender->OnLogMessage.AddDynamic(this, &ACS_PlayerController::LogWebSocketSender);

	FindPlayerStart();
	StartCheckMapLoaded();

	PlayerState = GetPlayerState<ACS_PlayerState>();

	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	if (ViewportClient)
	{
		FName CursorPath = FName(FPaths::ProjectContentDir() + TEXT("/Images/Cursor/T_MouseCursor"));
		FVector2D HotSpot = FVector2D(0.0f, 0.0f);
		ViewportClient->SetHardwareCursor(EMouseCursor::Type::Default, CursorPath, HotSpot);
	}

	SetFPSLimit(FPSProjectLimit);

	BindToHotKeys();
}

void ACS_PlayerController::Tick(float DeltaTime)
{
	if (bEnableTraceMouseCursor)
		TraceMouseCursor();
}

void ACS_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ACS_Character* LS_Character = Cast<ACS_Character>(InPawn)) {
		PlayerCharacter = LS_Character;
		PlayerCharacter->StartUsingPawn(this);
		PlayerCharacter->OnZoomChanged.AddDynamic(this, &ACS_PlayerController::ZoomChanged);
	}
}

#pragma region Input

void ACS_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{	
		//Левый клик
		EnhancedInput->BindAction(LeftClickAction, ETriggerEvent::Triggered, this, &ACS_PlayerController::TryLeftClick);

		//Правый клик
		EnhancedInput->BindAction(RightClickAction, ETriggerEvent::Triggered, this, &ACS_PlayerController::CheckUnderCursorRigthClick);

		//Esc
		EnhancedInput->BindAction(ESCAction, ETriggerEvent::Triggered, this, &ACS_PlayerController::EscLogic);

		//Задание
		EnhancedInput->BindAction(QuestAction, ETriggerEvent::Triggered, this, &ACS_PlayerController::ShowQuest);

		//Подсказака
		EnhancedInput->BindAction(InfoAction, ETriggerEvent::Triggered, this, &ACS_PlayerController::ShowInfo);
		
		//Журнал
		EnhancedInput->BindAction(LogAction, ETriggerEvent::Triggered, this, &ACS_PlayerController::ShowLogAction);
		
		//Назад
		EnhancedInput->BindAction(BackAction, ETriggerEvent::Triggered, this, &ACS_PlayerController::BackButton);

		//Зум
		EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ACS_PlayerController::HandleZoomInput);

		if (CameraPanAction)
		{
			EnhancedInput->BindAction(CameraPanAction, ETriggerEvent::Started, this, &ACS_PlayerController::StartCameraPan);
			EnhancedInput->BindAction(CameraPanAction, ETriggerEvent::Completed, this, &ACS_PlayerController::StopCameraPan);
			EnhancedInput->BindAction(CameraPanAction, ETriggerEvent::Canceled, this, &ACS_PlayerController::StopCameraPan);
		}

		if (MouseDeltaAction)
			EnhancedInput->BindAction(MouseDeltaAction, ETriggerEvent::Triggered, this, &ACS_PlayerController::HandleCameraPan);

		//Helper
		if (const UCS_DeveloperSettings* Settings = GetDefault<UCS_DeveloperSettings>())
		{
			if (const UCS_SimulatorParamsAsset* SimulatorParams = Settings->SimulatorParamsAsset.LoadSynchronous())
			{
				if (SimulatorParams->Params.bIsHelperAvailable)
				{
					EnhancedInput->BindAction(HelperAction, ETriggerEvent::Triggered, this, &ACS_PlayerController::CallHelperAction);
				}
			}
		}
		

	}
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(ControllerMappingContext, 0);
	}
}

void ACS_PlayerController::EscLogic()
{
	if (GetPlayerHUD())
		GetPlayerHUD()->EscButton();

	/*if (ACS_Character* LS_Character = Cast<ACS_Character>(GetPawn()))
	{
		LS_Character->ZoomReset();
	}*/
}

void ACS_PlayerController::ShowQuest()
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowQuest();
}

void ACS_PlayerController::ShowInfo()
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowInfo();
}

void ACS_PlayerController::BackButton()
{
	if (GetPlayerHUD()) {
		if (!bBackButtonActive || GetPlayerHUD()->IsShowingFullscreenWidget()) return;
		GetPlayerHUD()->BackButtonLogic();
	}

	if (GetPlayerCharacter())
	{
		GetPlayerCharacter()->bUseTrace = true;
		bEnableTraceMouseCursor = false;
		GetPlayerCharacter()->UnCrouch(false);
	}

	SendBack();
}

void ACS_PlayerController::SetActivateBackButton(bool bIsActive)
{
	bool is3DMode = true;

	if (GetPlayerCharacter())
		is3DMode = GetPlayerCharacter()->bUseTrace;

	bBackButtonActive = bIsActive && (!is3DMode || bPossiblityEnableBackButtonIn3D);

	if (GetPlayerHUD())
		GetPlayerHUD()->ActivateBackButton(bBackButtonActive);
}

void ACS_PlayerController::ShowTargetDescription(const FString& Description)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowTargetDescription(Description);
}

void ACS_PlayerController::ShowMouseDescription(const FString& ObjectId, const FString& Description)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowMouseDescription(ObjectId, Description);
}

void ACS_PlayerController::ShowLogAction()
{
	UE_LOG(LogACS_PlayerController, Display, TEXT("ShowLogAction"));
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowLogs();
}

void ACS_PlayerController::CallHelperAction()
{
	//25.08.26: Has no info about this function.
	//TODO: Create Helper log and call it?
	UE_LOG(LogACS_PlayerController, Display, TEXT("CallHelperAction"));
	if (GetPlayerHUD())
		GetPlayerHUD()->CallHelper();
}

#pragma endregion

#pragma region SendToES

void ACS_PlayerController::SendBack()
{
	if (WebSocketSender)
		WebSocketSender->SendNavAction("gotoBack");
}

void ACS_PlayerController::SendStartLocation() const
{
	if (WebSocketSender)
		WebSocketSender->SendEndTween(UCS_UtilityFunctionLibrary::GetStartLocation());
}

void ACS_PlayerController::OnCustomEvent(const FString& ObjectId, const FString& CustomString)
{
	if (WebSocketSender)
		WebSocketSender->SendCustomEvent(ObjectId, CustomString);
}

void ACS_PlayerController::OnMenuEvent(const FString CastedToStringAttribute)
{
	if (WebSocketSender)
		WebSocketSender->SendActionEvent("OnMenu", CastedToStringAttribute);
}

void ACS_PlayerController::NavStart()
{
	if (WebSocketSender)
		WebSocketSender->SendNavAction("start");
}

void ACS_PlayerController::NavActionFinish()
{
	if (WebSocketSender)
		WebSocketSender->SendNavAction("finish");
}

void ACS_PlayerController::NavActionExit()
{
	if (WebSocketSender) {
		WebSocketSender->SendNavAction("exit");
	}

	//The delay is necessary so that the step editor can receive the exit command.
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateWeakLambda(this, [this](float)
			{
				ConsoleCommand(TEXT("quit"));
				return false;
			}),
		1.0f);

}

void ACS_PlayerController::NavActionCustom(const FString& String)
{
	if (WebSocketSender)
		WebSocketSender->SendNavAction(String);
}

void ACS_PlayerController::OnClickObject(const FString& ObjectId, const FString& String)
{	
	FString OriginalID = UCS_UtilityFunctionLibrary::RemoveSlashSuffix(ObjectId);

	K2_ClickObject(OriginalID);

	if (WebSocketSender)
		WebSocketSender->SendClickObject(OriginalID, String);
}

void ACS_PlayerController::OnPointAction(const FString& ObjectId, const EPointAction& ActionType)
{
	if (WebSocketSender)
		WebSocketSender->SendPointAction(ObjectId, UEnum::GetDisplayValueAsText(ActionType).ToString());
}

void ACS_PlayerController::EndTween(const FString& Point)
{
	if (WebSocketSender && !Point.IsEmpty())
		WebSocketSender->SendEndTween(Point);
}

void ACS_PlayerController::EndTweens(const TArray<FString>& _Points)
{
	if (WebSocketSender)
		WebSocketSender->SendEndTweens(_Points);
}

void ACS_PlayerController::OnDialogPoint(const FString& String)
{
	if (WebSocketSender)
		WebSocketSender->SendActionEvent("OnDialogPoint", String);
}

void ACS_PlayerController::ReasonsActionEvent(const FString& ObjectId)
{
	if (WebSocketSender)
		WebSocketSender->SendActionEvent("OnReason", ObjectId);
}

void ACS_PlayerController::OnMeasureActionEvent(const FString& Attribute)
{
	if (WebSocketSender)
		WebSocketSender->SendActionEvent("OnMeasure", Attribute);
}

void ACS_PlayerController::ExitPlace()
{
	// TODO: Make logging for invalid WebSocketServer in all other cases of sending messages.
	// TODO: Create new log category for plugin.
	if (!WebSocketServer)
	{
		UE_LOG(LogACS_PlayerController, Error,
			TEXT("Trying call 'ExitPlace' function with invald WebSocketServer!"));
		return;
	}

	if (WebSocketSender)
		WebSocketSender->SendActionEvent("ExitPlace", GetCS_PlayerState()->GetCurrentLocationAPI_Id());
}

void ACS_PlayerController::SendDialogClose()
{
	if (WebSocketSender)
		WebSocketSender->SendNavAction("dlgClose");
}

#pragma endregion

#pragma region Dialogs

void ACS_PlayerController::ShowDialogContacts(FString API_Id, const TArray<FDialogButtonParameters>& PointsDialog, ETypeDialog TypeDialog)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowDialogContacts(API_Id, PointsDialog, TypeDialog);
}

void ACS_PlayerController::InitShowDialog(FString API_Id, FString nameTitle, const TArray<FDialogParameters>& Params, ETypeDialog TypeDialog)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->InitShowDialog(API_Id, nameTitle, Params, TypeDialog);
}

void ACS_PlayerController::InitAddMessage(const TArray<FDialogParameters>& Params)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->InitAddMessage(Params);
}

void ACS_PlayerController::InitUpdateDialog(const FString& NewAPI_Id, const TArray<FDialogParameters>& Params)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->InitUpdateDialog(NewAPI_Id, Params);
}

ETypeDialog ACS_PlayerController::GetCurrentTypeDialog()
{
	if (GetPlayerHUD())
		return GetPlayerHUD()->GetCurrentTypeDialog();

	return ETypeDialog::None;
}

#pragma endregion

ACS_HUD* ACS_PlayerController::GetPlayerHUD()
{	
	if (!IsValid(PlayerHUD))
	{
		PlayerHUD = Cast<ACS_HUD>(GetHUD());
	}

	return PlayerHUD.Get();
}

void ACS_PlayerController::SetWebSocketServer(AWebSocketServer* r_WebSocketServer)
{
	WebSocketServer = r_WebSocketServer;

	if (WebSocketSender)
		WebSocketSender->Initialize(WebSocketServer);
}

void ACS_PlayerController::SetInteractableActor(ACS_Interactable_Actor* Interactable_Actor)
{
	CurrentInteractableObject = Interactable_Actor;

	if (!Interactable_Actor)
		CurrentActiveCellModeTag = FGameplayTag::EmptyTag;

	if (GetPlayerHUD())
		GetPlayerHUD()->CloseContextMenu();

	if (Interactable_Actor != nullptr) {
		OnInteractableActorUpdated.Broadcast(Interactable_Actor);
	}

	if (!CurrentInteractableObject.IsValid())
		if (GetPlayerHUD())
			GetPlayerHUD()->StopInteractWithObject();

	if (CurrentInteractableObject.IsValid()) {
		if (GetPlayerCharacter())
			GetPlayerCharacter()->bUseTrace = false;
	}
	else
	{
		if (GetPlayerCharacter())
			GetPlayerCharacter()->bUseTrace = true;
	}

	ShowZoomToInteractObject();
}

void ACS_PlayerController::StartInteractWithObject(const bool& bUseUpDownMoveZone, const bool& bUseLeftRightMoveZone, TFunction<void(bool Lock)> LockCameraMoveFun)
{
	if (GetPlayerCharacter())
	{
		GetPlayerCharacter()->bUseTrace = false;
		bEnableTraceMouseCursor = true;
	}
	if (GetPlayerHUD())
		GetPlayerHUD()->StartInteractWithObject(bUseUpDownMoveZone, bUseLeftRightMoveZone, LockCameraMoveFun);
}

#pragma region MeasureDevice

void ACS_PlayerController::StartUseMeasureDevice(const FString& MeasureId, const TArray<FDeviceData>& PointsData)
{
	if (GetPlayerCharacter())
	{
		GetPlayerCharacter()->StartUseMeasureDevice(CurrentInteractableObject, MeasureId, CurrentDeviceAndPointsData.PointsData);

		if (PlayerState)
			PlayerState->SetCurrentInteractMode(EInteractMode::MeausureMode);
	}

	ClearCurrentComponent();
}

void ACS_PlayerController::StopUseMeasureDevice()
{
	if (GetPlayerCharacter())
	{
		GetPlayerCharacter()->StopUseMeasureDevice(CurrentInteractableObject);

		if (PlayerState)
			PlayerState->SetCurrentInteractMode(EInteractMode::Normal);
	}

	ClearCurrentComponent();
}

void ACS_PlayerController::UpdateMeasureDevice(const float& Result) const
{
	if (GetPlayerCharacter())
	{
		GetPlayerCharacter()->UpdateMeasureDevice(Result);
	}
}

void ACS_PlayerController::ShowMeasureDevicesList(const FDeviceAndPointsData& DeviceAndPointsData)
{
	CurrentDeviceAndPointsData = DeviceAndPointsData;
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowTool(DeviceAndPointsData);
}

#pragma endregion

#pragma region Other_ES

void ACS_PlayerController::UpdateSteps(const int CountSteps)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->UpdateSteps(CountSteps);
}

void ACS_PlayerController::ShowLearningSticker(const FStickerParams& StickerParams)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowSticker(StickerParams);

	OnLearingnStickerShow.Broadcast(StickerParams.Penalty);
}

void ACS_PlayerController::UpdateSchemes(const TArray<FLoadingImageInfo>& SchemesParams)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->UpdateSchemes(SchemesParams);
}

void ACS_PlayerController::AddMessageLog(const FLogParams& LogParams)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->AddMessageLog(LogParams);
}

void ACS_PlayerController::ShowObjectMenu(const FFullMenuReasons& FullMenuReasons)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowObjectMenu(FullMenuReasons);
}

void ACS_PlayerController::ShowContextMenu(FString ApiID, const TArray<FContextMenuItem>& ContextMenuItems)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowContextMenu(ApiID, ContextMenuItems);
}

void ACS_PlayerController::ShowReaction(const FString& Description, const FString& Action)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowReaction(Description);

	NavActionCustom(Action);
}

void ACS_PlayerController::ShowMessage(const FShowMessageParams& ShowMessageParams)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowMessage(ShowMessageParams);
}

void ACS_PlayerController::ShowResult(const FShowResultParams& ShowResultParams)
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	if (GetPlayerHUD())
		GetPlayerHUD()->ShowResult(ShowResultParams);
}

void ACS_PlayerController::SetTime(const FDateTime& DateTo)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->SetTime(DateTo);
}

void ACS_PlayerController::SetLocationName(const FString& LocationName)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->SetLocationName(LocationName);
}

void ACS_PlayerController::ShowWelcome(const FShowWelcomeParams& ShowWelcomeParams)
{
	UE_LOG(LogACS_PlayerController, Display, TEXT("Show Welcome"));
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowWelcome(ShowWelcomeParams);
}

void ACS_PlayerController::InitMainMenu(const FMainMenuParams& MainMenuParams)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->InitMainMenu(MainMenuParams);
}

#pragma endregion

void ACS_PlayerController::ShowAWP(UCS_AWP_Widget* AWP_Widget)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowAWP(AWP_Widget);
}

void ACS_PlayerController::HideAWP()
{
	if (GetPlayerHUD())
		GetPlayerHUD()->HideAWP();
}

void ACS_PlayerController::StartCheckMapLoaded()
{
	UWorld* World = GetWorld();

	const bool bGameModeReady = GetMainCourseMapLoadedFun && GetMainCourseMapLoadedFun();
	const bool bStreamingReady = bLevelStreamingLoaded;
	const bool bActorsReady = World && World->AreActorsInitialized();

	if (!bGameModeReady || !bStreamingReady || !bActorsReady)
	{
		FTimerHandle EndTweenTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(EndTweenTimerHandle, this, &ThisClass::StartCheckMapLoaded, 1.f, false);
	}
	else
	{
		UE_LOG(LogACS_PlayerController, Display, TEXT("Map Loaded"));
		StartCourse();
	}
}

void ACS_PlayerController::FindPlayerStart()
{
	TArray<AActor*> Actors;
	Actors.Empty();
	TObjectPtr<ACS_PlayerStart> FoundPlayerStart = nullptr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACS_PlayerStart::StaticClass(), Actors);
	for (AActor*& Actor : Actors)
	{
		if (ACS_PlayerStart* PlayerStart = Cast<ACS_PlayerStart>(Actor))
		{
			if (PlayerStart->GetWorldPartitionSteamingChecker())
			{
				FoundPlayerStart = PlayerStart;
				FoundPlayerStart->GetWorldPartitionSteamingChecker()->OnStreamingCompleted.AddDynamic(this, &ACS_PlayerController::MainCourseMapLoadingFinish);
				FoundPlayerStart->GetWorldPartitionSteamingChecker()->StartCheckForActiveLoadingsInSteaming();
				break;
			}
		}
	}
	if (!IsValid(FoundPlayerStart))
	{
		FTimerHandle TimeTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimeTimerHandle, this, &ThisClass::FindPlayerStart, 1.f, false);
	}
}

void ACS_PlayerController::MainCourseMapLoadingFinish(bool WorldIsLoad)
{
	bLevelStreamingLoaded = WorldIsLoad;
}

void ACS_PlayerController::SetMouseToCenter()
{
	int32 SizeX = 0;
	int32 SizeY = 0;
	
	GetViewportSize(SizeX, SizeY);

	// Set -2 on X for doesn't activate top element of some menu
	SetMouseLocation(SizeX/2 - 2, SizeY/2);
}

void ACS_PlayerController::SetIsExitPlaceCooldown(const bool IsExitPlaceCooldown)
{
	bIsExitPlaceCooldown = IsExitPlaceCooldown;
}

void ACS_PlayerController::SetIsExitPlaceCooldownFalse()
{
	bIsExitPlaceCooldown = false;
}

void ACS_PlayerController::LogWebSocketSender(const FString& Message)
{
	UE_LOG(LogACS_PlayerController, Log, TEXT("%s"), *Message);
}

void ACS_PlayerController::ClearPointingUpdatePlace()
{
	ClearCurrentComponent();

	if (GetPlayerCharacter())
		GetPlayerCharacter()->ClearHitActor();
}

void ACS_PlayerController::TryClickedDialog()
{
	if (GetPlayerHUD())
	{
		FString DialogID = GetPlayerHUD()->GetLastDialogID();

		if (!DialogID.IsEmpty()) 
		{
			OnClickObject(DialogID, DialogID);
		}
	}
}

#pragma region MouseLogic

void ACS_PlayerController::EnableRightClick()
{
	bEnableRightClick = true;
}

void ACS_PlayerController::DisableRightClick()
{
	bEnableRightClick = false;
}

void ACS_PlayerController::CheckUnderCursorRigthClick()
{	
	if (!bEnableRightClick)
	{
		return;
	}

	FHitResult HitResult;

	if (GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery3, false, HitResult))
	{
		if (HitResult.GetActor() && HitResult.GetActor()->GetClass()->ImplementsInterface(UCS_Interact_Interface::StaticClass()))
		{
			ICS_Interact_Interface* InterfaceObject = Cast<ICS_Interact_Interface>(HitResult.GetActor());
			if (InterfaceObject)
			{
				InterfaceObject->Execute_RightClick(HitResult.GetActor(), HitResult.GetComponent());
			}
		}
	}
}

void ACS_PlayerController::TryLeftClick()
{
	if (GetPlayerHUD())
	{
		if (GetPlayerHUD()->IsMouseOverAnyWidget())
		{
			ClearCurrentComponent();
			return;
		}
	}

	if (CurrentComponent && CurrentComponent->Implements<UCS_Interact_Component_Interface>())
	{
		if (ICS_Interact_Component_Interface::Execute_GetInteractable(CurrentComponent, PlayerState->GetCurrentInteractMode())) 
		{
			ICS_Interact_Component_Interface::Execute_LeftClickInteract(CurrentComponent);

			if (ICS_Interact_Component_Interface::Execute_GetTypePoint(CurrentComponent) != ETypeInteractPoint::MeausurePoint)
			{
				OnClickObject(ICS_Interact_Component_Interface::Execute_GetInteractID(CurrentComponent), ICS_Interact_Component_Interface::Execute_GetInteractID(CurrentComponent));
			}
		}
	}
}

void ACS_PlayerController::TraceMouseCursor()
{
	FHitResult HitResult;

	if (GetPlayerHUD()) 
	{
		if (GetPlayerHUD()->IsMouseOverAnyWidget())
		{
			ClearCurrentComponent();
			return;
		}
	}
	bool bHitFound = false;

	if (GetHitResultUnderCursor(SIM_INTERACT, true, HitResult))
	{
		bHitFound = true;
	}
	/*else if (GetHitResultUnderCursor(Sim_Interact, false, HitResult))
	{
		bHitFound = true;
	}*/

	if (bHitFound)
	{
		UActorComponent* HitComp = HitResult.GetComponent();

		if (HitComp && HitComp != CurrentComponent && HitComp->Implements<UCS_Interact_Component_Interface>())
		{
			ClearCurrentComponent();

			if (ICS_Interact_Component_Interface::Execute_GetInteractable(HitComp, PlayerState->GetCurrentInteractMode()))
			{
				CurrentComponent = HitComp;
				ICS_Interact_Component_Interface::Execute_SetOutline(CurrentComponent, true);
				ShowMouseDescription(ICS_Interact_Component_Interface::Execute_GetInteractID(CurrentComponent), ICS_Interact_Component_Interface::Execute_GetDescription(CurrentComponent));
			}
		}

		if (HitComp && !HitComp->Implements<UCS_Interact_Component_Interface>())
			ClearCurrentComponent();
	}
	else
	{
		ClearCurrentComponent();
	}
}

void ACS_PlayerController::ClearCurrentComponent()
{
	if (CurrentComponent && CurrentComponent->Implements<UCS_Interact_Component_Interface>())
	{
		ICS_Interact_Component_Interface::Execute_SetOutline(CurrentComponent, false);
		ShowMouseDescription(ICS_Interact_Component_Interface::Execute_GetInteractID(CurrentComponent), FString());

		CurrentComponent = nullptr;
	}
}

#pragma endregion

void ACS_PlayerController::SetGetMainCourseMapLoadedFun(TFunction<bool()> Func)
{
	GetMainCourseMapLoadedFun = Func;
}

void ACS_PlayerController::StartCourse()
{
	if (IsValid(WebSocketServer))
	{
		int32 PortNumber = 8080;
		FParse::Value(FCommandLine::Get(), TEXT("-p"), PortNumber);

		if (!WebSocketServer->Start(PortNumber))
		{
			FTimerHandle EndTweenTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(EndTweenTimerHandle, this, &ThisClass::StartCheckMapLoaded, 1.f, false);
		}
	}
}

void ACS_PlayerController::SetLastInteractActorComp(UActorComponent* ActorComponent)
{
	LastInteractActorComp = ActorComponent;
}

UActorComponent* ACS_PlayerController::GetLastInteractActorComp() const
{
	return LastInteractActorComp.Get();
}

#pragma region Zoom

void ACS_PlayerController::HandleZoomInput(const FInputActionValue& Value)
{	
	if (bIsLockZoom) return;

	float ZoomValue = Value.Get<float>();

	if (CurrentInteractableObject.IsValid())
	{
		CurrentInteractableObject->Zoom(ZoomValue);
		ZoomChanged(CurrentInteractableObject->GetZoomPercent());
	}
}

void ACS_PlayerController::ApplyZoom(const float Direction)
{
	if (CurrentInteractableObject.IsValid())
	{
		CurrentInteractableObject->Zoom(Direction);
		ZoomChanged(CurrentInteractableObject->GetZoomPercent());
	}
}

void ACS_PlayerController::ResetZoom()
{
	bool bResetObject = true;

	if (GetPlayerHUD())
	{
		bResetObject = !(GetPlayerHUD()->ResetZoomInUI());
	}

	if (CurrentInteractableObject.IsValid() && bResetObject)
	{
		CurrentInteractableObject->ResetZoom();
	}
}

void ACS_PlayerController::ShowZoomToInteractObject()
{
	if (CurrentInteractableObject.IsValid())
		ZoomChanged(CurrentInteractableObject->GetZoomPercent());
	else if (GetPlayerCharacter())
		ZoomChanged(GetPlayerCharacter()->GetZoomPercent());
}

void ACS_PlayerController::ZoomChanged(int PercentZoom)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ZoomChanged(PercentZoom);
}

void ACS_PlayerController::Lock2DZoom(const bool bIsLocking)
{
	bIsLockZoom = bIsLocking;
}

#pragma endregion

#pragma region CameraPanning

void ACS_PlayerController::StartCameraPan()
{
	// Защита на случай, если Interactable удалён до входа сюда
	if (!CurrentInteractableObject.IsValid())
	{
		UE_LOG(LogACS_PlayerController, Warning, TEXT("StartCameraPan: CurrentInteractableObject is invalid"));
		return;
	}

	if (!Cast<ACS_Interactable_2d_Actor>(CurrentInteractableObject.Get()))
	{
		UE_LOG(LogACS_PlayerController, Verbose, TEXT("StartCameraPan: CurrentInteractableObject is not 2D actor"));
		return;
	}

	// Проверяем LocalPlayer и ViewportClient перед вызовом GetMousePosition/CenterMouseCursor
	if (!(GetLocalPlayer() && GetLocalPlayer()->ViewportClient))
	{
		UE_LOG(LogACS_PlayerController, Warning, TEXT("StartCameraPan: No LocalPlayer or ViewportClient"));
		return;
	}

	bIsCameraPanning = true;

	// Переключаем режим ввода
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

	// Безопасно получить позицию мыши (GetMousePosition возвращает bool)
	float X = 0.f, Y = 0.f;
	if (GetMousePosition(X, Y))
	{
		LastMousePosition = FVector2D(X, Y);
	}
	else
	{
		UE_LOG(LogACS_PlayerController, Warning, TEXT("StartCameraPan: GetMousePosition failed"));
		LastMousePosition = FVector2D::ZeroVector;
	}

	// Центрируем курсор только если есть Viewport
	FVector2D ViewportSize;
	GetLocalPlayer()->ViewportClient->GetViewportSize(ViewportSize);
	FVector2D Center = ViewportSize * 0.5f;
	SetMouseLocation(FMath::RoundToInt(Center.X), FMath::RoundToInt(Center.Y));

	UE_LOG(LogACS_PlayerController, Log, TEXT("Camera pan started"));
}

void ACS_PlayerController::StopCameraPan()
{
	if (!bIsCameraPanning) return;

	bIsCameraPanning = false;

	if (CurrentInteractableObject.IsValid())
		if (Cast<ACS_Interactable_2d_Actor>(CurrentInteractableObject.Get()))
		{
			SetMouseToCenter();
			bShowMouseCursor = true;
			FInputModeGameAndUI InputMode;
			SetInputMode(InputMode);
		}

	SetMouseLocation(FMath::RoundToInt(LastMousePosition.X), FMath::RoundToInt(LastMousePosition.Y));

	UE_LOG(LogACS_PlayerController, Log, TEXT("Camera pan stopped"));
}

void ACS_PlayerController::HandleCameraPan(const FInputActionValue& Value)
{	
	UE_LOG(LogACS_PlayerController, VeryVerbose, TEXT("HandleCameraPan called"));

	if (!bIsCameraPanning)
		return;

	if (!CurrentInteractableObject.IsValid())
	{
		UE_LOG(LogACS_PlayerController, Warning, TEXT("HandleCameraPan: CurrentInteractableObject became invalid while panning — stopping pan"));
		StopCameraPan();
		return;
	}

	if (!(GetLocalPlayer() && GetLocalPlayer()->ViewportClient))
	{
		UE_LOG(LogACS_PlayerController, Warning, TEXT("HandleCameraPan: Missing LocalPlayer or ViewportClient"));
		return;
	}

	FVector2D InputDelta = Value.Get<FVector2D>();
	UE_LOG(LogACS_PlayerController, Log, TEXT("Mouse Delta: X=%.2f Y=%.2f"), InputDelta.X, InputDelta.Y);

	CurrentInteractableObject->CameraPanning(InputDelta);

	CenterMouseCursor();
}

void ACS_PlayerController::CenterMouseCursor()
{
	if (GetLocalPlayer() && GetLocalPlayer()->ViewportClient)
	{
		FVector2D ViewportSize;
		GetLocalPlayer()->ViewportClient->GetViewportSize(ViewportSize);

		FVector2D CenterPosition = ViewportSize * 0.5f;
		SetMouseLocation(CenterPosition.X, CenterPosition.Y);
	}
}

#pragma endregion

void ACS_PlayerController::UpdateMoveZones(const bool& bUseUpMoveZone, const bool& bUseDownMoveZone, const bool& bUseLeftMoveZone, const bool& bUseRightMoveZone)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->UpdateArrowsHUD(bUseUpMoveZone, bUseDownMoveZone, bUseLeftMoveZone, bUseRightMoveZone);
}

void ACS_PlayerController::TryPlaySound(const FString& ID_Sound)
{

	if (!GetWorld()) return;

	if (ACS_VoiceManager* VoiceManager = Cast<ACS_VoiceManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACS_VoiceManager::StaticClass())))
		VoiceManager->PlaySound(ID_Sound);
}

void ACS_PlayerController::InitQueueSoundsDialog(const TArray<FString> QueueSounds)
{
	if (ACS_VoiceManager* VoiceManager = Cast<ACS_VoiceManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACS_VoiceManager::StaticClass())))
		VoiceManager->AddQueueSounds(QueueSounds);
}

void ACS_PlayerController::InitAddQueueSoundsToDialog(const TArray<FString> QueueSounds)
{
	if (ACS_VoiceManager* VoiceManager = Cast<ACS_VoiceManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACS_VoiceManager::StaticClass())))
		VoiceManager->AddQueueSounds(QueueSounds);
}

void ACS_PlayerController::RemoveSoundsInQueue()
{
	if (ACS_VoiceManager* VoiceManager = Cast<ACS_VoiceManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACS_VoiceManager::StaticClass())))
		VoiceManager->RemoveAllSoundsInQueue();
}

void ACS_PlayerController::StopVoiceManager(bool bPause)
{
	if (ACS_VoiceManager* VoiceManager = Cast<ACS_VoiceManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACS_VoiceManager::StaticClass())))
		VoiceManager->PauseSounds(bPause);
}

void ACS_PlayerController::CallStartCourse()
{
	OnStartedCourse.Broadcast();

	bCourseStarted = true;
}

void ACS_PlayerController::CallTeleport()
{
	OnTeleport.Broadcast();
}

#pragma region EndZone

void ACS_PlayerController::TryShowEndZone()
{
	CounterEndZone++;

	if ((CounterEndZone > 0) && (CounterEndZone < 2) && !LockedBoundaryHideTimerHandle.IsValid())
	{
		ShowEndZone();
	}
}

void ACS_PlayerController::ShowEndZone()
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowEndZone(true);
}

void ACS_PlayerController::TryCloseEndZone()
{
	CounterEndZone--;

	if (CounterEndZone <= 0)
	{
		CloseEndZone();
	}
}

void ACS_PlayerController::CloseEndZone()
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowEndZone(false);
}

#pragma endregion

#pragma region LockedBoundary

void ACS_PlayerController::TryShowLockedBoundary()
{
	CounterLockedBoundary++;

	if (CounterEndZone > 0)
	{
		CloseEndZone();
	}
	
	if ((CounterLockedBoundary > 0) && (CounterLockedBoundary < 2))
	{
		ShowLockedBoundary();
	}

	//Set timer for send message to AOS
	if (!bIsExitPlaceCooldown)
	{
		bIsExitPlaceCooldown = true;
		ExitPlace();

		FTimerHandle TenSecondsTimerHandle = FTimerHandle();
		GetWorldTimerManager().SetTimer(TenSecondsTimerHandle,
			this, &ACS_PlayerController::SetIsExitPlaceCooldownFalse,10.f, false, -1.f);
	}

	//Set timer for hide LockedBoundary warning
	if (LockedBoundaryHideTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(LockedBoundaryHideTimerHandle);
	}
	LockedBoundaryHideTimerHandle = FTimerHandle();
	GetWorldTimerManager().SetTimer(LockedBoundaryHideTimerHandle,
			this, &ACS_PlayerController::CheckAllBoundaryWarnings,3.f, false, -1.f);
}

void ACS_PlayerController::ShowLockedBoundary()
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowLockedBoundaryWidget(true);
}

void ACS_PlayerController::TryHideLockedBoundary()
{
	CounterLockedBoundary--;
	
	if (CounterLockedBoundary <= 0)
	{
		HideLockedBoundary();
	}

	if (CounterEndZone > 0)
	{
		ShowEndZone();
	}
}

void ACS_PlayerController::HideLockedBoundary()
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ShowLockedBoundaryWidget(false);

	//If hide timer valid, invalidate it
	if (LockedBoundaryHideTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(LockedBoundaryHideTimerHandle);
	}
}

void ACS_PlayerController::CheckAllBoundaryWarnings()
{
	//Hide all warnings
	if (GetPlayerHUD())
	{
		CloseEndZone();
		HideLockedBoundary();
	}
	
	//Enable warnings. EndZone has more priority.
	if (CounterEndZone > 0)
	{
		ShowEndZone();
	}
}

void ACS_PlayerController::SetLockedBoundaryWidgetText(const FString& InText)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->SetLockedBoundaryWidgetText(InText);
}

#pragma endregion

ACS_PlayerState* ACS_PlayerController::GetCS_PlayerState() const
{
	return PlayerState.IsValid() ? PlayerState.Get() : nullptr;
}

#pragma region HotKeys

void ACS_PlayerController::ActivateHotKey(bool bEnable, FGameplayTag ActivatedTag, TArray<FGameplayTag> UsedTags)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ActivateHotKey(bEnable, ActivatedTag, UsedTags);
}

void ACS_PlayerController::BindToHotKeys()
{
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (const UCS_DeveloperSettings* Settings = GetDefault<UCS_DeveloperSettings>()) {
			if (UCS_SimulatorParamsAsset* SimulatorParams = Settings->SimulatorParamsAsset.LoadSynchronous()) {
				HotKeysStruct = SimulatorParams->HotKeys;
				int Counter = 0;

				for (int i = 0; i < HotKeysStruct.Num(); i++)
				{
					if (!HotKeysStruct[i].bEnabled) continue;

					if (HotKeysAction.IsValidIndex(Counter))
					{
						if (UInputAction* InputAction = HotKeysAction[Counter])
						{
							EnhancedInput->BindAction(InputAction, ETriggerEvent::Triggered, this,
								&ACS_PlayerController::OnAnyHotKey);

							if (HotKeysStruct[i].Activated != FGameplayTag::EmptyTag) {
								MapHotKeysTag.Add(InputAction, HotKeysStruct[i].Activated);
								MapActiveTagToUsingTags.Add(HotKeysStruct[i].Activated, HotKeysStruct[i].Used);
							}

							if (HotKeysStruct[i].Type == ETypeHotKey::CellMode) {

								MapActiveTagCellMode.Add(HotKeysStruct[i].Activated, false);

								if (GetPlayerHUD())
									for (int j = 0; j < HotKeysStruct[i].Used.Num(); j++)
										GetPlayerHUD()->TagsForDisableInputForCharacter.AddTag(HotKeysStruct[i].Used[j]);
							}
						}
					}
					Counter++;
				}
			}
		}
	}
}

void ACS_PlayerController::OnAnyHotKey(const FInputActionInstance& Instance)
{
	if (const UInputAction* Action = Instance.GetSourceAction())
	{
		if (MapHotKeysTag.Contains(Action))
		{
			FGameplayTag Tag = MapHotKeysTag[Action];
			HotKeyObjectClick(Tag);
		}
	}
}

void ACS_PlayerController::EnableBlinkingToHotKeysByTag(const FString SubKey, const FGameplayTag ActivatedTag, const bool bEnable)
{
	if (GetPlayerHUD()) {
		GetPlayerHUD()->EnableBlinkingToHotKeysByTag(ActivatedTag, bEnable);

		K2_BlinkingSpecialPoint(SubKey, ActivatedTag, bEnable);
	}
}

void ACS_PlayerController::HotKeyObjectClick(FGameplayTag InputActiveTag)
{
	if (GetPlayerHUD())
		if (GetPlayerHUD()->IsShowingFullscreenWidget()) {
			
			TArray<FGameplayTag> UsingTags;
			if (MapActiveTagToUsingTags.Contains(InputActiveTag))
				UsingTags = MapActiveTagToUsingTags[InputActiveTag];

			for (int i = 0; i < UsingTags.Num(); i++)
				if (!GetPlayerHUD()->IsShowingFullscreenWidgetByTag(UsingTags[i]))
					return;
		}

	if (MapActiveTagToApiID.Contains(InputActiveTag)) 
	{
		FString ApiID = MapActiveTagToApiID[InputActiveTag];

		if (!MapActiveTagUsing.Contains(InputActiveTag)) 
		{
			if (!ApiID.IsEmpty()) 
			{
				if (GetPlayerHUD())
				{
					GetPlayerHUD()->TrySwitchDialogHotKey(InputActiveTag);
				}

				OnClickObject(ApiID, ApiID);
				K2_ClickedHotKey(InputActiveTag, ApiID, true);
				MapActiveTagUsing.Add(InputActiveTag, true);

				ApplyUsingTags(InputActiveTag, true);
			}
		}
		else 
		{
			MapActiveTagUsing.Remove(InputActiveTag);

			K2_ClickedHotKey(InputActiveTag, ApiID, false);

			if (GetPlayerHUD())
			{
				GetPlayerHUD()->TryCloseHotKeyWidget(InputActiveTag);
				ApplyUsingTags(InputActiveTag, false);
			}

			if (CurrentActiveCellModeTag == InputActiveTag)
			{
				if (CurrentInteractableObject.IsValid()) {
					//CurrentInteractableObject->StopInteract();
					BackButton();
				}

				CurrentActiveCellModeTag = FGameplayTag::EmptyTag;
			}
			
		}
	}
}

ACS_Character* ACS_PlayerController::GetPlayerCharacter() const
{
	if (PlayerCharacter)
		return PlayerCharacter;


	if (ACS_Character* LS_Character = Cast<ACS_Character>(GetPawn()))
	{
		PlayerCharacter = LS_Character;
		return PlayerCharacter;
	}

	return nullptr;
}

void ACS_PlayerController::SetFPSLimit(int32 FPSLimit)
{
	if (!IsInGameThread())
	{
		AsyncTask(ENamedThreads::GameThread, [this, FPSLimit]()
			{
				SetFPSLimit(FPSLimit);
			});
		return;
	}
	FString Command = FString::Printf(TEXT("t.MaxFPS %d"), FPSLimit);
}

void ACS_PlayerController::AddActiveTagToApiID(FGameplayTag ActiveTag, FString ApiID)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ChangeActivatedTag(ActiveTag, true);

	MapActiveTagToApiID.Add(ActiveTag, ApiID);
}

void ACS_PlayerController::RemoveActiveTag(FGameplayTag ActiveTag)
{
	if (GetPlayerHUD())
		GetPlayerHUD()->ChangeActivatedTag(ActiveTag, false);

	MapActiveTagToApiID.Remove(ActiveTag);
}

void ACS_PlayerController::ClearActiveTagToApiID()
{
	TArray<FGameplayTag> ActivateTags;
	MapActiveTagToApiID.GetKeys(ActivateTags);

	for (int i = ActivateTags.Num() - 1; i >= 0; i--) 
	{
		if (GetPlayerHUD()) 
		{
			if (ActivateTags[i] == CurrentActiveCellModeTag) continue;

			GetPlayerHUD()->ChangeActivatedTag(ActivateTags[i], false);
			GetPlayerHUD()->TryCloseHotKeyWidget(ActivateTags[i]);

			if (MapActiveTagToUsingTags.Contains(ActivateTags[i]))
				GetPlayerHUD()->ChangeUsingTags(MapActiveTagToUsingTags[ActivateTags[i]], false);


			MapActiveTagToApiID.Remove(ActivateTags[i]);
		}
	}
}

void ACS_PlayerController::ClearActiveTagUsing()
{
	TArray<FGameplayTag> UsingTags;
	MapActiveTagUsing.GetKeys(UsingTags);

	for (int i = UsingTags.Num() - 1; i >= 0; i--)
	{
		if (GetPlayerHUD())
		{
			if (UsingTags[i] != CurrentActiveCellModeTag) 
				MapActiveTagUsing.Remove(UsingTags[i]);
		}
	}
}

void ACS_PlayerController::ApplyUsingTags(FGameplayTag ActiveTag, bool bUsing)
{
	TArray<FGameplayTag> UsingTags;
	if (MapActiveTagToUsingTags.Contains(ActiveTag))
		UsingTags = MapActiveTagToUsingTags[ActiveTag];

	if (GetPlayerHUD())
		GetPlayerHUD()->ChangeUsingTags(UsingTags, bUsing);
}

#pragma endregion

void ACS_PlayerController::UpdateCurrentCellMode(FGameplayTag ActiveTagCellMode, FString ApiID)
{
	AddActiveTagToApiID(ActiveTagCellMode, ApiID);

	ApplyUsingTags(ActiveTagCellMode, true);

	MapActiveTagUsing.Add(ActiveTagCellMode, true);

	if (CurrentActiveCellModeTag != FGameplayTag::EmptyTag)
	{
		if (CurrentInteractableObject.IsValid())
			CurrentInteractableObject->StopInteract();

		CurrentActiveCellModeTag = FGameplayTag::EmptyTag;
	}

	if (MapActiveTagCellMode.Contains(ActiveTagCellMode))
		CurrentActiveCellModeTag = ActiveTagCellMode;
}

bool ACS_PlayerController::Teleport(FString LocationID)
{
	if (CurrentInteractableObject.IsValid())
	{
		if (CurrentInteractableObject->TryToSubTeleport_Implementation(LocationID))
		{
			EndTween(LocationID);
			CurrentInteractableObject->StopInteract();

			return true;
		}
	}


	UWorld* World = GetWorld();
	if (World)
	{
		AGameStateBase* GameState = World->GetGameState();
		if (GameState)
		{
			if (GameState->Implements<UCS_GameState_Interface>()) {

				TArray<AActor*> InteractActors;

				InteractActors = ICS_GameState_Interface::Execute_GetAllInteractableActors(GameState);

				for (AActor*& Actor : InteractActors)
				{
					if (ICS_Interact_Interface::Execute_IsUseKey(Actor, LocationID)) {
						
						EndTween(LocationID);

						if (CurrentInteractableObject.IsValid())
						{
							CurrentInteractableObject->StopInteract();
							if (GetPlayerHUD())
								GetPlayerHUD()->StopInteractWithObject();
						}

						ICS_Interact_Interface::Execute_Teleport(Actor, this, LocationID);
						CallTeleport();

						return true;
					}
				}
			}
		}
	}

	return false;
}
