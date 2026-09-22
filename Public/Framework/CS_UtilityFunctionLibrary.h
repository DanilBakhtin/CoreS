// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_UtilityFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CORE_SIMULATOR_API UCS_UtilityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//Метод для получения стартовой локации (используеться как заглушка, для начала проходения курса)
	UFUNCTION(BlueprintCallable)
	static FString GetStartLocation();
	//Метод для чтения json файла находящегося в директории проекта
	UFUNCTION(BlueprintCallable)
	static bool ReadFromStorage(const FString& StorageName, FString& OutData);
	//Метод для создания/перезаписи json файла находящегося в директории проекта
	UFUNCTION(BlueprintCallable)
	static void SaveToStorage(const FString& StorageName, const FString& Data);
	//Метод позволяющий реализовывать логику в блупринтах  только в редакторе 
	UFUNCTION(BlueprintCallable, meta =(ExpandBoolAsExecs = "ReturnValue"))
	static bool RunInEditor();

	//Метод который подготавливает шаблонный ответ клиенту 
	static FString FBodyOfAnswerToString(const FBodyOfAnswer& BodyOfAnswer);

	//Метод для замены части текста в FString
	UFUNCTION(BlueprintCallable)
	static FString ReplaceSubstring(const FString& SourceString, const FString& SearchString, const FString& ReplaceString);

	//Функция для вызова функции/ивента по имени внутри объекта
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Object", HideSelfPin = "true"), Category = "CS_UtilityFunctionLibrary")
	static bool CallFunctionByName(UObject* Object, FString FunctionName, const FVariables& Variables);

	UFUNCTION()
	static TArray<FString> GetCallableFunctionNames(UObject* Object, bool bBlueprintOnly = false);

	UFUNCTION(BlueprintPure)
	static FString RemoveSlashSuffix(const FString& InputString);

	UFUNCTION(BlueprintPure)
	static int32 CountLettersOnly(const FString& Text);

	UFUNCTION(BlueprintPure)
	static FText DecodeHtmlEntities(const FString& InputString);
	
};
