// Core Simulator


#include "Framework/CS_UtilityFunctionLibrary.h"
#include "Framework/CS_CoreTypes.h"
#include "JsonObjectConverter.h"
#include "Engine/AssetManager.h"
#include "Framework/CS_DeveloperSettings.h"

FString UCS_UtilityFunctionLibrary::GetStartLocation()
{
	return GetDefault<UCS_DeveloperSettings>()->StartLocation;
}

bool UCS_UtilityFunctionLibrary::ReadFromStorage(const FString& StorageName, FString& OutData)
{
	FString Path;
#if WITH_EDITOR
	Path = FPaths::ProjectSavedDir() + "UserData/" + StorageName + ".json";
#else
	Path = FPaths::GameSourceDir() + "UserData/" + StorageName + ".json";
#endif
	return FFileHelper::LoadFileToString(OutData, *Path);
}

void UCS_UtilityFunctionLibrary::SaveToStorage(const FString& StorageName, const FString& Data)
{
	FString Path;
#if WITH_EDITOR
	Path = FPaths::ProjectSavedDir() + "UserData/" + StorageName + ".json";
#else
	Path = FPaths::GameSourceDir() + "UserData/" + StorageName + ".json";
#endif
	
	FFileHelper::SaveStringToFile(Data, *Path);
}

bool UCS_UtilityFunctionLibrary::RunInEditor()
{
	bool RunInEditor = true;
#if WITH_EDITOR
	RunInEditor = true;
#else
	RunInEditor = false;
#endif

	return RunInEditor;
}

FString UCS_UtilityFunctionLibrary::FBodyOfAnswerToString(const FBodyOfAnswer& BodyOfAnswer)
{
	TSharedPtr<FJsonObject> JsonObj = FJsonObjectConverter::UStructToJsonObject<FBodyOfAnswer>(BodyOfAnswer);
	TSharedRef<FJsonValueObject> JsonValue = MakeShareable( new FJsonValueObject(JsonObj));

	FString JsonString = "";
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	
	FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);
	return JsonString;
}

FString UCS_UtilityFunctionLibrary::ReplaceSubstring(const FString& SourceString, const FString& SearchString,
	const FString& ReplaceString)
{
	FString ResultString = SourceString;

	// Находим первое вхождение строки, которую мы хотим заменить
	int32 Index = ResultString.Find(SearchString);
	while (Index != INDEX_NONE)
	{
		// Заменяем найденное вхождение на новую строку
		ResultString = ResultString.Left(Index) + ReplaceString + ResultString.RightChop(Index + SearchString.Len());

		// Продолжаем поиск следующего вхождения
		Index = ResultString.Find(SearchString, ESearchCase::CaseSensitive, ESearchDir::FromStart, Index + ReplaceString.Len());
	}

	return ResultString;
}

bool UCS_UtilityFunctionLibrary::CallFunctionByName(UObject* Object, FString FunctionName, const FVariables& Variables)
{
    if (Object)
    {
        if (UFunction* Function = Object->FindFunction(FName(*FunctionName)))
        {
            // Проверяем сигнатуру функции
            TArray<FProperty*> InputParams;
            for (TFieldIterator<FProperty> It(Function); It && (It->PropertyFlags & CPF_Parm); ++It)
            {
                if (!(It->PropertyFlags & CPF_OutParm))
                {
                    InputParams.Add(*It);
                }
            }

            // Если функция не принимает параметров
            if (InputParams.Num() == 0)
            {
                Object->ProcessEvent(Function, nullptr);
                return true;
            }
            // Если функция принимает один параметр типа FVariables
            else if (InputParams.Num() == 1)
            {
                FProperty* FirstParam = InputParams[0];
                if (FirstParam->GetClass() == FStructProperty::StaticClass())
                {
                    FStructProperty* StructProp = CastField<FStructProperty>(FirstParam);
                    if (StructProp->Struct == FVariables::StaticStruct())
                    {
                        // Выделяем память для параметра
                        uint8* Params = (uint8*)FMemory_Alloca(Function->ParmsSize);
                        FMemory::Memzero(Params, Function->ParmsSize);

                        // Копируем структуру
                        FVariables* VariablesPtr = StructProp->ContainerPtrToValuePtr<FVariables>(Params);
                        *VariablesPtr = Variables;

                        Object->ProcessEvent(Function, Params);

                        // Очищаем память
                        FirstParam->DestroyValue_InContainer(Params);

                        return true;
                    }
                }
            }
        }
    }
    return false;
}

TArray<FString> UCS_UtilityFunctionLibrary::GetCallableFunctionNames(UObject* Object, bool bBlueprintOnly)
{
    TArray<FString> Result;

    if (!Object)
    {
        return Result;
    }

    for (TFieldIterator<UFunction> It(Object->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
    {
        UFunction* Function = *It;

        if (bBlueprintOnly && Function->HasAnyFunctionFlags(FUNC_Native))
            continue;

        TArray<FProperty*> InputParams;
        for (TFieldIterator<FProperty> ParamIt(Function); ParamIt && (ParamIt->PropertyFlags & CPF_Parm); ++ParamIt)
        {
            if (!(ParamIt->PropertyFlags & CPF_OutParm))
            {
                InputParams.Add(*ParamIt);
            }
        }

        bool bIsValid = false;

        if (InputParams.Num() == 0)
        {
            bIsValid = true;
        }
        else if (InputParams.Num() == 1)
        {
            FProperty* FirstParam = InputParams[0];
            if (FStructProperty* StructProp = CastField<FStructProperty>(FirstParam))
            {
                if (StructProp->Struct == FVariables::StaticStruct())
                {
                    bIsValid = true;
                }
            }
        }

        if (bIsValid)
        {
            Result.Add(Function->GetName());
        }
    }

    return Result;
}

FString UCS_UtilityFunctionLibrary::RemoveSlashSuffix(const FString& InputString)
{
    if (InputString.IsEmpty())
    {
        return InputString;
    }

    // Ищем позицию первого символа "/"
    int32 SlashPosition = InputString.Find(TEXT("/"));

    // Если "/" не найден, возвращаем исходную строку
    if (SlashPosition == INDEX_NONE)
    {
        return InputString;
    }

    // Возвращаем левую часть строки до первого "/"
    return InputString.Left(SlashPosition);
}

int32 UCS_UtilityFunctionLibrary::CountLettersOnly(const FString& Text)
{
    int32 LetterCount = 0;

    for (TCHAR Ch : Text)
    {
        if (FChar::IsAlpha(Ch))
        {
            LetterCount++;
        }
    }

    return LetterCount;
}

FText UCS_UtilityFunctionLibrary::DecodeHtmlEntities(const FString& InputString)
{
    FString Result = InputString;
    Result = ReplaceSubstring(Result, "&quot;", "\"");
    Result = ReplaceSubstring(Result, "&amp;", "&");
    Result = ReplaceSubstring(Result, "&apos;", "'");
    Result = ReplaceSubstring(Result, "&lt;", "<");
    Result = ReplaceSubstring(Result, "&gt;", ">");
    Result = ReplaceSubstring(Result, "&#34;", "\"");

    return FText::FromString(Result);
}
