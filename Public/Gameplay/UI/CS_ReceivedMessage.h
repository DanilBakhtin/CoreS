
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_ReceivedMessage.generated.h"

class UTextBlock;

UCLASS()
class CORE_SIMULATOR_API UCS_ReceivedMessage : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ReceivedMessage", meta = (BindWidget))
	TObjectPtr<UTextBlock> Message_TextBlock;

	void InitText(const FString& Text) const;
};
