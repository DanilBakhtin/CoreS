#include "Gameplay/UI/CS_ObjectMenu.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Gameplay/Player/CS_PlayerState.h"
#include "Gameplay/UI/CS_ContextItemButton.h"
#include "Gameplay/UI/CS_ObjectMenuElement.h"

void UCS_ObjectMenu::NativeConstruct()
{
	Super::NativeConstruct();

	check(ObjectMenu_CanvasPanel);
	check(ObjectMenuElement_TextBlock);
	check(ObjectMenuElement_TextBlock_Name);
	check(ObjectMenu_ClearButton);
		ObjectMenu_ClearButton->OnReleased.Clear();
		ObjectMenu_ClearButton->OnReleased.AddDynamic(this, &UCS_ObjectMenu::OnReleasedClearButton);
	check(ObjectMenu_BackgroundButton);
		ObjectMenu_BackgroundButton->OnReleased.Clear();
		ObjectMenu_BackgroundButton->OnReleased.AddDynamic(this, &UCS_ObjectMenu::OnReleasedEmptyPlace);
	check(ObjectMenu_SizeBox);
	check(ObjectMenu_VerticalBox);
	check(ObjectMenuElementClass);
}

void UCS_ObjectMenu::SetPlayerController(ACS_PlayerController* NewPlayerController)
{
	PlayerController = NewPlayerController;
}

void UCS_ObjectMenu::SetCloseObjectMenuFun(const TFunction<void()>& NewCloseObjectMenuFunction)
{
	CloseObjectMenuFunction = NewCloseObjectMenuFunction;
}

void UCS_ObjectMenu::SetReleasedOnObject(
	const TFunction<void(const FString& Type)>& NewClickOnElementFunction)
{
	ClickOnElementFunction = NewClickOnElementFunction;
}

void UCS_ObjectMenu::InitObjectMenuElements(const FFullMenuReasons& FullMenuReasons)
{
	if(!PlayerController.IsValid())
	{
		return;
	}

	/*
	 * Warning: In Object Menu case, we except FFullMenuReasons to be nested two level deep.
	 * First level: message "Choose fault reason".
	 * Second level: object info, like name, apiID, attempts, etc.
	 * Third level: reasons info.
	 */
	FFullMenuReasons ObjectReason = FullMenuReasons.Reasons[0];

	bIsClosed = ObjectReason.bIsClosed;
	ObjectAPI_Id = ObjectReason.API_Id;
	ObjectMenuElement_TextBlock_Name.Get()->SetText(FText::FromString(ObjectReason.Name));
	ObjectMenuElement_TextBlock.Get()->SetText(FText::FromString(FullMenuReasons.Name));

	CreateChildren(ObjectReason);
	SetWidgetViewportPosition();
	
}

void UCS_ObjectMenu::CreateChildren(const FFullMenuReasons& ObjectReason)
{
	ObjectMenu_VerticalBox->ClearChildren();
	bIsNotChosenDefect = true;
	
	for(const FFullMenuReasons& Reason : ObjectReason.Reasons)
	{
		if(UCS_ObjectMenuElement* NewElement = CreateWidget<UCS_ObjectMenuElement>(GetWorld(), ObjectMenuElementClass))
		{
			
			ObjectMenu_VerticalBox->AddChildToVerticalBox(NewElement);
			NewElement->SetPadding(FMargin(0.f, 1.f, 0.f, 1.f));
			
			NewElement->InitObjectMenuElement(ObjectReason.API_Id, Reason);
			NewElement->SetPlayerController(PlayerController.Get());
			NewElement->SetCloseObjectMenuFunction(CloseObjectMenuFunction);
			NewElement->SetClickOnObject(ClickOnElementFunction);

			if (Reason.bIsChecked)
			{
				bIsNotChosenDefect = false;
			}
			NewElement->bIsClosed = this->bIsClosed;
		}
	}
}

void UCS_ObjectMenu::SetWidgetViewportPosition()
{
	const FVector2D MenuSize = ObjectMenu_SizeBox->GetDesiredSize();
	if (MenuSize.IsNearlyZero())
		return;
	
	FIntVector2 ViewportSize = FIntVector2();
	FVector2D MousePosition = FVector2D();
	
	PlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);

	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(this);
	FVector2D ViewportSizeScaled = FVector2D(ViewportSize.X / DPIScale, ViewportSize.Y / DPIScale);
	FVector2D ViewportSizeScaledCenter = FVector2D(ViewportSizeScaled/2);

	FVector2D MousePositionScaled(MousePosition.X, MousePosition.Y);
	MousePositionScaled /= DPIScale;

	constexpr float CursorOffset = 6.f;

	FVector2D Position = ViewportSizeScaledCenter + FVector2D(CursorOffset, CursorOffset);

	const float SpaceBelow = ViewportSizeScaled.Y - MousePositionScaled.Y;
	const float SpaceAbove = MousePositionScaled.Y;

	if (SpaceBelow < MenuSize.Y && SpaceAbove >= MenuSize.Y)
	{
		Position.Y = ViewportSizeScaledCenter.Y - MenuSize.Y - CursorOffset;
	}
	Position.X = FMath::Clamp(Position.X, 0.f, ViewportSizeScaled.X - MenuSize.X);
	Position.Y = FMath::Clamp(Position.Y, 0.f, ViewportSizeScaled.Y - MenuSize.Y);

	if(UCanvasPanelSlot* ContextObjectSlot = Cast<UCanvasPanelSlot>(ObjectMenu_SizeBox->Slot))
	{
		ContextObjectSlot->SetAnchors(FAnchors(0.f, 0.f));
		ContextObjectSlot->SetAlignment(FVector2D(0.f, 0.f));
		ContextObjectSlot->SetPosition(Position);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
// Cause: Function used in delegate.
void UCS_ObjectMenu::OnReleasedEmptyPlace()
{
	if(CloseObjectMenuFunction)
	{
		CloseObjectMenuFunction();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ObjectMenu: CloseObjectMenuFunction wasn't set!"))
	}
}

void UCS_ObjectMenu::OnReleasedClearButton()
{
	if (!PlayerController.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectMenu: Player controller is invalid!"));
		return;
	}
	

	if (bIsClosed || bIsNotChosenDefect)
	{
		CloseObjectMenuFunction();
		return;
	}
	
	// "castedToStringAttribute":"reasons";
	// "reasons" = "{"place":"placeAPI_id", "object":"objectAPI_Id", "checked":[]}"
	const FString CastedToStringAttribute = FString::Printf(
		TEXT("{\"place\":\"%s\", \"object\": \"%s\", \"checked\":[]}"),
		*PlayerController->GetCS_PlayerState()->GetCurrentLocationAPI_Id(),
		*ObjectAPI_Id);
	
	if(ClickOnElementFunction)
	{
		ClickOnElementFunction(CastedToStringAttribute);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ObjectMenuElement: ClickOnElementFunction wasn't set! "
								"Click wouldn't call reaction in AOS!"))
	}
	
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
