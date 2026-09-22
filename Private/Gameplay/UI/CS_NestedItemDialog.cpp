// Core Simulator


#include "Gameplay/UI/CS_NestedItemDialog.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCS_NestedItemDialog::NativeConstruct()
{
	check(ArrowImage)
	check(TextDescription)
}

void UCS_NestedItemDialog::SetDescriptionText(const FString& Description)
{
	if (TextDescription)
		TextDescription->SetText(FText::FromString(Description));
}

void UCS_NestedItemDialog::SetShowArrowImage(bool bShow)
{
	if (ArrowImage)
		ArrowImage->SetVisibility(bShow? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}
