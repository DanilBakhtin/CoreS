
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_SendedMessage.generated.h"

class UTextBlock;

UCLASS()
class CORE_SIMULATOR_API UCS_SendedMessage : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "CS_SendedMessage", meta = (BindWidget))
	TObjectPtr<UTextBlock> Message_TextBlock;

	void InitText(const FString& Text) const;
};
