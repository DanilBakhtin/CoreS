// Core Simulator


#include "Gameplay/UI/CS_DialogContainerWidget.h"

#include "Gameplay/UI/CS_ContactSelectorWidget.h"
#include "Gameplay/UI/CS_DialogWidget.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

void UCS_DialogContainerWidget::NativePreConstruct()
{
	CreateDialogWidgets();

	Super::NativePreConstruct();
}

void UCS_DialogContainerWidget::NativeConstruct()
{
	check(OverlayContainer)

	Super::NativeConstruct();
}

bool UCS_DialogContainerWidget::ShowContactWidget(ETypeDialog TypeDialog, const TArray<FDialogButtonParameters>& Parameters)
{
	RemoveDelegatsFromWidgets();

	if (ShowWidgetImpl(TypeDialog, MapContactWidget, CurrentContactWidget, CurrentDialogWidget)) 
	{
		StateDialogVis = EStateDialogVis::Contact;

		CurrentContactWidget->InitConctactList(Parameters);

		CurrentContactWidget->OnContactClose.AddDynamic(this, &UCS_DialogContainerWidget::DialogClose);
		CurrentContactWidget->OnContactSelect.AddDynamic(this, &UCS_DialogContainerWidget::ContactSelect);

		return true;
	}

	return false;
}

bool UCS_DialogContainerWidget::ShowDialogWidget(ETypeDialog TypeDialog, FString nameTitle, const TArray<FDialogParameters>& Params)
{
	RemoveDelegatsFromWidgets();

	if (ShowWidgetImpl(TypeDialog, MapDialogWidget, CurrentDialogWidget, CurrentContactWidget))
	{
		StateDialogVis = EStateDialogVis::Dialog;

		CurrentDialogWidget->InitShowDialog(nameTitle, Params);

		CurrentDialogWidget->ShowReturnToContactsButton(bActivateBackButton);

		CurrentDialogWidget->UpdateIcon(nullptr);

		CurrentDialogWidget->OnDialogWidgetClose.AddDynamic(this, &UCS_DialogContainerWidget::DialogClose);
		CurrentDialogWidget->OnSendMessage.AddDynamic(this, &UCS_DialogContainerWidget::SendMessage);
		CurrentDialogWidget->OnReturnToDialogs.AddDynamic(this, &UCS_DialogContainerWidget::BackDialog);

		return true;
	}
	
	return false;
}

bool UCS_DialogContainerWidget::AddMessage(const TArray<FDialogParameters>& Params)
{
	if (CurrentDialogWidget)
	{
		CurrentDialogWidget->InitAddMessage(Params);
		return true;
	}

	return false;
}

bool UCS_DialogContainerWidget::UpdateDialog(const TArray<FDialogParameters>& Params)
{
	if (CurrentDialogWidget)
	{
		CurrentDialogWidget->ShowReturnToContactsButton(bActivateBackButton);
		CurrentDialogWidget->InitUpdateDialog(Params);
		return true;
	}

	return false;
}

void UCS_DialogContainerWidget::UpdateNestingList(const TArray<FString>& NameList)
{
	if (CurrentDialogWidget)
		CurrentDialogWidget->UpdateNestingList(NameList);
}

void UCS_DialogContainerWidget::UpdateDialogIcon(UTexture2D* IconDialog)
{
	if (CurrentDialogWidget)
		CurrentDialogWidget->UpdateIcon(IconDialog);
}

void UCS_DialogContainerWidget::ClearNestingList()
{
	if (CurrentDialogWidget)
		CurrentDialogWidget->ClearNestingList();
}

void UCS_DialogContainerWidget::ActivateBackButton(bool bActivate)
{
	bActivateBackButton = bActivate;

	if (CurrentDialogWidget)
		CurrentDialogWidget->ShowReturnToContactsButton(bActivateBackButton);
}

bool UCS_DialogContainerWidget::IsContactShowing()
{
	return IsVisible() && StateDialogVis == EStateDialogVis::Contact;
}

void UCS_DialogContainerWidget::CreateDialogWidgets()
{
	if (OverlayContainer) 
	{
		OverlayContainer->ClearChildren();

		for (TPair<ETypeDialog, FDialogWidgetStructure>& Elem : TempMapDialogClasses)
		{
			if (Elem.Value.ContactWidgetClass)
			{
				if (UCS_ContactSelectorWidget* Widget = CreateWidget<UCS_ContactSelectorWidget>(GetWorld(), Elem.Value.ContactWidgetClass))
				{
					Widget->SetVisibility(ESlateVisibility::Collapsed);

					OverlayContainer->AddChild(Widget);
					MapContactWidget.Add(Elem.Key, Widget);

					if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget->Slot))
					{
						OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
						OverlaySlot->SetVerticalAlignment(VAlign_Fill);
						OverlaySlot->SetPadding(FMargin(0, 0, 0, 0));
					}
				}
			}
			if (Elem.Value.DialogWidgetClass) {
				if (UCS_DialogWidget* Widget = CreateWidget<UCS_DialogWidget>(GetWorld(), Elem.Value.DialogWidgetClass))
				{
					Widget->SetVisibility(ESlateVisibility::Collapsed);

					OverlayContainer->AddChild(Widget);
					MapDialogWidget.Add(Elem.Key, Widget);

					if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget->Slot))
					{
						OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
						OverlaySlot->SetVerticalAlignment(VAlign_Fill);
						OverlaySlot->SetPadding(FMargin(0, 0, 0, 0));
					}
				}
			}
		}
	}
}

void UCS_DialogContainerWidget::ContactSelect(FDialogButtonParameters Params)
{
	OnContactSelect.Broadcast(Params);
}

void UCS_DialogContainerWidget::SendMessage(FDialogParameters Params)
{
	OnSendMessage.Broadcast(Params);
}

void UCS_DialogContainerWidget::BackDialog()
{
	OnBackDialog.Broadcast();
}

void UCS_DialogContainerWidget::DialogClose()
{
	OnDialogClose.Broadcast();
}

void UCS_DialogContainerWidget::RemoveDelegatsFromWidgets()
{
	if (CurrentDialogWidget)
	{
		CurrentDialogWidget->OnDialogWidgetClose.RemoveDynamic(this, &UCS_DialogContainerWidget::DialogClose);
		CurrentDialogWidget->OnSendMessage.RemoveDynamic(this, &UCS_DialogContainerWidget::SendMessage);
		CurrentDialogWidget->OnReturnToDialogs.RemoveDynamic(this, &UCS_DialogContainerWidget::BackDialog);
	}

	if (CurrentContactWidget)
	{
		CurrentContactWidget->OnContactClose.RemoveDynamic(this, &UCS_DialogContainerWidget::DialogClose);
		CurrentContactWidget->OnContactSelect.RemoveDynamic(this, &UCS_DialogContainerWidget::ContactSelect);
	}
}

bool UCS_DialogContainerWidget::NeedSendCloseDialog()
{
	return CurrentDialogWidget != nullptr;
}
