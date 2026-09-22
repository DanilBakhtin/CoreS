
#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Framework/CS_MeasureDevices_Actor.h"
#include "GameFramework/Actor.h"
#include "CS_C43101.generated.h"

class ACS_Interactable_Actor;
class UStaticMeshComponent;
class UBoxComponent;
class UInputAction;
struct FInputActionValue;
struct FInputActionInstance;

UENUM(BlueprintType)
enum class EVoltage : uint8
{
	Direct,
	Resistance,
	Alternate 
};

USTRUCT()
struct FDipstick
{
	GENERATED_BODY()
	
	FString PointId = "";
	FTransform Dipstick_PointPosition;
	bool UseDipstickMesh = false;

};

UCLASS()
class CORE_SIMULATOR_API ACS_C43101 : public ACS_MeasureDevices_Actor
{
	GENERATED_BODY()
	
public:	

	ACS_C43101();

	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<UStaticMeshComponent> Base_Mesh;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<USceneComponent> Power_Component;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<UStaticMeshComponent> Power_Button;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<USceneComponent> Direct_Component;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<UStaticMeshComponent> Direct_Button;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<USceneComponent> Resistance_Component;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<UStaticMeshComponent> Resistance_Button;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<USceneComponent> Alternate_Component;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<UStaticMeshComponent> Alternate_Button;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<USceneComponent>  Switcher_Component;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<UStaticMeshComponent> Switcher_Button;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<USceneComponent>  Arrow_Component;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<UStaticMeshComponent> Arrow_Button;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<UBoxComponent> Switcher_R_BoxComponent;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<UBoxComponent> Switcher_L_BoxComponent;

	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<USceneComponent>  BlackDipstick_Component;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<UStaticMeshComponent> BlackDipstick_Mesh;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<USceneComponent> RedDipstick_Component;
	UPROPERTY(VisibleDefaultsOnly, Category="CS_C43101")
	TObjectPtr<UStaticMeshComponent> RedDipstick_Mesh;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CS_C43101", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MouseAction;

	virtual void SetPlayerController(ACS_PlayerController* PlayerController) override;
protected:

	virtual void BeginPlay() override;

	virtual void AttachMeasureDeviceToPrev(UActorComponent* PrevComponent) override;	
	virtual  void ClickOnMeasurePoint(const FString& InteractID, const FTransform& PositionMeasure) override;
	virtual void UpdateMeasureDevice(const float& Result) override;
	virtual void ResetDevice() override;
	
	UFUNCTION()
	void Power_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Power_EndMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Power_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	bool IsPower_Click = false;
	
	UFUNCTION()
	void Direct_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Direct_EndMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Direct_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	bool IsDirect_Click = false;
	
	UFUNCTION()
	void Resistance_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Resistance_EndMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Resistance_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	bool IsResistance_Click = false;
	
	UFUNCTION()
	void Alternate_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Alternate_EndMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Alternate_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	bool IsAlternate_Click = false;

	UFUNCTION()
	void Switcher_R_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Switcher_R_EndMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Switcher_R_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UFUNCTION()
	void Switcher_L_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Switcher_L_EndMouseOverLap(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void Switcher_L_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	bool IsSwitcher_Button_OverLap= false;
	float Switcher_Button_Rotate = 0.f; 
	void OnMouseWheel(const FInputActionInstance& Instance);
	
	FDipstick BlackDipstick;
	FDipstick RedDipstick;

	void RotateSwitch(float Current);
	float CurrentRotateSwitch = 0.f;
	
	FString MakeParams(float Current = 0.f, bool UseFloat = false);
	void SelectCurrentType(const FString& CurrentType);
	FString GetMeasureType() const;
	FString GetQuality(const float& Current);
	void GetVoltage(float value);
	void SetArrowRotation(const float& RotationAngle);
	void Fuse();
	FString CurrentType = "";
	FString Voltage = "";
	float MaxValue = 0.f;

	float CurrentArrowRotation = 0.f;
	float NewArrowRotation = 0.f;
	float OldArrowRotation = 0.f;
	
	FTimerHandle QualityTimerHandle;
	UPROPERTY()
	UTimelineComponent* Timeline;
	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* FloatCurve;
	FOnTimelineFloat InterpFunction{};
	FOnTimelineEvent TimelineFinished{};
	UFUNCTION()
	void TimelineUpdate(float Value);
	UFUNCTION()
	void TimelineFinishedCallback();

	void RemovesAllDelegates();
	
public:
		
	virtual void Tick(float DeltaTime) override;
	
};
