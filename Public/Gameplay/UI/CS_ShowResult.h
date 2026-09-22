
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_ShowResult.generated.h"

class UBorder;
class USizeBox;
class UWrapBox;
class UTextBlock;
class UCS_ResultCategory;

class UCS_BaseButton;

UCLASS()
class CORE_SIMULATOR_API UCS_ShowResult : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ShowResult")
	TSubclassOf<UCS_ResultCategory> ResultCategoryClass;
	
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowResult", meta = (BindWidget))
	TObjectPtr<UTextBlock> Eval_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowResult", meta = (BindWidget))
	TObjectPtr<UTextBlock> EvalDescription_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowResult", meta = (BindWidget))
	TObjectPtr<UWrapBox> Category_WrapBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowResult", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton> Exit_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowResult", meta = (BindWidget))
	TObjectPtr<UTextBlock> Description_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowResult", meta = (BindWidget))
	TObjectPtr<UBorder> Description_Border;
	
	void Init(const FShowResultParams& ShowResultParams);
	void SetExitCourseFunc(TFunction<void()> Func);

	void ShowDescription(UCS_ResultCategory* ResultCategory);
protected:

	UFUNCTION()
	void Exit();

	UPROPERTY()
	TArray<UCS_ResultCategory*> CategoryButtons;
	
	TFunction<void()> ExitCourseFunc;	
};
