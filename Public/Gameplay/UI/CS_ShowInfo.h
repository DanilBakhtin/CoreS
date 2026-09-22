// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_ShowInfo.generated.h"

class UTextBlock;

UCLASS()
class CORE_SIMULATOR_API UCS_ShowInfo : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowInfo", meta = (BindWidget))
	TObjectPtr<UTextBlock> Info_TextBlock;

	void Init(const FShowWelcomeParams& Params) const;
};
