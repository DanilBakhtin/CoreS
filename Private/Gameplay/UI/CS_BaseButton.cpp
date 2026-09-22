#include "Gameplay/UI/CS_BaseButton.h"
#include "Gameplay/UI/CS_LocalizableTextBlock.h"
#include "Components/ButtonSlot.h"

UCS_BaseButton::UCS_BaseButton()
{
	NormalColorText = FSlateColor(FLinearColor::White);
	HoverColorText = FSlateColor(FLinearColor::Yellow);

	TextPadding = FMargin(0.f);

	bIsHoveredInternal = false;
}

TSharedRef<SWidget> UCS_BaseButton::RebuildWidget()
{
	TSharedRef<SWidget> ButtonWidget = Super::RebuildWidget();

	if (!ButtonText)
	{
		ButtonText = NewObject<UCS_LocalizableTextBlock>(this);
	}

	if (ButtonText)
	{
		SetContent(ButtonText);
		ButtonText->SetPreviewText(PreviewText);

		if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(ButtonText->Slot))
		{
			ButtonSlot->SetPadding(TextPadding);
		}

		if (!OnHovered.IsBound())
		{
			OnHovered.AddDynamic(this, &UCS_BaseButton::HandleOnHovered);
		}

		if (!OnUnhovered.IsBound())
		{
			OnUnhovered.AddDynamic(this, &UCS_BaseButton::HandleOnUnhovered);
		}
	}

	return ButtonWidget;
}

void UCS_BaseButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!ButtonText)
		return;

	ButtonText->SetFont(TextStyle);
	ButtonText->LocalizationKey = LocalizationKey;
	ButtonText->SetText(PreviewText);

	ButtonText->SetColorAndOpacity(
		bIsHoveredInternal ? HoverColorText : NormalColorText
	);
}

void UCS_BaseButton::HandleOnHovered()
{
	bIsHoveredInternal = true;
	SynchronizeProperties();
}

void UCS_BaseButton::HandleOnUnhovered()
{
	bIsHoveredInternal = false;
	SynchronizeProperties();
}