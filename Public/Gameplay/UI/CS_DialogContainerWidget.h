// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/UI/CS_UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include <Gameplay/Player/CS_HUD.h>
#include "CS_DialogContainerWidget.generated.h"

class UOverlay;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogClose);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnContactSelectParams, FDialogButtonParameters);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSendMessageParams, FDialogParameters);
DECLARE_MULTICAST_DELEGATE(FOnReturnToContacts);

UENUM(BlueprintType)
enum class EStateDialogVis : uint8
{
	Unvisible,
	Contact,
	Dialog
};

UCLASS()
class CORE_SIMULATOR_API UCS_DialogContainerWidget : public UCS_UserWidget
{
	GENERATED_BODY()

public:

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	bool ShowContactWidget(ETypeDialog TypeDialog, const TArray<FDialogButtonParameters>& Parameters);

	bool ShowDialogWidget(ETypeDialog TypeDialog, FString nameTitle, const TArray<FDialogParameters>& Params);

	bool AddMessage(const TArray<FDialogParameters>& Params);
	
	bool UpdateDialog(const TArray<FDialogParameters>& Params);

	void UpdateNestingList(const TArray<FString>& NameList);

	void UpdateDialogIcon(UTexture2D* IconDialog);

	void ClearNestingList();
	
	bool NeedSendCloseDialog();

	void ActivateBackButton(bool bActivate);

	UPROPERTY(BlueprintAssignable, Category = "CS_DialogContainerWidget")
	FOnDialogClose OnDialogClose;

	FOnContactSelectParams OnContactSelect;

	FOnSendMessageParams OnSendMessage;

	FOnReturnToContacts OnBackDialog;

	UFUNCTION()
	bool IsContactShowing();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_DialogContainerWidget")
	TMap<ETypeDialog, FDialogWidgetStructure> TempMapDialogClasses;	

	UPROPERTY(BlueprintReadOnly, Category = "CS_DialogContainerWidget", meta = (BindWidget))
	TObjectPtr<UOverlay> OverlayContainer;

	TMap<ETypeDialog, UCS_ContactSelectorWidget*> MapContactWidget;
	TMap<ETypeDialog, UCS_DialogWidget*> MapDialogWidget;

	UCS_ContactSelectorWidget* CurrentContactWidget = nullptr;
	UCS_DialogWidget* CurrentDialogWidget = nullptr;

	UPROPERTY()
	bool bActivateBackButton = false;

	void CreateDialogWidgets();

	UFUNCTION()
	void ContactSelect(FDialogButtonParameters Params);

	UFUNCTION()
	void SendMessage(FDialogParameters Params);

	UFUNCTION()
	void BackDialog();

	UFUNCTION()
	void DialogClose();

	UFUNCTION()
	void RemoveDelegatsFromWidgets();

	UPROPERTY()
	EStateDialogVis StateDialogVis;
};

template<typename WidgetMapT, typename CurrentWidgetT, typename OtherWidgetT>
bool ShowWidgetImpl(ETypeDialog TypeDialog, WidgetMapT& WidgetMap, CurrentWidgetT& CurrentWidget, OtherWidgetT& OtherWidget)
{
    if (WidgetMap.Contains(TypeDialog))
    {
        if (OtherWidget)
        {
            OtherWidget->SetVisibility(ESlateVisibility::Collapsed);
            OtherWidget = nullptr;
        }

        if (CurrentWidget)
            CurrentWidget->SetVisibility(ESlateVisibility::Collapsed);

        CurrentWidget = WidgetMap[TypeDialog];
        CurrentWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

        return true;
    }

    return false;
}
