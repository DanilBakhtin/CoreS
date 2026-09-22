// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_ShowQuest.generated.h"

class UCS_ShowScheme;
/**
 * 
 */
UCLASS()
class CORE_SIMULATOR_API UCS_ShowQuest : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	void InitHelpImages(TArray<TSoftObjectPtr<UTexture2D>> Textures) const;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowQuest", meta = (BindWidget))
	TObjectPtr<UCS_ShowScheme> ImageViewer;

	void SetCloseQuestFunc(TFunction<void()> Func);
	
private:

	void CloseShowQuest();

	TFunction<void()> CloseQuestFunc;
};
