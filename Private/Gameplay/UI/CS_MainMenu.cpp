#include "Gameplay/UI/CS_MainMenu.h"

#include "Framework/CS_CoreTypes.h"
#include "Framework/CS_DeveloperSettings.h"
#include "Framework/CS_GameInstance.h"
#include "Framework/CS_SimulatorParamsAsset.h"
#include "Gameplay/UI/CS_SelectCategoryItem.h"
#include "Gameplay/UI/CS_ShowScheme.h"
#include "Gameplay/UI/CS_ButtonMainMenu.h"
#include "Gameplay/Utilities/CS_LocalizationManager.h"
#include "Gameplay/UI/CS_BaseButton.h"

#include "Components/BackgroundBlur.h"
#include "Components/Border.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"
#include "Components/VerticalBox.h"

void UCS_MainMenu::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UCS_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	check(Main_SizeBox)
	check(Response_SizeBox)
	check(Quest_SizeBox)
	check(Help_ImageViewer)
	check(ConfirmExit_SizeBox)

	check(ContainerButtons)
	check(ContainerDynamicButtons)
	check(Menu_WidgetSwitcher)
	check(ButtonMainMenuWidgetClass)
	check(CategoryReasons_WrapBox)
	check(Answer_ListView)
	check(Answer_Border)
	check(Menu_BackgroundBlur)
	check(BalanceOfResponses_TextBlock)

	check(ConfirmExit_Button);
		Exit_Button->OnReleased.AddDynamic(this, &UCS_MainMenu::OpenConfirmExit);
	check(CloseMain_Button);
		CloseMain_Button->OnReleased.AddDynamic(this, &UCS_MainMenu::CloseMainMenu);
	check(CloseReasons_Button);
		CloseReasons_Button->OnReleased.AddDynamic(this, &UCS_MainMenu::ReturnToMainMenu);
	check(MakeReasons_Button)
		MakeReasons_Button->OnReleased.AddDynamic(this, &UCS_MainMenu::MakeReasons);
	check(QuestBack_Button);
		QuestBack_Button->OnReleased.AddDynamic(this, &UCS_MainMenu::ReturnToMainMenu);
	check(Help_ImageViewer);
		Help_ImageViewer->SetClosSchemeFunc([&]() {ReturnToMainMenu(); });
	check(CloseConfirmExit_Button);
		CloseConfirmExit_Button->OnReleased.AddDynamic(this, &UCS_MainMenu::ReturnToMainMenu);
	check(ConfirmExit_Button);
		ConfirmExit_Button->OnReleased.AddDynamic(this, &UCS_MainMenu::Exit);
	check(ImageViewer);
		ImageViewer->SetClosSchemeFunc([&]() {ReturnToMainMenu(); });


	check(Button_Reasons)
		Button_Reasons->OnClickActionName.AddDynamic(this, &UCS_MainMenu::OpenReasons);
	check(Button_Quest)
		Button_Quest->OnClickActionName.AddDynamic(this, &UCS_MainMenu::OpenQuest);
	check(Button_Help)
		Button_Help->OnClickActionName.AddDynamic(this, &UCS_MainMenu::OpenHelp);

	++BalanceOfResponses;
	MakeReasons();
}

void UCS_ListViewItem::Init(FAnswerButton _AnswerButton)
{
	AnswerButton = _AnswerButton;
	bIsCollapsed = false;
}

FAnswerButton UCS_ListViewItem::GetAnswerButtonData(bool& bEquipped)
{
	if (AnswerButton.apiId != "")
	{
		bEquipped = true;
		return AnswerButton;
	}
	bEquipped = false;
	return FAnswerButton();
}

void UCS_ListViewItem::SetMainMenu(UCS_MainMenu* _MainMenu)
{
	MainMenu = _MainMenu;
}

void UCS_ListViewItem::SetCollapse()
{
	bIsCollapsed = true;
	Collapse.Broadcast();
}

void UCS_MainMenu::InitMainMenu(const FMainMenuParams& _MainMenuParams)
{
	MainMenuParams = _MainMenuParams;

	CategoryReasons_WrapBox->ClearChildren();
	Answers.Empty();
	
	if (MainMenuParams.MainMenuReasons.IsEmpty()) {
		Button_Reasons->SetVisibility(ESlateVisibility::Collapsed);
	}
	else {
		for (const FMainMenuReasons& Reason : MainMenuParams.MainMenuReasons)
		{
			if (UCS_SelectCategoryItem* Widget = CreateWidget<UCS_SelectCategoryItem>(GetWorld(), SelectCategoryItemClass))
			{
				CategoryReasons_WrapBox->AddChildToWrapBox(Widget);
				Widget->InitCategory(Reason.ReasonId, Reason.ReasonName);
				Widget->OnSelectCategoryItem.AddDynamic(this, &UCS_MainMenu::SelectCategory);
			}
			Answers.Add(Reason.ReasonId, Reason.AnswerButtonArray);
		}
	}

	if (MainMenuParams.MainMenuExitInfo.bInit) 
	{
		if (!MainMenuParams.MainMenuExitInfo.warn.IsEmpty())
			TitleTextConfirmExit_TextBlock->SetText(FText::FromString(MainMenuParams.MainMenuExitInfo.warn));

		//Text in "Confirm Exit" menu
		QuestTextConfirmExit_TextBlock->SetText(FText::FromString(MainMenuParams.MainMenuExitInfo.quest));
		TextConfirmExit_TextBlock->SetText(FText::FromString(MainMenuParams.MainMenuExitInfo.text));
	}


	ContainerDynamicButtons->ClearChildren();
	for (const auto& Elem : MainMenuParams.MapMainMenuPointsInfo)
	{
		const FString& Key = Elem.Key;
		const FMainMenuPointInfo& Value = Elem.Value;

		if (UCS_ButtonMainMenu* Widget = CreateWidget<UCS_ButtonMainMenu>(GetWorld(), ButtonMainMenuWidgetClass))
		{
			if (ContainerDynamicButtons->AddChild(Widget)) {

				FButtonMainMenu ButtonMainMenu = FButtonMainMenu();

				ButtonMainMenu.PreviewText = FText::FromString(Value.Name);
				ButtonMainMenu.ActionName = FName(*Key);
				Widget->InitWidget(ButtonMainMenu);

				Widget->OnClickActionName.RemoveAll(this);
				Widget->OnClickActionName.AddDynamic(this, &UCS_MainMenu::OpenImageWidget);
			}
			else {
				Widget->RemoveFromParent();
				Widget->ConditionalBeginDestroy();
			}
		}
	}
}

void UCS_MainMenu::SelectCategory(const FString& CategoryId, UCS_SelectCategoryItem* SelectCategoryItem, bool IsChecked)
{
	Answer_ListView->RegenerateAllEntries();
	if(IsChecked)
	{		
		for(UWidget*& Children : CategoryReasons_WrapBox->GetAllChildren())
		{
			if(UCS_SelectCategoryItem* Widget = Cast<UCS_SelectCategoryItem>(Children))
			{
				if (Widget != SelectCategoryItem)
				{
					Widget->Uncheck();
				}
			}
		}
		MakeItemList(CategoryId);
		Answer_Border->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UCS_MainMenu::MakeItemList(const FString& Category)
{

	Answer_ListView->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	FString UsedCategory = Category;
	if (ListEquipItems.Num() <= 0)
	{
		if (const FAnswerButtonArray* Items = Answers.Find(UsedCategory))
		{
			for(const FAnswerButton& Item : Items->AnswerButtons)
			{
				if(UCS_ListViewItem* EquipItem = NewObject<UCS_ListViewItem>(this, UCS_ListViewItem::StaticClass()))
				{
					EquipItem->Init(Item);
					EquipItem->SetMainMenu(this);
					ListEquipItems.Add(EquipItem);
					Answer_ListView->AddItem(EquipItem);
				}
			}
		}
	}
	else
	{
		const TArray<FAnswerButton> AnswerButtons = Answers.Find(Category)->AnswerButtons;
		const int32 ListEquipLength = ListEquipItems.Num();
		const int32 NewItemList =  AnswerButtons.Num();
		int32 Steps;
		if (ListEquipLength > NewItemList)
			Steps = ListEquipLength;
		else
			Steps = NewItemList;

		for (int32 i = 0; i < Steps; ++i)
		{
			if (i < ListEquipLength && i < NewItemList)
			{
				ListEquipItems[i]->Init(AnswerButtons[i]);
				ListEquipItems[i]->UpdateItem.Broadcast();
			}
			else
			{
				if(i < NewItemList && i >= ListEquipLength)
				{
					if(UCS_ListViewItem* EquipItem = NewObject<UCS_ListViewItem>(this, UCS_ListViewItem::StaticClass()))
					{
						EquipItem->Init(AnswerButtons[i]);
						EquipItem->SetMainMenu(this);
						ListEquipItems.Add(EquipItem);
						Answer_ListView->AddItem(EquipItem);
						
					}
				}
				else
					if(i < ListEquipItems.Num())
						ListEquipItems[i]->SetCollapse();
			}
		}
	}
}

void UCS_MainMenu::SetReasonActionEventFun(TFunction<void(const FString& AnswerId)> Fun)
{
	ReasonActionEventFun = Fun;
}

void UCS_MainMenu::SetExitCourseFunc(TFunction<void()> Func)
{
	ExitCourseFunc = Func;
}

void UCS_MainMenu::SetCloseMainMenuFunc(TFunction<void()> Func)
{
	CloseMainMenuFunc= Func;
}

void UCS_MainMenu::SetLockFun(TFunction<void()> Func)
{
	LockFunc = Func;
}

void UCS_MainMenu::SetIsLockedMenuFun(TFunction<bool()> Func)
{
	IsLockedMenuFunc = Func;
}

void UCS_MainMenu::SetAnswer(const FString& AnswerCategoryId, const FString& AnswerId)
{
	CheckItem.Broadcast(AnswerId);
	SelectedAnswerId = AnswerId;
	const TFunction<void(const FString& IdForCheck)> CheckItems = [this](const FString& IdForCheck)
	{
		for(UWidget*& Children : CategoryReasons_WrapBox->GetAllChildren())
		{
			if(const UCS_SelectCategoryItem* Widget = Cast<UCS_SelectCategoryItem>(Children))
			{
				Widget->CheckMarker(IdForCheck);
			}
		}
	};
	if(AnswerId != "")
	{
		MakeReasons_Button->SetIsEnabled(true);
		CheckItems(AnswerCategoryId);
	}
	else
	{
		MakeReasons_Button->SetIsEnabled(false);
		CheckItems(FString());
	}
}

void UCS_MainMenu::ResetMenu()
{
	ReturnToMainMenu();
	Menu_BackgroundBlur->SetVisibility(ESlateVisibility::Collapsed);
	//Answer_Border->SetVisibility(ESlateVisibility::Collapsed);
}

void UCS_MainMenu::Exit()
{
	if(ExitCourseFunc)
		ExitCourseFunc();
}

void UCS_MainMenu::CloseMainMenu()
{
	if(CloseMainMenuFunc)
	{
		CloseMainMenuFunc();
		if(IsLockedMenuFunc)
		{
			if(!IsLockedMenuFunc())
			{
				ResetMenu();
			}
		}
	}
}

void UCS_MainMenu::InitQuestInfo(const FShowWelcomeParams& Params)
{
	Quest_TextBlock->SetText(FText::FromString(Params.FaultInfoDescription));
	Quest_Footer_TextBlock->SetText(FText::FromString(Params.FaultInfoQuest));
}

void UCS_MainMenu::InitHelpImage(TArray<TSoftObjectPtr<UTexture2D>> Textures) const
{
	Help_ImageViewer->InitSchemeImage(Textures);
}

void UCS_MainMenu::HideLoadingWidget(bool bHide)
{
	Menu_BackgroundBlur->SetVisibility(bHide? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
}

void UCS_MainMenu::ReturnToMainMenu()
{
	Menu_WidgetSwitcher->SetActiveWidget(Main_SizeBox);
	OnShowImageViewer.Broadcast(false);

	for (UWidget*& Children : CategoryReasons_WrapBox->GetAllChildren())
	{
		if (UCS_SelectCategoryItem* Widget = Cast<UCS_SelectCategoryItem>(Children))
		{
				Widget->Uncheck();
		}
	}

	Answer_ListView->SetVisibility(ESlateVisibility::Collapsed);
}

void UCS_MainMenu::MakeReasons()
{
	if(ReasonActionEventFun)
		ReasonActionEventFun(SelectedAnswerId);
	Menu_BackgroundBlur->SetVisibility(ESlateVisibility::Visible);
	--BalanceOfResponses;
	
	FString BalanceText = TEXT("Осталось попыток: ") + FString::FromInt(BalanceOfResponses);

	if (UCS_GameInstance* GameInstance = Cast<UCS_GameInstance>(GetGameInstance())) {
		FText LocalizedText = GameInstance->GetLocalizationManager()->GetText("MAIN_MENU_RESPONSE_LEFT_ATTEMPTS");
		BalanceText = LocalizedText.ToString() + FString::FromInt(BalanceOfResponses);
	}


	BalanceOfResponses_TextBlock->SetText(FText::FromString(BalanceText));
	if(LockFunc)
		LockFunc();
}

void UCS_MainMenu::OpenConfirmExit()
{
	Menu_WidgetSwitcher->SetActiveWidget(ConfirmExit_SizeBox);
}

void UCS_MainMenu::OpenImageWidget(const FName NamePoint)
{
	if (FMainMenuPointInfo* PointInfo = MainMenuParams.MapMainMenuPointsInfo.Find(NamePoint.ToString())) {

		TArray<FString> IDs;
		FMainMenuPointInfo Info = *PointInfo;

		if (ImageViewer->InitSchemeImageFromInfo(Info.LoadingImagesInfo)) {
			Menu_WidgetSwitcher->SetActiveWidgetIndex(9);
			OnShowImageViewer.Broadcast(true);
		}
	}
}

void UCS_MainMenu::OpenReasons(const FName NamePoint)
{
	Menu_WidgetSwitcher->SetActiveWidget(Response_SizeBox);
}

void UCS_MainMenu::OpenQuest(const FName NamePoint)
{
	Menu_WidgetSwitcher->SetActiveWidget(Quest_SizeBox);
}

void UCS_MainMenu::OpenHelp(const FName NamePoint)
{
	Menu_WidgetSwitcher->SetActiveWidget(Help_ImageViewer);
	OnShowImageViewer.Broadcast(true);
}


