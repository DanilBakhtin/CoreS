
#include "Gameplay/UI/CS_SendedMessage.h"
#include "Components/TextBlock.h"

void UCS_SendedMessage::NativeConstruct()
{
	Super::NativeConstruct();
	check(Message_TextBlock);
}

void UCS_SendedMessage::InitText(const FString& Text) const
{
	Message_TextBlock->SetText(FText::FromString(Text));
}
