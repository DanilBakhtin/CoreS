// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CS_GameMode.h"

#include "JsonObjectConverter.h"
#include "Framework/CS_CoreTypes.h"
#include "Framework/CS_DeveloperSettings.h"
#include "Framework/CS_SimulatorParamsAsset.h"
#include "Framework/CS_UtilityFunctionLibrary.h"
#include "Gameplay/ObjectsOfInteract/CS_Interact_Interface.h"
#include "Gameplay/Player/CS_Character.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Kismet/KismetStringLibrary.h"
#include "WebSocketServer/WebSocketServer.h"
#include <Framework/CS_GameInstance.h>
#include <Framework/CS_JsonParser.h>

#include "Gameplay/ObjectsOfInteract/CS_Location_Interface.h"
#include "Gameplay/Player/CS_PlayerState.h"

#include "Framework/CS_GameState_Interface.h"

#include "Gameplay/Utilities/CS_ImageManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogACS_GameMode, All, All);

ACS_GameMode::ACS_GameMode()
{
	TShowWelcome = [this](const FParametersObject&  Parameters){ return ShowWelcome(Parameters);};
	DataFunction.Add("showWelcome",TShowWelcome);
	TShowDialog = [this](const FParametersObject&  Parameters){ return ShowDialog(Parameters);};
	DataFunction.Add("showDialog", TShowDialog);
	TAddDialogMessage = [this](const FParametersObject&  Parameters){ return AddDialogMessage(Parameters);};
	DataFunction.Add("addDialogMessage", TAddDialogMessage);
	TUpdateDialog = [this](const FParametersObject&  Parameters){ return UpdateDialog(Parameters);};
	DataFunction.Add("updateDialog", TUpdateDialog);
	TShowMenu = [this](const FParametersObject&  Parameters){ return ShowMenu(Parameters);};
	DataFunction.Add("showMenu",TShowMenu);
	TShowMessage = [this](const FParametersObject&  Parameters){ return ShowMessage(Parameters);};
	DataFunction.Add("showMessage",TShowMessage);
	TShowResult = [this](const FParametersObject&  Parameters){ return ShowResult(Parameters);};
	DataFunction.Add("showResult",TShowResult);
	TMeasureDevices = [this](const FParametersObject&  Parameters){ return MeasureDevices(Parameters);};
	DataFunction.Add("showMeasure", TMeasureDevices);
	TShowMeasureResult = [this](const FParametersObject&  Parameters){ return ShowMeasureResult(Parameters);};
	DataFunction.Add("showMeasureResult", TShowMeasureResult);
	TShowReaction = [this](const FParametersObject&  Parameters){ return ShowReaction(Parameters);};
	DataFunction.Add("showReaction", TShowReaction);
	TShowStepInfo = [this](const FParametersObject& Parameters) { return ShowStepInfo(Parameters); };
	DataFunction.Add("showStepInfo", TShowStepInfo);
	TShowLearningSticker = [this](const FParametersObject& Parameters) { return ShowLearningSticker(Parameters); };
	DataFunction.Add("showLearningSticker", TShowLearningSticker);
	TAddAction = [this](const FParametersObject& Parameters) { return AddAction(Parameters); };
	DataFunction.Add("addAction", TAddAction);
	TPlaySound = [this](const FParametersObject& Parameters) { return PlaySound(Parameters); };
	DataFunction.Add("playSound", TPlaySound);
	// ~Begin #VSP_only
	TShowObjectMenu = [this](const FParametersObject& Parameters) { return ShowObjectMenu(Parameters); };
	DataFunction.Add("showObjectMenu", TShowObjectMenu);
	// ~End VSP_only
	TShowTime = [this](const FParametersObject& Parameters) { return ShowTime(Parameters); };
	DataFunction.Add("showTime", TShowTime);
	TShowPlace = [this](const FParametersObject& Parameters) { return ShowPlace(Parameters); };
	DataFunction.Add("showPlace", TShowPlace);
	TTeleport = [this](const FParametersObject& Parameters) { return Teleport(Parameters); };
	DataFunction.Add("Teleport", TTeleport);
	TUpdatePlace = [this](const FParametersObject& Parameters) { return UpdatePlace(Parameters); };
	DataFunction.Add("updatePlace", TUpdatePlace);
	TShowPoints = [this](const FParametersObject& Parameters) { return ShowPoints(Parameters); };
	DataFunction.Add("showPoints", TShowPoints);
	// ~Begin #VSP_only
	TUpdatePlaceLock = [this](const FParametersObject& Parameters) { return UpdatePlaceLock(Parameters); };
	DataFunction.Add("updatePlaceLock", TUpdatePlaceLock);
	// ~End VSP_only
	TShowScheme = [this](const FParametersObject& Parameters) { return ShowScheme(Parameters); };
	DataFunction.Add("showScheme", TShowScheme);
}

void ACS_GameMode::BeginPlay()
{
	Super::BeginPlay();

	InitializeJsonParser();

	WebSocketServer = GetWorld()->SpawnActor<AWebSocketServer>(AWebSocketServer::StaticClass(), FVector(), FRotator());
	check(WebSocketServer)
	WebSocketServer->OnJsonReceived.AddDynamic(this, &ThisClass::JsonReceived);
	WebSocketServer->OnClientConnected.AddDynamic(this, &ThisClass::ClientConnected);
	WebSocketServer->SetTickableWhenPaused(true);

	if(UCS_SimulatorParamsAsset* SimulatorParams = GetDefault<UCS_DeveloperSettings>()->SimulatorParamsAsset.LoadSynchronous())
	{
		TSoftObjectPtr<UWorld> MainCourseMap = SimulatorParams->Params.MainCourseMap.LoadSynchronous();
		if(MainCourseMap.IsValid())
		{
			bool Success = false;
			TObjectPtr<ULevelStreamingDynamic> LoadedLevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(this,MainCourseMap,FTransform(),Success);
			if(IsValid(LoadedLevel))
				LoadedLevel->OnLevelShown.AddDynamic(this, &ACS_GameMode::MainCourseMapLoaded);

			if (UCS_ImageManager* ImageManager = GetGameInstance()->GetSubsystem<UCS_ImageManager>())
			{
				ImageManager->LoadingAllIcons();
			}
		}

		//Init map dialogs subkey
		MapDialogSubKeys = SimulatorParams->Params.MapDialogSubKeys;
	}
}

void ACS_GameMode::StartPlay()
{
	Super::StartPlay();
}

void ACS_GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	TFunction<void()> EndTweenDelay = [this, NewPlayer, &EndTweenDelay]()
	{
		if(IsValid(WebSocketServer))
		{
			if(ACS_PlayerController* PC = Cast<ACS_PlayerController>(NewPlayer))
			{
				PC->SetWebSocketServer(WebSocketServer);
				PC->SetGetMainCourseMapLoadedFun([&](){ return GetMainCourseMapLoaded();});
				CurrentPlayerController = PC;
			}
		}
		else
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle,FTimerDelegate::CreateLambda(EndTweenDelay), 0.5f, false);
		}
	};
	FTimerHandle EndTweenTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(EndTweenTimerHandle,FTimerDelegate::CreateLambda(EndTweenDelay), 0.5f, false);
}

void ACS_GameMode::UseMethod(const FString& Method, const FParametersObject& Parameters)
{
	if(!DataFunction.Contains(Method))
		return;
	DataFunction[Method](Parameters);

	if(Method != "showTime")
		UE_LOG(LogACS_GameMode, Display, TEXT("UseMethod: %s"), *Method);
}

void ACS_GameMode::UseReactMethod(const FString& Method, const FParametersObject& Parameters)
{
	if(Method == "")
		return;

	FVariables Variable;

	if (Parameters.Parameters.Num() >= 1) {
		Variable.parameterName = Parameters.Parameters[0].parameterName;
		Variable.parameterValue = Parameters.Parameters[0].parameterValue;
		Variable.parameterType = Parameters.Parameters[0].parameterType;

		if (IsGameStateImplementedCSInterface())
			Variable.bStartedState = ICS_GameState_Interface::Execute_GetStartingStateInitialized(GameState.Get());
	}

	UE_LOG(LogACS_GameMode, Display, TEXT("Call ReactMethod: %s"),*Method);

	FindAndUseObjectByReactMethod(Method, Variable);
}

void ACS_GameMode::JsonReceived(const FString& Payload)
{	
	if (!JsonParser)
	{
		UE_LOG(LogACS_GameMode, Error, TEXT("JSON Parser is not initialized!"));
		return;
	}

	FString ObjectId, Method;
	FParametersObject Parameters;
	double DelaySeconds = 0.0;

	if (!JsonParser->ParseJsonPayload(Payload, ObjectId, Method, Parameters, DelaySeconds))
	{
		UE_LOG(LogACS_GameMode, Warning, TEXT("Failed to parse JSON payload: %s"), *Payload);
		return;
	}

	if (Method != "showTime")
		UE_LOG(LogACS_GameMode, Display, TEXT("JSON STRING: %s"),*Payload);

	auto ExecuteMethod = [this, ObjectId, Method, Parameters]()
	{
		if (Method == "showTime")
		{
			UE_LOG(LogACS_GameMode, VeryVerbose, TEXT("Executing method: %s for object: %s with %d parameters"),
				*Method, *ObjectId, Parameters.Parameters.Num());
		}
		else
		{
			UE_LOG(LogACS_GameMode, Display, TEXT("Executing method: %s for object: %s with %d parameters"),
				*Method, *ObjectId, Parameters.Parameters.Num());
		}
		
	if (ObjectId == TEXT("api") + FString::FromInt(GetDefault<UCS_DeveloperSettings>()->CourseVersion))
		UseMethod(Method, Parameters);
	else
		UseReactMethod(ObjectId + TEXT("_") + Method, Parameters);

	if (Method != TEXT("showTime"))
		UE_LOG(LogACS_GameMode, Display, TEXT("Executed JSON Method: %s"), *Method);
			
	};

	if (DelaySeconds > KINDA_SMALL_NUMBER)
		{
			FTimerHandle Th;
			GetWorldTimerManager().SetTimer(
				Th,
				FTimerDelegate::CreateLambda(ExecuteMethod),
				DelaySeconds,
				false
			);
		}
	else
		ExecuteMethod();
}

void ACS_GameMode::ClientConnected()
{
	auto Delay = [this]()
	{
		FBodyOfAnswer ConnectAnswer = FBodyOfAnswer("connection","Event","OnReadyToAction","Ready to Action");
	
		TSharedPtr<FJsonObject> JsonObj = FJsonObjectConverter::UStructToJsonObject<FBodyOfAnswer>(ConnectAnswer);
		TSharedRef<FJsonValueObject> JsonValue = MakeShareable( new FJsonValueObject(JsonObj));

		FString JsonString = "";
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	
		FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);
		WebSocketServer->Send(JsonString);
		UE_LOG(LogACS_GameMode, Display, TEXT("Client Connected: %s"), *JsonString);
		StartCourse();
	};

	//Задержка необходима для того чтобы редактор шагов успел проинициализироваться.
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,Delay, 1.f, false);
	
}

void ACS_GameMode::StartCourse()
{
	if(IsValid(CurrentPlayerController) && GetMainCourseMapLoaded() && bMainCourseMapLoaded)
	{
		//Делаем запрос клиенту что бы прислал нам данные для заполнения Основного меню (Main Menu)
		CurrentPlayerController->OnMenuEvent();
		//Сообщаем клиенту что находмися на стартовой локации что бы получить Show Welcome (логика заглушка, без нее мы не можем начать прохожение курса)
		CurrentPlayerController->SendStartLocation();
	}
	else
	{
		FTimerHandle EndTweenTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(EndTweenTimerHandle, this, &ThisClass::StartCourse, 1.f, false);
	}
}

void ACS_GameMode::MainCourseMapLoaded()
{
	bMainCourseMapLoaded = true;
}

void ACS_GameMode::ShowWelcome(const FParametersObject& Parameters)
{
	if(IsValid(CurrentPlayerController))
	{
		APawn* CurrentPawn = CurrentPlayerController->GetPawn();
		FActorSpawnParameters SpawnParameters;
		FTransform CurrentPawnTransform = CurrentPawn->GetTransform();
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ACS_Character* NewPawn = GetWorld()->SpawnActor<ACS_Character>(CharacterClass, CurrentPawnTransform.GetLocation(), CurrentPawnTransform.GetRotation().Rotator(), SpawnParameters);
		CurrentPlayerController->Possess(NewPawn);
		CurrentPawn->Destroy();

		FShowWelcomeParams ShowWelcomeParams = FShowWelcomeParams();
		
		if (JsonParser->ParseShowWelcome(Parameters, ShowWelcomeParams)) {
			CurrentPlayerController->ShowWelcome(ShowWelcomeParams);
		}

		if (IsGameStateImplementedCSInterface())
			ICS_GameState_Interface::Execute_SetStartingStateInitialized(GameState.Get());
	}
}

void ACS_GameMode::ShowTime(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController) return;

	FTimespan Second;
	if (IsGameStateImplementedCSInterface())
	{
		if(JsonParser->ParseShowTime(Parameters, Second))
		{
			ICS_GameState_Interface::Execute_SetCurrentTime(GameState.Get(), FDateTime() + Second);
			CurrentPlayerController->SetTime(ICS_GameState_Interface::Execute_GetCurrentTime(GameState.Get()));
		}
		else
		{
			UE_LOG(LogACS_GameMode, Error, TEXT("Parsing Show Time was failed!"));
		}
	}
	else
	{
		UE_LOG(LogACS_GameMode, Error, TEXT("Game State is not implemented CS Interface!"
									  "Check setted Game State in Game mode."))
	}
}

void ACS_GameMode::Teleport(const FParametersObject& Parameters)
{
	for(const FParameters& Param : Parameters.Parameters)
	{
		if(Param.parameterName == "location")
		{
			if (CurrentPlayerController)
				if (CurrentPlayerController->Teleport(Param.parameterValue))
					return;
		}
	}
}

void ACS_GameMode::ShowPlace(const FParametersObject& Parameters)
{
	DisableInteractActors();

	DeactivateSpecialPoints();

	if (!CurrentPlayerController) return;

	CurrentPlayerController->ClearActiveTagUsing();

	FLocation LocationInfo = FLocation();
	if (JsonParser->ParseLocationInfo(Parameters, LocationInfo)) {

		CurrentPlayerController->GetCS_PlayerState()->SetCurrentLocation(LocationInfo.Name, LocationInfo.ApiID);
		CurrentPlayerController->SetLocationName(LocationInfo.Name);
		CurrentPlayerController->SetActivateBackButton(LocationInfo.bHasBackButton);

		// ~Begin #VSP_only
		// If location is locked, block player in location
		if (LocationInfo.bIsLocked)
		{	
			if (IsGameStateImplementedCSInterface()) {

				TArray<AActor*> TransferLocations;

				TransferLocations = ICS_GameState_Interface::Execute_GetLocationInteractActors(GameState.Get());

				for (AActor*& Actor : TransferLocations)
				{
					ICS_Location_Interface::Execute_SetLockLocation(Actor, LocationInfo.ApiID, LocationInfo.bIsLocked);
					CurrentPlayerController->SetLockedBoundaryWidgetText(LocationInfo.LockText);
				}
			}
		}
		// ~End VSP_only
	}

	TArray<FPlacePointData> PointsData;
	if (JsonParser->ParsePointsData(Parameters, PointsData)) {

		RecursionActivatePoints(PointsData, false);
	}
}

void ACS_GameMode::ShowPoints(const FParametersObject& Parameters)
{

	if (IsGameStateImplementedCSInterface()) {

		TArray<AActor*> InteractActors;
		InteractActors = ICS_GameState_Interface::Execute_GetAllInteractableActors(GameState.Get());

		for (AActor*& Actor : InteractActors)
		{
			ICS_Interact_Interface::Execute_DisablePoint(Actor);
		}
	}
	
	if (!CurrentPlayerController) return;

	FString ApiID = "";

	JsonParser->FindApiID(Parameters, ApiID);

	if (ApiID.IsEmpty()) return;

	for (TPair<FString, ETypeDialog>& Elem : MapDialogSubKeys)
	{
		if (ApiID.Contains(Elem.Key)) {

			if (CurrentPlayerController->GetCurrentTypeDialog() != ETypeDialog::None)
			{
				UpdateDialog(Parameters);
				return;
			}

			TArray<FDialogButtonParameters> DialogPointsParameters;
			JsonParser->ParseDialogPointsParams(Parameters, DialogPointsParameters);
			CheckBlinkingDialogItem(DialogPointsParameters);
			CurrentPlayerController->ShowDialogContacts(ApiID, DialogPointsParameters, Elem.Value);

			return;
		}
	}

	TArray<FContextMenuItem> ContextMenuItems;
	if (JsonParser->ParseContextMenu(Parameters, ApiID, ContextMenuItems)) {

		if (IsGameStateImplementedCSInterface()) {
			CurrentPlayerController->ShowContextMenu(ApiID, ContextMenuItems);
		}

	}
}

void ACS_GameMode::ShowDialog(const FParametersObject& Parameters)
{	
	if (!CurrentPlayerController) return;

	FString ApiID;
	FString NameTitle;
	FString IconTitlePath;
	TArray<FDialogParameters> Dialog;
	TArray<FString> QueueSoundsDialog;
	ETypeDialog TypeDialog;

	if (JsonParser->ParseDialogParameters(Parameters, ApiID, NameTitle, IconTitlePath, Dialog, QueueSoundsDialog, TypeDialog))
	{			
		if (UCS_ImageManager* ImageManager = GetGameInstance()->GetSubsystem<UCS_ImageManager>())
		{
			ImageManager->UpdatedInfoIconImageByPath(ApiID, IconTitlePath);
		}

		CurrentPlayerController->InitShowDialog(ApiID, NameTitle, Dialog, TypeDialog);
		CurrentPlayerController->InitQueueSoundsDialog(QueueSoundsDialog);
	}
	else
	{
		UE_LOG(LogACS_GameMode, Warning, TEXT("Failed to parse dialog parameters"));
	}
}

void ACS_GameMode::AddDialogMessage(const FParametersObject& Parameters)
{	
	if (!CurrentPlayerController)
		return;

	TArray<FDialogParameters> Dialog;
	TArray<FString> QueueSoundsDialog;

	if (JsonParser->ParseAddDialogMessageParameters(Parameters, Dialog, QueueSoundsDialog))
	{
		CurrentPlayerController->InitAddMessage(Dialog);
		CurrentPlayerController->InitAddQueueSoundsToDialog(QueueSoundsDialog);
	}
	else
	{
		UE_LOG(LogACS_GameMode, Warning, TEXT("Failed to parse add dialog message parameters"));
	}
}

void ACS_GameMode::UpdateDialog(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController)
		return;

	FString ApiID = "";
	JsonParser->FindApiID(Parameters, ApiID);
	TArray<FDialogParameters> Dialog;

	if (JsonParser->ParseUpdateDialogParameters(Parameters, Dialog))
	{
		CurrentPlayerController->InitUpdateDialog(ApiID, Dialog);
	}
	else
	{
		UE_LOG(LogACS_GameMode, Warning, TEXT("Failed to parse update dialog parameters"));
	}
}

void ACS_GameMode::ShowMenu(const FParametersObject& Parameters)
{
	FMainMenuParams MainMenuParams = FMainMenuParams();
	if (!CurrentPlayerController)
		return;


	if (JsonParser->ParseShowMenu(Parameters, MainMenuParams)) {

		if (MainMenuParams.ShowAlgorithmButton)
			SimulatorMode = ESimulatorMode::TrainingMode;
		else
			SimulatorMode = ESimulatorMode::ExamMode;


		if (UCS_ImageManager* ImageManager = GetGameInstance()->GetSubsystem<UCS_ImageManager>()) {

			TArray<FLoadingImageInfo> ImagesInfo;

			for (const auto& Elem : MainMenuParams.MapMainMenuPointsInfo)
			{
				const FString& Key = Elem.Key;
				const FMainMenuPointInfo& Value = Elem.Value;

				for (int j = 0; j < Value.LoadingImagesInfo.Num(); j++) {
					ImagesInfo.Add(Value.LoadingImagesInfo[j]);
				}
			}

			ImageManager->TryLoadImages(ImagesInfo);
			ImageManager->SetImagesPointsInfo(MainMenuParams.MapMainMenuPointsInfo);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Invalid ImageManager"));

		CurrentPlayerController->InitMainMenu(MainMenuParams);
	}
}

void ACS_GameMode::ShowMessage(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController) return;

	FShowMessageParams ShowMessageParams = FShowMessageParams();
	if (JsonParser->ParseShowMessage(Parameters, ShowMessageParams)) {

		if (ShowMessageParams.Modal) {
			CurrentPlayerController->ShowMessage(ShowMessageParams);
			switch (ShowMessageParams.Alarm) {
			case 0:
				AddMessageToLog(ShowMessageParams.Text, ELogType::Correct);
				break;
			case 1:
				AddMessageToLog(ShowMessageParams.Text, ELogType::Wrong);
				break;
			case 2:
				AddMessageToLog(ShowMessageParams.Text, ELogType::Attention);
				break;
			default:
				AddMessageToLog(ShowMessageParams.Text, ELogType::Reaction);
			}
		}
		else {
			CurrentPlayerController->ShowReaction(ShowMessageParams.Text);
			AddMessageToLog(ShowMessageParams.Text, ELogType::Reaction);
		}
	}
	
}

void ACS_GameMode::ShowResult(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController) return;

	FShowResultParams ShowResultParams = FShowResultParams();

	if (JsonParser->ParseShowResult(Parameters, ShowResultParams))
		CurrentPlayerController->ShowResult(ShowResultParams);
}

void ACS_GameMode::MeasureDevices(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController) return;

	FDeviceAndPointsData DeviceAndPointsData = FDeviceAndPointsData();
	if (JsonParser->ParseMeasureDevices(Parameters, DeviceAndPointsData)) {
		CurrentPlayerController->ShowMeasureDevicesList(DeviceAndPointsData);
	}
}

void ACS_GameMode::ShowMeasureResult(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController) return;

	float Result = 0.f;
	if (JsonParser->ParseShowMeasureResult(Parameters, Result))
		CurrentPlayerController->UpdateMeasureDevice(Result);
}

void ACS_GameMode::ShowReaction(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController) return;

	FString TextReaction = "";
	if (JsonParser->ParseShowReaction(Parameters, TextReaction))
	{
		CurrentPlayerController->ShowReaction(TextReaction);
		
		AddMessageToLog(TextReaction, ELogType::Reaction);
	}
}

void ACS_GameMode::ShowStepInfo(const FParametersObject& Parameters)
{	
	if (!CurrentPlayerController) return;

	int currentStep;
	if (JsonParser->ParseShowStepInfo(Parameters, currentStep))
		CurrentPlayerController->UpdateSteps(currentStep);
}

void ACS_GameMode::ShowLearningSticker(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController) return;

	FStickerParams StickerParams = FStickerParams();
	if (JsonParser->ParseShowLearningSticker(Parameters, StickerParams))
	{
		CurrentPlayerController->ShowLearningSticker(StickerParams);
	}
}

void ACS_GameMode::AddAction(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController) return;

	FString TextLog = "";
	if (JsonParser->ParseAddAction(Parameters, TextLog))
	{
		AddMessageToLog(TextLog, ELogType::Log);
	}
}

void ACS_GameMode::PlaySound(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController) return;

	FString IdSound = "";
	if (JsonParser->ParsePlaySound(Parameters, IdSound)) {
		CurrentPlayerController->TryPlaySound(IdSound);
	}
}

void ACS_GameMode::UpdatePlace(const FParametersObject& Parameters)
{
	DisableInteractActors();

	CurrentPlayerController->ClearPointingUpdatePlace();
	CurrentPlayerController->TryClickedDialog();

	TArray<FPlacePointData> PointsData;
	if (JsonParser->ParseUpdatePointsData(Parameters, PointsData)) {

		RecursionActivatePoints(PointsData, true);
	}
	
}

void ACS_GameMode::ShowObjectMenu(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController) return;

	FFullMenuReasons FullMenuReasons = FFullMenuReasons();
	if (JsonParser->ParseShowObjectMenu(Parameters, FullMenuReasons))
		CurrentPlayerController->ShowObjectMenu(FullMenuReasons);
}

void ACS_GameMode::UpdatePlaceLock(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController)
	{
		UE_LOG(LogTemp, Error,
			TEXT("GameMode: Trying to call 'UpdatePlaceLock', but CurrentPlayerController is invalid!"));
		return;
	}

	FLocation LocationInfo = FLocation();
	
	if (JsonParser->ParseUpdatePlaceLock(Parameters, LocationInfo))
	{
		if (IsGameStateImplementedCSInterface()) {

			TArray<AActor*> TransferLocations;

			TransferLocations = ICS_GameState_Interface::Execute_GetLocationInteractActors(GameState.Get());

			for (AActor*& Actor : TransferLocations)
			{
				ICS_Location_Interface::Execute_SetLockLocation(Actor, LocationInfo.ApiID, LocationInfo.bIsLocked);

				// UpdatePlaceLock called only for locked location. There is no need to check locked boolean.
				CurrentPlayerController->SetLockedBoundaryWidgetText(LocationInfo.LockText);
				//CurrentPlayerController->ShowLockedBoundary();
			}
		}
	}
}

void ACS_GameMode::ShowScheme(const FParametersObject& Parameters)
{
	if (!CurrentPlayerController) return;

	TArray<FLoadingImageInfo> SchemesParams;


	if (JsonParser->ParseSchemes(Parameters, SchemesParams)) {
		CurrentPlayerController->UpdateSchemes(SchemesParams);
	}
}

ESimulatorMode ACS_GameMode::GetSimulatorMode_Implementation() const
{
	return SimulatorMode;
}

void ACS_GameMode::InitializeJsonParser()
{
	JsonParser = NewObject<UCS_JsonParser>(this);
	if (!JsonParser)
	{
		UE_LOG(LogACS_GameMode, Error, TEXT("Failed to create JSON Parser!"));
	}
}

void ACS_GameMode::TryActivateSpecialPoints(const FString& ApiID, const FString& IconPath, const bool bUpdate)
{
	if (!CurrentPlayerController) return;

	if (UCS_SimulatorParamsAsset* SimulatorParams = GetDefault<UCS_DeveloperSettings>()->SimulatorParamsAsset.LoadSynchronous())
	{
		for (FHotkeySlot& HotKeyInfo : SimulatorParams->HotKeys)
		{
			if (!HotKeyInfo.SubKey.IsEmpty() && HotKeyInfo.bEnabled && ApiID.Contains(HotKeyInfo.SubKey))
			{
				UE_LOG(LogACS_GameMode, Warning, TEXT("TryActivateSpecialPoints Contains: ApiID %s Key %s"), *ApiID, *HotKeyInfo.SubKey);

				CurrentPlayerController->ActivateHotKey(true, HotKeyInfo.Activated, HotKeyInfo.Used);

				if (HotKeyInfo.Activated != FGameplayTag::EmptyTag) {
					CurrentPlayerController->EnableBlinkingToHotKeysByTag(HotKeyInfo.SubKey, HotKeyInfo.Activated, CheckBlinkingActor(IconPath));
					CurrentPlayerController->AddActiveTagToApiID(HotKeyInfo.Activated, ApiID);
				}

				break;
			}
		}
	}
}

void ACS_GameMode::RecursionActivatePoints(const TArray<FPlacePointData>& ArrayPlacePoints, bool bUpdate)
{
	for (const FPlacePointData& PointData : ArrayPlacePoints) {
		
		TryActivateSpecialPoints(PointData.ApiID, PointData.IconPath, bUpdate);

		FindAndUseObjectById(PointData.ApiID, PointData.Name);

		TryBlinkingObject(PointData.ApiID, PointData.IconPath);

		//if (!PointData.ViewStruct.ViewID.IsEmpty())
		FindAndUseViewById(PointData.ViewStruct);

		RecursionActivatePoints(PointData.ChildPoints, bUpdate);
	}
}

bool ACS_GameMode::IsGameStateImplementedCSInterface() const
{	
	return GameState->Implements<UCS_GameState_Interface>();
}

void ACS_GameMode::AddMessageToLog(FString& Descriptions, const ELogType InLogType)
{
	FLogParams LogParams = FLogParams();
	LogParams.Description = Descriptions;
	LogParams.LogType = InLogType;
	if (GameState.Get()->Implements<UCS_GameState_Interface>())
	{
		LogParams.Time = ICS_GameState_Interface::Execute_GetCurrentTime(GameState.Get());
	}

	CurrentPlayerController->AddMessageLog(LogParams);
}

void ACS_GameMode::DeactivateSpecialPoints()
{
	if (!CurrentPlayerController) return;

	if (UCS_SimulatorParamsAsset* SimulatorParams = GetDefault<UCS_DeveloperSettings>()->SimulatorParamsAsset.LoadSynchronous())
	{
		for (FHotkeySlot& HotKeyInfo : SimulatorParams->HotKeys) {

			CurrentPlayerController->ActivateHotKey(false, HotKeyInfo.Activated, HotKeyInfo.Used);

			if (HotKeyInfo.Activated != FGameplayTag::EmptyTag) 
			{
				CurrentPlayerController->EnableBlinkingToHotKeysByTag(HotKeyInfo.SubKey, HotKeyInfo.Activated, false);
				CurrentPlayerController->ClearActiveTagToApiID();
			}
		}
	}
}

bool ACS_GameMode::CheckBlinkingActor(const FString& ImagePath)
{
	if (ImagePath.IsEmpty())
		return false;

	return ImagePath.Contains("_call", ESearchCase::CaseSensitive);
}

void ACS_GameMode::CheckBlinkingDialogItem(TArray<FDialogButtonParameters>& DialogItems)
{
	for (FDialogButtonParameters& Param : DialogItems)
	{
		Param.bCalling = CheckBlinkingActor(Param.IconPath);
	}
}

void ACS_GameMode::FindAndUseObjectById(const FString& IdObject, const FString& DescriptionObject)
{
	if (IsGameStateImplementedCSInterface()) 
		ICS_GameState_Interface::Execute_ActivateObjectInActors(GameState.Get(), IdObject, DescriptionObject);
}

void ACS_GameMode::FindAndUseViewById(const FObjectViewID& StructObjectViewID)
{	
	if (IsGameStateImplementedCSInterface())
		ICS_GameState_Interface::Execute_ActivateViewInActors(GameState.Get(), StructObjectViewID);
}

void ACS_GameMode::FindAndUseObjectByReactMethod(const FString& IdObject, const FVariables& Variables)
{
	if (IsGameStateImplementedCSInterface())
		ICS_GameState_Interface::Execute_ActivateReactMethodInActors(GameState.Get(), IdObject, Variables);
}

void ACS_GameMode::TryBlinkingObject(const FString& IdObject, const FString& IconPath)
{
	if (IsGameStateImplementedCSInterface())
		ICS_GameState_Interface::Execute_BlinkingActor(GameState.Get(), IdObject, CheckBlinkingActor(IconPath));
}

void ACS_GameMode::DisableInteractActors()
{
	if (IsGameStateImplementedCSInterface())
		ICS_GameState_Interface::Execute_DisableActivatedActors(GameState.Get());
}