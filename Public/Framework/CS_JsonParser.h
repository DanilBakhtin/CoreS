// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Dom/JsonObject.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_JsonParser.generated.h"


UCLASS()
class CORE_SIMULATOR_API UCS_JsonParser : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION()
	bool ParseJsonPayload(const FString& Payload, FString& OutObjectId, FString& OutMethod, FParametersObject& OutParameters, double& OutDelay);

	UFUNCTION()
	bool ParseShowWelcome(const FParametersObject& Parameters, FShowWelcomeParams& OutParams);

	UFUNCTION()
	bool ParseShowTime(const FParametersObject& Parameters, FTimespan& OutParams);

	UFUNCTION()
	bool ParseShowMenu(const FParametersObject& Parameters, FMainMenuParams& OutParams);

	UFUNCTION()
	bool ParseLocationInfo(const FParametersObject& Parameters, FLocation& OutParams);

	UFUNCTION()
	bool ParsePointsData(const FParametersObject& Parameters, TArray<FPlacePointData>& OutParams);

	UFUNCTION()
	bool ParseUpdatePointsData(const FParametersObject& Parameters, TArray<FPlacePointData>& OutParams);

	UFUNCTION()
	bool ParseDialogParameters(const FParametersObject& Parameters, FString& OutAPI_Id_Dialog, FString& OutNameTitle, FString& IconTitlePath, TArray<FDialogParameters>& OutDialog, TArray<FString>& OutQueueSounds, ETypeDialog& TypeDialog);

	UFUNCTION()
	bool ParseAddDialogMessageParameters(const FParametersObject& Parameters, TArray<FDialogParameters>& OutDialog, TArray<FString>& OutQueueSounds);

	UFUNCTION()
	bool ParseUpdateDialogParameters(const FParametersObject& Parameters, TArray<FDialogParameters>& OutDialog);

	UFUNCTION()
	bool ParseShowMessage(const FParametersObject& Parameters, FShowMessageParams& OutParams);

	UFUNCTION()
	bool ParseMeasureDevices(const FParametersObject& Parameters, FDeviceAndPointsData& OutParams);

	UFUNCTION()
	bool ParseShowMeasureResult(const FParametersObject& Parameters, float& OutParams);

	UFUNCTION()
	bool ParseShowReaction(const FParametersObject& Parameters, FString& OutParams);

	UFUNCTION()
	bool ParseShowStepInfo(const FParametersObject& Parameters, int& OutParams);

	UFUNCTION()
	bool ParseShowLearningSticker(const FParametersObject& Parameters, FStickerParams& OutParams);

	UFUNCTION()
	bool ParseAddAction(const FParametersObject& Parameters, FString& OutParams);

	UFUNCTION()
	bool ParsePlaySound(const FParametersObject& Parameters, FString& OutParams);

	// #VSP_only
	UFUNCTION()
	bool ParseShowObjectMenu(const FParametersObject& Parameters, FFullMenuReasons& OutParams);

	UFUNCTION()
	bool ParseShowResult(const FParametersObject& Parameters, FShowResultParams& OutParams);

	// #VSP_only
	// Used instead of ParseShowResult.
	UFUNCTION()
	bool ParseShowResultWithShowedNesting(const FParametersObject& Parameters, FShowResultParams& OutParams);
	

	UFUNCTION()
	bool ParseWalkieParams(const FParametersObject& Parameters, TArray<FDialogButtonParameters>& OutParams, const FString& Subkey);

	UFUNCTION()
	bool ParseDialogPointsParams(const FParametersObject& Parameters, TArray<FDialogButtonParameters>& OutParams);

	bool ParsePhoneParams(const FParametersObject& Parameters, TMap<FString, TArray<FDialogButtonParameters>>& OutParams, const FString& Subkey);

	UFUNCTION()
	bool ParseContextMenu(const FParametersObject& Parameters, FString& ApiID, TArray<FContextMenuItem>& OutParams);

	bool ParseViewObject(const TSharedPtr<FJsonObject>* ViewJsonObject, FObjectViewID& OutParams);

	// #VSP_only
	UFUNCTION()
	bool ParseUpdatePlaceLock(const FParametersObject& Parameters, FLocation& OutParams);

	UFUNCTION()
	bool ParseSchemes(const FParametersObject& Parameters, TArray<FLoadingImageInfo>& OutParams);

	UFUNCTION()
	bool FindApiID(const FParametersObject& Parameters, FString& OutApiID);

private:

	bool ParseParametersArray(const TArray<TSharedPtr<FJsonValue>>& ParametersJsonData, FParametersObject& OutParameters);
	bool ParseDialogMessage(const TSharedPtr<FJsonObject>& OutMSGParamObject, FDialogParameters& OutDialogParams, TArray<FString>& OutQueueSounds);
	bool ParseDialogPoints(const TArray<TSharedPtr<FJsonValue>>& PointsJsonData, TArray<FDialogParameters>& OutDialog);
	void ParseRecursionFillFullMenuReasons(const FJsonObject* ReasonsJsonData, FFullMenuReasons& FullMenuReason);
	// #VSP_only
	void ParseRecursionResultParams(const FJsonObject* ReasonsJsonData, TArray<FResultParams>& ResultParams);

	bool ParsePlacePointObject(const TSharedPtr<FJsonObject>& PointParamObject, FPlacePointData& OutPointData, const FPlacePointData& ParentPointData = FPlacePointData());
	void ParseViewAndChildData(const TSharedPtr<FJsonObject>& PointParamObject, FPlacePointData& OutPointData, const FPlacePointData& ParentPointData = FPlacePointData());


	int Global_ZOrder = 0;
	TSet<FString> ParsedPointIds;
	int32 ExtractZIndexSimple(const FString& InputString);

	
};
