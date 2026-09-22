
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_Description_Mouse.generated.h"

class UCS_DescriptionObject;
class UCanvasPanel;
class ACS_PlayerController;

UCLASS()
class CORE_SIMULATOR_API UCS_Description_Mouse : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "CS_Description_Mouse", meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Description_CanvasPanel;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_Description_Mouse", meta = (BindWidget))
	TObjectPtr<UCS_DescriptionObject> DescriptionObjectWidget;

	void SetPlayerController(ACS_PlayerController* PlayerController);
	void UpdateDescriptionPosition() const;

	void ShowTargetDescription(const FString& ObjectId, const FString& Description);
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:

	FString CurrentDescriptionObjectId = "";
	
	UPROPERTY()
	TWeakObjectPtr<ACS_PlayerController> PlayerController;
};
