#include "Gameplay/UI/CS_ShowReaction.h"
#include "Components/TextBlock.h"
#include "Framework/CS_UtilityFunctionLibrary.h"
#include "Components/Button.h"

void UCS_ShowReaction::NativeConstruct()
{
	Super::NativeConstruct();

	check(ShowReaction_TextBlock);

	check(OpenAnimation);
	{
		FWidgetAnimationDynamicEvent OpenFinishedEvent;
		OpenFinishedEvent.BindDynamic(this, &UCS_ShowReaction::OnOpenAnimationFinished);
		BindToAnimationFinished(OpenAnimation, OpenFinishedEvent);
	}

	check(CloseAnimation);
	{
		FWidgetAnimationDynamicEvent CloseFinishedEvent;
		CloseFinishedEvent.BindDynamic(this, &UCS_ShowReaction::OnCloseAnimationFinished);
		BindToAnimationFinished(CloseAnimation, CloseFinishedEvent);
	}

	check(ButtonClose)
		ButtonClose->OnClicked.AddDynamic(this, &UCS_ShowReaction::ManualCloseReaction);
}

void UCS_ShowReaction::NativeDestruct()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoCloseTimer);
	}
	Super::NativeDestruct();
}

void UCS_ShowReaction::InitShowReaction(const FString& Description, float _TimeLife)
{	
	TimeLife = _TimeLife;

	FString DescriptionText = UCS_UtilityFunctionLibrary::ReplaceSubstring(Description, "<br>", "\n");
	ShowReaction_TextBlock->SetText(FText::FromString(DescriptionText));

	PlayAnimation(OpenAnimation);
}

void UCS_ShowReaction::ManualCloseReaction()
{
	GetWorld()->GetTimerManager().ClearTimer(AutoCloseTimer);
	HideReaction();

}

void UCS_ShowReaction::CloseShowReaction()
{	
	SetVisibility(ESlateVisibility::Collapsed);
	RemoveFromParent();

	OnReactionRemoving.Broadcast(this);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Close reaction!")));

}

void UCS_ShowReaction::OnOpenAnimationFinished()
{
	if (GetWorld())
	{
		//GetWorld()->GetTimerManager().ClearTimer(AutoCloseTimer);

		TWeakObjectPtr<UCS_ShowReaction> WeakThis(this);

		GetWorld()->GetTimerManager().SetTimer(
			AutoCloseTimer,
			[WeakThis]()
			{
				if (WeakThis.IsValid())
				{
					WeakThis->HideReaction();
				}
			},
			TimeLife,
			false
		);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Timer is active!")));
	}
}

void UCS_ShowReaction::OnCloseAnimationFinished()
{
	CloseShowReaction();
}

void UCS_ShowReaction::HideReaction()
{
	PlayAnimation(CloseAnimation);
}