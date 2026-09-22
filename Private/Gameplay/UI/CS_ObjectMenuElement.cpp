#include "Gameplay/UI/CS_ObjectMenuElement.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Gameplay/Player/CS_PlayerState.h"

void UCS_ObjectMenuElement::NativeConstruct()
{
	Super::NativeConstruct();

	check(ObjectMenuElement_CheckBox);
	check(ObjectMenuElement_TextBlock);
	check(ObjectMenuElement_Button);
		ObjectMenuElement_Button->OnReleased.Clear();
		ObjectMenuElement_Button->OnReleased.AddDynamic(this, &UCS_ObjectMenuElement::OnClickElement);
}

void UCS_ObjectMenuElement::InitObjectMenuElement(const FString& NewObjectAPI_Id, const FFullMenuReasons& FullMenuReasons)
{
	ECheckBoxState CheckBoxState;
	
	if (FullMenuReasons.bIsChecked)
	{
		CheckBoxState = ECheckBoxState::Checked;
		
		FButtonStyle ButtonStyle = ObjectMenuElement_Button->GetStyle();
		ButtonStyle.Normal.TintColor = FSlateColor(FColor(187, 187, 187, 255));
		ObjectMenuElement_Button->SetStyle(ButtonStyle);
	}
	else
	{
		CheckBoxState = ECheckBoxState::Unchecked;
	}
	
	ObjectAPI_Id = NewObjectAPI_Id;
	ReasonAPI_Id = FullMenuReasons.API_Id;
	bIsClosed = FullMenuReasons.bIsClosed;
	
	ObjectMenuElement_CheckBox.Get()->SetCheckedState(CheckBoxState);
	ObjectMenuElement_TextBlock.Get()->SetText(FText::FromString(FullMenuReasons.Name));
}

void UCS_ObjectMenuElement::SetCloseObjectMenuFunction(const TFunction<void()>& NewCloseObjectMenuFunction)
{
	CloseObjectMenuFunction = NewCloseObjectMenuFunction;
}

void UCS_ObjectMenuElement::SetClickOnObject(
	const TFunction<void(const FString& CastedToStringAttribute)>& NewClickOnElementFunction)
{
	ClickOnElementFunction = NewClickOnElementFunction;
}

void UCS_ObjectMenuElement::SetPlayerController(ACS_PlayerController* NewPlayerController)
{
	PlayerController = NewPlayerController;
}

// ReSharper disable once CppMemberFunctionMayBeConst
// Cause: Function used in delegate.
void UCS_ObjectMenuElement::OnClickElement()
{
	if (!PlayerController.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectMenuElement: Player controller is invalid! How did you do that???"));
		return;
	}

	if (bIsClosed)
	{
		CallCloseObjectMenu();
		return;
	}
	
	if (ObjectMenuElement_CheckBox.Get()->GetCheckedState() == ECheckBoxState::Checked)
	{
		CallCloseObjectMenu();
		return;
	}
	
	// "castedToStringAttribute":"reasons";
	// "reasons" = "{"place":"placeAPI_id", "object":"objectAPI_Id", "checked":["reasonApi_ID"]}"
	const FString CastedToStringAttribute = FString::Printf(
		TEXT("{\"place\":\"%s\", \"object\": \"%s\", \"checked\":[\"%s\"]}"),
		*PlayerController->GetCS_PlayerState()->GetCurrentLocationAPI_Id(),
		*ObjectAPI_Id,
		*ReasonAPI_Id);
	
	if(ClickOnElementFunction)
	{
		ClickOnElementFunction(CastedToStringAttribute);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ObjectMenuElement: ClickOnElementFunction wasn't set! "
								"Click wouldn't call reaction in AOS!"))
	}
	
	CallCloseObjectMenu();
}

void UCS_ObjectMenuElement::CallCloseObjectMenu() const
{
	if(CloseObjectMenuFunction)
	{
		UE_LOG(LogTemp, Log, TEXT("ObjectMenuElement: Close object menu."))
		
		CloseObjectMenuFunction();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ObjectMenuElement: CloseObjectMenuFunction wasn't set!"))
	}
}
