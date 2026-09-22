// Core Simulator


#include "Gameplay/UI/CS_ShowMessage.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Gameplay/UI/CS_BaseButton.h"
#include "Framework/CS_CoreTypes.h"
#include "Framework/CS_UtilityFunctionLibrary.h"


void UCS_ShowMessage::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(ShowMessageIcon_SizeBox);
	check(ShowMessage_WidgetSwitcher);
	check(Close_Button);
		Close_Button->OnReleased.AddDynamic(this, &UCS_ShowMessage::CloseShowMessage);
	check(Title_TextBlock);
	check(Text_TextBlock);
	check(Footer_TextBlock);
}

void UCS_ShowMessage::SetCloseShowMessageFunc(TFunction<void()> Func)
{
	CloseShowMessageFunc = Func;
}

void UCS_ShowMessage::InitParams(const FShowMessageParams& ShowMessageParams) const
{
	if(ShowMessageParams.Alarm == -1)
	{
		//ShowMessageIcon_SizeBox->SetVisibility(ESlateVisibility::Collapsed);
		ShowMessageIcon_SizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		ShowMessage_WidgetSwitcher->SetActiveWidgetIndex(2);
		Title_TextBlock->SetText(BaseTitleText);
	}
	else
	{
		ShowMessageIcon_SizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		ShowMessage_WidgetSwitcher->SetActiveWidgetIndex(ShowMessageParams.Alarm); 
		Title_TextBlock->SetText(FText::FromString(ShowMessageParams.Header));
	}
	
	FString FooterText = UCS_UtilityFunctionLibrary::ReplaceSubstring(ShowMessageParams.Footer, "<br>", "\n");
	Footer_TextBlock->SetText(FText::FromString(FooterText));
	FString MainText = UCS_UtilityFunctionLibrary::ReplaceSubstring(ShowMessageParams.Text, "<br>", "\n");
	Text_TextBlock->SetText(FText::FromString(MainText));
}

void UCS_ShowMessage::CloseShowMessage()
{
	if(CloseShowMessageFunc)
		CloseShowMessageFunc();
}
