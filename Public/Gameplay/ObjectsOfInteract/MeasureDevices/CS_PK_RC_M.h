#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Framework/CS_MeasureDevices_Actor.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Sound/SoundBase.h"
#include "CS_PK_RC_M.generated.h"


class UStaticMeshComponent;
class UTextRenderComponent;
class UInputAction;
class UCS_WidgetForPKRCM;
struct FInputActionInstance;

// Универсальная функция для циклического переключения значений перечисления.
template<typename TEnum, typename Pred>
static TEnum GetNextAllowed(
	const TArray<TEnum>& Sequence,
	TEnum                  Current,
	Pred                   IsAllowedPredicate
)
{
	int32 CurrentIndex = Sequence.IndexOfByKey(Current);
	int32 Count = Sequence.Num();
	if (CurrentIndex == INDEX_NONE) CurrentIndex = 0;

	for (int32 Offset = 1; Offset < Count; ++Offset)
	{
		int32 NextIndex = (CurrentIndex + Offset) % Count;
		TEnum Candidate = Sequence[NextIndex];
		if (IsAllowedPredicate(Candidate)) return Candidate;
	}
	return Current;
}

USTRUCT()
struct FPKDipstick
{
	GENERATED_BODY()

	FString PointId = "";
	FTransform Dipstick_PointPosition;
	bool UseDipstickMesh = false;
};

UCLASS()
class CORE_SIMULATOR_API ACS_PK_RC_M : public ACS_MeasureDevices_Actor
{
	GENERATED_BODY()

public:
	ACS_PK_RC_M();

	// Настройки

	//Для виджета дисплея
	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings")
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "PK_RC_M|Settings")
	TObjectPtr<UWidgetComponent> WidgetComponent;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings")
	EPKCurrentType DefaultType;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings")
	EPKCurrentVoltage DefaultVoltage;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings")
	float MeasurementDelay = 5.0f;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings")
	USoundBase* Sound_MeasureStart;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings")
	USoundBase* Sound_MeasureEnd;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings|State")
	bool ActivateOnStartup;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings|State")
	bool StaticVoltage;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings|State|Type")
	bool DirectType;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings|State|Type")
	bool AlternateType;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings|State|Type")
	bool ResistanceType;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings|State|Voltage")
	bool Type_A;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings|State|Voltage")
	bool Type_R;

	UPROPERTY(EditAnywhere, Category = "PK_RC_M|Settings|State|Voltage")
	bool Type_V;

	// Компоненты
	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<UStaticMeshComponent> Base_Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<USceneComponent> Multimeter_Component;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<UStaticMeshComponent> Multimeter_Button;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<USceneComponent> SwitchType_Component;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<UStaticMeshComponent> SwitchType_Button;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<USceneComponent> SwitchVoltage_Component;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<UStaticMeshComponent> SwitchVoltage_Button;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<USceneComponent> StartStop_Component;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<UStaticMeshComponent> StartStop_Button;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<USceneComponent> BlackDipstick_Component;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<UStaticMeshComponent> BlackDipstick_Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<USceneComponent> RedDipstick_Component;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<UStaticMeshComponent> RedDipstick_Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "PK_RC_M")
	TObjectPtr<USceneComponent> TargetPositionForRVoltage_Component;

	UPROPERTY()
	UCS_WidgetForPKRCM* WidgetInstance;

	// Методы
	virtual void SetPlayerController(ACS_PlayerController* PlayerController) override;

protected:
	virtual void BeginPlay() override;
	virtual void AttachMeasureDeviceToPrev(UActorComponent* PrevComponent) override;
	virtual void ClickOnMeasurePoint(const FString& InteractID, const FTransform& PositionMeasure) override;
	virtual void UpdateMeasureDevice(const float& Result) override;
	virtual void ResetDevice() override;

	void Initialization();
	void ShowValueOnDisplay(float Result);
	void ShowValueVoltageDisplay(float MinVoltage, float MaxVoltage);
	void RemovesAllDelegates();
	void Internal_OnMeasurementFinished();
	void FinishMeasurement();
	void StartMeasurement();
	void StopMeasurement();
	void SwitchVoltage();
	FString MakeParams(float Current = 0.f, bool UseFloat = false);

	// Обработчики кнопок
	UFUNCTION()
	void Multimeter_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Multimeter_EndMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Multimeter_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	bool IsMultimeter_On;
	UFUNCTION()
	void SwitchType_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void SwitchType_EndMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void SwitchType_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	UFUNCTION()
	void SwitchVoltage_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void SwitchVoltage_EndMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void SwitchVoltage_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	UFUNCTION()
	void StartStop_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void StartStop_EndMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void StartStop_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	// Щупы
	FPKDipstick BlackDipstick;
	FPKDipstick RedDipstick;

	// Текущее состояние
	EPKCurrentType CurrentType;
	EPKCurrentVoltage CurrentVoltage;

	//Таймер
	FTimerHandle TimerHandle_FinishMeasure;

	// Пределы и константы
	inline static constexpr float MinMilliVolt_V = 0.001f;
	inline static constexpr float MaxMilliVolt_V = 0.999f;
	inline static constexpr float MinVolt = 1.0f;
	inline static constexpr float MaxVolt = 220.0f;

	inline static constexpr float MinMilliAmper = 0.001f;
	inline static constexpr float MaxMilliAmper = 0.999f;
	inline static constexpr float MinAmper = 1.0f;
	inline static constexpr float MaxAmper = 100.0f;

	inline static constexpr float MinMiliOm = 1.0f;
	inline static constexpr float MaxMiliOm = 999.0f;
	inline static constexpr float MinOm = 1000.0f;
	inline static constexpr float MaxOm = 50000.0f;
	//------

	inline static constexpr const TCHAR* UnitMilliVolt = TEXT("mB");
	inline static constexpr const TCHAR* UnitVolt = TEXT("B");

	inline static constexpr const TCHAR* UnitMilliAmper = TEXT("mA");
	inline static constexpr const TCHAR* UnitAmper = TEXT("A");

	inline static constexpr const TCHAR* UnitMilliOm = TEXT("Om");
	inline static constexpr const TCHAR* UnitOm = TEXT("kOm");

	static constexpr const TCHAR* kCURRENT_AC_L = TEXT("acl");
	static constexpr const TCHAR* kCURRENT_DC_L = TEXT("dcl");
	static constexpr const TCHAR* kVOLTAGE_AC_U = TEXT("acU");
	static constexpr const TCHAR* kVOLTAGE_DC_U = TEXT("dcU");
	static constexpr const TCHAR* kVOLTAGE_OH_M = TEXT("ohm");

	// Для результата измерения
	float ResultValue;

	float CurrentMinMiliValue;
	float CurrentMaxMiliValue;
	float CurrentMinValue;
	float CurrentMaxValue;

	FString CurrentMiliValueText;
	FString CurrentValueText;

	//Для процесса измерения
	bool bMeasuring = false;

public:
	virtual void Tick(float DeltaTime) override;
};

