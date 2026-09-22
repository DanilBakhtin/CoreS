
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CS_SelectAnswer.generated.h"

class UWidgetSwitcher;
class UCheckBox;
class UTextBlock;
class UCS_ListViewItem;
class UCS_MainMenu;

UCLASS()
class CORE_SIMULATOR_API UCS_SelectAnswer : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

		
public:

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "CS_SelectAnswer", meta = (BindWidget))
	TObjectPtr<UTextBlock> Answer_TextBlock;

	UPROPERTY(BlueprintReadOnly, Category = "CS_SelectAnswer", meta = (BindWidget))
	TObjectPtr<UCheckBox> Answer_UCheckBox;

	UPROPERTY(BlueprintReadOnly, Category = "CS_SelectAnswer", meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Answer_WidgetSwitcher;

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;

	UPROPERTY()
	TObjectPtr<UCS_ListViewItem> ListViewItem;
	UPROPERTY()
	TObjectPtr<UCS_MainMenu> MainMenuWidget;

	UFUNCTION()
	void CheckItem(const FString& apiId);
	
	FString CategoryID = "";
	FString AnswerId = "";
	FString CheckedAnswerId = "";
	UFUNCTION()
	void Collapse();
	UFUNCTION()
	void UpdateItem();

	UFUNCTION()
	void Check(bool Check);
	void Uncheck() const;
	
};
