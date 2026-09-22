#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_Timer.h"
#include "CS_Steps.h"
#include "CS_DescriptionObject.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_TimeAndLocation.generated.h"

class UPanelWidget;
class UTextBlock;
class UCS_Timer;
class UCS_Steps;
class UCS_AWP_Widget;
class UCS_ZoomInfo;

UCLASS()
class CORE_SIMULATOR_API UCS_TimeAndLocation : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "CS_TimeAndLocation", meta = (BindWidget))
	TObjectPtr<USizeBox> LocationName_SizeBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_TimeAndLocation", meta = (BindWidget))
	TObjectPtr<UTextBlock> LocationName_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_TimeAndLocation", meta = (BindWidget))
	TObjectPtr<UCS_Timer> TopTimer;
	UPROPERTY(BlueprintReadOnly, Category = "CS_TimeAndLocation", meta = (BindWidget))
	TObjectPtr<UCS_Steps> TopSteps;
	UPROPERTY(BlueprintReadOnly, Category = "CS_TimeAndLocation", meta = (BindWidget))
	TObjectPtr<UCS_Timer> DownTimer;
	UPROPERTY(BlueprintReadOnly, Category = "CS_TimeAndLocation", meta = (BindWidget))
	TObjectPtr<UCS_Steps> DownSteps;
	UPROPERTY(BlueprintReadOnly, Category = "CS_TimeAndLocation", meta = (BindWidget))
	TObjectPtr<UPanelWidget> Top_TimerSteps;
	UPROPERTY(BlueprintReadOnly, Category = "CS_TimeAndLocation", meta = (BindWidget))
	TObjectPtr<UPanelWidget> Down_TimerSteps;
	UPROPERTY(BlueprintReadOnly, Category = "CS_TimeAndLocation", meta = (BindWidget))
	TObjectPtr<UCS_ZoomInfo> ZoomInfo;

	void SetTime(const FDateTime& DateTo) const;
	void SetSteps(const int CurrentSteps) const;
	void SetLocationName(const FString& LocationName) const;
	void SetTimerParameters(const FShowWelcomeParams& ShowWelcomeParams);
	void SetStepsParameters(const FShowWelcomeParams& ShowWelcomeParams);
	
	void ShowVisibleDownTime(bool Show) const;
	void SetLocationVisibility(const ESlateVisibility NewVisibility);

	UFUNCTION()
	void UpdateZoomInfo(const int Percent);
};
