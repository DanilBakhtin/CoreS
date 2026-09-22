// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CS_JsonParser.h"
#include <Framework/CS_UtilityFunctionLibrary.h>

bool UCS_JsonParser::ParseJsonPayload(const FString& Payload, FString& OutObjectId, FString& OutMethod, FParametersObject& OutParameters, double& OutDelay)
{
	const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Payload);
	TSharedPtr<FJsonValue> JsonValue;

	if (!FJsonSerializer::Deserialize(Reader, JsonValue) || !JsonValue.IsValid())
		return false;
	
	TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();
	if (!JsonObject.IsValid())
		return false;

	JsonObject->TryGetStringField(TEXT("objectId"), OutObjectId);
	JsonObject->TryGetStringField(TEXT("methodName"), OutMethod);
	JsonObject->TryGetNumberField(TEXT("delay"), OutDelay);

	const TArray<TSharedPtr<FJsonValue>>* ParametersJsonData;
	if (JsonObject->TryGetArrayField(TEXT("parameters"), ParametersJsonData))
	{
		return ParseParametersArray(*ParametersJsonData, OutParameters);
	}

    return true;
}

bool UCS_JsonParser::ParseParametersArray(const TArray<TSharedPtr<FJsonValue>>& ParametersJsonData, FParametersObject& OutParameters)
{
	for (const auto& ParamValue : ParametersJsonData)
	{
		const TSharedPtr<FJsonObject> ParamObj = ParamValue->AsObject();
		if (ParamObj.IsValid())
		{
			FParameters ParamData;
			ParamObj->TryGetStringField(TEXT("parameterName"), ParamData.parameterName);
			ParamObj->TryGetStringField(TEXT("parameterType"), ParamData.parameterType);
			ParamObj->TryGetStringField(TEXT("parameterValue"), ParamData.parameterValue);

			OutParameters.Parameters.Add(ParamData);
		}
	}
	return true;
}

bool UCS_JsonParser::ParseShowWelcome(const FParametersObject& Parameters, FShowWelcomeParams& OutParams)
{
	for (auto& Param : Parameters.Parameters)
	{
		const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
		TSharedPtr<FJsonValue> JsonValue;
		if (Param.parameterName == "welcome")
		{
			if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
			{
				TSharedPtr<FJsonObject>* JsonObject;
				JsonValue->TryGetObject(JsonObject);
				JsonObject->Get()->TryGetStringField(TEXT("name"), OutParams.ShowWelcomeTitle);
				FString baseText = "";
				JsonObject->Get()->TryGetStringField(TEXT("text"), OutParams.ShowWelcomeDescription);
			}
		}
		if (Param.parameterName == "faultInfo")
		{
			if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
			{
				TSharedPtr<FJsonObject>* JsonObject;
				JsonValue->TryGetObject(JsonObject);
				JsonObject->Get()->TryGetStringField(TEXT("name"), OutParams.FaultInfoTitle);
				JsonObject->Get()->TryGetStringField(TEXT("text"), OutParams.FaultInfoDescription);
				JsonObject->Get()->TryGetStringField(TEXT("footer"), OutParams.FaultInfoQuest);

				const TSharedPtr<FJsonObject>* TimeObject;
				if (JsonObject->Get()->TryGetObjectField(TEXT("time"), TimeObject))
				{
					(*TimeObject)->TryGetNumberField(TEXT("max"), OutParams.MaxTime);
					(*TimeObject)->TryGetNumberField(TEXT("opt"), OutParams.OptTime);
				}

				const TSharedPtr<FJsonObject>* StepsObject;
				if (JsonObject->Get()->TryGetObjectField(TEXT("steps"), StepsObject))
				{
					(*StepsObject)->TryGetNumberField(TEXT("countAll"), OutParams.StepsCount);
				}
			}
		}
	}

	return true;
}

bool UCS_JsonParser::ParseShowTime(const FParametersObject& Parameters, FTimespan& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "time")
        {
            OutParams = FTimespan::FromSeconds(FCString::Atoi(*Param.parameterValue));
            return true;
        }
    }

    return false;
}

bool UCS_JsonParser::ParseShowMenu(const FParametersObject& Parameters, FMainMenuParams& OutParams)
{
    const TFunction<void(const TSharedPtr<FJsonObject>* JsonObject, const FString& ApiIdPoint, FMainMenuParams& OutParams)>
        ParseImages = [this](const TSharedPtr<FJsonObject>* JsonObject, const FString& ApiIdPoint, FMainMenuParams& OutParams)
        {
            TArray<FLoadingImageInfo> LoadingImagesInfo;
            FString NamePoint;

            const TSharedPtr<FJsonObject>* JsonData;
            if (JsonObject->Get()->TryGetObjectField(ApiIdPoint, JsonData))
            {
                JsonData->Get()->TryGetStringField(TEXT("name"), NamePoint);

                const TArray<TSharedPtr<FJsonValue>>* SchemesArray;
                if (JsonData->Get()->TryGetArrayField(TEXT("schemes"), SchemesArray))
                {
                    for (const auto& Scheme : *SchemesArray)
                    {
                        if (Scheme.IsValid())
                        {
                            const TSharedPtr<FJsonObject>* SchemeObject;
                            if (Scheme->TryGetObject(SchemeObject))
                            {
                                FLoadingImageInfo ImageInfo = FLoadingImageInfo();

                                SchemeObject->Get()->TryGetStringField(TEXT("apiId"), ImageInfo.ApiID);
                                SchemeObject->Get()->TryGetStringField(TEXT("img"), ImageInfo.Path);
                                SchemeObject->Get()->TryGetStringField(TEXT("name"), ImageInfo.Name);
                                SchemeObject->Get()->TryGetStringField(TEXT("num"), ImageInfo.Num);

                                if (!ImageInfo.Path.IsEmpty()) {
                                    UE_LOG(LogTemp, Log, TEXT("Parsed img for (%s) - name: %s,num: %s, apiId: %s, img: %s"), *ApiIdPoint, *NamePoint, *ImageInfo.Num, *ImageInfo.ApiID, *ImageInfo.Path);

                                    LoadingImagesInfo.Add(ImageInfo);
                                }
                            }
                        }
                    }
                }
            }

            if (LoadingImagesInfo.Num() > 0) {

                FMainMenuPointInfo MainMenuPointInfo = FMainMenuPointInfo();

                MainMenuPointInfo.Name = NamePoint;
                MainMenuPointInfo.LoadingImagesInfo = LoadingImagesInfo;

                OutParams.MapMainMenuPointsInfo.Add(ApiIdPoint,MainMenuPointInfo);
            }
        };

    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "data")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;
            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                const TSharedPtr<FJsonObject>* MenuObject;
                if (JsonValue->TryGetObject(MenuObject))
                {
                    const TSharedPtr<FJsonObject>* ReasonsJsonData;
                    if (MenuObject->Get()->TryGetObjectField(TEXT("reasons"), ReasonsJsonData))
                    {
                        const TArray<TSharedPtr<FJsonValue>>* ReasonsValueArray;
                        if (ReasonsJsonData->Get()->TryGetArrayField(TEXT("reasons"), ReasonsValueArray))
                        {
                            for (const auto& Reason : *ReasonsValueArray)
                            {
                                if (Reason.IsValid()) {

                                    const TSharedPtr<FJsonObject>* ParametersObject;
                                    if (Reason->TryGetObject(ParametersObject))
                                    {
                                        FMainMenuReasons MainMenuReasons = FMainMenuReasons();
                                        ParametersObject->Get()->TryGetStringField(TEXT("apiId"), MainMenuReasons.ReasonId);
                                        ParametersObject->Get()->TryGetStringField(TEXT("name"), MainMenuReasons.ReasonName);
                                        const TArray<TSharedPtr<FJsonValue>>* AnswerButtonArray;
                                        ParametersObject->Get()->TryGetArrayField(TEXT("reasons"), AnswerButtonArray);
                                        TArray<FAnswerButton> AnsButton;
                                        for (const auto& AnswerButton : *AnswerButtonArray)
                                        {
                                            const TSharedPtr<FJsonObject>* AnswerButtonObject;
                                            if (AnswerButton->TryGetObject(AnswerButtonObject))
                                            {
                                                FString aapiId = "";
                                                AnswerButtonObject->Get()->TryGetStringField(TEXT("apiId"), aapiId);
                                                FString aname = "";
                                                AnswerButtonObject->Get()->TryGetStringField(TEXT("name"), aname);
                                                AnsButton.Add(FAnswerButton(MainMenuReasons.ReasonId, aapiId, aname));
                                            }
                                        }
                                        MainMenuReasons.AnswerButtonArray.AnswerButtons = AnsButton;
                                        OutParams.MainMenuReasons.Add(MainMenuReasons);
                                    }
                                }
                            }
                        }

                    }
                    const TSharedPtr<FJsonObject>* FaultInfoData;
                    if (MenuObject->Get()->TryGetObjectField(TEXT("fltInfo"), FaultInfoData))
                    {
                        if (FaultInfoData->IsValid())
                        {
                            FMainMenuFaultInfo MainMenuFaultInfo = FMainMenuFaultInfo();
                            FaultInfoData->Get()->TryGetStringField(TEXT("name"), MainMenuFaultInfo.name);
                            FaultInfoData->Get()->TryGetStringField(TEXT("text"), MainMenuFaultInfo.text);
                            FaultInfoData->Get()->TryGetStringField(TEXT("footer"), MainMenuFaultInfo.footer);
                            OutParams.MainMenuFaultInfo.Add(MainMenuFaultInfo);
                        }
                    }
                    const TSharedPtr<FJsonObject>* AlgJsonData;
                    if (MenuObject->Get()->TryGetObjectField(TEXT("algs"), AlgJsonData))
                        OutParams.ShowAlgorithmButton = true;
                    else
                        OutParams.ShowAlgorithmButton = false;

                    const TSharedPtr<FJsonObject>* ExitInfoJsonData;
                    if (MenuObject->Get()->TryGetObjectField(TEXT("exitInfo"), ExitInfoJsonData)) {

                        FMainMenuExitInfo MainMenuExitInfo = FMainMenuExitInfo();
                        ExitInfoJsonData->Get()->TryGetStringField(TEXT("name"), MainMenuExitInfo.name);
                        ExitInfoJsonData->Get()->TryGetStringField(TEXT("quest"), MainMenuExitInfo.quest);
                        ExitInfoJsonData->Get()->TryGetStringField(TEXT("text"), MainMenuExitInfo.text);
                        ExitInfoJsonData->Get()->TryGetStringField(TEXT("warn"), MainMenuExitInfo.warn);
                        MainMenuExitInfo.bInit = true;
                        OutParams.MainMenuExitInfo = MainMenuExitInfo;
                    }

                    ParseImages(MenuObject, "schemes", OutParams);
                    ParseImages(MenuObject, "algs", OutParams);
                    ParseImages(MenuObject, "maps", OutParams);
                    
                }
            }
            return true;
        }
    }

    return false;
}

bool UCS_JsonParser::ParseLocationInfo(const FParametersObject& Parameters, FLocation& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "places")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;

            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
                JsonValue->TryGetArray(JsonValueArray);
                for (auto& Value : *JsonValueArray)
                {
                    const TSharedPtr<FJsonObject>* ValueParamObject;
                    if (Value->TryGetObject(ValueParamObject))
                    {
                        const TSharedPtr<FJsonObject>* PlaceObject;
                        if (ValueParamObject->Get()->TryGetObjectField(TEXT("place"), PlaceObject))
                        {
                            PlaceObject->Get()->TryGetStringField(TEXT("name"), OutParams.Name);
                            PlaceObject->Get()->TryGetStringField(TEXT("apiId"), OutParams.ApiID);

                            // ~Begin #VSP_only
                            const TSharedPtr<FJsonObject>* JsonLockObject;
                            if (PlaceObject->Get()->TryGetObjectField(TEXT("lock"), JsonLockObject))
                            {
                                JsonLockObject->Get()->TryGetBoolField(TEXT("locked"), OutParams.bIsLocked);
                                JsonLockObject->Get()->TryGetStringField(TEXT("text"), OutParams.LockText);
                            }
                            // ~End VSP_only

                            const TArray<TSharedPtr<FJsonValue>>* PointsArray;
                            if (ValueParamObject->Get()->TryGetArrayField(TEXT("points"), PointsArray))
                            {
                                for (auto& PointValue : *PointsArray)
                                {
                                    const TSharedPtr<FJsonObject>* PointObject;
                                    if (PointValue->TryGetObject(PointObject))
                                    {
                                        const TSharedPtr<FJsonObject>* ResultObject;
                                        if (PointObject->Get()->TryGetObjectField(TEXT("result"), ResultObject))
                                        {
                                            FString OptType;
                                            if (ResultObject->Get()->TryGetStringField(TEXT("opt_type"), OptType) && OptType == "scheme")
                                            {
                                                // Ids schemes
                                                const TArray<TSharedPtr<FJsonValue>>* SchemesArray;
                                                if (ResultObject->Get()->TryGetArrayField(TEXT("schemes"), SchemesArray))
                                                {
                                                    for (auto& SchemeValue : *SchemesArray)
                                                    {
                                                        const TSharedPtr<FJsonObject>* SchemeObject;
                                                        if (SchemeValue->TryGetObject(SchemeObject))
                                                        {
                                                            int32 ScmId;
                                                            if (SchemeObject->Get()->TryGetNumberField(TEXT("scm_id"), ScmId))
                                                            {
                                                                OutParams.SchemeIds.Add(ScmId);
                                                                UE_LOG(LogTemp, Log, TEXT("Found scm_id: %d"), ScmId);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            //return true;
                        }
                    }
                }
            }
        }

        if (Param.parameterName == "nav")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;

            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                const TSharedPtr<FJsonObject>* NavObject;
                if (JsonValue->TryGetObject(NavObject))
                {
                    // Check has button "back"
                    const TSharedPtr<FJsonObject>* BackObject;
                    if (NavObject->Get()->TryGetObjectField(TEXT("back"), BackObject))
                    {
                        FString Action;
                        if (BackObject->Get()->TryGetStringField(TEXT("action"), Action))
                        {
                            if (Action == "gotoBack")
                            {
                                OutParams.bHasBackButton = true;
                                UE_LOG(LogTemp, Log, TEXT("Found gotoBack action"));

                                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Found gotoBack action"));
                            }
                        }
                    }
                }
            }
        }
    }

    return !OutParams.ApiID.IsEmpty();
}

bool UCS_JsonParser::ParsePointsData(const FParametersObject& Parameters, TArray<FPlacePointData>& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "places")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;

            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                Global_ZOrder = 0;
                ParsedPointIds.Empty();

                const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
                JsonValue->TryGetArray(JsonValueArray);
                for (auto& Value : *JsonValueArray)
                {
                    const TSharedPtr<FJsonObject>* ValueParamObject;
                    if (Value->TryGetObject(ValueParamObject))
                    {
                        const TArray<TSharedPtr<FJsonValue>>* PointsValueArray;
                        if (ValueParamObject->Get()->TryGetArrayField(TEXT("points"), PointsValueArray))
                        {
                            for (const auto& Point : *PointsValueArray)
                            {
                                const TSharedPtr<FJsonObject>* PointParamObject;
                                if (Point->TryGetObject(PointParamObject))
                                {   

                                    FPlacePointData PointData;

                                    if (ParsePlacePointObject(*PointParamObject, PointData))
                                    {
                                        OutParams.Add(PointData);
                                    }
                                }
                            }
                        }
                    }
                }
                return true;
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseUpdatePointsData(const FParametersObject& Parameters, TArray<FPlacePointData>& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "points")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;

            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {

                Global_ZOrder = 0;
                ParsedPointIds.Empty();

                const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
                JsonValue->TryGetArray(JsonValueArray);
                for (auto& Value : *JsonValueArray)
                {
                    const TSharedPtr<FJsonObject>* ValueParamObject;
                    if (Value->TryGetObject(ValueParamObject))
                    {
                        const TArray<TSharedPtr<FJsonValue>>* PointsValueArray;
                        if (ValueParamObject->Get()->TryGetArrayField(TEXT("points"), PointsValueArray))
                        {
                            for (const auto& Point : *PointsValueArray)
                            {
                                const TSharedPtr<FJsonObject>* PointParamObject;
                                if (Point->TryGetObject(PointParamObject))
                                {

                                    FPlacePointData PointData;

                                    if (ParsePlacePointObject(*PointParamObject, PointData))
                                    {
                                        OutParams.Add(PointData);
                                    }
                                }
                            }
                        }


                        //NEEDED?
                        const TSharedPtr<FJsonObject>* SoundObject;
                        if (ValueParamObject->Get()->TryGetObjectField(TEXT("sound"), SoundObject))
                        {
                            FString SoundApiId = "";
                            FString SoundSnd = "";
                            SoundObject->Get()->TryGetStringField(TEXT("apiId"), SoundApiId);
                            SoundObject->Get()->TryGetStringField(TEXT("snd"), SoundSnd);

                            if (!SoundSnd.IsEmpty())
                            {
                                // Play sound?
                            }
                        }
                    }
                }

                return true;
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseDialogParameters(const FParametersObject& Parameters, FString& OutAPI_Id_Dialog, FString& OutNameTitle, FString& IconTitlePath,
    TArray<FDialogParameters>& OutDialog, TArray<FString>& OutQueueSounds, ETypeDialog& TypeDialog)
{
    for (const auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "data")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;

            if (!FJsonSerializer::Deserialize(Reader, JsonValue) || !JsonValue.IsValid())
                return false;

            const TSharedPtr<FJsonObject>* DialogObject;
            if (!JsonValue->TryGetObject(DialogObject))
                return false;

            const TSharedPtr<FJsonObject>* DialogJsonData;
            if (DialogObject->Get()->TryGetObjectField(TEXT("dialog"), DialogJsonData))
            {
                DialogJsonData->Get()->TryGetStringField(TEXT("apiId"), OutAPI_Id_Dialog);

                const TSharedPtr<FJsonObject>* DudeObject;
                if (DialogJsonData->Get()->TryGetObjectField(TEXT("dude"), DudeObject))
                {
                    DudeObject->Get()->TryGetStringField(TEXT("name"), OutNameTitle);
                    DudeObject->Get()->TryGetStringField(TEXT("img"), IconTitlePath);
                }

                const TArray<TSharedPtr<FJsonValue>>* MSGJsonData;
                if (DialogJsonData->Get()->TryGetArrayField(TEXT("out_msg"), MSGJsonData))
                {
                    for (auto& Value : *MSGJsonData)
                    {
                        const TSharedPtr<FJsonObject>* OutMSGParamObject;
                        if (Value->TryGetObject(OutMSGParamObject))
                        {
                            FDialogParameters DialogParams;
                            if (ParseDialogMessage(*OutMSGParamObject, DialogParams, OutQueueSounds))
                            {
                                OutDialog.Add(DialogParams);
                            }
                        }
                    }
                }
                const TSharedPtr<FJsonObject>* DstObject;
                if (DialogJsonData->Get()->TryGetObjectField(TEXT("dst"), DstObject))
                {
                    FString OptType;
                    if (DstObject->Get()->TryGetStringField(TEXT("opt_type"), OptType))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Dialog type: %s"), *OptType);

                        if (OptType == "phone") {
                            TypeDialog = ETypeDialog::Phone;
                        }
                        else if (OptType == "radio") {
                            TypeDialog = ETypeDialog::Radio;
                        }
                        else if (OptType == "logging") {
                            TypeDialog = ETypeDialog::Journal;
                        }
                        else if (OptType == "people") {
                            TypeDialog = ETypeDialog::People;
                        }
                        else {
                            TypeDialog = ETypeDialog::None;
                        }
                    }
                }
            }

            const TArray<TSharedPtr<FJsonValue>>* PointsJsonData;
            if (DialogObject->Get()->TryGetArrayField(TEXT("points"), PointsJsonData))
            {
                ParseDialogPoints(*PointsJsonData, OutDialog);
            }

            return true;
        }
    }

    return false;
}

bool UCS_JsonParser::ParseAddDialogMessageParameters(const FParametersObject& Parameters, TArray<FDialogParameters>& OutDialog, TArray<FString>& OutQueueSounds)
{
    for (const auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "messages")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;

            if (!FJsonSerializer::Deserialize(Reader, JsonValue) || !JsonValue.IsValid())
                return false;

            const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
            if (!JsonValue->TryGetArray(JsonValueArray))
                return false;

            for (auto& Value : *JsonValueArray)
            {
                const TSharedPtr<FJsonObject>* OutMSGParamObject;
                if (Value->TryGetObject(OutMSGParamObject))
                {
                    FDialogParameters DialogParameters;
                    if (ParseDialogMessage(*OutMSGParamObject, DialogParameters, OutQueueSounds))
                    {
                        OutDialog.Add(DialogParameters);
                    }
                }
            }
            return true;
        }
    }
    return false;
}

bool UCS_JsonParser::ParseUpdateDialogParameters(const FParametersObject& Parameters, TArray<FDialogParameters>& OutDialog)
{
    for (const auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "points")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;

            if (!FJsonSerializer::Deserialize(Reader, JsonValue) || !JsonValue.IsValid())
                return false;

            const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
            if (!JsonValue->TryGetArray(JsonValueArray))
                return false;

            return ParseDialogPoints(*JsonValueArray, OutDialog);
        }
    }
    return false;
}

bool UCS_JsonParser::ParseShowMessage(const FParametersObject& Parameters, FShowMessageParams& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "message")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;

            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                TSharedPtr<FJsonObject>* JsonObject;
                if (JsonValue->TryGetObject(JsonObject))
                {
                    JsonObject->Get()->TryGetNumberField(TEXT("alarm"), OutParams.Alarm);
                    JsonObject->Get()->TryGetStringField(TEXT("footer"), OutParams.Footer);
                    JsonObject->Get()->TryGetStringField(TEXT("header"), OutParams.Header);
                    JsonObject->Get()->TryGetStringField(TEXT("name"), OutParams.Name);
                    JsonObject->Get()->TryGetStringField(TEXT("text"), OutParams.Text);
                    JsonObject->Get()->TryGetBoolField(TEXT("modal"), OutParams.Modal);
                    

                    return true;
                }
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseMeasureDevices(const FParametersObject& Parameters, FDeviceAndPointsData& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "data")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;
            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                const TSharedPtr<FJsonObject>* DataParamObject;
                if (JsonValue->TryGetObject(DataParamObject))
                {
                    const TArray<TSharedPtr<FJsonValue>>* DevicesArrayJsonData;
                    if (DataParamObject->Get()->TryGetArrayField(TEXT("devices"), DevicesArrayJsonData))
                    {
                        TArray<FDeviceData> DevicesData;
                        for (auto& Value : *DevicesArrayJsonData)
                        {
                            const TSharedPtr<FJsonObject>* ValueParamObject;
                            if (Value->TryGetObject(ValueParamObject))
                            {
                                FDeviceData DeviceData;
                                ValueParamObject->Get()->TryGetStringField(TEXT("apiId"), DeviceData.DeviceId);
                                ValueParamObject->Get()->TryGetStringField(TEXT("name"), DeviceData.DeviceName);
                                const TArray<TSharedPtr<FJsonValue>>* ParamsJsonData;
                                if (ValueParamObject->Get()->TryGetArrayField(TEXT("mtypes"), ParamsJsonData))
                                {
                                    for (const auto& ParamDev : *ParamsJsonData)
                                    {
                                        const TSharedPtr<FJsonObject>* ParamValueObject;
                                        if (ParamDev->TryGetObject(ParamValueObject))
                                        {
                                            FDeviceParam DeviceParam;
                                            ParamValueObject->Get()->TryGetStringField(TEXT("id"), DeviceParam.Id);
                                            ParamValueObject->Get()->TryGetStringField(TEXT("name"), DeviceParam.Name);
                                            DeviceData.Param.Add(DeviceParam);
                                        }
                                    }
                                }
                                DevicesData.Add(DeviceData);
                            }
                        }
                        OutParams.DevicesData = DevicesData;
                    }
                    if (DataParamObject->Get()->TryGetArrayField(TEXT("points"), DevicesArrayJsonData))
                    {
                        TArray<FDeviceData> PointsData;
                        for (auto& Value : *DevicesArrayJsonData)
                        {
                            const TSharedPtr<FJsonObject>* ValueParamObject;
                            if (Value->TryGetObject(ValueParamObject))
                            {
                                FDeviceData PointData;
                                ValueParamObject->Get()->TryGetStringField(TEXT("apiId"), PointData.DeviceId);
                                ValueParamObject->Get()->TryGetStringField(TEXT("name"), PointData.DeviceName);
                                const TArray<TSharedPtr<FJsonValue>>* ParamsJsonData;
                                if (ValueParamObject->Get()->TryGetArrayField(TEXT("points"), ParamsJsonData))
                                {
                                    for (const auto& ParamDev : *ParamsJsonData)
                                    {
                                        const TSharedPtr<FJsonObject>* ParamValueObject;
                                        if (ParamDev->TryGetObject(ParamValueObject))
                                        {
                                            FDeviceParam PointParam;
                                            ParamValueObject->Get()->TryGetStringField(TEXT("apiId"), PointParam.Id);
                                            ParamValueObject->Get()->TryGetStringField(TEXT("name"), PointParam.Name);
                                            PointData.Param.Add(PointParam);
                                        }
                                    }
                                    PointsData.Add(PointData);
                                }
                            }
                            OutParams.PointsData = PointsData;
                        }
                    }
                    return true;
                }
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseShowMeasureResult(const FParametersObject& Parameters, float& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "result")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;
            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                const TSharedPtr<FJsonObject>* PointParamObject;
                if (JsonValue->TryGetObject(PointParamObject))
                {
                    if (PointParamObject->Get()->TryGetNumberField(TEXT("result"), OutParams))
                        return true;
                }
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseShowReaction(const FParametersObject& Parameters, FString& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "reaction")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;

            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                TSharedPtr<FJsonObject>* JsonObject;
                JsonValue->TryGetObject(JsonObject);

                if (JsonObject->Get()->TryGetStringField(TEXT("text"), OutParams))
                    return true;
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseShowStepInfo(const FParametersObject& Parameters, int& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "steps")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;
            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                TSharedPtr<FJsonObject>* JsonObject;
                JsonValue->TryGetObject(JsonObject);
                
                if (JsonObject->Get()->TryGetNumberField(TEXT("countClosed"), OutParams))
                    return true;
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseShowLearningSticker(const FParametersObject& Parameters, FStickerParams& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "sticker")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;
            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                TSharedPtr<FJsonObject>* JsonObject;
                JsonValue->TryGetObject(JsonObject);
                JsonObject->Get()->TryGetStringField(TEXT("header"), OutParams.Header);
                JsonObject->Get()->TryGetStringField(TEXT("text"), OutParams.Text);
                JsonObject->Get()->TryGetNumberField(TEXT("penalty"), OutParams.Penalty);

                return true;
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseAddAction(const FParametersObject& Parameters, FString& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "action")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;
            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                TSharedPtr<FJsonObject>* JsonObject;
                if (JsonValue->TryGetObject(JsonObject))
                {
                    FString ApiId;
                    if (JsonObject->Get()->TryGetStringField(TEXT("apiId"), ApiId))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("API ID: %s"), *ApiId);
                    }

                    if (JsonObject->Get()->TryGetStringField(TEXT("name"), OutParams))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Action name: %s"), *OutParams);
                        return true;
                    }

                }
            }
        }

    }

    return false;
}

bool UCS_JsonParser::ParsePlaySound(const FParametersObject& Parameters, FString& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "sound")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;
            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                TSharedPtr<FJsonObject>* JsonObject;
                if (JsonValue->TryGetObject(JsonObject))
                {
                    if (JsonObject->Get()->TryGetStringField(TEXT("apiId"), OutParams))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("API ID: %s"), *OutParams);
                        //GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Sound id - %s"), *OutParams));
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseShowObjectMenu(const FParametersObject& Parameters, FFullMenuReasons& OutParams)
{
    for (const FParameters& Parameter : Parameters.Parameters)
    {
        if (Parameter.parameterName == "data")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Parameter.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;
            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                const TSharedPtr<FJsonObject>* MenuObject;
                if (JsonValue->TryGetObject(MenuObject))
                {
                    const TSharedPtr<FJsonObject>* ReasonsJsonData;
                    if (MenuObject->Get()->TryGetObjectField(TEXT("reasons"), ReasonsJsonData))
                    {
                        ParseRecursionFillFullMenuReasons(ReasonsJsonData->Get(), OutParams);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseShowResult(const FParametersObject& Parameters, FShowResultParams& OutParams)
{
    for (const FParameters& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "result")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;

            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                const TSharedPtr<FJsonObject>* ResultObject;
                if (JsonValue->TryGetObject(ResultObject))
                {
                    ResultObject->Get()->TryGetStringField(TEXT("eval"), OutParams.Eval);
                    ResultObject->Get()->TryGetStringField(TEXT("text"), OutParams.EvalDescription);

                    const TArray<TSharedPtr<FJsonValue>>* ResultJsonData;
                    
                    if (ResultObject->Get()->TryGetArrayField(TEXT("result"), ResultJsonData))
                    {
                        for (const TSharedPtr<FJsonValue>& Result : *ResultJsonData)
                        {
                            const TSharedPtr<FJsonObject>* ParametersObject;
                            
                            if (Result->TryGetObject(ParametersObject))
                            {
                                FResultParams ResultParams = FResultParams();
                                ParametersObject->Get()->TryGetNumberField(TEXT("penalty"), ResultParams.Penalty);
                                ParametersObject->Get()->TryGetStringField(TEXT("name"), ResultParams.Name);
                                FString MSG = "";
                                const TArray<TSharedPtr<FJsonValue>>* MSGL1_JsonData;
                                
                                if (ParametersObject->Get()->TryGetArrayField(TEXT("msg"), MSGL1_JsonData))
                                {
                                    for (const TSharedPtr<FJsonValue>& MSG1_Layer : *MSGL1_JsonData)
                                    {
                                        FString Msg2;
                                        if (MSG1_Layer->TryGetString(Msg2))
                                            MSG = MSG + "\n" + Msg2;
                                        const TSharedPtr<FJsonObject>* MSG2_JsonObject;
                                        
                                        if (MSG1_Layer->TryGetObject(MSG2_JsonObject))
                                        {
                                            FString MSG1_Name = "";
                                            if (MSG2_JsonObject->Get()->TryGetStringField(TEXT("name"), MSG1_Name))
                                                MSG = MSG + "" + MSG1_Name;
                                            const TArray<TSharedPtr<FJsonValue>>* MSGL2_JsonData;
                                            
                                            if (MSG2_JsonObject->Get()->TryGetArrayField(TEXT("msg"), MSGL2_JsonData))
                                            {
                                                for (const TSharedPtr<FJsonValue>& MSG2_Layer : *MSGL2_JsonData)
                                                {
                                                    FString Msg3;
                                                    if (MSG2_Layer->TryGetString(Msg3))
                                                        MSG = MSG + "\n" + Msg3 + "";
                                                }
                                            }
                                        }
                                    }
                                }
                                ResultParams.MSG = UCS_UtilityFunctionLibrary::ReplaceSubstring(MSG, "&emsp;", " ");
                                ResultParams.bIsHasMessage = !ResultParams.MSG.IsEmpty();
                                OutParams.Results.Add(ResultParams);
                            }
                        }
                    }
                }
            }

            return true;
        }
    }

    return false;
}

bool UCS_JsonParser::ParseShowResultWithShowedNesting(const FParametersObject& Parameters, FShowResultParams& OutParams)
{
    for (const FParameters& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "result")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;

            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                const TSharedPtr<FJsonObject>* ResultObject;
                if (JsonValue->TryGetObject(ResultObject))
                {
                    ResultObject->Get()->TryGetStringField(TEXT("eval"), OutParams.Eval);
                    ResultObject->Get()->TryGetStringField(TEXT("text"), OutParams.EvalDescription);

                    const TArray<TSharedPtr<FJsonValue>>* ResultJsonData;
                    if (ResultObject->Get()->TryGetArrayField(TEXT("result"), ResultJsonData))
                    {
                        for (const TSharedPtr<FJsonValue>& Result : *ResultJsonData)
                        {
                            const TSharedPtr<FJsonObject>* ParametersObject;
                            if (Result->TryGetObject(ParametersObject))
                            {
                                ParseRecursionResultParams(ParametersObject->Get(), OutParams.Results);
                            }
                        }
                    }
                }
            }
            return true;
        }
    }

    return false;
}

bool UCS_JsonParser::ParseWalkieParams(const FParametersObject& Parameters, TArray<FDialogButtonParameters>& OutParams, const FString& Subkey)
{
    FString ApiID;

    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "apiId")
        {   
            ApiID = Param.parameterValue;
            for (auto& ParamParam : Parameters.Parameters)
            {
                if (ApiID.Contains(Subkey)) {
                    if (ParamParam.parameterName == "points")
                    {
                        const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(ParamParam.parameterValue);
                        TSharedPtr<FJsonValue> JsonValue;
                        if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
                        {
                            const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
                            if (JsonValue->TryGetArray(JsonValueArray))
                            {
                                for (auto& Value : *JsonValueArray)
                                {
                                    const TSharedPtr<FJsonObject>* ValueParamObject;
                                    if (Value->TryGetObject(ValueParamObject))
                                    {
                                        FDialogButtonParameters WalkieParam = FDialogButtonParameters();
                                        ValueParamObject->Get()->TryGetStringField(TEXT("apiId"), WalkieParam.IdDialog);
                                        ValueParamObject->Get()->TryGetStringField(TEXT("name"), WalkieParam.Description);
                                        ValueParamObject->Get()->TryGetStringField(TEXT("icon"), WalkieParam.IconPath);

                                        OutParams.Add(WalkieParam);
                                    }
                                }
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseDialogPointsParams(const FParametersObject& Parameters, TArray<FDialogButtonParameters>& OutParams)
{
    FString ApiID;

    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "apiId")
        {
            ApiID = Param.parameterValue;
            for (auto& ParamParam : Parameters.Parameters)
            {
                if (ParamParam.parameterName == "points")
                {
                    const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(ParamParam.parameterValue);
                    TSharedPtr<FJsonValue> JsonValue;
                    if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
                    {
                        const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
                        if (JsonValue->TryGetArray(JsonValueArray))
                        {
                            for (auto& Value : *JsonValueArray)
                            {
                                const TSharedPtr<FJsonObject>* ValueParamObject;
                                if (Value->TryGetObject(ValueParamObject))
                                {
                                    FDialogButtonParameters WalkieParam = FDialogButtonParameters();
                                    ValueParamObject->Get()->TryGetStringField(TEXT("apiId"), WalkieParam.IdDialog);
                                    ValueParamObject->Get()->TryGetStringField(TEXT("name"), WalkieParam.Description);
                                    ValueParamObject->Get()->TryGetStringField(TEXT("icon"), WalkieParam.IconPath);
                                    ValueParamObject->Get()->TryGetStringField(TEXT("action"), WalkieParam.Action);

                                    OutParams.Add(WalkieParam);
                                }
                            }
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParsePhoneParams(const FParametersObject& Parameters, TMap<FString,TArray<FDialogButtonParameters>>& OutParams, const FString& Subkey)
{
    bool FindSomeDialog = false;
    FString ApiID;

    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "apiId")
        {
            ApiID = Param.parameterValue;

            TArray<FDialogButtonParameters> DialogPoints;

            for (auto& ParamParam : Parameters.Parameters)
            {
                if (ApiID.Contains(Subkey)) {
                    if (ParamParam.parameterName == "points")
                    {
                        const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(ParamParam.parameterValue);
                        TSharedPtr<FJsonValue> JsonValue;
                        if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
                        {
                            const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
                            if (JsonValue->TryGetArray(JsonValueArray))
                            {
                                for (auto& Value : *JsonValueArray)
                                {
                                    const TSharedPtr<FJsonObject>* ValueParamObject;
                                    if (Value->TryGetObject(ValueParamObject))
                                    {
                                        FDialogButtonParameters WalkieParam = FDialogButtonParameters();
                                        ValueParamObject->Get()->TryGetStringField(TEXT("apiId"), WalkieParam.IdDialog);
                                        ValueParamObject->Get()->TryGetStringField(TEXT("name"), WalkieParam.Description);
                                        ValueParamObject->Get()->TryGetStringField(TEXT("icon"), WalkieParam.IconPath);

                                        DialogPoints.Add(WalkieParam);
                                    }
                                }
                                FindSomeDialog = true;
                            }
                        }
                    }
                    OutParams.Add(ApiID, DialogPoints);
                }
            }
        }
    }

    return FindSomeDialog;
}

bool UCS_JsonParser::ParseContextMenu(const FParametersObject& Parameters, FString& ApiID, TArray<FContextMenuItem>& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "apiId")
        {
            ApiID = Param.parameterValue;
            for (auto& PointParam : Parameters.Parameters)
            {
                if (PointParam.parameterName == "points")
                {
                    const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(PointParam.parameterValue);
                    TSharedPtr<FJsonValue> JsonValue;
                    if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
                    {
                        const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
                        JsonValue->TryGetArray(JsonValueArray);
                        for (auto& Value : *JsonValueArray)
                        {
                            const TSharedPtr<FJsonObject>* ValueParamObject;
                            if (Value->TryGetObject(ValueParamObject))
                            {
                                FContextMenuItem ContextMenuItem = FContextMenuItem();
                                ValueParamObject->Get()->TryGetStringField(TEXT("apiId"), ContextMenuItem.ApiID);
                                ValueParamObject->Get()->TryGetStringField(TEXT("name"), ContextMenuItem.Name);
                                ValueParamObject->Get()->TryGetStringField(TEXT("tool"), ContextMenuItem.Tool);

                                //if (!ContextMenuItem.ApiID.IsEmpty() && !ContextMenuItem.Name.IsEmpty() && !ContextMenuItem.Tool.IsEmpty())
                                if (!ContextMenuItem.ApiID.IsEmpty() && !ContextMenuItem.Name.IsEmpty())
                                    OutParams.Add(ContextMenuItem);
                            }
                        }

                        if (OutParams.Num() > 0)
                            return true;
                    }
                }
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseViewObject(const TSharedPtr<FJsonObject>* ViewJsonObject, FObjectViewID& OutParams)
{
    const TSharedPtr<FJsonObject>* ViewObject;
    if (ViewJsonObject->Get()->TryGetObjectField(TEXT("view"), ViewObject))
    {
        if (ViewObject->Get()->TryGetStringField(TEXT("apiId"), OutParams.ViewID))
        {
            ViewObject->Get()->TryGetStringField(TEXT("desc"), OutParams.DecsID);
        }
    }

    if (ViewJsonObject->Get()->TryGetStringField(TEXT("apiId"), OutParams.PointID))
    {
        const TArray<TSharedPtr<FJsonValue>>* StatesJsonArray;
        if (ViewJsonObject->Get()->TryGetArrayField(TEXT("st_objs"), StatesJsonArray))
        {
            for (const TSharedPtr<FJsonValue>& StateValue : *StatesJsonArray)
            {
                if (StateValue->Type == EJson::String)
                {
                    FString StateString = StateValue->AsString();
                    OutParams.States.Add(StateString);
                }
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool UCS_JsonParser::ParseUpdatePlaceLock(const FParametersObject& Parameters, FLocation& OutParams)
{
    for (const FParameters& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "places")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;
            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
                if (JsonValue->TryGetArray(JsonValueArray))
                {
                    for (const TSharedPtr<FJsonValue>& Value : *JsonValueArray)
                    {
                        const TSharedPtr<FJsonObject>* ValueParamObject;
                        if (Value->TryGetObject(ValueParamObject))
                        {
                            ValueParamObject->Get()->TryGetStringField(TEXT("place"), OutParams.ApiID);
                            
                            const TSharedPtr<FJsonObject>* JsonLockObject;
                            if (ValueParamObject->Get()->TryGetObjectField(TEXT("lock"), JsonLockObject))
                            {
                                JsonLockObject->Get()->TryGetBoolField(TEXT("locked"), OutParams.bIsLocked);
                                JsonLockObject->Get()->TryGetStringField(TEXT("text"), OutParams.LockText);
                            }
                        }
                    }
                    
                    return true;
                }
            }
        }
    }

    return false;
}

bool UCS_JsonParser::ParseSchemes(const FParametersObject& Parameters, TArray<FLoadingImageInfo>& OutParams)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "scheme")
        {
            const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Param.parameterValue);
            TSharedPtr<FJsonValue> JsonValue;

            if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
            {
                const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
                if (JsonValue->TryGetArray(JsonValueArray))
                {
                    for (auto& Value : *JsonValueArray)
                    {
                        const TSharedPtr<FJsonObject>* ValueParamObject;
                        if (Value->TryGetObject(ValueParamObject))
                        {
                            FLoadingImageInfo SchemeParam = FLoadingImageInfo();

                            ValueParamObject->Get()->TryGetStringField(TEXT("apiId"), SchemeParam.ApiID);
                            ValueParamObject->Get()->TryGetStringField(TEXT("img"), SchemeParam.Path);
                            ValueParamObject->Get()->TryGetStringField(TEXT("name"), SchemeParam.Name);
                            ValueParamObject->Get()->TryGetStringField(TEXT("num"), SchemeParam.Num);

                            OutParams.Add(SchemeParam);

                            UE_LOG(LogTemp, Log, TEXT("(ShowScheme) Found scm_id: %s"), *SchemeParam.Num);
                        }
                    }
                    return true;
                }
            }
        }
    }

    return false;
}

bool UCS_JsonParser::FindApiID(const FParametersObject& Parameters, FString& OutApiID)
{
    for (auto& Param : Parameters.Parameters)
    {
        if (Param.parameterName == "apiId")
        {
            OutApiID = Param.parameterValue;
            return true;
        }
    }

    return false;
}

bool UCS_JsonParser::ParseDialogMessage(const TSharedPtr<FJsonObject>& OutMSGParamObject, FDialogParameters& OutDialogParams, TArray<FString>& OutQueueSounds)
{
    const TSharedPtr<FJsonObject>* CharacterObject;
    if (OutMSGParamObject->TryGetObjectField(TEXT("character"), CharacterObject))
    {
        CharacterObject->Get()->TryGetStringField(TEXT("opt_type"), OutDialogParams.IdDialog);
    }

    const TSharedPtr<FJsonObject>* SoundObject;
    if (OutMSGParamObject->TryGetObjectField(TEXT("sound"), SoundObject))
    {
        FString SoundApiId;
        if (SoundObject->Get()->TryGetStringField(TEXT("apiId"), SoundApiId))
        {
            OutQueueSounds.Add(SoundApiId);
            UE_LOG(LogTemp, Warning, TEXT("Sound apiId: %s"), *SoundApiId);
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("ID Sound : %s"), *SoundApiId));
        }
    }

    if (OutDialogParams.IdDialog == "dude")
    {
        OutMSGParamObject->TryGetStringField(TEXT("msg"), OutDialogParams.Message);
        OutDialogParams.DialogElement = EDialogElement::Dude;
        return true;
    }
    else if (OutDialogParams.IdDialog == "user")
    {
        OutMSGParamObject->TryGetStringField(TEXT("msg"), OutDialogParams.Message);
        OutDialogParams.DialogElement = EDialogElement::User;
        return true;
    }

    return false;
}

bool UCS_JsonParser::ParseDialogPoints(const TArray<TSharedPtr<FJsonValue>>& PointsJsonData, TArray<FDialogParameters>& OutDialog)
{
    for (auto& Value : PointsJsonData)
    {
        const TSharedPtr<FJsonObject>* ValueParamObject;
        if (Value->TryGetObject(ValueParamObject))
        {
            FDialogParameters DialogParameters = FDialogParameters();
            ValueParamObject->Get()->TryGetStringField(TEXT("apiId"), DialogParameters.IdDialog);
            ValueParamObject->Get()->TryGetStringField(TEXT("name"), DialogParameters.Message);
            DialogParameters.DialogElement = EDialogElement::Message;

            const TSharedPtr<FJsonObject>* ResultObject;
            if (ValueParamObject->Get()->TryGetObjectField(TEXT("result"), ResultObject))
            {
                FString OptType;
                if (ResultObject->Get()->TryGetStringField(TEXT("opt_type"), OptType))
                {
                    DialogParameters.OptType = OptType;
                }
            }

            OutDialog.Add(DialogParameters);
        }
    }
    return true;
}

void UCS_JsonParser::ParseRecursionFillFullMenuReasons(const FJsonObject* ReasonsJsonData, FFullMenuReasons& FullMenuReason)
{
    ReasonsJsonData->TryGetStringField(TEXT("name"), FullMenuReason.Name);
    ReasonsJsonData->TryGetStringField(TEXT("apiId"), FullMenuReason.API_Id);
    ReasonsJsonData->TryGetNumberField(TEXT("alarm"), FullMenuReason.Alarm);
    ReasonsJsonData->TryGetStringField(TEXT("result"), FullMenuReason.Result);
    ReasonsJsonData->TryGetStringField(TEXT("attempt"), FullMenuReason.Attempt);
    ReasonsJsonData->TryGetNumberField(TEXT("att_used"), FullMenuReason.Att_Used);
    ReasonsJsonData->TryGetBoolField(TEXT("closed"), FullMenuReason.bIsClosed);
    ReasonsJsonData->TryGetBoolField(TEXT("checked"), FullMenuReason.bIsChecked);

    const TArray<TSharedPtr<FJsonValue>>* Reasons;
    if (ReasonsJsonData->TryGetArrayField(TEXT("reasons"), Reasons))
    {
        for (const TSharedPtr<FJsonValue>& Reason : *Reasons)
        {
            const TSharedPtr<FJsonObject>* JsonValue;
            if (Reason->TryGetObject(JsonValue))
            {
                FFullMenuReasons NestedReason = FFullMenuReasons();
                ParseRecursionFillFullMenuReasons(JsonValue->Get(), NestedReason);
                FullMenuReason.AddNestedReason(NestedReason);
            }
        }
    }
}

void UCS_JsonParser::ParseRecursionResultParams(const FJsonObject* ReasonsJsonData, TArray<FResultParams>& ResultParams)
{
    FResultParams OutResultParams = FResultParams();
    
    ReasonsJsonData->TryGetNumberField(TEXT("penalty"), OutResultParams.Penalty);
    ReasonsJsonData->TryGetStringField(TEXT("name"), OutResultParams.Name);

    const TArray<TSharedPtr<FJsonValue>>* JsonDataArray;

    if (ReasonsJsonData->TryGetArrayField(TEXT("msg"), JsonDataArray))
    {
        TArray<FResultParams> NestedResults;
        
        for (const TSharedPtr<FJsonValue>& JsonValue : *JsonDataArray)
        {
            if(const TSharedPtr<FJsonObject>* ResultObject;
                JsonValue->TryGetObject(ResultObject))
            {
                OutResultParams.bIsHasMessage = false;
                
                ParseRecursionResultParams(ResultObject->Get(), NestedResults);
            }
            else if (FString MSG = "";
                JsonValue->TryGetString(MSG))
            {
                OutResultParams.MSG = OutResultParams.MSG + "\n" +
                    UCS_UtilityFunctionLibrary::ReplaceSubstring(MSG, "&emsp;", " ");
                OutResultParams.bIsHasMessage = true;
            }
        }
        ResultParams.Add(OutResultParams);
        
        for (auto& NestedResult : NestedResults)
        {
            ResultParams.Add(NestedResult);
        }
    }
}

bool UCS_JsonParser::ParsePlacePointObject(const TSharedPtr<FJsonObject>& PointParamObject, FPlacePointData& OutPointData, const FPlacePointData& ParentPointData)
{
    PointParamObject->TryGetStringField(TEXT("apiId"), OutPointData.ApiID);

    if (ParsedPointIds.Contains(OutPointData.ApiID))
    {
        return false;
    }

    PointParamObject->TryGetStringField(TEXT("icon"), OutPointData.IconPath);

    PointParamObject->TryGetStringField(TEXT("name"), OutPointData.Name);

    OutPointData.ViewStruct.PointID = OutPointData.ApiID;

    ParseViewAndChildData(PointParamObject, OutPointData, ParentPointData);

    return true;
}

void UCS_JsonParser::ParseViewAndChildData(const TSharedPtr<FJsonObject>& PointParamObject, FPlacePointData& OutPointData, const FPlacePointData& ParentPointData)
{
    const TSharedPtr<FJsonObject>* ViewObject;
    if (PointParamObject->TryGetObjectField(TEXT("view"), ViewObject))
    {
        ViewObject->Get()->TryGetStringField(TEXT("apiId"), OutPointData.ViewStruct.ViewID);

        if (!OutPointData.ViewStruct.ViewID.IsEmpty()) {

            ViewObject->Get()->TryGetStringField(TEXT("desc"), OutPointData.ViewStruct.DecsID);

            FString CssStyle = "";

            ViewObject->Get()->TryGetStringField(TEXT("cssstyle"), CssStyle);

            OutPointData.ViewStruct.ZOrder = 1 + ParentPointData.ViewStruct.ZOrder + Global_ZOrder;
            OutPointData.ViewStruct.Z_CSS = ExtractZIndexSimple(CssStyle) + ParentPointData.ViewStruct.Z_CSS;

            Global_ZOrder++;

            UE_LOG(LogTemp, Display, TEXT("VIEWID: %s, Z: %d"), *(OutPointData.ViewStruct.ViewID), OutPointData.ViewStruct.ZOrder);
        }
    }

    const TSharedPtr<FJsonObject>* LocObject;
    if (PointParamObject->TryGetObjectField(TEXT("loc"), LocObject))
    {
        LocObject->Get()->TryGetNumberField(TEXT("left"), OutPointData.ViewStruct.OffsetX);
        LocObject->Get()->TryGetNumberField(TEXT("top"), OutPointData.ViewStruct.OffsetY);
        LocObject->Get()->TryGetNumberField(TEXT("width"), OutPointData.ViewStruct.SizeX);
        LocObject->Get()->TryGetNumberField(TEXT("height"), OutPointData.ViewStruct.SizeY);

        OutPointData.ViewStruct.OffsetX += ParentPointData.ViewStruct.OffsetX;
        OutPointData.ViewStruct.OffsetY += ParentPointData.ViewStruct.OffsetY;
    }

    const TArray<TSharedPtr<FJsonValue>>* StatesJsonArray;
    if (PointParamObject->TryGetArrayField(TEXT("st_objs"), StatesJsonArray))
    {
        for (const TSharedPtr<FJsonValue>& StateValue : *StatesJsonArray)
        {
            if (StateValue->Type == EJson::String)
            {
                OutPointData.ViewStruct.States.Add(StateValue->AsString());
            }
        }
    }

    const TArray<TSharedPtr<FJsonValue>>* ChildsValueArray;
    if (PointParamObject->TryGetArrayField(TEXT("childs"), ChildsValueArray))
    {
        for (const auto& Child : *ChildsValueArray)
        {
            const TSharedPtr<FJsonObject>* ChildObject;
            if (Child->TryGetObject(ChildObject))
            {
                FPlacePointData ChildPointData;
                if (ParsePlacePointObject(*ChildObject, ChildPointData, OutPointData))
                {
                    OutPointData.ChildPoints.Add(ChildPointData);
                }
            }
        }
    }
}

int32 UCS_JsonParser::ExtractZIndexSimple(const FString& InputString)
{
    TArray<FString> Parts;
    InputString.ParseIntoArray(Parts, TEXT(":"), true);

    if (Parts.Num() == 2 && Parts[0].TrimStartAndEnd().Equals(TEXT("z-index")))
    {   
        return FCString::Atoi(*Parts[1].TrimStartAndEnd());
    }

    return 0;
}
