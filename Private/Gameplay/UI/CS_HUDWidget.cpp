// Core Simulator


#include "Gameplay/UI/CS_HUDWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Gameplay/UI/CS_ButtonWidget.h"
#include "Gameplay/UI/CS_CheckWidget.h"
#include "Gameplay/UI/CS_AWP_Widget.h"
#include "Framework/CS_DeveloperSettings.h"
#include "Framework/CS_SimulatorParamsAsset.h"

DEFINE_LOG_CATEGORY_STATIC(LogUСS_InfoQuest, All, All);


void UCS_HUDWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsValid(TabButton))
	{
		if (bIsTabHidden)
		{
			TabButton->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			TabButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}

	if (IsValid(Helper_HudCheck))
	{
		if (const UCS_DeveloperSettings* Settings = GetDefault<UCS_DeveloperSettings>())
		{
			if (UCS_SimulatorParamsAsset* SimulatorParams = Settings->SimulatorParamsAsset.LoadSynchronous())
			{
				if (SimulatorParams->Params.bIsHelperAvailable)
				{
					Helper_HudCheck->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}
				else
				{
					Helper_HudCheck->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
		}
	}
	
	CreateHotKeys();
}

void UCS_HUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//Подписка на динамически созданные кнопки
	for (UCS_CheckWidget* Widget : HotKeyWidgets)
	{
		if (Widget && Widget->IsValidLowLevel())
		{
			Widget->OnCheckedChangeWithActiveTag.AddDynamic(this, &UCS_HUDWidget::OnDynamicButtonCheckedChange);
		}
	}

}

void UCS_HUDWidget::GetUITagContainer(TFunction<FGameplayTagContainer()> Func)
{
	GetTagContainerFun = Func;
	if(!GetTagContainerFun)
	{
		UE_LOG(LogUСS_InfoQuest, Error, TEXT("Getter func is not set for LS_InfoQuest"))
		return;
	}
	check(Quest_HudCheck)
		Quest_HudCheck->GetUITagContainer(GetTagContainerFun);
	check(Info_HudCheck)
		Info_HudCheck->GetUITagContainer(GetTagContainerFun);
	check(Log_HudCheck)
		Log_HudCheck->GetUITagContainer(GetTagContainerFun);
	check(Helper_HudCheck)
		Helper_HudCheck->GetUITagContainer(GetTagContainerFun);
	check(Menu_HudButton)
		Menu_HudButton->GetUITagContainer(GetTagContainerFun);

	check(ContainerHotKeys)

	for (UCS_CheckWidget* Widget : HotKeyWidgets)
	{
		if (Widget)
		{
			Widget->GetUITagContainer(GetTagContainerFun);
		}
	}


}

void UCS_HUDWidget::AddAddTag(TFunction<void(TArray<FGameplayTag> Tags)> Func)
{
	Super::AddAddTag(Func);

	if(IsValid(Quest_HudCheck))
		Quest_HudCheck->AddAddTag(Func);
	if(IsValid(Info_HudCheck))
		Info_HudCheck->AddAddTag(Func);
	if (IsValid(Log_HudCheck))
		Log_HudCheck->AddAddTag(Func);
	if (IsValid(Helper_HudCheck))
		Helper_HudCheck->AddAddTag(Func);
	if(IsValid(Menu_HudButton))
		Menu_HudButton->AddAddTag(Func);

	for (UCS_CheckWidget* Widget : HotKeyWidgets)
	{
		if (Widget)
		{
			Widget->AddAddTag(Func);
		}
	}
}

void  UCS_HUDWidget::UpdateCondition() const
{
	if(!GetTagContainerFun)
	{
		UE_LOG(LogUСS_InfoQuest, Error, TEXT("Getter func is not set for СS_InfoQuest"))
		return;
	}
	if(!AddTagFun)
	{
		UE_LOG(LogUСS_InfoQuest, Error, TEXT("AddTag func is not set for СS_InfoQuest"))
		return;
	}
	if(IsValid(TabButton))
	{
		TabButton->UpdateButtonCondition(GetTagContainerFun());
	}
	if(IsValid(Quest_HudCheck))
	{
		Quest_HudCheck->UpdateButtonCondition(GetTagContainerFun());
	}
	if(IsValid(Info_HudCheck))
	{
		Info_HudCheck->UpdateButtonCondition(GetTagContainerFun());
	}
	if(IsValid(Menu_HudButton))
	{
		Menu_HudButton->UpdateButtonCondition(GetTagContainerFun());
	}
	if (IsValid(Log_HudCheck))
	{
		Log_HudCheck->UpdateButtonCondition(GetTagContainerFun());
	}
	if (IsValid(Helper_HudCheck))
	{
		Helper_HudCheck->UpdateButtonCondition(GetTagContainerFun());
	}

	for (UCS_CheckWidget* Widget : HotKeyWidgets)
	{
		if (Widget && Widget->IsValidLowLevel())
		{
			Widget->UpdateButtonCondition(GetTagContainerFun());
		}
	}
}

FReply UCS_HUDWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UCS_HUDWidget::AddRemoveTag(TFunction<void(TArray<FGameplayTag> Tags)> Func)
{
	Super::AddRemoveTag(Func);

	if(IsValid(Quest_HudCheck))
		Quest_HudCheck->AddRemoveTag(Func);
	if(IsValid(Info_HudCheck))
		Info_HudCheck->AddRemoveTag(Func);
	if (IsValid(Log_HudCheck))
		Log_HudCheck->AddRemoveTag(Func);
	if (IsValid(Helper_HudCheck))
		Helper_HudCheck->AddRemoveTag(Func);
	if(IsValid(Menu_HudButton))
		Menu_HudButton->AddRemoveTag(Func);

	for (UCS_CheckWidget* Widget : HotKeyWidgets)
	{
		if (Widget)
		{
			Widget->AddRemoveTag(Func);
		}
	}
}

void UCS_HUDWidget::EnableBlinkingToHotKeys(const int indexHotKey, const bool bEnable)
{
	if (HotKeyWidgets.IsValidIndex(indexHotKey))
	{
		HotKeyWidgets[indexHotKey]->EnableBlinking(bEnable);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid indexHotKey: %d"), indexHotKey);
	}
}

void UCS_HUDWidget::EnableBlinkingToHotKeysByTag(const FGameplayTag ActivatedTag, const bool bEnable)
{
	if (MapCheckWidgets.Contains(ActivatedTag))
		MapCheckWidgets[ActivatedTag]->EnableBlinking(bEnable);
}

void UCS_HUDWidget::AddAWP_Widget(UCS_AWP_Widget* AWP_Widget)
{
	if (AWP_Widget)
	{
		UCanvasPanelSlot* SlotAWP = ContainerAWP->AddChildToCanvas(AWP_Widget);

		AWP_Widget->InitSize(SizeAWP);
		SlotAWP->SetSize(SizeAWP);

		SlotAWP->SetPosition(FVector2D(0.0f, 0.0f));
		SlotAWP->SetMinimum(FVector2D(0.0f, 0.0f));
		SlotAWP->SetMaximum(FVector2D(0.0f, 0.0f));
		SlotAWP->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s: AWP widget is invalid!"), *GetName());
	}
}

void UCS_HUDWidget::ClearAWPContainer()
{
	ContainerAWP->ClearChildren();
}

void UCS_HUDWidget::CreateHotKeys()
{
	if (const UCS_DeveloperSettings* Settings = GetDefault<UCS_DeveloperSettings>()) {
		if (UCS_SimulatorParamsAsset* SimulatorParams = Settings->SimulatorParamsAsset.LoadSynchronous()) {
			int Counter = 1;
			
			ContainerHotKeys->ClearChildren();
			HotKeyWidgets.Empty();
			
			for (FHotkeySlot& HotKey : SimulatorParams->HotKeys) {

				if (!HotKey.bEnabled) continue;

				if (UCS_CheckWidget* Widget = CreateWidget<UCS_CheckWidget>(GetWorld(), CheckWidgetClass))
				{
					ContainerHotKeys->AddChild(Widget);

					FSoftObjectPath SoftIconPath = HotKey.Icon.ToSoftObjectPath();
					UObject* LoadedIcon = SoftIconPath.ResolveObject();

					if (!LoadedIcon)
						LoadedIcon = SoftIconPath.TryLoad();

					Widget->Init(HotKey.Icon.Get(), FString::FromInt(Counter), HotKey);
					Widget->SetPadding(PaddingBtwHotKeys);

					HotKeyWidgets.Add(Widget);

					MapCheckWidgets.Add(HotKey.Activated, Widget);
				}

				Counter++;

				if (Counter > 9)
					Counter = 0;
			}
		}
	}
}

void UCS_HUDWidget::OnDynamicButtonCheckedChange(FGameplayTag ActiveTag, bool bIsChecked)
{
	OnCheckedChancgeDynamicButton.Broadcast(ActiveTag, bIsChecked);
}
