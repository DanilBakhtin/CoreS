// Core Simulator


#include "Gameplay/UI/CS_DialogWidget.h"

#include "Framework/CS_CoreTypes.h"
#include "Gameplay/UI/CS_DialogSendMessage.h"
#include "Gameplay/UI/CS_RadioListItem.h"
#include "Gameplay/UI/CS_ReceivedMessage.h"
#include "Gameplay/UI/CS_SendedMessage.h"
#include "Gameplay/UI/CS_NestedItemDialog.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"

void UCS_DialogWidget::NativeConstruct()
{
	check(ReturnToContactsButton)
		ReturnToContactsButton->OnReleased.AddDynamic(this, &UCS_DialogWidget::ReturnToContacts);
	check(CloseButton)
		CloseButton->OnReleased.AddDynamic(this, &UCS_DialogWidget::CloseDialog);

	check(TextNameUser)

	check(Dialog_WrapBox)
	check(Dialog_ScrollBox)

	check(Request_WrapBox)
	check(Request_ScrollBox)

	check(NestedList_WrapBox)

	check(Icon_Image)

	if (Icon_Image) {
		const FSlateBrush & Brush = Icon_Image->GetBrush();
		UObject* ResourceObject = Brush.GetResourceObject();

		DefaultIconTexture = Cast<UTexture2D>(ResourceObject);
	}

	Super::NativeConstruct();
}

void UCS_DialogWidget::InitShowDialog(FString nameTitle, const TArray<FDialogParameters>& Params)
{
	Dialog_WrapBox->ClearChildren();
	Request_WrapBox->ClearChildren();

	TextNameUser->SetText(FText::FromString(nameTitle));

	for (const FDialogParameters& Param : Params)
	{
		if (Param.DialogElement == EDialogElement::Dude)
			AddDudeMSG(Param.Message);
		if (Param.DialogElement == EDialogElement::User)
			AddUserMSG(Param.Message);
		if (Param.DialogElement == EDialogElement::Message)
			AddRequestItem(Param);
	}
	ScrollDown();
}

void UCS_DialogWidget::UpdateNestingList(const TArray<FString>& NameList)
{
	if (NestedList_WrapBox)
	{
		NestedList_WrapBox->ClearChildren();

		for (int i = 0; i < NameList.Num(); i++)
		{
			if (IsValid(NestedItemClass))
			{
				if (UCS_NestedItemDialog* Widget = CreateWidget<UCS_NestedItemDialog>(GetWorld(), NestedItemClass))
				{
					NestedList_WrapBox->AddChildToWrapBox(Widget);
					
					Widget->SetDescriptionText(NameList[i]);
					Widget->SetShowArrowImage(i != (NameList.Num() - 1));
				}
			}
		}
	}
}

void UCS_DialogWidget::ClearNestingList()
{
	if (NestedList_WrapBox)
		NestedList_WrapBox->ClearChildren();
}

void UCS_DialogWidget::AddDudeMSG(const FString& Text) const
{
	if (UCS_ReceivedMessage* Widget = CreateWidget<UCS_ReceivedMessage>(GetWorld(), ReceivedMessageClass))
	{
		Dialog_WrapBox->AddChildToWrapBox(Widget);
		Widget->InitText(Text);
	}
}

void UCS_DialogWidget::AddUserMSG(const FString& Text) const
{
	if (UCS_SendedMessage* Widget = CreateWidget<UCS_SendedMessage>(GetWorld(), SendedMessageClass))
	{
		Dialog_WrapBox->AddChildToWrapBox(Widget);
		Widget->InitText(Text);
	}
}

void UCS_DialogWidget::AddRequestItem(const FDialogParameters& Params)
{
	if (UCS_DialogSendMessage* Widget = CreateWidget<UCS_DialogSendMessage>(GetWorld(), DialogSendMessage))
	{
		Request_WrapBox->AddChildToWrapBox(Widget);
		Widget->InitDialogMessage(Params);
		DialogSendItems.Add(Widget);

		Widget->OnClickSendMessageButton.AddDynamic(this, &UCS_DialogWidget::OnClickSendMessage);
	}
}

void UCS_DialogWidget::UpdateIcon(UTexture2D* NewTexture)
{
	if (IsValid(Icon_Image))
	{
		if (NewTexture)
		{
			FSlateBrush Brush = Icon_Image->GetBrush();
			Brush.SetResourceObject(NewTexture);
			Icon_Image->SetBrush(Brush);
		}
		else if (DefaultIconTexture)
		{
			FSlateBrush Brush = Icon_Image->GetBrush();
			Brush.SetResourceObject(DefaultIconTexture);
			Icon_Image->SetBrush(Brush);
		}
	}
}

void UCS_DialogWidget::InitAddMessage(const TArray<FDialogParameters>& Params)
{
	for (const FDialogParameters& Param : Params)
	{
		if (Param.DialogElement == EDialogElement::Dude)
			AddDudeMSG(Param.Message);
		if (Param.DialogElement == EDialogElement::User)
			AddUserMSG(Param.Message);
	}
	ScrollDown();
}

void UCS_DialogWidget::InitUpdateDialog(const TArray<FDialogParameters>& Params)
{
	Request_WrapBox->ClearChildren();
	for (const FDialogParameters Param : Params)
	{
		AddRequestItem(Param);
	}
	ScrollDown();
}

void UCS_DialogWidget::ShowReturnToContactsButton(bool bShow)
{
	if (ReturnToContactsButton)
	{
		ReturnToContactsButton->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UCS_DialogWidget::ScrollDown() const
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			Dialog_ScrollBox->SetScrollOffset(Dialog_ScrollBox->GetScrollOffsetOfEnd());
		},
		0.1f,
		false);
}

void UCS_DialogWidget::ReturnToContacts()
{
	OnReturnToDialogs.Broadcast();
}

void UCS_DialogWidget::CloseDialog()
{
	OnDialogWidgetClose.Broadcast();
}

void UCS_DialogWidget::OnClickSendMessage(FDialogParameters Params)
{
	OnSendMessage.Broadcast(Params);
}
