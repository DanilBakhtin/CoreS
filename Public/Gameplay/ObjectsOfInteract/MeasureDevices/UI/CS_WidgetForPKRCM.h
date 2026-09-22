#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Framework/CS_CoreTypes.h>
#include "CS_WidgetForPKRCM.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;

UENUM(BlueprintType)
enum class EMeasurementState : uint8
{
	Idle,
	Measuring,
	Done
};

UCLASS()
class CORE_SIMULATOR_API UCS_WidgetForPKRCM : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) void SetMultimeterState(bool bOn);
	UFUNCTION(BlueprintCallable) void SetSwitchType(EPKCurrentType Type);
	UFUNCTION(BlueprintCallable) void SetSwitchVoltage(EPKCurrentVoltage Voltage);
	UFUNCTION(BlueprintCallable) void SetStartStopState(bool bMeasuring);
	UFUNCTION(BlueprintCallable) void SetMeasurementState(EMeasurementState State);
	UFUNCTION(BlueprintCallable) void SetDisplayVoltageText(const FText& MinValue, const FText& MaxValue);
	UFUNCTION(BlueprintCallable) void SetDisplayText(const FText& InValue, const FText& InText);

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* ImgMultimeter;

	UPROPERTY(meta = (BindWidget))
	UImage* ImgSwitchType;

	UPROPERTY(meta = (BindWidget))
	UImage* ImgSwitchVoltage;

	UPROPERTY(meta = (BindWidget))
	UImage* ImgBackGround;

	UPROPERTY(meta = (BindWidget))
	UImage* ImgStartStop;

	UPROPERTY(meta = (BindWidget))
	UImage* IconCurrentStatus;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TxtValue;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TxtDisplay;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TxtMinVoltage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TxtMaxVoltage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|BackGround")
	UTexture2D* Icon_BackGround_For_U_I_voltage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|BackGround")
	UTexture2D* Icon_BackGround_For_R_voltage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|OnOff")
	UTexture2D* Icon_Multimeter_On;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|OnOff")
	UTexture2D* Icon_Multimeter_Off;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|StopStart")
	UTexture2D* Icon_Start;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|StopStart")
	UTexture2D* Icon_Stop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|Type")
	UTexture2D* Icon_Switch_Direct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|Type")
	UTexture2D* Icon_Switch_Resistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|Type")
	UTexture2D* Icon_Switch_Alternate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|Voltage")
	UTexture2D* Icon_Switch_V;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|Voltage")
	UTexture2D* Icon_Switch_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|Voltage")
	UTexture2D* Icon_Switch_A;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|Status")
	UTexture2D* ImgReady;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|Status")
	UTexture2D* ImgMeasurement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PKRCM|Icons|Status")
	UTexture2D* ImgMeasurementDone;

	virtual void NativeConstruct() override;
};