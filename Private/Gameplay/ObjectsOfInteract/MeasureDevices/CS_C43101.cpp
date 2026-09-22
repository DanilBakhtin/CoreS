
#include "Gameplay/ObjectsOfInteract/MeasureDevices/CS_C43101.h"

#include "EnhancedInputComponent.h"
#include "Components/BoxComponent.h"
#include "Gameplay/ObjectsOfInteract/MeasureDevices/CS_C43101_Prev_StaticMeshComp.h"
#include "Gameplay/Player/CS_PlayerController.h"

ACS_C43101::ACS_C43101()
{
	PrimaryActorTick.bCanEverTick = true;

	Base_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base_Mesh"));
	RootComponent = Base_Mesh;
	Power_Component = CreateDefaultSubobject<USceneComponent>(TEXT("Power_Component"));
	Power_Component->SetupAttachment(Base_Mesh);
	Power_Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Power_Button"));
	Power_Button->SetupAttachment(Power_Component);
	Direct_Component = CreateDefaultSubobject<USceneComponent>(TEXT("Direct_Component"));
	Direct_Component->SetupAttachment(Base_Mesh);
	Direct_Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Direct_Button"));
	Direct_Button->SetupAttachment(Direct_Component);
	Resistance_Component = CreateDefaultSubobject<USceneComponent>(TEXT("Resistance_Component"));
	Resistance_Component->SetupAttachment(Base_Mesh);
	Resistance_Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Resistance_Button"));
	Resistance_Button->SetupAttachment(Resistance_Component);
	Alternate_Component = CreateDefaultSubobject<USceneComponent>(TEXT("Alternate_Component"));
	Alternate_Component->SetupAttachment(Base_Mesh);
	Alternate_Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Alternate_Button"));
	Alternate_Button->SetupAttachment(Alternate_Component);
	Arrow_Component = CreateDefaultSubobject<USceneComponent>(TEXT("Arrow_Component"));
	Arrow_Component->SetupAttachment(Base_Mesh);
	Arrow_Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow_Button"));
	Arrow_Button->SetupAttachment(Arrow_Component);
	Switcher_Component = CreateDefaultSubobject<USceneComponent>(TEXT("Switcher_Component"));
	Switcher_Component->SetupAttachment(Base_Mesh);
	Switcher_Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Switcher_Button"));
	Switcher_Button->SetupAttachment(Switcher_Component);
	Switcher_R_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Switcher_R_BoxComponent"));
	Switcher_R_BoxComponent->SetupAttachment(Switcher_Button);
	Switcher_L_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Switcher_L_BoxComponent"));
	Switcher_L_BoxComponent->SetupAttachment(Switcher_Button);

	BlackDipstick_Component = CreateDefaultSubobject<USceneComponent>(TEXT("BlackDipstick_Component"));
	BlackDipstick_Component->SetupAttachment(Base_Mesh);
	BlackDipstick_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlackDipstick_Mesh"));
	BlackDipstick_Mesh->SetupAttachment(BlackDipstick_Component);
	RedDipstick_Component = CreateDefaultSubobject<USceneComponent>(TEXT("RedDipstick_Component"));
	RedDipstick_Component->SetupAttachment(Base_Mesh);
	RedDipstick_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RedDipstick_Mesh"));
	RedDipstick_Mesh->SetupAttachment(RedDipstick_Component);

	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));
}

void ACS_C43101::SetPlayerController(ACS_PlayerController* _PlayerController)
{
	Super::SetPlayerController(_PlayerController);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		EnhancedInput->BindAction(MouseAction, ETriggerEvent::Triggered, this, &ACS_C43101::OnMouseWheel);
	}
}

void ACS_C43101::BeginPlay()
{
	Super::BeginPlay();
	
	RemovesAllDelegates();

	Power_Button->SetCollisionProfileName(FName("InteractObject"), true);
	Power_Button->OnBeginCursorOver.AddDynamic(this, &ACS_C43101::Power_BeginMouseOverLap);
	Power_Button->OnEndCursorOver.AddDynamic(this, &ACS_C43101::Power_EndMouseOverLap);
	Power_Button->OnClicked.AddDynamic(this, &ACS_C43101::Power_OnMouseClicked);
	
	Direct_Button->SetCollisionProfileName(FName("InteractObject"), true);
	Direct_Button->OnBeginCursorOver.AddDynamic(this, &ACS_C43101::Direct_BeginMouseOverLap);
	Direct_Button->OnEndCursorOver.AddDynamic(this, &ACS_C43101::Direct_EndMouseOverLap);
	Direct_Button->OnClicked.AddDynamic(this, &ACS_C43101::Direct_OnMouseClicked);
	
	Resistance_Button->SetCollisionProfileName(FName("InteractObject"), true);
	Resistance_Button->OnBeginCursorOver.AddDynamic(this, &ACS_C43101::Resistance_BeginMouseOverLap);
	Resistance_Button->OnEndCursorOver.AddDynamic(this, &ACS_C43101::Resistance_EndMouseOverLap);
	Resistance_Button->OnClicked.AddDynamic(this, &ACS_C43101::Resistance_OnMouseClicked);
	
	Alternate_Button->SetCollisionProfileName(FName("InteractObject"), true);
	Alternate_Button->OnBeginCursorOver.AddDynamic(this, &ACS_C43101::Alternate_BeginMouseOverLap);
	Alternate_Button->OnEndCursorOver.AddDynamic(this, &ACS_C43101::Alternate_EndMouseOverLap);
	Alternate_Button->OnClicked.AddDynamic(this, &ACS_C43101::Alternate_OnMouseClicked);
	
	Switcher_R_BoxComponent->SetCollisionProfileName(FName("InteractObject"), true);
	Switcher_R_BoxComponent->OnBeginCursorOver.AddDynamic(this, &ACS_C43101::Switcher_R_BeginMouseOverLap);
	Switcher_R_BoxComponent->OnEndCursorOver.AddDynamic(this, &ACS_C43101::Switcher_R_EndMouseOverLap);
	Switcher_R_BoxComponent->OnClicked.AddDynamic(this, &ACS_C43101::Switcher_R_OnMouseClicked);
	Switcher_L_BoxComponent->SetCollisionProfileName(FName("InteractObject"), true);
	Switcher_L_BoxComponent->OnBeginCursorOver.AddDynamic(this, &ACS_C43101::Switcher_L_BeginMouseOverLap);
	Switcher_L_BoxComponent->OnEndCursorOver.AddDynamic(this, &ACS_C43101::Switcher_L_EndMouseOverLap);
	Switcher_L_BoxComponent->OnClicked.AddDynamic(this, &ACS_C43101::Switcher_L_OnMouseClicked);
	SetActorTickEnabled(false);
	if (FloatCurve)
	{
		InterpFunction.BindUFunction(this, FName("TimelineUpdate"));
		TimelineFinished.BindUFunction(this, FName("TimelineFinishedCallback"));
	}
}

void ACS_C43101::AttachMeasureDeviceToPrev(UActorComponent* PrevComponent)
{
	if(UCS_C43101_Prev_StaticMeshComp* Prev_StaticMeshComp = Cast<UCS_C43101_Prev_StaticMeshComp>(PrevComponent))
	{
		AttachToComponent(Prev_StaticMeshComp, FAttachmentTransformRules::KeepWorldTransform);
		SetActorRelativeLocation(FVector());
		SetActorRelativeRotation(FRotator());
		SetActorRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
	}
}

void ACS_C43101::ClickOnMeasurePoint(const FString& InteractID, const FTransform& PositionMeasure)
{
	if(BlackDipstick.PointId == "" && RedDipstick.PointId != InteractID)
	{
		BlackDipstick.PointId = InteractID;
		BlackDipstick.Dipstick_PointPosition = PositionMeasure;
		BlackDipstick.UseDipstickMesh = true;
		PlayerController->OnMeasureActionEvent(MakeParams());
		return;
	}
	if(BlackDipstick.PointId != "" && RedDipstick.PointId == "" && BlackDipstick.PointId != InteractID)
	{
		RedDipstick.PointId = InteractID;
		RedDipstick.Dipstick_PointPosition = PositionMeasure;
		RedDipstick.UseDipstickMesh = true;
		PlayerController->OnMeasureActionEvent(MakeParams());
		return;
	}
	if(BlackDipstick.PointId == InteractID)
	{
		BlackDipstick.PointId = "";
		BlackDipstick.UseDipstickMesh = false;
		PlayerController->OnMeasureActionEvent(MakeParams());
		return;
	}
	if(RedDipstick.PointId == InteractID)
	{
		RedDipstick.PointId = "";
		RedDipstick.UseDipstickMesh = false;
		PlayerController->OnMeasureActionEvent(MakeParams());
		return;
	}
	if(BlackDipstick.PointId != "" && BlackDipstick.PointId != InteractID && RedDipstick.PointId != "" && RedDipstick.PointId != InteractID)
	{
		RedDipstick.PointId = InteractID;
		RedDipstick.Dipstick_PointPosition = PositionMeasure;
		RedDipstick.UseDipstickMesh = true;
		PlayerController->OnMeasureActionEvent(MakeParams());
	}
	
}

void ACS_C43101::UpdateMeasureDevice(const float& Result)
{
	PlayerController->OnMeasureActionEvent(MakeParams(Result,true));
}

void ACS_C43101::ResetDevice()
{
	Power_Button->SetRelativeLocation(FVector());
	IsPower_Click = false;
	Direct_Button->SetRelativeLocation(FVector());
	IsDirect_Click = false;
	Resistance_Button->SetRelativeLocation(FVector());
	IsResistance_Click = false;
	Alternate_Button->SetRelativeLocation(FVector());
	IsAlternate_Click = false;
	Switcher_Button->SetRelativeRotation(FRotator(0.f,0.f,0.f));
	CurrentRotateSwitch = 0.f;
	CurrentArrowRotation = 0.f;
	NewArrowRotation = 0.f;
	OldArrowRotation = 0.f;
	RedDipstick.PointId = "";
	RedDipstick.UseDipstickMesh = false;
	BlackDipstick.PointId = "";
	BlackDipstick.UseDipstickMesh = false;
	BlackDipstick_Mesh->SetRelativeTransform(BlackDipstick_Component->GetRelativeTransform());
	RedDipstick_Mesh->SetRelativeTransform(RedDipstick_Component->GetRelativeTransform());
	SelectCurrentType("");
}

void ACS_C43101::Power_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	Power_Button->SetRenderCustomDepth(true);
}

void ACS_C43101::Power_EndMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	Power_Button->SetRenderCustomDepth(false);
}

void ACS_C43101::Power_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if(IsPower_Click)
	{
		Power_Button->SetRelativeLocation(FVector());
		IsPower_Click = false;
		GetVoltage(0.f);
	}
	else
	{
		Power_Button->SetRelativeLocation(FVector(0.f,0.f,-0.5f));
		IsPower_Click = true;
		GetVoltage(CurrentRotateSwitch);
		SelectCurrentType(CurrentType);
	}
	
}

void ACS_C43101::Direct_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	Direct_Button->SetRenderCustomDepth(true);
}

void ACS_C43101::Direct_EndMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	Direct_Button->SetRenderCustomDepth(false);
}

void ACS_C43101::Direct_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	
	if(IsDirect_Click)
	{
		Direct_Button->SetRelativeLocation(FVector());
		IsDirect_Click = false;
		SelectCurrentType("");
	}
	else
	{
		Direct_Button->SetRelativeLocation(FVector(0.f,0.f,-0.5f));
		IsDirect_Click = true;
		Resistance_Button->SetRelativeLocation(FVector());
		IsResistance_Click = false;
		Alternate_Button->SetRelativeLocation(FVector());
		IsAlternate_Click = false;
		SelectCurrentType("Direct");
	}
}

void ACS_C43101::Resistance_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	Resistance_Button->SetRenderCustomDepth(true);
}

void ACS_C43101::Resistance_EndMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	Resistance_Button->SetRenderCustomDepth(false);
}

void ACS_C43101::Resistance_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if(IsResistance_Click)
	{
		Resistance_Button->SetRelativeLocation(FVector());
		IsResistance_Click = false;
		SelectCurrentType("");
	}
	else
	{
		Resistance_Button->SetRelativeLocation(FVector(0.f,0.f,-0.5f));
		IsResistance_Click = true;
		Alternate_Button->SetRelativeLocation(FVector());
		IsAlternate_Click = false;
		Direct_Button->SetRelativeLocation(FVector());
		IsDirect_Click = false;
		SelectCurrentType("Resistance");
	}
}

void ACS_C43101::Alternate_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	Alternate_Button->SetRenderCustomDepth(true);
}

void ACS_C43101::Alternate_EndMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	Alternate_Button->SetRenderCustomDepth(false);
}

void ACS_C43101::Alternate_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{	
	if(IsAlternate_Click)
	{
		Alternate_Button->SetRelativeLocation(FVector());
		IsAlternate_Click = false;
		SelectCurrentType("");
	}
	else
	{
		Alternate_Button->SetRelativeLocation(FVector(0.f,0.f,-0.5f));
		IsAlternate_Click = true;
		Direct_Button->SetRelativeLocation(FVector());
		IsDirect_Click = false;
		Resistance_Button->SetRelativeLocation(FVector());
		IsResistance_Click = false;
		SelectCurrentType("Alternate");
	}
}

void ACS_C43101::Switcher_R_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	Switcher_Button->SetRenderCustomDepth(true);
	IsSwitcher_Button_OverLap = true;
}

void ACS_C43101::Switcher_R_EndMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	Switcher_Button->SetRenderCustomDepth(false);
	IsSwitcher_Button_OverLap = false;
}

void ACS_C43101::Switcher_R_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	
}

void ACS_C43101::Switcher_L_BeginMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	Switcher_Button->SetRenderCustomDepth(true);
	IsSwitcher_Button_OverLap = true;
}

void ACS_C43101::Switcher_L_EndMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
	Switcher_Button->SetRenderCustomDepth(false);
	IsSwitcher_Button_OverLap = false;		
}

void ACS_C43101::Switcher_L_OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	
}

void ACS_C43101::OnMouseWheel(const FInputActionInstance& Instance)
{
	if(IsSwitcher_Button_OverLap)
	{
		// input is a Vector2D
		float ScrollValue = Instance.GetValue().Get<float>();
		if(ScrollValue > 0.f)
		{
			Switcher_Button_Rotate += 15.f;
			Switcher_Button->SetRelativeRotation(FRotator(0.f,Switcher_Button_Rotate,0.f));
			RotateSwitch(ScrollValue);

		}
		else
		{
			Switcher_Button_Rotate -= 15.f;
			Switcher_Button->SetRelativeRotation(FRotator(0.f,Switcher_Button_Rotate,0.f));
			RotateSwitch(ScrollValue);
		}
	}
}

void ACS_C43101::RotateSwitch(float Current)
{
	if(Current > KINDA_SMALL_NUMBER)
	{
		CurrentRotateSwitch += 15.f;
	}
	else if(Current < KINDA_SMALL_NUMBER)
	{
		CurrentRotateSwitch -= 15.f;
	}
	else
	{
		return;
	}
	if(CurrentRotateSwitch > 360.f)
		CurrentRotateSwitch = 15.f;
	if(CurrentRotateSwitch < 0.f)
		CurrentRotateSwitch = 345.f;
	if(Current == KINDA_SMALL_NUMBER)
		CurrentRotateSwitch = 0.f;

	if(IsPower_Click)
		GetVoltage(CurrentRotateSwitch);
	else
		GetVoltage(0.f);
}

FString ACS_C43101::MakeParams(float Current, bool UseFloat)
{
	FString Params = "";
	//Params = "c_43101:" + GetMeasureType() + ":" + BlackDipstick.PointId + ":" + RedDipstick.PointId + ":";
	Params = IdDevice + ":" + GetMeasureType() + ":" + BlackDipstick.PointId + ":" + RedDipstick.PointId + ":";

	if(UseFloat)
		Params += GetQuality(Current);
    
	return Params;
}

void ACS_C43101::SelectCurrentType(const FString& _CurrentType)
{
	CurrentType = _CurrentType;
	PlayerController->OnMeasureActionEvent(MakeParams());
}

FString ACS_C43101::GetMeasureType() const
{
	if (CurrentType == "Alternate" && Voltage == "mA" || CurrentType == "Alternate" && Voltage == "A")
		return "acl";
	if (CurrentType == "Direct" && Voltage == "mA" || CurrentType == "Direct" && Voltage == "A")
		return "dcl";
	if (CurrentType == "Alternate" && Voltage == "V")
		return "acU";
	if (CurrentType == "Direct" && Voltage == "V")
		return "dcU";
	if (CurrentType == "Resistance" && Voltage == "R")
		return "ohm";

	return "";
}

FString ACS_C43101::GetQuality(const float& Current)
{
	GetWorldTimerManager().ClearTimer(QualityTimerHandle);
	bool bNegativeDeviation = false; 
	if(Current < 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Current: %f, TargetRotation: %f, DivisionValue: %f, MaxValue: %f"), Current, TargetRotation, DivisionValue, MaxValue);
		SetArrowRotation( FMath::Lerp(0.f, 90.f, -6/84.f));
		bNegativeDeviation = true;
	}
	if(MaxValue * 2 < FMath::Abs(Current))
	{
		Fuse();
		return "GUARDON";
	}
	const float DivisionValue =  108.f / MaxValue;
	float TargetRotation = 1.f;
	FString retVal = "HIGH";
	if (Current < 0)
	{
		if (Current > -1 * MaxValue)
			TargetRotation *= DivisionValue * -0.01f;
		else
			TargetRotation *= DivisionValue * -0.015f;
		retVal = "NEGOVERBOUND";
	}
	else if (Current > MaxValue)
	{
		if (Current >= 2 * MaxValue)
			TargetRotation *= DivisionValue * MaxValue * 0.80f;
		else
			TargetRotation *= DivisionValue * MaxValue * 0.78f;
		retVal = "POSOVERBOUND";
	}
	else
		TargetRotation *= DivisionValue * Current *0.76f;

	FTimerDelegate QualityTimerDelegate;
	TFunction<void()> MakeCalculateQualityRotation = [this,TargetRotation,Current,DivisionValue]()
	{
		UE_LOG(LogTemp, Warning, TEXT("Current: %f, TargetRotation: %f, DivisionValue: %f, MaxValue: %f"), Current, TargetRotation, DivisionValue, MaxValue);
		SetArrowRotation( FMath::Lerp(0.f, 90.f, TargetRotation/84.f));
	};
	QualityTimerDelegate.BindLambda(MakeCalculateQualityRotation);
	
	if(bNegativeDeviation)
		GetWorld()->GetTimerManager().SetTimer(QualityTimerHandle, QualityTimerDelegate, 1.f, false);
	else
		MakeCalculateQualityRotation();
	
	//targetRotation += new Vector3(-90, 0, -41);
	//arrowTransform.localRotation = Quaternion.Euler(targetRotation);
	return retVal;
}

void ACS_C43101::GetVoltage(float value)
{
	 if (value == 345.f)
        {
           MaxValue = 1000.f;
	        Voltage  = "V";
        }
        else if (value == 330.f)
        {
            MaxValue = 500.f;
            Voltage  = "V";
        }
        else if (value == 315.f)
        {
            MaxValue = 250.f;
            Voltage  = "V";
        }
        else if (value == 300.f)
        {
            MaxValue = 100.f;
            Voltage  = "V";
        }
        else if (value == 285.f)
        {
            MaxValue = 50.f;
            Voltage  = "V";
        }
        else if (value == 270.f)
        {
            MaxValue = 25.f;
            Voltage  = "V";
        }
        else if (value == 255.f)
        {
            MaxValue = 10.f;
            Voltage  = "V";
        }
        else if (value == 240.f)
        {
            MaxValue = 2.5f;
            Voltage  = "V";
        }
        else if (value == 225.f)
        {
            MaxValue = 0.5f;
            Voltage  = "V";
        }
        else if (value == 210.f)
        {
            MaxValue = 200.f;
            Voltage  = "R";
        }
        else if (value == 195.f)
        {
            MaxValue = 9999.f;
            Voltage  = "R";
        }
        else if (value == 180.f)
        {
           MaxValue = 9999.f;
           Voltage  = "R"; 
        }
        else if (value == 165.f)
        {
            MaxValue = 9999.f;
            Voltage  ="R";
        }
        else if (value == 150.f)
        {
            MaxValue =  9999.f;
            Voltage  = "R";
        }
        /*else if (value == 135.f)
        {
            MaxValue = 9999.f;
            Voltage  = "R";
        }*/
		else if (value == 135.f)
		{
			MaxValue = 0.1f;
			Voltage = "V";
		}
        else if (value == 120.f)
        {
            MaxValue = 0.5f;
           Voltage  = "mA";
        }
        else if (value == 105.f)
        {
            MaxValue = 1.f;
            Voltage  = "mA";
        }
        else if (value == 90.f)
        {
            MaxValue = 5.f;
           Voltage  = "mA";
        }
        else if (value == 75.f)
        {
            MaxValue = 10.f;
            Voltage  = "mA";
        }
        else if (value == 60.f)
        {
            MaxValue = 50.f;
            Voltage  = "mA";
        }
        else if (value == 45.f)
        {
            MaxValue = 0.25f;
            Voltage  = "A";
        }
        else if (value == 30.f)
        {
            MaxValue = 1.f;
            Voltage  ="A";
        }
        else if (value == 15.f)
        {
            MaxValue = 6.f;
            Voltage  ="A";
        }
        else
        {
            MaxValue = 0.001f;
            Voltage  = "";
        }

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Angle: %f, MaxValue: %f, Voltage: %s"), value, MaxValue, *Voltage));

	PlayerController->OnMeasureActionEvent(MakeParams());
}

void ACS_C43101::SetArrowRotation(const float& RotationAngle)
{
	NewArrowRotation = RotationAngle;
	if(OldArrowRotation != NewArrowRotation)
		OldArrowRotation = CurrentArrowRotation;
	else
		OldArrowRotation = NewArrowRotation;

	if (FloatCurve)
	{
		Timeline->AddInterpFloat(FloatCurve, InterpFunction);
		Timeline->SetTimelineFinishedFunc(TimelineFinished);
		Timeline->SetLooping(false);
		Timeline->PlayFromStart();
	}
	
}

void ACS_C43101::Fuse()
{
	IsPower_Click = false;
	Power_Button->SetRelativeLocation(FVector());
	GetVoltage(0.f);
	SetArrowRotation(100.f);

	
	//Задержка необходимо что бы у скрола хватило времени отрисовать все элементы
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			SetArrowRotation(0.f);
		},
		2.f,
		false);
}

void ACS_C43101::TimelineUpdate(float Value)
{
	CurrentArrowRotation = FMath::Lerp(OldArrowRotation, NewArrowRotation, Value);
	Arrow_Button->SetRelativeRotation(FRotator(0.f,CurrentArrowRotation,0.f));
}

void ACS_C43101::TimelineFinishedCallback()
{
	OldArrowRotation = CurrentArrowRotation;
}

void ACS_C43101::RemovesAllDelegates()
{
	if (Power_Button)
	{
		Power_Button->OnBeginCursorOver.RemoveAll(this);
		Power_Button->OnEndCursorOver.RemoveAll(this);
		Power_Button->OnClicked.RemoveAll(this);
	}

	if (Direct_Button)
	{
		Direct_Button->OnBeginCursorOver.RemoveAll(this);
		Direct_Button->OnEndCursorOver.RemoveAll(this);
		Direct_Button->OnClicked.RemoveAll(this);
	}

	if (Resistance_Button)
	{
		Resistance_Button->OnBeginCursorOver.RemoveAll(this);
		Resistance_Button->OnEndCursorOver.RemoveAll(this);
		Resistance_Button->OnClicked.RemoveAll(this);
	}

	if (Alternate_Button)
	{
		Alternate_Button->OnBeginCursorOver.RemoveAll(this);
		Alternate_Button->OnEndCursorOver.RemoveAll(this);
		Alternate_Button->OnClicked.RemoveAll(this);
	}

	if (Switcher_R_BoxComponent)
	{
		Switcher_R_BoxComponent->OnBeginCursorOver.RemoveAll(this);
		Switcher_R_BoxComponent->OnEndCursorOver.RemoveAll(this);
		Switcher_R_BoxComponent->OnClicked.RemoveAll(this);
	}

	if (Switcher_L_BoxComponent)
	{
		Switcher_L_BoxComponent->OnBeginCursorOver.RemoveAll(this);
		Switcher_L_BoxComponent->OnEndCursorOver.RemoveAll(this);
		Switcher_L_BoxComponent->OnClicked.RemoveAll(this);
	}

	if (PlayerController.IsValid() && PlayerController->InputComponent)
	{
		PlayerController->InputComponent->ClearBindingsForObject(this);
	}
}

void ACS_C43101::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(BlackDipstick.UseDipstickMesh)
		BlackDipstick_Mesh->SetWorldTransform(BlackDipstick.Dipstick_PointPosition);
	else
		BlackDipstick_Mesh->SetWorldTransform(BlackDipstick_Component->GetComponentTransform());
	if(RedDipstick.UseDipstickMesh)
		RedDipstick_Mesh->SetWorldTransform(RedDipstick.Dipstick_PointPosition);
	else
		RedDipstick_Mesh->SetWorldTransform(RedDipstick_Component->GetComponentTransform());
}


