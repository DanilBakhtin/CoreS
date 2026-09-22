#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "CS_BaseButton.generated.h"

class UCS_LocalizableTextBlock;

UCLASS()
class CORE_SIMULATOR_API UCS_BaseButton : public UButton
{
	GENERATED_BODY()

public:

	UCS_BaseButton();

protected:

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;

	UFUNCTION()
	virtual void HandleOnHovered();
	UFUNCTION()
	virtual void HandleOnUnhovered();

	UPROPERTY(Transient)
	bool bIsHoveredInternal;

	UPROPERTY()
	UCS_LocalizableTextBlock* ButtonText;

	UPROPERTY(EditAnywhere, Category = "CS_Button_Text")
	FSlateColor NormalColorText;

	UPROPERTY(EditAnywhere, Category = "CS_Button_Text")
	FSlateColor HoverColorText;

	UPROPERTY(EditAnywhere, Category = "CS_Button_Text")
	FText PreviewText;

	UPROPERTY(EditAnywhere, Category = "CS_Button_Text")
	FString LocalizationKey;

	UPROPERTY(EditAnywhere, Category = "CS_Button_Text")
	FSlateFontInfo TextStyle;

	UPROPERTY(EditAnywhere, Category = "CS_Button_Text")
	FMargin TextPadding;
};
