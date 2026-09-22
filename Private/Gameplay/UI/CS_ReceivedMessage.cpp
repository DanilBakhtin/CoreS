#include "Gameplay/UI/CS_ReceivedMessage.h"
#include "Components/TextBlock.h"

void UCS_ReceivedMessage::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(Message_TextBlock);
}

void UCS_ReceivedMessage::InitText(const FString& Text) const
{
	Message_TextBlock->SetText(FText::FromString(Text));
}
