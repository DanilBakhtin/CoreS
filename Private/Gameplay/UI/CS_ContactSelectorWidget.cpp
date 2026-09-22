// Core Simulator


#include "Gameplay/UI/CS_ContactSelectorWidget.h"

#include "Gameplay/UI/CS_RadioListItem.h"

#include "Components/Button.h"
#include "Components/WrapBox.h"

void UCS_ContactSelectorWidget::NativeConstruct()
{
	check(CloseButton)
		CloseButton->OnReleased.AddDynamic(this, &UCS_ContactSelectorWidget::CloseContacts);
	check(ListContacts_WrapBox)

	Super::NativeConstruct();
}

void UCS_ContactSelectorWidget::InitConctactList(const TArray<FDialogButtonParameters>& Parameters)
{
	//Unsubscribe before destruction
	for (int i = 0; i < CurrentContactItems.Num(); i++)
		if (IsValid(CurrentContactItems[i]))
			CurrentContactItems[i]->OnClickContactButton.RemoveDynamic(this, &UCS_ContactSelectorWidget::OnClickContact);

	ListContacts_WrapBox->ClearChildren();
	CurrentContactItems.Empty();

	for (const FDialogButtonParameters& ContactItem : Parameters)
	{
		if (UCS_RadioListItem* Widget = CreateWidget<UCS_RadioListItem>(GetWorld(), ContactListItemClass))
		{
			ListContacts_WrapBox->AddChildToWrapBox(Widget);
			Widget->InitRadioList(ContactItem);
			Widget->Calling(ContactItem.bCalling);
			CurrentContactItems.Add(Widget);

			Widget->OnClickContactButton.AddDynamic(this, &UCS_ContactSelectorWidget::OnClickContact);
		}
	}
}

void UCS_ContactSelectorWidget::CloseContacts()
{
	OnContactClose.Broadcast();
}

void UCS_ContactSelectorWidget::OnClickContact(FDialogButtonParameters Params)
{
	OnContactSelect.Broadcast(Params);
}
