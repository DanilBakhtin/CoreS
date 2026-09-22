#include <Gameplay/ObjectsOfInteract/MeasureDevices/CS_PK_RC_M.h>

#include "EnhancedInputComponent.h"
#include "Components/BoxComponent.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_Interact_Component_Interface.h"
#include "Gameplay/ObjectsOfInteract/Components/CS_MeasurePoint.h"
#include "Gameplay/ObjectsOfInteract/MeasureDevices/CS_C43101_Prev_StaticMeshComp.h"
#include <Gameplay/ObjectsOfInteract/MeasureDevices/UI/CS_WidgetForPKRCM.h>
#include "Gameplay/Player/CS_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


ACS_PK_RC_M::ACS_PK_RC_M()
{
	PrimaryActorTick.bCanEverTick = true;

	Base_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base_Mesh"));
	RootComponent = Base_Mesh;

	Multimeter_Component = CreateDefaultSubobject<USceneComponent>(TEXT("Multimeter_Component"));
	Multimeter_Component->SetupAttachment(Base_Mesh);
	Multimeter_Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Multimeter_Button"));
	Multimeter_Button->SetupAttachment(Multimeter_Component);

	SwitchType_Component = CreateDefaultSubobject<USceneComponent>(TEXT("SwitchType_Component"));
	SwitchType_Component->SetupAttachment(Base_Mesh);
	SwitchType_Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchType_Button"));
	SwitchType_Button->SetupAttachment(SwitchType_Component);

	SwitchVoltage_Component = CreateDefaultSubobject<USceneComponent>(TEXT("SwitchVoltage_Component"));
	SwitchVoltage_Component->SetupAttachment(Base_Mesh);
	SwitchVoltage_Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchVoltage_Button"));
	SwitchVoltage_Button->SetupAttachment(SwitchVoltage_Component);

	StartStop_Component = CreateDefaultSubobject<USceneComponent>(TEXT("StartStop_Component"));
	StartStop_Component->SetupAttachment(Base_Mesh);
	StartStop_Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StartStop_Button"));
	StartStop_Button->SetupAttachment(StartStop_Component);

	BlackDipstick_Component = CreateDefaultSubobject<USceneComponent>(TEXT("BlackDipstick_Component"));
	BlackDipstick_Component->SetupAttachment(Base_Mesh);
	BlackDipstick_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlackDipstick_Mesh"));
	BlackDipstick_Mesh->SetupAttachment(BlackDipstick_Component);

	RedDipstick_Component = CreateDefaultSubobject<USceneComponent>(TEXT("RedDipstick_Component"));
	RedDipstick_Component->SetupAttachment(Base_Mesh);
	RedDipstick_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RedDipstick_Mesh"));
	RedDipstick_Mesh->SetupAttachment(RedDipstick_Component);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PK_RC_Widget"));
	WidgetComponent->SetupAttachment(Base_Mesh);
}

void ACS_PK_RC_M::SetPlayerController(ACS_PlayerController* _PlayerController)
{
	Super::SetPlayerController(_PlayerController);
}

void ACS_PK_RC_M::BeginPlay()
{
	Super::BeginPlay();

	RemovesAllDelegates();
	Initialization();
}

void ACS_PK_RC_M::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BlackDipstick.UseDipstickMesh)
		BlackDipstick_Mesh->SetWorldTransform(BlackDipstick.Dipstick_PointPosition);
	else
		BlackDipstick_Mesh->SetWorldTransform(BlackDipstick_Component->GetComponentTransform());

	if (RedDipstick.UseDipstickMesh)
		RedDipstick_Mesh->SetWorldTransform(RedDipstick.Dipstick_PointPosition);
	else
		RedDipstick_Mesh->SetWorldTransform(RedDipstick_Component->GetComponentTransform());


	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("bMeasur: %f"), bMeasuring ? 1.0f : 0.0f));
}

void ACS_PK_RC_M::Initialization()
{
	// Multimeter button
	if (Multimeter_Button)
	{
		Multimeter_Button->SetCollisionProfileName(FName("InteractObject"), true);
		Multimeter_Button->OnBeginCursorOver.AddDynamic(this, &ACS_PK_RC_M::Multimeter_BeginMouseOverLap);
		Multimeter_Button->OnEndCursorOver.AddDynamic(this, &ACS_PK_RC_M::Multimeter_EndMouseOverLap);
		Multimeter_Button->OnClicked.AddDynamic(this, &ACS_PK_RC_M::Multimeter_OnMouseClicked);
	}

	// SwitchType button
	if (SwitchType_Button)
	{
		SwitchType_Button->SetCollisionProfileName(FName("InteractObject"), true);
		SwitchType_Button->OnBeginCursorOver.AddDynamic(this, &ACS_PK_RC_M::SwitchType_BeginMouseOverLap);
		SwitchType_Button->OnEndCursorOver.AddDynamic(this, &ACS_PK_RC_M::SwitchType_EndMouseOverLap);
		SwitchType_Button->OnClicked.AddDynamic(this, &ACS_PK_RC_M::SwitchType_OnMouseClicked);
	}

	// SwitchVoltage button
	if (SwitchVoltage_Button)
	{
		SwitchVoltage_Button->SetCollisionProfileName(FName("InteractObject"), true);
		SwitchVoltage_Button->OnBeginCursorOver.AddDynamic(this, &ACS_PK_RC_M::SwitchVoltage_BeginMouseOverLap);
		SwitchVoltage_Button->OnEndCursorOver.AddDynamic(this, &ACS_PK_RC_M::SwitchVoltage_EndMouseOverLap);
		SwitchVoltage_Button->OnClicked.AddDynamic(this, &ACS_PK_RC_M::SwitchVoltage_OnMouseClicked);
	}

	// StartStop button
	if (StartStop_Button)
	{
		StartStop_Button->SetCollisionProfileName(FName("InteractObject"), true);
		StartStop_Button->OnBeginCursorOver.AddDynamic(this, &ACS_PK_RC_M::StartStop_BeginMouseOverLap);
		StartStop_Button->OnEndCursorOver.AddDynamic(this, &ACS_PK_RC_M::StartStop_EndMouseOverLap);
		StartStop_Button->OnClicked.AddDynamic(this, &ACS_PK_RC_M::StartStop_OnMouseClicked);
	}

	if (WidgetComponent)
	{
		WidgetInstance = Cast<UCS_WidgetForPKRCM>(WidgetComponent->GetUserWidgetObject());
	}

	// Изначальное состояние
	IsMultimeter_On = ActivateOnStartup;
	CurrentType = DefaultType;

	CurrentVoltage = DefaultVoltage;
	SwitchVoltage();

	bMeasuring = false;
	ResultValue = 0.f;
	SetActorTickEnabled(true);

	if (WidgetInstance)
	{
		WidgetInstance->SetMultimeterState(IsMultimeter_On);
		WidgetInstance->SetSwitchType(CurrentType);
		WidgetInstance->SetSwitchVoltage(CurrentVoltage);
		WidgetInstance->SetStartStopState(bMeasuring);
		WidgetInstance->SetMeasurementState(EMeasurementState::Idle);

		ShowValueOnDisplay(0);
	}
}

// Поведение кнопок
void ACS_PK_RC_M::Multimeter_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	if (Multimeter_Button && !ActivateOnStartup) Multimeter_Button->SetRenderCustomDepth(true);
}

void ACS_PK_RC_M::Multimeter_EndMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	if (Multimeter_Button && !ActivateOnStartup) Multimeter_Button->SetRenderCustomDepth(false);
}

void ACS_PK_RC_M::SwitchType_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	if (SwitchType_Button && CurrentVoltage != EPKCurrentVoltage::R) SwitchType_Button->SetRenderCustomDepth(true);
}

void ACS_PK_RC_M::SwitchType_EndMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	if (SwitchType_Button && CurrentVoltage != EPKCurrentVoltage::R) SwitchType_Button->SetRenderCustomDepth(false);
}

void ACS_PK_RC_M::SwitchVoltage_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	if (SwitchVoltage_Button && !StaticVoltage) SwitchVoltage_Button->SetRenderCustomDepth(true);
}

void ACS_PK_RC_M::SwitchVoltage_EndMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	if (SwitchVoltage_Button && !StaticVoltage) SwitchVoltage_Button->SetRenderCustomDepth(false);
}

void ACS_PK_RC_M::StartStop_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	if (StartStop_Button) StartStop_Button->SetRenderCustomDepth(true);
}

void ACS_PK_RC_M::StartStop_EndMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	if (StartStop_Button) StartStop_Button->SetRenderCustomDepth(false);
}

void ACS_PK_RC_M::StartStop_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (IsMultimeter_On)
	{
		ShowValueOnDisplay(0.f);

		if (bMeasuring)
		{
			StopMeasurement();
			return;
		}

		StartMeasurement();
	}

	PlayerController->OnMeasureActionEvent(MakeParams());
}

void ACS_PK_RC_M::SwitchType_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (CurrentVoltage == EPKCurrentVoltage::R) return;

	if (bMeasuring) StopMeasurement();

	ShowValueOnDisplay(0);

	static const TArray<EPKCurrentType> TypeSequence = {
		   EPKCurrentType::Direct,
		   EPKCurrentType::Resistance,
		   EPKCurrentType::Alternate
	};

	auto IsTypeAllowed = [&](EPKCurrentType T) {
		switch (T)
		{
		case EPKCurrentType::Direct:     return DirectType;
		case EPKCurrentType::Resistance: return ResistanceType;
		case EPKCurrentType::Alternate:  return AlternateType;
		}
		return false;
		};

	CurrentType = GetNextAllowed(TypeSequence, CurrentType, IsTypeAllowed);
	if (WidgetInstance)
	{
		WidgetInstance->SetSwitchType(CurrentType);
		WidgetInstance->SetMeasurementState(EMeasurementState::Idle);
	}
}

void ACS_PK_RC_M::SwitchVoltage_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	static const TArray<EPKCurrentVoltage> VoltageSequence = {
		  EPKCurrentVoltage::A,
		  EPKCurrentVoltage::R,
		  EPKCurrentVoltage::V
	};

	auto IsVoltAllowed = [&](EPKCurrentVoltage V) {
		switch (V)
		{
		case EPKCurrentVoltage::A:
			return Type_A;
		case EPKCurrentVoltage::R:
			return Type_R;
		case EPKCurrentVoltage::V:
			return Type_V;
		}
		return false;
		};

	CurrentVoltage = GetNextAllowed(VoltageSequence, CurrentVoltage, IsVoltAllowed);
	if (bMeasuring) StopMeasurement();
	SwitchVoltage();
}

void ACS_PK_RC_M::Multimeter_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (!ActivateOnStartup)
	{
		IsMultimeter_On = !IsMultimeter_On;
		bMeasuring = false;

		if (WidgetInstance) WidgetInstance->SetMultimeterState(IsMultimeter_On);
	}
}

void ACS_PK_RC_M::ClickOnMeasurePoint(const FString& InteractID, const FTransform& PositionMeasure)
{
	//if (bMeasuring) StopMeasurement();
	//StopMeasurement();

	if (BlackDipstick.PointId == "" && RedDipstick.PointId != InteractID)
	{
		BlackDipstick.PointId = InteractID;
		BlackDipstick.Dipstick_PointPosition = PositionMeasure;
		BlackDipstick.UseDipstickMesh = true;
		PlayerController->OnMeasureActionEvent(MakeParams());
		return;
	}
	if (BlackDipstick.PointId != "" && RedDipstick.PointId == "" && BlackDipstick.PointId != InteractID)
	{
		RedDipstick.PointId = InteractID;
		RedDipstick.Dipstick_PointPosition = PositionMeasure;
		RedDipstick.UseDipstickMesh = true;
		PlayerController->OnMeasureActionEvent(MakeParams());
		return;
	}
	if (BlackDipstick.PointId == InteractID)
	{
		BlackDipstick.PointId = "";
		BlackDipstick.UseDipstickMesh = false;
		PlayerController->OnMeasureActionEvent(MakeParams());
		return;
	}
	if (RedDipstick.PointId == InteractID)
	{
		RedDipstick.PointId = "";
		RedDipstick.UseDipstickMesh = false;
		PlayerController->OnMeasureActionEvent(MakeParams());
		return;
	}
	if (BlackDipstick.PointId != "" && BlackDipstick.PointId != InteractID && RedDipstick.PointId != "" && RedDipstick.PointId != InteractID)
	{
		RedDipstick.PointId = InteractID;
		RedDipstick.Dipstick_PointPosition = PositionMeasure;
		RedDipstick.UseDipstickMesh = true;
		PlayerController->OnMeasureActionEvent(MakeParams());
	}
}

//Основные логические методы прибора
void ACS_PK_RC_M::UpdateMeasureDevice(const float& Result)
{
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("Value: %f"), Result));
	ResultValue = Result;
	FinishMeasurement();
}

void ACS_PK_RC_M::SwitchVoltage()
{
	switch (CurrentVoltage)
	{
	case EPKCurrentVoltage::A:
		CurrentMinMiliValue = MinMilliAmper;
		CurrentMaxMiliValue = MaxMilliAmper;
		CurrentMinValue = MinAmper;
		CurrentMaxValue = MaxAmper;
		CurrentMiliValueText = UnitMilliAmper;
		CurrentValueText = UnitAmper;
		break;
	case EPKCurrentVoltage::R:
		CurrentMinMiliValue = MinMiliOm;
		CurrentMaxMiliValue = MaxMiliOm;
		CurrentMinValue = MinOm;
		CurrentMaxValue = MaxOm;
		CurrentMiliValueText = UnitMilliOm;
		CurrentValueText = UnitOm;
		break;
	case EPKCurrentVoltage::V:
		CurrentMinMiliValue = MinMilliVolt_V;
		CurrentMaxMiliValue = MaxMilliVolt_V;
		CurrentMinValue = MinVolt;
		CurrentMaxValue = MaxVolt;
		CurrentMiliValueText = UnitMilliVolt;
		CurrentValueText = UnitVolt;
		break;
	}

	if (WidgetInstance) WidgetInstance->SetSwitchVoltage(CurrentVoltage);
	ShowValueVoltageDisplay(CurrentMinMiliValue, CurrentMaxValue);
}

FString ACS_PK_RC_M::MakeParams(float Current, bool UseFloat)
{
	FString TypeStr = TEXT("");

	if (CurrentType == EPKCurrentType::Direct && CurrentVoltage == EPKCurrentVoltage::A)
	{
		TypeStr = kCURRENT_DC_L;
	}
	if (CurrentType == EPKCurrentType::Alternate && CurrentVoltage == EPKCurrentVoltage::A)
	{
		TypeStr = kCURRENT_AC_L;
	}
	if (CurrentType == EPKCurrentType::Alternate && CurrentVoltage == EPKCurrentVoltage::V)
	{
		TypeStr = kVOLTAGE_AC_U;
	}
	if (CurrentType == EPKCurrentType::Direct && CurrentVoltage == EPKCurrentVoltage::V)
	{
		TypeStr = kVOLTAGE_DC_U;
	}
	if (CurrentType == EPKCurrentType::Resistance && CurrentVoltage == EPKCurrentVoltage::R)
	{
		TypeStr = kVOLTAGE_OH_M;
	}
	if (!bMeasuring || !BlackDipstick.UseDipstickMesh || !RedDipstick.UseDipstickMesh)
	{
		TypeStr = "";
	}


	FString Params = "";
	Params = IdDevice + ":" + TypeStr + ":" + BlackDipstick.PointId + ":" + RedDipstick.PointId + ":";

	if (UseFloat && !TypeStr.IsEmpty())
		Params += "HIGH";

	return Params;
}

void ACS_PK_RC_M::AttachMeasureDeviceToPrev(UActorComponent* PrevComponent)
{
	if (UCS_C43101_Prev_StaticMeshComp* Prev_StaticMeshComp = Cast<UCS_C43101_Prev_StaticMeshComp>(PrevComponent))
	{
		AttachToComponent(Prev_StaticMeshComp, FAttachmentTransformRules::KeepWorldTransform);
		SetActorRelativeLocation(FVector());
		SetActorRelativeRotation(FRotator());
		SetActorRelativeScale3D(FVector(1.0f));
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
	}
}

void ACS_PK_RC_M::ResetDevice()
{
	if (!ActivateOnStartup) IsMultimeter_On = false;

	CurrentType = EPKCurrentType::Direct;
	bMeasuring = false;
	ShowValueOnDisplay(0);

	if (WidgetInstance)
	{
		WidgetInstance->SetMultimeterState(IsMultimeter_On);
		WidgetInstance->SetSwitchType(CurrentType);
		WidgetInstance->SetStartStopState(false);
		WidgetInstance->SetMeasurementState(EMeasurementState::Idle);
	}

	BlackDipstick.PointId = "";
	BlackDipstick.UseDipstickMesh = false;

	RedDipstick.PointId = "";
	RedDipstick.UseDipstickMesh = false;

	if (BlackDipstick_Mesh) BlackDipstick_Mesh->SetRelativeTransform(BlackDipstick_Component->GetRelativeTransform());
	if (RedDipstick_Mesh) RedDipstick_Mesh->SetRelativeTransform(RedDipstick_Component->GetRelativeTransform());
}

void ACS_PK_RC_M::RemovesAllDelegates()
{
	if (Multimeter_Button)
	{
		Multimeter_Button->OnBeginCursorOver.RemoveAll(this);
		Multimeter_Button->OnEndCursorOver.RemoveAll(this);
		Multimeter_Button->OnClicked.RemoveAll(this);
	}
	if (SwitchType_Button)
	{
		SwitchType_Button->OnBeginCursorOver.RemoveAll(this);
		SwitchType_Button->OnEndCursorOver.RemoveAll(this);
		SwitchType_Button->OnClicked.RemoveAll(this);
	}
	if (SwitchVoltage_Button)
	{
		SwitchVoltage_Button->OnBeginCursorOver.RemoveAll(this);
		SwitchVoltage_Button->OnEndCursorOver.RemoveAll(this);
		SwitchVoltage_Button->OnClicked.RemoveAll(this);
	}
	if (StartStop_Button)
	{
		StartStop_Button->OnBeginCursorOver.RemoveAll(this);
		StartStop_Button->OnEndCursorOver.RemoveAll(this);
		StartStop_Button->OnClicked.RemoveAll(this);
	}
	if (PlayerController.IsValid() && PlayerController->InputComponent)
	{
		PlayerController->InputComponent->ClearBindingsForObject(this);
	}
}

void ACS_PK_RC_M::StartMeasurement()
{
	bMeasuring = true;

	if (Sound_MeasureStart)
		UGameplayStatics::PlaySound2D(this, Sound_MeasureStart);

	if (WidgetInstance)
	{
		WidgetInstance->SetStartStopState(bMeasuring);
		WidgetInstance->SetMeasurementState(EMeasurementState::Measuring);
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_FinishMeasure,
			this,
			&ACS_PK_RC_M::Internal_OnMeasurementFinished,
			MeasurementDelay,
			false
		);
	}
}

void ACS_PK_RC_M::StopMeasurement()
{
	bMeasuring = false;
	Internal_OnMeasurementFinished();

	if (WidgetInstance)
	{
		WidgetInstance->SetStartStopState(bMeasuring);
		WidgetInstance->SetMeasurementState(EMeasurementState::Idle);
	}
}

void ACS_PK_RC_M::FinishMeasurement()
{
	if (PlayerController.IsValid())
		PlayerController->OnMeasureActionEvent(MakeParams(ResultValue, true));

	if (Sound_MeasureEnd)
		UGameplayStatics::PlaySound2D(this, Sound_MeasureEnd);

	//bMeasuring = false;

	/*if (WidgetInstance)
	{
		WidgetInstance->SetStartStopState(bMeasuring);
		WidgetInstance->SetMeasurementState(EMeasurementState::Done);
	}*/

	ShowValueOnDisplay(ResultValue);

	ResultValue = 0.f;
}

void ACS_PK_RC_M::Internal_OnMeasurementFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_FinishMeasure);
	PlayerController->OnMeasureActionEvent(MakeParams());
	bMeasuring = false;

	if (WidgetInstance)
	{
		WidgetInstance->SetStartStopState(bMeasuring);
		WidgetInstance->SetMeasurementState(EMeasurementState::Idle);
	}
}

void ACS_PK_RC_M::ShowValueVoltageDisplay(float MinVoltage, float MaxVoltage)
{
	FString MinValueText = FString::Printf(TEXT("%g"), MinVoltage);
	FString MaxValueText = FString::Printf(TEXT("%g"), MaxVoltage);

	MinValueText += "  " + CurrentMiliValueText;
	MaxValueText += "  " + CurrentValueText;

	if (WidgetInstance)
	{
		WidgetInstance->SetDisplayVoltageText(
			FText::FromString(MinValueText),
			FText::FromString(MaxValueText)
		);
	}
}

void ACS_PK_RC_M::ShowValueOnDisplay(float Result)
{
	const float AbsVal = FMath::Abs(Result);

	FString NumberStr;
	FString UnitStr;

	const bool bNegative = (Result < 0.f && CurrentType == EPKCurrentType::Direct);
	const TCHAR* Sign = bNegative ? TEXT("-") : TEXT("");

	if (AbsVal >= CurrentMinMiliValue && AbsVal <= CurrentMaxMiliValue)
	{
		int32 DisplayValue = FMath::RoundToInt(AbsVal * 1000.f);
		NumberStr = FString::Printf(TEXT("%s%d"), Sign, DisplayValue);
		UnitStr = CurrentMiliValueText;
	}
	else if (AbsVal >= CurrentMinValue && AbsVal <= CurrentMaxValue)
	{
		int32 DisplayValue = FMath::RoundToInt(AbsVal);
		NumberStr = FString::Printf(TEXT("%s%d"), Sign, DisplayValue);
		UnitStr = CurrentValueText;
	}
	else
	{
		NumberStr = TEXT("O");
		UnitStr = TEXT("");
	}

	if (WidgetInstance)
	{
		WidgetInstance->SetDisplayText(
			FText::FromString(NumberStr),
			FText::FromString(UnitStr)
		);
	}
}
