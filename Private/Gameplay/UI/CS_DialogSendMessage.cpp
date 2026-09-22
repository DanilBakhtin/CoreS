// Core Simulator


#include "Gameplay/UI/CS_DialogSendMessage.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UCS_DialogSendMessage::NativeConstruct()
{
	Super::NativeConstruct();
	check(Message_TextBlock);
	check(Message_Button);
		Message_Button->OnClicked.AddDynamic(this, &UCS_DialogSendMessage::ClickOnDialogButton);
}

void UCS_DialogSendMessage::InitDialogMessage(const FDialogParameters& Params)
{
	ButtonParameters = Params;
	Message_TextBlock->SetText(FText::FromString(Params.Message));
}

void UCS_DialogSendMessage::ClickOnDialogButton()
{
	OnClickSendMessageButton.Broadcast(ButtonParameters);
}
