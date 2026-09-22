// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CS_CoreTypes.generated.h"

#define SIM_INTERACT ECC_GameTraceChannel1
#define SIM_LOCATION ECC_GameTraceChannel2

//This enum for Device Measurement
UENUM(BlueprintType)
enum class EPKCurrentType : uint8
{
	Direct,
	Resistance,
	Alternate
};

UENUM(BlueprintType)
enum class EPKCurrentVoltage : uint8
{
	V,
	R,
	A
};

USTRUCT(BlueprintType)
struct FBodyOfAnswer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString objectId = "";
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString type = "";
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString eventName = "";
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString castedToStringAttribute = "";

	FBodyOfAnswer(){};

	FBodyOfAnswer(const FString& _objectId, const FString& _type, const FString& _eventName,const FString& _castedToStringAttribute)
	{
		objectId = _objectId;
		type = _type;
		eventName = _eventName;
		castedToStringAttribute = _castedToStringAttribute;
	}
};

USTRUCT(BlueprintType)
struct FLocation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ApiID = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasBackButton = false;

	// ~Begin #VSP_only
	// Lock user in location or not.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLocked = false;

	// Text of lock user in location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LockText = "";
	// ~End #VSP_only

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<int32> SchemeIds;
};

USTRUCT(BlueprintType)
struct FParameters 
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString parameterName = "";

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString parameterType = "";

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString parameterValue = "";
};

USTRUCT(BlueprintType)
struct FParametersObject
{
	GENERATED_BODY()

	int32 Elements = 0;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FParameters> Parameters;

	FParametersObject(){}
	FParametersObject(const TArray<FParameters> _Parameters)
	{
		Parameters = _Parameters;
		Elements = _Parameters.Num();
	}
	
};

USTRUCT()
struct FShowWelcomeParams
{
	GENERATED_BODY()

	FString ShowWelcomeTitle = "";
	FString ShowWelcomeDescription = "";
	FString FaultInfoTitle = "";
	FString FaultInfoDescription = "";
	FString FaultInfoQuest = "";

	int32 MaxTime;
	int32 OptTime;
	int32 StepsCount;
};

USTRUCT()
struct FStickerParams
{
	GENERATED_BODY()

	FString Header = "";
	FString Text = "";
	int32 Penalty;

};

USTRUCT(BlueprintType)
struct FVariables
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString parameterName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString parameterValue = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString parameterType = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStartedState = true;
};

USTRUCT()
struct FDialogButtonParameters
{
	GENERATED_BODY()
	
	FString IdDialog = "";
	FString Description = "";
	FString IconPath = "";
	FString Action = "";
	bool bCalling = false;

	FDialogButtonParameters(){}
	FDialogButtonParameters(const FString& _IdDialog, const FString& _Description)
	{
		IdDialog = _IdDialog;
		Description = _Description;
	}
};

UENUM(BlueprintType)
enum class EInteractMode : uint8
{
	Normal,
	MeausureMode
};

UENUM(BlueprintType)
enum class ETypeInteractPoint : uint8
{
	ActionPoint,
	LocationPoint,
	MeausurePoint
};

UENUM(BlueprintType)
enum class EDialogElement : uint8
{
	Dude,
	User,
	Message 
};

UENUM(BlueprintType)
enum class ETypeDialog : uint8
{
	None,
	Phone,
	Radio,
	Journal,
	People
};

USTRUCT()
struct FDialogParameters
{
	GENERATED_BODY()
	EDialogElement DialogElement;
	FString IdDialog = "";
	FString Message = "";
	FString OptType = "";
};

USTRUCT()
struct FAnswerButton
{
	GENERATED_BODY()
	
	FString categoryId = "";
	FString apiId = "";
	FString name = "";

	FAnswerButton(){}
	FAnswerButton(const FString& _categoryId, const FString& _apiId, const FString& _name)
	{
		categoryId = _categoryId;
		apiId = _apiId;
		name = _name;
	}
};

USTRUCT()
struct FAnswerButtonArray
{
	GENERATED_BODY()
	
	TArray<FAnswerButton> AnswerButtons;

	FAnswerButtonArray(){}
	FAnswerButtonArray(const TArray<FAnswerButton>& _AnswerButtons)
	{
		AnswerButtons = _AnswerButtons;
	}
};

USTRUCT()
struct FMainMenuReasons
{
	GENERATED_BODY()

	FString ReasonId = "";
	FString ReasonName = "";
	FAnswerButtonArray AnswerButtonArray;
};
USTRUCT()
struct FMainMenuFaultInfo
{
	GENERATED_BODY()
	FString name = "";
	FString text = "";
	FString footer = "";
	
};
USTRUCT()
struct FMainMenuExitInfo
{
	GENERATED_BODY()
	FString name = "";
	FString quest = "";
	FString text = "";
	FString warn = "";
	bool bInit = false;

};

USTRUCT(BlueprintType)
struct FLoadingImageInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString ApiID;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Num;

	UPROPERTY()
	FString Path;

	FLoadingImageInfo()
		: ApiID(TEXT(""))
		, Name(TEXT(""))
		, Num(TEXT(""))
		, Path()
	{
	}

	FLoadingImageInfo(const FString& InApiID, const FString& InName, const FString& InNum, const FString& InPath)
		: ApiID(InApiID)
		, Name(InName)
		, Num(InNum)
		, Path(InPath)
	{
	}
};

USTRUCT(BlueprintType)
struct F_CS_ImageInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	TObjectPtr<UTexture2D> Image = nullptr;

	F_CS_ImageInfo()
		: Name(TEXT(""))
	{
	}
};

USTRUCT(BlueprintType)
struct FMainMenuPointInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	TArray<FLoadingImageInfo> LoadingImagesInfo;

	FMainMenuPointInfo()
		: Name(TEXT(""))
	{
	}
};

USTRUCT()
struct FMainMenuParams
{
	GENERATED_BODY()

	TArray<FMainMenuReasons> MainMenuReasons;
	TArray<FMainMenuFaultInfo> MainMenuFaultInfo;
	TMap<FString, FMainMenuPointInfo> MapMainMenuPointsInfo;
	FMainMenuExitInfo MainMenuExitInfo;
	bool ShowAlgorithmButton = false;
};

USTRUCT()
struct FShowMessageParams
{
	GENERATED_BODY()
	int32 Alarm = -1;
	FString Footer = "";
	FString Header = "";
	FString Name = "";
	FString Text = "";
	bool Modal = false;
};

USTRUCT()
struct FResultParams
{
	GENERATED_BODY()

	int32 Penalty = 0;
	FString Name = "";
	FString MSG = "";

	// If true, show 'details' image
	bool bIsHasMessage = false;
};

USTRUCT()
struct FShowResultParams
{
	GENERATED_BODY()

	FString Eval = "";
	FString EvalDescription = "";
	TArray<FResultParams> Results;
};

USTRUCT()
struct FContextMenuItem
{
	GENERATED_BODY()

	FString ApiID = "";
	FString Name = "";
	FString Tool = "";
};

USTRUCT(BlueprintType)
struct FContextMenuIconItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Type = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> SoftTexture = nullptr;
};

USTRUCT()
struct FDeviceParam
{
	GENERATED_BODY()
	
	FString Id = "";
	FString Name = "";
};

USTRUCT()
struct FDeviceData
{
	GENERATED_BODY()
	
	FString DeviceId = "";
	TArray<FDeviceParam> Param;
	FString DeviceName = "";
};

USTRUCT()
struct FDeviceAndPointsData
{
	GENERATED_BODY()

	TArray<FDeviceData> DevicesData;
	TArray<FDeviceData> PointsData;
};

UENUM(BlueprintType)
enum class ELogType : uint8
{
	Reaction,
	Hint,
	Log,
	Attention,
	Wrong,
	Correct
};

USTRUCT(BlueprintType)
struct FLogParams
{
	GENERATED_BODY()

	FString Description = "";
	FDateTime Time;
	ELogType LogType;

};

UENUM(BlueprintType)
enum class ETypeHotKey : uint8
{
	EditorStep,
	CellMode,
	Custom
};


USTRUCT(BlueprintType)
struct FHotkeySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotkey")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SubKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotkey")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotkey")
	ETypeHotKey Type = ETypeHotKey::EditorStep;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag Activated;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FGameplayTag> Used;
};

USTRUCT(BlueprintType)
struct FWidgetConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ZOrder = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag UsedGameplayTag;
};

USTRUCT(BlueprintType)
struct FCameraLock
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ViewportTopLock = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ViewportDownLock = -100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ViewportLeftLock = -100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ViewportRightLock = 100;
};

USTRUCT(BlueprintType)
struct FButtonMainMenu
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PreviewText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LocalizationKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionName;
};

UENUM(BlueprintType)
enum class EPointAction : uint8
{
	OnClick UMETA(DisplayName = "OnClick"),
	OnDblClick UMETA(DisplayName = "OnDblClick"),
	OnRClick UMETA(DisplayName = "OnRClick"),
	OnMouseDown UMETA(DisplayName = "OnMouseDown"),
	OnMouseUp UMETA(DisplayName = "OnMouseUp"),
	OnMouseOver UMETA(DisplayName = "OnMouseOver"),
	OnMouseOut UMETA(DisplayName = "OnMouseOut")
};

UENUM(BlueprintType)
enum class ESimulatorMode : uint8
{
	None UMETA(DisplayName = "None"),
	ExamMode UMETA(DisplayName = "Exam Mode"),
	TrainingMode UMETA(DisplayName = "Training Mode")
};

USTRUCT(BlueprintType)
struct FObjectViewID
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PointID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ViewID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DecsID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> States;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OffsetX = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OffsetY = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SizeX = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SizeY = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ZOrder = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Z_CSS = 0;
};

/**
 * Full structure of incoming fault reasons. Used in FPK, VSP.
 * #VSP_only
 *
 * @param Name			Name of reason.
 * @param API_Id		API Id of place/object/reason.
 * @param Alarm			Indicator of correct answer. 0 - Correct answer; 1 - Incorrect answer; 2 (default) - No answer.
 * @param Result		Description of result of previous answer.
 * @param Attempt		Description of number of remaining attempts.
 * @param Att_Used		Number of attempts used.
 * @param bIsClosed		Closing the answer selection.
 * @param bIsChecked	The answer was select in last time.
 * @param Reasons		Nested reasons.
 */
USTRUCT(BlueprintType)
struct FFullMenuReasons
{
	GENERATED_BODY()

	// Name of reason.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = "";

	// API Id of place/object/reason.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString API_Id = "";

	// Indicator of correct answer. 0 - Correct answer; 1 - Incorrect answer; 2 (default) - No answer.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMax = 2, ClampMin = 0))
	uint8 Alarm = 2;

	// Description of result of previous answer.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Result = "";

	// Description of number of remaining attempts.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Attempt = "";

	// Number of attempts used.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Att_Used = 0;

	// Closing the answer selection.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsClosed = false;

	// The answer was select in last time.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsChecked = false;

	// Nested reasons.
	TArray<FFullMenuReasons> Reasons;

	void AddNestedReason(const FFullMenuReasons& Reason)
	{
		Reasons.Add(Reason);
	}

	int GetNumberOfReasons() const
	{
		return Reasons.Num();
	}
};

USTRUCT(BlueprintType)
struct FPlacePointData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString ApiID;

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString IconPath;

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> States;

	TArray<FPlacePointData> ChildPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FObjectViewID ViewStruct;


	FPlacePointData()
		: ApiID(TEXT(""))
		, Name(TEXT(""))
		, IconPath(TEXT(""))
		, ViewStruct()
	{
	}
};