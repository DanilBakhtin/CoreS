
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_ShowReaction.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReactionRemoving, UCS_ShowReaction*, Reference);

UCLASS()
class CORE_SIMULATOR_API UCS_ShowReaction : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UPROPERTY()
	float TimeLife = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowReaction", meta = (BindWidget))
	TObjectPtr<UButton> ButtonClose;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowReaction", meta = (BindWidget))
	TObjectPtr<UTextBlock> ShowReaction_TextBlock;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* OpenAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* CloseAnimation;

	void InitShowReaction(const FString& Description, float _TimeLife = 3.0f);
	
	UPROPERTY()
	FOnReactionRemoving OnReactionRemoving;


protected:

	UFUNCTION()
	void ManualCloseReaction();

	UFUNCTION()
	void CloseShowReaction();
	
	UPROPERTY()
	FTimerHandle AutoCloseTimer;

	UPROPERTY()
	bool bIsShowing = false;

	UPROPERTY()
	bool bIsClosing = false;

	UPROPERTY()
	bool bHasPendingReaction = false;

	UPROPERTY()
	FString PendingDescription;

	UFUNCTION()
	void OnOpenAnimationFinished();

	UFUNCTION()
	void OnCloseAnimationFinished();

	UFUNCTION()
	void HideReaction();

	UPROPERTY()
	FString CashDescription = "";
};
