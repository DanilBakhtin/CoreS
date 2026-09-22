// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CS_ShowScheme.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_ShowWelcome.generated.h"

class UWidgetSwitcher;
class UBorder;
class UCS_ButtonWidget;
class UButton;
class UTextBlock;

class UCS_BaseButton;

UCLASS()
class CORE_SIMULATOR_API UCS_ShowWelcome : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	void InitParams(const FShowWelcomeParams& Params) const;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UTextBlock> ShowWelcomeTitle_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UTextBlock> ShowWelcomeDescription_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UTextBlock> FaultInfoTitle_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UTextBlock> FaultInfoDescription_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UTextBlock> FaultInfoQuest_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton> ExitOne_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton> ExitTwo_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton> Next_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton> Start_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton> Guide_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> ShowWelcome_WidgetSwitcher;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UCS_ShowScheme> ImageViewer;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UBorder> MainBorder;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowWelcome", meta = (BindWidget))
	TObjectPtr<UUserWidget> BlurBG;
	
	void SetStartCourseFunc(TFunction<void()> Func);
	void SetExitCourseFunc(TFunction<void()> Func);

	void InitImages(TArray<TSoftObjectPtr<UTexture2D>> Textures) const;

protected:
	
	UFUNCTION()
	void NextIndex();
	UFUNCTION()
	void StartCourse();
	UFUNCTION()
	void Exit();
	UFUNCTION()
	void Guide();
	UFUNCTION()
	void CloseScheme();
	
	TFunction<void()> StartCourseFunc;
	TFunction<void()> ExitCourseFunc;

private:

	UPROPERTY()
	UTexture2D* MainTexture;

	UPROPERTY(EditAnywhere)
	FString PathToMainImage = "Images/ShowWelcome/ShowWelcome.png";

	UFUNCTION()
	void InitBackground();

};
