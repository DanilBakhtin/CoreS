
#include "Gameplay/Player/CS_HUD.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "AudioDevice.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Framework/CS_DeveloperSettings.h"
#include "Framework/CS_SimulatorParamsAsset.h"
#include "Gameplay/Player/CS_Character.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Gameplay/UI/CS_ButtonWidget.h"
#include "Gameplay/UI/CS_CheckWidget.h"
#include "Gameplay/UI/CS_ContextMenu.h"
#include "Gameplay/UI/CS_Description_Mouse.h"
#include "Gameplay/UI/CS_Description_Target.h"
#include "Gameplay/UI/CS_HUDWidget.h"
#include "Gameplay/UI/CS_MainMenu.h"
#include "Gameplay/UI/CS_MeasureDevicesList.h"
#include "Gameplay/UI/CS_MoveZone.h"
#include "Gameplay/UI/CS_ShowInfo.h"
#include "Gameplay/UI/CS_ShowMessage.h"
#include "Gameplay/UI/CS_ShowQuest.h"
#include "Gameplay/UI/CS_ShowResult.h"
#include "Gameplay/UI/CS_ShowScheme.h"
#include "Gameplay/UI/CS_WaitStart.h"
#include "Gameplay/UI/CS_ShowWelcome.h"
#include "Gameplay/UI/CS_TimeAndLocation.h"
#include "Gameplay/UI/CS_ActionLog.h"
#include "Gameplay/UI/CS_FadeWidget.h"
#include "Gameplay/UI/CS_HelpCellMode.h"
#include "Gameplay/UI/CS_ObjectMenu.h"
#include "Gameplay/UI/CS_PanelStickers.h"
#include "Gameplay/UI/CS_EndWorkingAreaWidget.h"
#include "Gameplay/UI/CS_WindowContainerWidget.h"
#include "Gameplay/UI/CS_DialogContainerWidget.h"
#include "Gameplay/Utilities/CS_ImageManager.h"
#include "Gameplay/Utilities/CS_Tag.h"

void ACS_HUD::BeginPlay()
{
	PlayerController = Cast<ACS_PlayerController>(GetOwningPlayerController());

	WindowContainerWidget = CreateWidget<UCS_WindowContainerWidget>(GetWorld(), WindowContainerClass);
	WindowContainerWidget->AddToViewport(WindowZOrder);
	WindowContainerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	HUDWidget = CreateAndInitWidget<UCS_HUDWidget>(HUDWidgetClass, [this](UCS_HUDWidget* Widget)
		{
			Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Widget->GetUITagContainer([&]() { return GetUITagContainer(); });
			Widget->AddAddTag([&](TArray<FGameplayTag> UsedTags) {UITagsAddTag(UsedTags); });
			Widget->AddRemoveTag([&](TArray<FGameplayTag> UsedTags) {UITagsRemoveTag(UsedTags); });
			Widget->UpdateCondition();

			Widget->OnCheckedChancgeDynamicButton.AddDynamic(this, &ACS_HUD::OnCheckedDynamicButton);

			check(Widget->TabButton)
				Widget->TabButton->Button->OnClicked.AddDynamic(this, &ACS_HUD::BackButton);
			check(Widget->Quest_HudCheck)
				Widget->Quest_HudCheck->CheckBox->OnCheckStateChanged.AddDynamic(this, &ACS_HUD::QuestCheckStateChanged);
			check(Widget->Info_HudCheck)
				Widget->Info_HudCheck->CheckBox->OnCheckStateChanged.AddDynamic(this, &ACS_HUD::InfoCheckStateChanged);
			check(Widget->Log_HudCheck)
				Widget->Log_HudCheck->CheckBox->OnCheckStateChanged.AddDynamic(this, &ACS_HUD::LogCheckStateChanged);
			check(Widget->Helper_HudCheck)
				Widget->Helper_HudCheck->CheckBox->OnCheckStateChanged.AddDynamic(this, &ACS_HUD::HelperCheckStateChanged);
			check(Widget->Menu_HudButton)
				Widget->Menu_HudButton->Button->OnClicked.AddDynamic(this, &ACS_HUD::MenuCheckStateChanged);
		});

	MainMenuWidget = CreateAndInitWidget<UCS_MainMenu>(MainMenuClass, [this](UCS_MainMenu* Widget)
		{
			Widget->SetReasonActionEventFun([&](const FString& AnswerId)
			{
				PlayerController->ReasonsActionEvent(AnswerId);
			});

			Widget->SetExitCourseFunc([&]() {FinishCourse(); });
			Widget->SetCloseMainMenuFunc([&]() {ShowMainMenu(); });
			Widget->SetLockFun([&]() {UITags.AddTag(TAG_Gameplay_HUD_MainMenu_Locked); });
			Widget->SetIsLockedMenuFun([&]() { return IsLockedMenu(); });
			Widget->OnShowImageViewer.AddDynamic(this, &ACS_HUD::ActionToShowImageViewer);

			if (UCS_SimulatorParamsAsset* SimulatorParams = GetDefault<UCS_DeveloperSettings>()->SimulatorParamsAsset.LoadSynchronous())
			{
				Widget->InitHelpImage(SimulatorParams->Params.HelpTexture);
			}
		});

	SchemeWidget = CreateAndInitWidget<UCS_ShowScheme>(ShowSchemeClass, [this](UCS_ShowScheme* Widget)
		{
			Widget->SetClosSchemeFunc([&]() {CloseScheme(); });
		});

	MeasureDevicesListWidget = CreateAndInitWidget<UCS_MeasureDevicesList>(MeasureDevicesListClass, [this](UCS_MeasureDevicesList* Widget)
		{
			Widget->SetStartUseMeasureDevice([&](const FString& MeasureId, const TArray<FDeviceData>& PointsData)
			{
				PlayerController->StartUseMeasureDevice(MeasureId, PointsData);
				this->TryUseWindowSystem(TAG_Gameplay_HUD_ToolsWindow_Used, false);
				this->UITags.AddTag(TAG_Gameplay_HUD_Tools_Used);
				if (this->HUDWidget)
				{
					this->HUDWidget->UpdateCondition();
				}
			});
			
			Widget->SetCloseMeasureDevicesFunc([&]() { PlayerController->HotKeyObjectClick(TAG_Gameplay_HUD_Tools_Activated); });
		});

	QuestWidget = CreateAndInitWidget<UCS_ShowQuest>(ShowQuestClass, [this](UCS_ShowQuest* Widget)
		{
			if (UCS_SimulatorParamsAsset* SimulatorParams = GetDefault<UCS_DeveloperSettings>()->SimulatorParamsAsset.LoadSynchronous())
			{
				Widget->InitHelpImages(SimulatorParams->Params.HelpTexture);
				Widget->SetCloseQuestFunc([&]() {ShowQuest(); });
			}
		});

	InfoWidget = CreateAndInitWidget<UCS_ShowInfo>(ShowInfoClass, [this](UCS_ShowInfo* Widget) {});

	WaitStartWidget = CreateAndInitWidget<UCS_WaitStart>(WaitStartClass, [this](UCS_WaitStart* Widget)
		{
			Widget->SetExitFun([&]() {ExitCourse(); });
			GetOwningPlayerController()->bShowMouseCursor = true;
			GetOwningPlayerController()->SetInputMode(FInputModeGameAndUI());
			if (PlayerController.Get())
			{
				PlayerController->SetMouseToCenter();
			}
			UITags.AddTag(TAG_Gameplay_HUD_WaitStart_Activated);
			UITags.AddTag(TAG_Gameplay_HUD_WaitStart_Used);

			Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		});

	ShowWelcomeWidget = CreateAndInitWidget<UCS_ShowWelcome>(ShowWelcomeClass, [this](UCS_ShowWelcome* Widget)
		{
			Widget->SetStartCourseFunc([&]() {StartCourse(); });
			Widget->SetExitCourseFunc([&]() {ExitCourse(); });

			if (const UCS_SimulatorParamsAsset* SimulatorParams = GetDefault<UCS_DeveloperSettings>()->SimulatorParamsAsset.LoadSynchronous())
				Widget->InitImages(SimulatorParams->Params.HelpTexture);

			UITags.AddTag(TAG_Gameplay_HUD_ShowWelcome_Activated);
		});

	TimeAndLocationWidget = CreateAndInitWidget<UCS_TimeAndLocation>(TimeAndLocationClass, [this](UCS_TimeAndLocation* Widget)
		{
			Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		});

	ShowMessageWidget = CreateAndInitWidget<UCS_ShowMessage>(ShowMessageClass, [this](UCS_ShowMessage* Widget)
		{
			Widget->SetCloseShowMessageFunc([&]() {CloseShowMessage(); });
		});

	ShowResultWidget = CreateAndInitWidget<UCS_ShowResult>(ShowResultClass, [this](UCS_ShowResult* Widget)
		{
			UITags.AddTag(TAG_Gameplay_HUD_ShowResult_Activated);
			Widget->SetExitCourseFunc([&]() {ExitCourse(); });
		});

	DescriptionTargetWidget = CreateAndInitWidget<UCS_Description_Target>(DescriptionTargetClass, [this](UCS_Description_Target* Widget)
		{
			Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
			UITags.AddTag(TAG_Gameplay_HUD_DescriptionTarget_Used);
		});

	DescriptionMouseWidget = CreateAndInitWidget<UCS_Description_Mouse>(DescriptionMouseClass, [this](UCS_Description_Mouse* Widget)
		{
			Widget->SetPlayerController(PlayerController.Get());
		});

	ContextMenuWidget = CreateAndInitWidget<UCS_ContextMenu>(ContextMenuClass, [this](UCS_ContextMenu* Widget)
		{
			Widget->SetPlayerController(PlayerController.Get());
			Widget->SetCloseContextMenuFun([&]() { CloseContextMenu(); });
			Widget->SetClickOnObject([&](const FString& Id, const FString& Type)
				{
					PlayerController->OnClickObject(Id, Type);
				});
		});

	PanelStickersWidget = CreateAndInitWidget<UCS_PanelStickers>(PanelStickersClass, [this](UCS_PanelStickers* Widget)
		{
			Widget->SetCloseStickerFun([&]() {CloseSticker(); });
			Widget->SetCloseReactionFun([&]() {CloseShowReaction(); });

			Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		});

	MoveZoneWidget = CreateAndInitWidget<UCS_MoveZone>(MoveZoneClass, [this](UCS_MoveZone* Widget) {});

	ActionLogWidget = CreateAndInitWidget<UCS_ActionLog>(ActionLogClass, [this](UCS_ActionLog* Widget)
		{
			Widget->SetCloseLogFunc([&]() {ShowLogs(); });
			ActivateShowLog(true);
		});

	HelpCellModeWidget = CreateAndInitWidget<UCS_HelpCellMode>(HelpCellModeWidgetClass, [this](UCS_HelpCellMode* Widget) {});

	ObjectMenuWidget = CreateAndInitWidget<UCS_ObjectMenu>(ObjectMenuWidgetClass, [this](UCS_ObjectMenu* Widget)
		{
			Widget->SetCloseObjectMenuFun([&]() { CloseObjectMenu(); });
			Widget->SetReleasedOnObject([&](const FString& CastedToStringAttribute)
				{
					PlayerController->ReasonsActionEvent(CastedToStringAttribute);
				});

			Widget->SetPlayerController(PlayerController.Get());
		});

	EndZoneWidget = CreateAndInitWidget<UCS_EndWorkingAreaWidget>(EndZoneWidgetClass,
		[this](UCS_EndWorkingAreaWidget* Widget) {});
	LockedBoundaryWidget = CreateAndInitWidget<UCS_EndWorkingAreaWidget>(LockedBoundaryWidgetClass,
		[this](UCS_EndWorkingAreaWidget* Widget) {});

	DialogContainerWidget = CreateAndInitWidget<UCS_DialogContainerWidget>(DialogContainerWidgetClass,
		[this](UCS_DialogContainerWidget* Widget)
		{
			Widget->OnContactSelect.AddLambda([this](const FDialogButtonParameters& Params)
				{
					PlayerController->OnClickObject(Params.IdDialog, Params.IdDialog);
					LastDialogAction = FDialogActionQueue(Params.IdDialog, EDialogActionType::OnClick);
				});

			Widget->OnSendMessage.AddLambda([this](const FDialogParameters& Params)
				{
					PlayerController->OnDialogPoint(Params.IdDialog);

					if (CurrentTypeDialog == ETypeDialog::Journal)
						LastDialogAction = FDialogActionQueue(Params.IdDialog, EDialogActionType::OnDialogPoint, Params.Message);

					if (Params.OptType == "rct" && CurrentTypeDialog == ETypeDialog::Journal)
						BackDialog();
				});

			Widget->OnBackDialog.AddLambda([this]()
				{
					BackDialog();
				});

			Widget->OnDialogClose.AddDynamic(this, &ACS_HUD::DialogClose);
		});

	InitWidgetsForHotKeys();
	InitOverlapContextMenu();

	Super::BeginPlay();
}

void ACS_HUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < DialogActionList.Num(); i++)
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("LIST : %s"), *DialogActionList[i].Description));
	}

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("LIST : %s"), *CurrentDialogAction.Description));
}

#pragma region WindowsSystem

bool ACS_HUD::IsShowingFullscreenWidget()
{
	const FGameplayTag TopWidgetTag = WindowsArray.IsEmpty() ? FGameplayTag().EmptyTag : WindowsArray[WindowsArray.Num() - 1];
	return UIFullscreenTags.HasTag(TopWidgetTag);
}

bool ACS_HUD::IsShowingFullscreenWidgetByTag(FGameplayTag UsedTag)
{
	if (IsShowingFullscreenWidget())
	{
		const FGameplayTag TopWidgetTag = WindowsArray.IsEmpty() ? FGameplayTag().EmptyTag : WindowsArray[WindowsArray.Num() - 1];

		return TopWidgetTag == UsedTag;
	}
	else 
	{
		return false;
	}
}

void ACS_HUD::InitCustomWidgetInWindowsSystem(bool bFullScreen, UUserWidget* Widget, FGameplayTag UsedTag)
{
	if (!Widget) return;

	if (bFullScreen)
		UIFullscreenTags.AddTag(UsedTag);
	else
		UIWindowsTags.AddTag(UsedTag);

	if (UsedTag != FGameplayTag::EmptyTag)
	{
		Widget->SetVisibility(ESlateVisibility::Collapsed);

		TagWidgetMap.Add(UsedTag, Widget);
		WidgetTagMap.Add(Widget, UsedTag);

		if (IsValid(WindowContainerWidget))
			WindowContainerWidget->AddWidgetToPanel(Widget);
	}
}

bool ACS_HUD::TryShowWidget(bool UpdatedByCheckbox, UUserWidget* Widget, const FGameplayTag& UsedTag, bool Checked)
{
	if (!IsValid(Widget)) return false;

	const bool bShouldShow = UpdatedByCheckbox ? Checked : !UITags.HasTag(UsedTag);

	if (UIWindowsTags.HasTag(UsedTag) || UIFullscreenTags.HasTag(UsedTag))
	{
		TryUseWindowSystem(UsedTag, bShouldShow);
	}
	else
	{
		if (bShouldShow)
		{
			Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (!UpdatedByCheckbox) UITags.AddTag(UsedTag);
		}
		else
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			if (!UpdatedByCheckbox) UITags.RemoveTag(UsedTag);
		}
	}

	UpdateCondition();

	return bShouldShow;
}

bool ACS_HUD::TryShowBorderWidget(const bool bIsShow, UUserWidget* Widget, const FGameplayTag& UsedTag)
{
	if (!IsValid(Widget)) return false;

	if (bIsShow)
	{
		if (!UITags.HasTag(UsedTag))
		{
			Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			UITags.AddTag(UsedTag);
		}
	}
	else
	{
		if (UITags.HasTag(UsedTag))
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			UITags.RemoveTag(UsedTag);
		}
	}

	UpdateCondition();

	return bIsShow;
}

void ACS_HUD::TryActivateWidget(bool bEnable, UUserWidget* Widget, const FGameplayTag& UsedTag, const FGameplayTag& ActivateTag)
{
	if (bEnable)
	{
		UITags.AddTag(ActivateTag);
		UE_LOG(LogTemp, Log, TEXT("HUD: Add Activate tag: %s"), *ActivateTag.ToString());
	}
	else
	{
		UITags.RemoveTag(ActivateTag);
		UITags.RemoveTag(UsedTag);
		WindowsArray.Remove(UsedTag);
		if (IsValid(Widget)) Widget->SetVisibility(ESlateVisibility::Collapsed);

		UE_LOG(LogTemp, Log, TEXT("HUD: Remove activate and used tag: %s, %s"),
			*ActivateTag.ToString(), *UsedTag.ToString());
	}

	UpdateCondition();
}

void ACS_HUD::TryUseWindowSystem(const FGameplayTag& UsedTag, const bool bIsOpenWidget)
{
	int32 CurrentTagIndexInArray = -1;
	// Additional ZOrder for windows
	// Functional enum for window widget type. 0 - not window; 1 - window; 2 - fullscreen.
	uint32 EWindowWidgetType = 0;
	const FGameplayTag TopWidgetTag = WindowsArray.IsEmpty() ? FGameplayTag().EmptyTag : WindowsArray[WindowsArray.Num() - 1];
	const bool bHasFullscreenTopWidget = UIFullscreenTags.HasTag(TopWidgetTag);

	const TFunction<void(const int Index, const FGameplayTag& Tag)>
		CollapseWidget = [this](const int Index, const FGameplayTag& Tag)
		{
			WindowsArray.RemoveAt(Index);
			if (UITags.HasTag(Tag))
			{
				UITags.RemoveTag(Tag);
			}

			UE_LOG(LogTemp, Log, TEXT("%i"), !!TagWidgetMap.Find(Tag));

			if (UUserWidget* Widget = *TagWidgetMap.Find(Tag))
			{
				Widget->SetVisibility(ESlateVisibility::Collapsed);
			}

			ActionToShowImageViewer(false);
			UpdateCondition();
		};
	const TFunction<void(const int Index, const FGameplayTag& Tag)>
		ShowWidget = [this](const int Index, const FGameplayTag& Tag)
		{
			if (UUserWidget* Widget = *TagWidgetMap.Find(Tag))
			{
				if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
				{
					CanvasSlot->SetZOrder(Index);
				}
				Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}

			WindowsArray.Add(Tag);

			if (!UITags.HasTag(Tag))
			{
				UITags.AddTag(Tag);
			}

			ActionToShowImageViewer(false);
			UpdateCondition();
		};

	if (UIWindowsTags.HasTag(UsedTag))
	{
		EWindowWidgetType = 1;
	}
	if (UIFullscreenTags.HasTag(UsedTag))
	{
		EWindowWidgetType = 2;
	}

	if (EWindowWidgetType == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("HUD.WindowSystem: Trying to use window system for not window widget!"));
		return;
	}

	for (int i = 0; i < WindowsArray.Num(); i++)
	{
		if (WindowsArray[i].GetTagName() == UsedTag.GetTagName())
		{
			CurrentTagIndexInArray = i;
			break;
		}
	}

	if (CurrentTagIndexInArray >= 0)
	{
		if (bIsOpenWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("HUD.WindowSystem: Trying to open already opened widget!"));
			return;
		}

		if (bHasFullscreenTopWidget)
		{
			if (TopWidgetTag.GetTagName() == UsedTag.GetTagName())
			{
				CollapseWidget(CurrentTagIndexInArray, UsedTag);

				for (int i = 0; i < WindowsArray.Num(); i++)
				{
					UUserWidget* Widget = *TagWidgetMap.Find(WindowsArray[i]);

					Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}

				//A temporary solution for the zoom display. Show the zoom (Interactive Actor)
				PlayerController->ShowZoomToInteractObject();
				UE_LOG(LogTemp, Log, TEXT("HUD.WindowSystem: Close fullscreen widget."));
				return;
			}

			UE_LOG(LogTemp, Warning, TEXT("HUD.WindowSystem: Trying to close widget when fullscreen widget already on screen!"));
			return;
		}

		CollapseWidget(CurrentTagIndexInArray, UsedTag);
		for (int i = CurrentTagIndexInArray; i < WindowsArray.Num(); i++)
		{
			UUserWidget* Widget = *TagWidgetMap.Find(WindowsArray[i]);

			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
			{
				CanvasSlot->SetZOrder(i);
			}

			Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		UE_LOG(LogTemp, Log, TEXT("HUD.WindowSystem: Close window widget."));
		return;
	}

	if (!bIsOpenWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD.WindowSystem: Trying to close already closed widget!"));
		return;
	}

	if (bHasFullscreenTopWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD.WindowSystem: Trying to open fullscreen widget when fullscreen widget already on screen!"));
		return;
	}

	if (EWindowWidgetType == 1)
	{
		ShowWidget(WindowsArray.Num(), UsedTag);

		UE_LOG(LogTemp, Log, TEXT("HUD.WindowSystem: Open window widget."));
		return;
	}

	if (EWindowWidgetType == 2)
	{
		ShowWidget(WindowsArray.Num(), UsedTag);

		for (int i = 0; i < WindowsArray.Num() - 1; i++)
		{
			UUserWidget* Widget = *TagWidgetMap.Find(WindowsArray[i]);

			Widget->SetVisibility(ESlateVisibility::Collapsed);
		}

		//A temporary solution for the zoom display. Hide the zoom (Interactive Actor)
		ZoomChanged(100);
		UE_LOG(LogTemp, Log, TEXT("HUD.WindowSystem: Open window widget."));
		return;
	}
}

#pragma endregion

#pragma region Dialogs

void ACS_HUD::InitShowDialog(FString API_Id, FString nameTitle, const TArray<FDialogParameters>& Params, ETypeDialog TypeDialog)
{
	if (DialogContainerWidget)
	{
		CurrentTypeDialog = TypeDialog;
		DialogContainerWidget->ShowDialogWidget(TypeDialog, nameTitle, Params);

		if (UCS_ImageManager* ImageManager = GetGameInstance()->GetSubsystem<UCS_ImageManager>()) 
		{
			if (UTexture2D* Texture = ImageManager->GetImageByID(API_Id)) 
			{
				DialogContainerWidget->UpdateDialogIcon(Texture);
			}
		}

		DialogContainerWidget->ClearNestingList();

		TryUseWindowSystem(TAG_Gameplay_HUD_Dialog_Used, true);

		UpdateDialogActions();
	}
}

void ACS_HUD::ShowDialogContacts(FString API_Id, const TArray<FDialogButtonParameters>& PointsDialog, ETypeDialog TypeDialog)
{
	if (DialogContainerWidget)
	{
		CurrentTypeDialog = ETypeDialog::None;
		DialogContainerWidget->ActivateBackButton(false);
		DialogContainerWidget->ShowContactWidget(TypeDialog, PointsDialog);
		DialogContainerWidget->ClearNestingList();

		TryUseWindowSystem(TAG_Gameplay_HUD_Dialog_Used, true);

		CurrentDialogAction = FDialogActionQueue(API_Id, EDialogActionType::OnClick);
	}
}

void ACS_HUD::InitAddMessage(const TArray<FDialogParameters>& Params)
{
	if (DialogContainerWidget)
	{
		DialogContainerWidget->AddMessage(Params);
	}
}

void ACS_HUD::InitUpdateDialog(const FString& NewAPI_Id, const TArray<FDialogParameters>& Params)
{
	if (DialogContainerWidget)
	{
		TArray<FString> List;

		DialogContainerWidget->UpdateDialog(Params);
		UpdateDialogActions();

		for (int i = 0; i < DialogActionList.Num(); i++)
		{
			if (!DialogActionList[i].Description.IsEmpty())
				List.Add(DialogActionList[i].Description);
		}

		if (!CurrentDialogAction.Description.IsEmpty())
			List.Add(CurrentDialogAction.Description);

		DialogContainerWidget->UpdateNestingList(List);
	}
}

FString ACS_HUD::GetLastDialogID()
{
	if (DialogContainerWidget)
	{
		if (DialogContainerWidget->IsContactShowing())
		{
			return CurrentDialogAction.API_Id;
		}
	}

	return FString();
}

void ACS_HUD::TrySwitchDialogHotKey(FGameplayTag NewActivatedDialogTag)
{
	if (TagsDialog.HasTag(NewActivatedDialogTag))
	{
		if (CurrentDialogTag != FGameplayTag::EmptyTag && CurrentDialogTag != NewActivatedDialogTag)
		{
			OnCheckedDynamicButton(CurrentDialogTag, false);
		}
		//If dialog is phone
		else
		{
			if (DialogContainerWidget->NeedSendCloseDialog())
				PlayerController->SendDialogClose();
		}

		CurrentDialogTag = NewActivatedDialogTag;
	}
}

void ACS_HUD::RemoveAllSoundsInQueue()
{
	if (PlayerController.IsValid())
		PlayerController->RemoveSoundsInQueue();
}

ETypeDialog ACS_HUD::GetCurrentTypeDialog()
{
	return CurrentTypeDialog;
}

void ACS_HUD::UpdateDialogActions()
{
	if (CurrentDialogAction.Action != EDialogActionType::None && LastDialogAction.Action != EDialogActionType::None)
	{
		DialogActionList.Add(CurrentDialogAction);
		CurrentDialogAction = LastDialogAction;
		LastDialogAction = FDialogActionQueue("", EDialogActionType::None);
	}

	DialogContainerWidget->ActivateBackButton(DialogActionList.Num() > 0);
}

void ACS_HUD::BackDialog()
{
	RemoveAllSoundsInQueue();

	if (DialogActionList.Num() > 0)
	{
		if (DialogActionList.Num() == 1)
		{
			PlayerController->SendDialogClose();
			CurrentTypeDialog = ETypeDialog::None;
		}

		FString ApiID = DialogActionList[DialogActionList.Num() - 1].API_Id;
		EDialogActionType Action = DialogActionList[DialogActionList.Num() - 1].Action;

		if (!ApiID.IsEmpty() && Action != EDialogActionType::None)
		{
			switch (Action)
			{
			case EDialogActionType::OnClick:
				PlayerController->OnClickObject(ApiID, ApiID);
				break;
			case EDialogActionType::OnDialogPoint:
				PlayerController->OnDialogPoint(ApiID);
				break;
			default:
				break;
			}
		}

		LastDialogAction = FDialogActionQueue("", EDialogActionType::None);
		CurrentDialogAction = DialogActionList[DialogActionList.Num() - 1];
		DialogActionList.RemoveAt(DialogActionList.Num() - 1);
	}

	DialogContainerWidget->ActivateBackButton(DialogActionList.Num() > 0);
}

void ACS_HUD::DialogClose()
{
	CurrentTypeDialog = ETypeDialog::None;

	TryUseWindowSystem(TAG_Gameplay_HUD_Dialog_Used, false);

	RemoveAllSoundsInQueue();

	DialogContainerWidget->ClearNestingList();
	DialogContainerWidget->ActivateBackButton(false);

	if (DialogContainerWidget->NeedSendCloseDialog())
		PlayerController->SendDialogClose();

	if (CurrentDialogTag != FGameplayTag::EmptyTag)
		OnCheckedDynamicButton(CurrentDialogTag, false);

	CurrentDialogTag = FGameplayTag::EmptyTag;

	DialogActionList.Empty();
	CurrentDialogAction = FDialogActionQueue("", EDialogActionType::None);
	LastDialogAction = FDialogActionQueue("", EDialogActionType::None);
}

void ACS_HUD::HotKeyDialogClose()
{
	if (!UITags.HasTag(TAG_Gameplay_HUD_Dialog_Used)) return;

	CurrentTypeDialog = ETypeDialog::None;

	TryUseWindowSystem(TAG_Gameplay_HUD_Dialog_Used, false);

	RemoveAllSoundsInQueue();

	DialogContainerWidget->ClearNestingList();
	DialogContainerWidget->ActivateBackButton(false);

	if (DialogContainerWidget->NeedSendCloseDialog())
		PlayerController->SendDialogClose();

	CurrentDialogTag = FGameplayTag::EmptyTag;

	DialogActionList.Empty();
	CurrentDialogAction = FDialogActionQueue("", EDialogActionType::None);
	LastDialogAction = FDialogActionQueue("", EDialogActionType::None);
}

#pragma endregion

#pragma region HotKeys

void ACS_HUD::EnableBlinkingToHotKeysByTag(const FGameplayTag ActivatedTag, const bool bEnable)
{
	if (IsValid(HUDWidget))
		HUDWidget->EnableBlinkingToHotKeysByTag(ActivatedTag, bEnable);
}

void ACS_HUD::ActivateHotKey(bool bEnable, FGameplayTag ActivatedTag, TArray<FGameplayTag> UsedTags)
{
	if (TagWidgetHotKeysMap.Contains(ActivatedTag))
	{
		UUserWidget* Widget = TagWidgetHotKeysMap[ActivatedTag];
		bool bNewEnable = bEnable;

		for (FGameplayTag& UsedTag : UsedTags)
		{
			TryActivateWidget(bNewEnable, Widget, UsedTag, ActivatedTag);
		}
	}
}

void ACS_HUD::TryCloseHotKeyWidget(FGameplayTag ActivatedTag)
{
	if (TagWidgetHotKeysMap.Contains(ActivatedTag))
	{
		UUserWidget* Widget = TagWidgetHotKeysMap[ActivatedTag];

		if (Widget && WidgetTagMap.Contains(Widget))
		{
			TryUseWindowSystem(WidgetTagMap[Widget], false);
		}
	}

	if (TagsDialog.HasTag(ActivatedTag))
	{
		HotKeyDialogClose();
	}

	if (ActivatedTag == TAG_Gameplay_HUD_Tools_Activated)
	{
		PlayerController->StopUseMeasureDevice();
		UITags.RemoveTag(TAG_Gameplay_HUD_Tools_Used);
		UITags.RemoveTag(TAG_Gameplay_HUD_ToolsWindow_Used);
	}
}

void ACS_HUD::InitWidgetsForHotKeys()
{
	TagWidgetHotKeysMap.Add(TAG_Gameplay_HUD_Scheme_Activated, SchemeWidget);
	TagWidgetHotKeysMap.Add(TAG_Gameplay_HUD_Tools_Activated, MeasureDevicesListWidget);

	TagWidgetHotKeysMap.Add(TAG_Gameplay_HUD_ActionLog_Activated, ActionLogWidget);
}

void ACS_HUD::AddWidgetsForHotKeys(const FGameplayTag ActivatedTag, UUserWidget* Widget)
{
	if (Widget && ActivatedTag != FGameplayTag::EmptyTag)
		TagWidgetHotKeysMap.Add(ActivatedTag, Widget);
}

void ACS_HUD::OnCheckedDynamicButton(FGameplayTag ActiveTag, bool bIsChecked)
{
	if (PlayerController.IsValid())
		PlayerController->HotKeyObjectClick(ActiveTag);
}

void ACS_HUD::ChangeActivatedTag(FGameplayTag ActivatedTag, bool bAdded)
{
	if (bAdded && !UITags.HasTag(ActivatedTag))
	{
		UITags.AddTag(ActivatedTag);
	}
	else if (!bAdded && UITags.HasTag(ActivatedTag))
	{
		UITags.RemoveTag(ActivatedTag);
	}

	UpdateCondition();
}

void ACS_HUD::ChangeUsingTags(TArray<FGameplayTag> UsingTags, bool bAdded)
{
	for (int i = 0; i < UsingTags.Num(); i++)
	{
		if (bAdded && !UITags.HasTag(UsingTags[i]))
		{
			UITags.AddTag(UsingTags[i]);
		}
		else if (!bAdded && UITags.HasTag(UsingTags[i]))
		{
			UITags.RemoveTag(UsingTags[i]);
		}
	}

	UpdateCondition();
}

#pragma endregion

#pragma region WidgetInteractionWithEditorStep

void ACS_HUD::UpdateSchemes(const TArray<FLoadingImageInfo>& SchemesParams)
{
	if (SchemeWidget && SchemesParams.Num() > 0) {
		SchemeWidget->InitSchemeImageFromInfo(SchemesParams);
		bSchemeInit = true;
		TryUseWindowSystem(TAG_Gameplay_HUD_Scheme_Used, true);
		ActionToShowImageViewer(true);
	}
	else {
		bSchemeInit = false;
	}
}

void ACS_HUD::CloseScheme()
{
	PlayerController->HotKeyObjectClick(TAG_Gameplay_HUD_Scheme_Activated);

	ActionToShowImageViewer(false);
}

void ACS_HUD::InitMeasureDevicesList(const FDeviceAndPointsData& DeviceAndPointsData) const
{
	MeasureDevicesListWidget->InitMeasureDevicesList(DeviceAndPointsData);
}

void ACS_HUD::ShowTool(const FDeviceAndPointsData& DevicesData)
{
	UITags.RemoveTag(TAG_Gameplay_HUD_Tools_Used);
	UITags.RemoveTag(TAG_Gameplay_HUD_ToolsWindow_Used);

	if (DevicesData.DevicesData.Num() > 1)
	{
		MeasureDevicesListWidget->OpenMeasureDevicesList(DevicesData);
		TryUseWindowSystem(TAG_Gameplay_HUD_ToolsWindow_Used, true);
	}
	else
	{
		PlayerController->StartUseMeasureDevice(DevicesData.DevicesData[0].DeviceId, DevicesData.PointsData);
		UITags.AddTag(TAG_Gameplay_HUD_Tools_Used);
	}
}

void ACS_HUD::ShowObjectMenu(FFullMenuReasons FullMenuReasons)
{
	if (!UITags.HasTag(TAG_Gameplay_HUD_ObjectMenu_Used))
	{
		UITags.AddTag(TAG_Gameplay_HUD_ObjectMenu_Used);
		ObjectMenuWidget->SetVisibility(ESlateVisibility::Visible);
		ObjectMenuWidget->InitObjectMenuElements(FullMenuReasons);

		UpdateCondition();

		UE_LOG(LogTemp, Log, TEXT("HUD: Show object menu."));

		return;
	}
	ObjectMenuWidget->InitObjectMenuElements(FullMenuReasons);

	UE_LOG(LogTemp, Log, TEXT("HUD: Object menu is already showing. Reinitialize menu elemets."));
}

void ACS_HUD::CloseObjectMenu()
{
	if (UITags.HasTag(TAG_Gameplay_HUD_ObjectMenu_Used))
	{
		UITags.RemoveTag(TAG_Gameplay_HUD_ObjectMenu_Used);
		ObjectMenuWidget->SetVisibility(ESlateVisibility::Collapsed);

		UpdateCondition();

		UE_LOG(LogTemp, Log, TEXT("HUD: Close object menu."));

		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("HUD: Trying to close object menu without active tag!"));
}

void ACS_HUD::ShowResult(const FShowResultParams& ShowResultParams)
{
	if (!UITags.HasTag(TAG_Gameplay_HUD_ShowResult_Activated))
		return;

	CollapseAllWidgets();

	ShowResultWidget->Init(ShowResultParams);
	ShowResultWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UITags.AddTag(TAG_Gameplay_HUD_ShowResult_Used);

	StopAllSoundInGame(true);
	UpdateCondition();
}

void ACS_HUD::ShowMessage(const FShowMessageParams& ShowMessageParams)
{
	ShowMessageWidget->InitParams(ShowMessageParams);

	if (!WindowsArray.IsEmpty())
	{
		if (UIFullscreenTags.HasTag(WindowsArray.Last()))
		{
			const FGameplayTag Tag = WindowsArray.Last();
			LastWindowTagBeforeShowMessage = WindowsArray.Last();
			TryUseWindowSystem(Tag, false);
		}
	}

	TryUseWindowSystem(TAG_Gameplay_HUD_ShowMessage_Used, true);

	if (UITags.HasTag(TAG_Gameplay_HUD_MainMenu_Locked))
	{
		UITags.RemoveTag(TAG_Gameplay_HUD_MainMenu_Locked);
		MainMenuWidget->HideLoadingWidget(true);
		//MainMenuWidget->CloseMainMenu();
	}
	else
		UpdateCondition();

}

void ACS_HUD::CloseShowMessage()
{
	if (UITags.HasTag(TAG_Gameplay_HUD_ShowMessage_Used))
	{
		TryUseWindowSystem(TAG_Gameplay_HUD_ShowMessage_Used, false);

		if (LastWindowTagBeforeShowMessage != FGameplayTag::EmptyTag)
		{
			const FGameplayTag Tag = LastWindowTagBeforeShowMessage;
			LastWindowTagBeforeShowMessage = FGameplayTag::EmptyTag;
			TryUseWindowSystem(Tag, true);
		}

		PlayerController->NavActionCustom("next");
		UpdateCondition();
	}
}

void ACS_HUD::ShowReaction(const FString& Description)
{
	if (!UITags.HasTag(TAG_Gameplay_HUD_ShowReaction_Used))
	{
		UITags.AddTag(TAG_Gameplay_HUD_ShowReaction_Used);
	}

	if (PanelStickersWidget)
		PanelStickersWidget->AddShowReaction(Description);
}

void ACS_HUD::CloseShowReaction()
{
	if (UITags.HasTag(TAG_Gameplay_HUD_ShowReaction_Used))
	{
		UITags.RemoveTag(TAG_Gameplay_HUD_ShowReaction_Used);
	}
}

void ACS_HUD::ShowSticker(const FStickerParams& StickerParams)
{
	if (!UITags.HasTag(TAG_Gameplay_HUD_LearningSticker_Used))
	{
		UITags.AddTag(TAG_Gameplay_HUD_LearningSticker_Used);
	}

	if (UITags.HasTag(TAG_Gameplay_HUD_ActionLog_Used)) {
		if (ActionLogWidget)
			ActionLogWidget->ShowLearningSticker(StickerParams);
	}
	else {
		if (PanelStickersWidget)
			PanelStickersWidget->ShowLearningSticker(StickerParams);
	}
}

void ACS_HUD::CloseSticker()
{
	if (UITags.HasTag(TAG_Gameplay_HUD_LearningSticker_Used))
	{
		UITags.RemoveTag(TAG_Gameplay_HUD_LearningSticker_Used);
	}
}

void ACS_HUD::ShowWelcome(const FShowWelcomeParams& ShowWelcomeParams)
{
	if (UITags.HasTag(TAG_Gameplay_HUD_WaitStart_Activated) && UITags.HasTag(TAG_Gameplay_HUD_WaitStart_Used))
	{
		WaitStartWidget->SetVisibility(ESlateVisibility::Collapsed);
		UITags.RemoveTag(TAG_Gameplay_HUD_WaitStart_Used);
		if (UITags.HasTag(TAG_Gameplay_HUD_ShowWelcome_Activated) && !UITags.HasTag(TAG_Gameplay_HUD_ShowWelcome_Used))
		{
			ShowWelcomeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			ShowWelcomeWidget->InitParams(ShowWelcomeParams);
			UITags.AddTag(TAG_Gameplay_HUD_ShowWelcome_Used);
			//Т.к. информация о прохождении курса должна быть еще и в окне инофомации о задании, мы эти данные передаем в окно инофомации о задании
			InfoWidget->Init(ShowWelcomeParams);
			MainMenuWidget->InitQuestInfo(ShowWelcomeParams);
			//Передаем данные для инициализации таймера
			TimeAndLocationWidget->SetTimerParameters(ShowWelcomeParams);
			//Передаем данные для иницализации количества шагов
			TimeAndLocationWidget->SetStepsParameters(ShowWelcomeParams);
		}
	}
}

void ACS_HUD::UpdateSteps(const int CountSteps)
{
	if (IsValid(TimeAndLocationWidget))
		TimeAndLocationWidget->SetSteps(CountSteps);
}

void ACS_HUD::SetTime(const FDateTime& DateTo) const
{
	TimeAndLocationWidget->SetTime(DateTo);
}

void ACS_HUD::SetLocationName(const FString& LocationName) const
{
	TimeAndLocationWidget->SetLocationName(LocationName);
}

#pragma endregion

#pragma region WidgetsRelated_To_The_F_keys

void ACS_HUD::ShowQuest(bool UpdatedByCheckbox, bool Checked)
{
	if (PlayerController->bCourseStarted && HUDWidget->IsVisible()
		&& !MainMenuWidget->IsVisible() && !ShowMessageWidget->IsVisible())
	{
		TryShowWidget(UpdatedByCheckbox, QuestWidget, TAG_Gameplay_HUD_Quest_Used, Checked);
		ActionToShowImageViewer(QuestWidget->GetVisibility() != ESlateVisibility::Collapsed);;
	}
}

void ACS_HUD::QuestCheckStateChanged(bool bIsChecked)
{
	ShowQuest(true, bIsChecked);
}

void ACS_HUD::HelperCheckStateChanged(bool bIsChecked)
{
	CallHelper(true, bIsChecked);
}

bool ACS_HUD::CallHelper(bool UpdatedByCheckbox, bool bIsChecked)
{
	// Not really functionality. Just switching button.
	bool bIsActivate = false;
	
	if (PlayerController->bCourseStarted && HUDWidget->IsVisible()
		&& !MainMenuWidget->IsVisible())
	{
		if (UpdatedByCheckbox ? bIsChecked : !UITags.HasTag(TAG_Gameplay_HUD_Helper_Used))
		{
			UITags.AddTag(TAG_Gameplay_HUD_Helper_Used);
			bIsActivate = true;
		}
		else
		{
			UITags.RemoveTag(TAG_Gameplay_HUD_Helper_Used);
		}

		UpdateCondition();
	}

	return bIsActivate;
}

void ACS_HUD::ShowInfo(bool UpdatedByCheckbox, bool Checked)
{
	if (PlayerController->bCourseStarted && HUDWidget->IsVisible()
		&& !MainMenuWidget->IsVisible() && !ShowMessageWidget->IsVisible())
		TryShowWidget(UpdatedByCheckbox, InfoWidget, TAG_Gameplay_HUD_Info_Used, Checked);
}

void ACS_HUD::InfoCheckStateChanged(bool bIsChecked)
{
	ShowInfo(true, bIsChecked);
}

void ACS_HUD::ActivateShowLog(bool Enable)
{
	TryActivateWidget(Enable, ActionLogWidget, TAG_Gameplay_HUD_ActionLog_Used, TAG_Gameplay_HUD_ActionLog_Activated);
}

void ACS_HUD::ShowLogs(bool UpdatedByCheckbox, bool Checked)
{
	if (PlayerController->bCourseStarted && HUDWidget->IsVisible()
		&& !MainMenuWidget->IsVisible() && !ShowMessageWidget->IsVisible()
		&& UITags.HasTag(TAG_Gameplay_HUD_ActionLog_Activated))
	{
		TryShowWidget(UpdatedByCheckbox, ActionLogWidget, TAG_Gameplay_HUD_ActionLog_Used, Checked);

		if (ActionLogWidget) {
			ActionLogWidget->Reset();
		}

	}
}

void ACS_HUD::LogCheckStateChanged(bool bIsChecked)
{
	ShowLogs(true, bIsChecked);
}

void ACS_HUD::AddMessageLog(const FLogParams& LogParams)
{
	ActionLogWidget->AddMessageLog(LogParams);
}

#pragma endregion

#pragma region BackButton

void ACS_HUD::EscButton()
{
	if (PlayerController.IsValid())
	{
		if (PlayerController->bCourseStarted)
		{
			if (!WindowsArray.IsEmpty())
			{
				if (UIFullscreenTags.HasTag(WindowsArray.Last()))
				{
					const FGameplayTag Tag = WindowsArray.Last();
					TryUseWindowSystem(Tag, false);
					UpdateCondition();
					return;
				}
			}
			ShowMainMenu();
		}
	}
}

void ACS_HUD::BackButton()
{
	PlayerController->BackButton();

	RemoveAllSoundsInQueue();
}

void ACS_HUD::BackButtonLogic()
{
	if (!HUDWidget->IsVisible())
		return;

	if (UITags.HasTag(TAG_Gameplay_HUD_Tools_Used))
	{
		UITags.RemoveTag(TAG_Gameplay_HUD_Tools_Used);
		PlayerController->StopUseMeasureDevice();
	}

	if (UITags.HasTag(TAG_Gameplay_HUD_AWP_Used))
	{
		HideAWP();
	}

	CloseContextMenu();

	RemoveAllSoundsInQueue();

	UpdateCondition();
}

bool ACS_HUD::GetBackButtonActive()
{
	return UITags.HasTag(TAG_Gameplay_HUD_Tab_Activated);
}

void ACS_HUD::ActivateBackButton(bool bActivate)
{
	if (bActivate)
	{
		if (!UITags.HasTag(TAG_Gameplay_HUD_Tab_Activated))
			UITags.AddTag(TAG_Gameplay_HUD_Tab_Activated);
	}
	else
	{
		if (UITags.HasTag(TAG_Gameplay_HUD_Tab_Activated))
			UITags.RemoveTag(TAG_Gameplay_HUD_Tab_Activated);
	}

	UpdateCondition();
}

#pragma endregion

#pragma region TagsLogic

FGameplayTagContainer ACS_HUD::GetUITagContainer() const
{
	return UITags;
}

void ACS_HUD::UITagsAddTag(TArray<FGameplayTag> UsedTags)
{
	for (const FGameplayTag Tag : UsedTags)
	{
		UITags.AddTag(Tag);
	}
}

void ACS_HUD::UITagsRemoveTag(TArray<FGameplayTag> UsedTags)
{
	for (const FGameplayTag Tag : UsedTags)
	{
		if (UITags.HasTag(Tag))
		{
			UITags.RemoveTag(Tag);
		}
	}
}

void ACS_HUD::UpdateTagsWidget(FGameplayTagContainer& TagContainer) const
{
	TagContainer = UITags;
}

#pragma endregion

#pragma region Course

void ACS_HUD::StartCourse()
{
	if (UITags.HasTag(TAG_Gameplay_HUD_ShowWelcome_Used))
	{
		if (PlayerController.IsValid()) {
			PlayerController->NavStart();
			PlayerController->CallStartCourse();
		}

		UITags.RemoveTag(TAG_Gameplay_HUD_ShowWelcome_Used);
		ShowWelcomeWidget->SetVisibility(ESlateVisibility::Collapsed);
		UpdateCondition();
		SpawnFadeWidget();
	}

	if (UITags.HasTag(TAG_Gameplay_HUD_MainMenu_Locked))
	{
		UITags.RemoveTag(TAG_Gameplay_HUD_MainMenu_Locked);
	}
}

void ACS_HUD::FinishCourse() const
{
	if (PlayerController.IsValid())
		PlayerController->NavActionFinish();
}

void ACS_HUD::ExitCourse() const
{
	if (PlayerController.IsValid())
		PlayerController->NavActionExit();
}

#pragma endregion

#pragma region MainMenu

void ACS_HUD::InitMainMenu(const FMainMenuParams& MainMenuParams) const
{
	MainMenuWidget->InitMainMenu(MainMenuParams);
}

void ACS_HUD::ShowMainMenu()
{
	if (UITags.HasTag(TAG_Gameplay_HUD_MainMenu_Locked))
		return;
	if (UITags.HasTag(TAG_Gameplay_HUD_MainMenu_Used))
	{
		TryUseWindowSystem(TAG_Gameplay_HUD_MainMenu_Used, false);

		if (UITags.HasTag(TAG_Gameplay_HUD_AWP_Used))
		{
			if (IsValid(TimeAndLocationWidget))
			{
				TimeAndLocationWidget->ShowVisibleDownTime(true);
				TimeAndLocationWidget->SetLocationVisibility(ESlateVisibility::Hidden);

			}
		}
	}
	else
	{
		TryUseWindowSystem(TAG_Gameplay_HUD_MainMenu_Used, true);
		MainMenuWidget->ResetMenu();

		if (UITags.HasTag(TAG_Gameplay_HUD_AWP_Used))
		{
			if (IsValid(TimeAndLocationWidget))
			{
				TimeAndLocationWidget->ShowVisibleDownTime(false);
				TimeAndLocationWidget->SetLocationVisibility(ESlateVisibility::Visible);
			}
		}
	}

	UpdateCondition();
}

bool ACS_HUD::IsLockedMenu() const
{
	return UITags.HasTag(TAG_Gameplay_HUD_MainMenu_Locked);
}

#pragma endregion

#pragma region Utilities

void ACS_HUD::CollapseAllWidgets()
{
	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllWidgets, UUserWidget::StaticClass());

	for (UUserWidget* Widget : AllWidgets)
	{
		Widget->SetVisibility(ESlateVisibility::Collapsed);
	}

	UITags.Reset();
}

#pragma endregion

#pragma region ContextMenu

void ACS_HUD::ShowContextMenu(FString ApiID, const TArray<FContextMenuItem>& ContextMenuItems)
{
	if (!UITags.HasTag(TAG_Gameplay_HUD_ContextMenu_Used))
	{
		if (!UITags.HasTag(TAG_Gameplay_HUD_DescriptionTarget_Locked))
			DescriptionTargetWidget->SetVisibility(ESlateVisibility::Collapsed);

		TryUseWindowSystem(TAG_Gameplay_HUD_ContextMenu_Used, true);
		ContextMenuWidget->InitContextMenuItems(ApiID, ContextMenuItems);
		UpdateCondition();
	}
}

void ACS_HUD::CloseContextMenu()
{
	ContextMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	if (!UITags.HasTag(TAG_Gameplay_HUD_DescriptionTarget_Locked))
		DescriptionTargetWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	TryUseWindowSystem(TAG_Gameplay_HUD_ContextMenu_Used, false);
	UpdateCondition();
}

void ACS_HUD::UpdateContextMenu()
{
	if (IsValid(ContextMenuWidget))
	{
		if (ContextMenuWidget->GetVisibility() != ESlateVisibility::Collapsed)
		{
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ContextMenuWidget->Slot))
			{
				CanvasSlot->SetZOrder(WindowsArray.Num() + 1);
			}

			ContextMenuWidget->FindWidgetsOverlapping();
		}
	}
}

void ACS_HUD::InitOverlapContextMenu()
{
	for (const FGameplayTag& Tag : UIOverlapingContextMenuTags)
	{
		if (TagWidgetMap.Contains(Tag))
			AddWidgetForOverlapContextMenu(TagWidgetMap[Tag]);
	}
}

void ACS_HUD::AddWidgetForOverlapContextMenu(UUserWidget* Widget)
{
	ContextMenuWidget->AddWidgetForCollision(Widget);
}

#pragma endregion

void ACS_HUD::ShowAWP(UCS_AWP_Widget* NewAWP_Widget)
{
	if (UITags.HasTag(TAG_Gameplay_HUD_AWP_Used))
	{
		return;
	}

	UITags.AddTag(TAG_Gameplay_HUD_AWP_Used);

	if (IsValid(TimeAndLocationWidget))
	{
		TimeAndLocationWidget->ShowVisibleDownTime(true);
		TimeAndLocationWidget->SetLocationVisibility(ESlateVisibility::Hidden);
	}

	UpdateCondition();

	if (IsValid(HUDWidget))
	{
		HUDWidget->AddAWP_Widget(NewAWP_Widget);
		HUDWidget->UpdateCondition();
	}

	if (PanelStickersWidget)
		PanelStickersWidget->SetIsArm(true);
}

void ACS_HUD::HideAWP()
{
	if (!UITags.HasTag(TAG_Gameplay_HUD_AWP_Used))
	{
		return;
	}

	UITags.RemoveTag(TAG_Gameplay_HUD_AWP_Used);

	if (IsValid(TimeAndLocationWidget))
	{
		TimeAndLocationWidget->ShowVisibleDownTime(false);
		TimeAndLocationWidget->SetLocationVisibility(ESlateVisibility::Visible);
	}

	UpdateCondition();

	if (IsValid(HUDWidget))
	{
		HUDWidget->ClearAWPContainer();
		HUDWidget->UpdateCondition();
	}

	if (PanelStickersWidget)
		PanelStickersWidget->SetIsArm(false);
}

void ACS_HUD::StartInteractWithObject(const bool& bUseUpDownMoveZone, const bool& bUseLeftRightMoveZone, TFunction<void(bool Lock)> LockCameraMoveFun)
{
	if (!UITags.HasTag(TAG_Gameplay_HUD_InteractableObject_Used))
	{
		if (!UITags.HasTag(TAG_Gameplay_HUD_DescriptionTarget_Locked))
		{
			UITags.AddTag(TAG_Gameplay_HUD_DescriptionTarget_Locked);
			DescriptionTargetWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (!UITags.HasTag(TAG_Gameplay_HUD_DescriptionMouse_Used))
		{
			UITags.AddTag(TAG_Gameplay_HUD_DescriptionMouse_Used);
			DescriptionMouseWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		UITags.AddTag(TAG_Gameplay_HUD_InteractableObject_Used);
		MoveZoneWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		MoveZoneWidget->SetShowArrows(bUseUpDownMoveZone, bUseLeftRightMoveZone);
		MoveZoneWidget->SetLockCameraMoveFun(LockCameraMoveFun);
		ShowTargetDescription(FString());
		UpdateCondition();
	}
}

void ACS_HUD::StopInteractWithObject()
{
	if (UITags.HasTag(TAG_Gameplay_HUD_InteractableObject_Used))
	{
		if (UITags.HasTag(TAG_Gameplay_HUD_DescriptionTarget_Locked))
		{
			UITags.RemoveTag(TAG_Gameplay_HUD_DescriptionTarget_Locked);
			DescriptionTargetWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		if (UITags.HasTag(TAG_Gameplay_HUD_DescriptionMouse_Used))
		{
			UITags.RemoveTag(TAG_Gameplay_HUD_DescriptionMouse_Used);
			DescriptionMouseWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		UITags.RemoveTag(TAG_Gameplay_HUD_InteractableObject_Used);
		MoveZoneWidget->SetShowArrows(false, false);
		MoveZoneWidget->SetLockCameraMoveFun(nullptr);
		MoveZoneWidget->SetVisibility(ESlateVisibility::Collapsed);

		UpdateCondition();
	}
}

void ACS_HUD::ShowTargetDescription(const FString& Description) const
{
	DescriptionTargetWidget->ShowTargetDescription(Description);
}

void ACS_HUD::ShowMouseDescription(const FString& ObjectId, const FString& Description) const
{
	DescriptionMouseWidget->ShowTargetDescription(ObjectId, Description);
}

void ACS_HUD::SetLockedBoundaryWidgetText(const FString& InText) const
{
	LockedBoundaryWidget->SetText(FText::FromString(InText));
}

void ACS_HUD::DisableInputForCharacter()
{
	if (!Character.IsValid())
		if (ACS_Character* Char = Cast<ACS_Character>(GetOwningPlayerController()->GetPawn()))
			Character = Char;

	if (UITags.HasAny(TagsForDisableInputForCharacter))
	{
		if (!PlayerController->bShowMouseCursor)
		{
			PlayerController->SetMouseToCenter();
		}
		if (Character.IsValid())
		{
			Character->DisableInput(GetOwningPlayerController());
		}
		PlayerController->bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		if (HUDWidget)
			InputMode.SetWidgetToFocus(HUDWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		InputMode.SetHideCursorDuringCapture(false);
		PlayerController->SetInputMode(InputMode);
	}
	else
	{
		Character->EnableInput(GetOwningPlayerController());
		GetOwningPlayerController()->bShowMouseCursor = false;
		FInputModeGameOnly InputMode;
		InputMode.SetConsumeCaptureMouseDown(false);
		GetOwningPlayerController()->SetInputMode(InputMode);
	}

	DescriptionTargetWidget->SetVisibility(UITags.HasAny(TagsForDisableCenterPoint) ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);

	K2_DisableInputCharacter();
}

void ACS_HUD::MenuCheckStateChanged()
{
	ShowMainMenu();
}

void ACS_HUD::ZoomChanged(int PercentZoom) {

	if (TimeAndLocationWidget)
		TimeAndLocationWidget->UpdateZoomInfo(PercentZoom);
}

void ACS_HUD::UpdateArrowsHUD(const bool& bUseUpMoveZone, const bool& bUseDownMoveZone, const bool& bUseLeftMoveZone, const bool& bUseRightMoveZone)
{
	if (MoveZoneWidget)
		MoveZoneWidget->SetShowArrows(bUseUpMoveZone, bUseDownMoveZone, bUseLeftMoveZone, bUseRightMoveZone);
}

void ACS_HUD::SpawnFadeWidget()
{
	if (FadeWidgetClass.WidgetClass)
		if (UCS_FadeWidget* Widget = CreateWidget<UCS_FadeWidget>(GetWorld(), FadeWidgetClass.WidgetClass))
		{
			Widget->AddToViewport(FadeWidgetClass.ZOrder);
		}
}

void ACS_HUD::StopAllSoundInGame(bool bPause)
{
	if (PlayerController.IsValid())
		PlayerController->StopVoiceManager(bPause);

	if (FAudioDevice* AudioDevice = GetWorld()->GetAudioDeviceRaw())
	{
		AudioDevice->Suspend(!bPause);
	}
}

bool ACS_HUD::ResetZoomInUI()
{
	UWorld* World = GetWorld();
	if (!World) return false;

	TArray<UUserWidget*> FoundWidgets;

	for (TObjectIterator<UUserWidget> It; It; ++It)
	{
		UUserWidget* Widget = *It;
		if (Widget && Widget->IsA(UCS_ShowScheme::StaticClass()) && Widget->GetWorld() == World)
		{
			FoundWidgets.Add(Widget);
		}
	}

	for (UUserWidget* Widget : FoundWidgets)
	{
		if (UCS_ShowScheme* ShowScheme = Cast<UCS_ShowScheme>(Widget))
		{
			if (ShowScheme->GetCurrentZoomPercent() > 100)
			{
				ShowScheme->ResetImageZoom();
				return true;
			}
		}
	}

	return false;
}

void ACS_HUD::ShowEndZone(const bool bIsShow)
{
	TryShowBorderWidget(bIsShow, EndZoneWidget, TAG_Gameplay_HUD_EndZone_Used);
}

void ACS_HUD::ShowLockedBoundaryWidget(const bool bIsShow)
{
	TryShowBorderWidget(bIsShow, LockedBoundaryWidget, TAG_Gameplay_HUD_LockedBoundary_Used);
}

void ACS_HUD::ActionToShowImageViewer(bool bHide)
{
	if (IsValid(TimeAndLocationWidget))
	{
		TimeAndLocationWidget->SetLocationVisibility(bHide ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	}
}

void ACS_HUD::UpdateCondition()
{
	if (IsValid(HUDWidget))
		HUDWidget->UpdateCondition();

	DisableInputForCharacter();
	

	if (UITags.HasTag(TAG_Gameplay_HUD_ActionLog_Used)) 
		PanelStickersWidget->SetVisibility(ESlateVisibility::Collapsed);
	else 
		PanelStickersWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (UITags.HasAny(TagsForDisableMoveCamera2D) || !WindowsArray.IsEmpty())
		MoveZoneWidget->SetLockCameraMove(true);
	else
		MoveZoneWidget->SetLockCameraMove(false);
	
	UpdateContextMenu();
	CheckDisableZoomCamera();
}

void ACS_HUD::CheckDisableZoomCamera()
{
	if (PlayerController.IsValid())
		PlayerController->Lock2DZoom(UITags.HasAny(TagsForDisableZoomCamera));
}

bool ACS_HUD::IsMouseOverAnyWidget()
{
	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllWidgets, UUserWidget::StaticClass());

	for (UUserWidget* Widget : AllWidgets)
	{
		if (Widget->IsHovered())
			return true;
	}

	return false;
}
