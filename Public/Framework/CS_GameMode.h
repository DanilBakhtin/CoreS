// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Framework/CS_CoreTypes.h"
#include "Framework/CS_GameMode_Interface.h"
#include "CS_GameMode.generated.h"

class AWebSocketServer;
class ACS_Character;
class ACS_PlayerController;
class UCS_JsonParser;
class AGameStateBase;

UCLASS()
class CORE_SIMULATOR_API ACS_GameMode : public AGameModeBase, public ICS_GameMode_Interface
{
	GENERATED_BODY()

public:
	ACS_GameMode();

	virtual void BeginPlay() override;
	virtual void StartPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_Params")
	TSubclassOf<ACS_Character> CharacterClass;
		
	void UseMethod(const FString& Method, const FParametersObject&  Parameters);
	void UseReactMethod(const FString& Method, const FParametersObject&  Parameters);
	TMap<FString, TFunction<void(const FParametersObject&  Parameters)>> DataFunction;

	//Методы для взаимодействия с объектами на сцене
	void FindAndUseObjectById(const FString& IdObject, const FString& DescriptionObject);
	void FindAndUseViewById(const FObjectViewID& StructObjectViewID);
	void FindAndUseObjectByReactMethod(const FString& IdObject, const FVariables& Variables);
	void TryBlinkingObject(const FString& IdObject, const FString& ImagePath);
	
protected:
	//Метод отрабатывает когда получаем json от клиетна
	UFUNCTION()
	void JsonReceived(const FString& Payload);
	//Метод отрабатывает когда клиент подключается 
	UFUNCTION()
	void ClientConnected();

	void StartCourse();
	
	UPROPERTY()
	TObjectPtr<AWebSocketServer> WebSocketServer;

	UFUNCTION()
	void MainCourseMapLoaded();
	bool bMainCourseMapLoaded = false;

	// Методы на обработку полученных сообщений
	TFunction<void(const FParametersObject&  Parameters)> TShowWelcome;
	void ShowWelcome(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TShowTime;
	void ShowTime(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TTeleport;
	void Teleport(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TShowPlace;
	void ShowPlace(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TShowPoints;
	void ShowPoints(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TShowDialog;
	void ShowDialog(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TAddDialogMessage;
	void AddDialogMessage(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TUpdateDialog;
	void UpdateDialog(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TShowMenu;
	void ShowMenu(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TShowMessage;
	void ShowMessage(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TShowResult;
	void ShowResult(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TMeasureDevices;
	void MeasureDevices(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TShowMeasureResult;
	void ShowMeasureResult(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject&  Parameters)> TShowReaction;
	void ShowReaction(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject& Parameters)> TShowStepInfo;
	void ShowStepInfo(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject& Parameters)> TShowLearningSticker;
	void ShowLearningSticker(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject& Parameters)> TAddAction;
	void AddAction(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject& Parameters)> TPlaySound;
	void PlaySound(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject& Parameters)> TUpdatePlace;
	void UpdatePlace(const FParametersObject& Parameters);
	// ~Begin #VSP_only
	TFunction<void(const FParametersObject& Parameters)> TShowObjectMenu;
	void ShowObjectMenu(const FParametersObject& Parameters);
	TFunction<void(const FParametersObject& Parameters)> TUpdatePlaceLock;
	void UpdatePlaceLock(const FParametersObject& Parameters);
	// ~End VSP_only
	TFunction<void(const FParametersObject& Parameters)> TShowScheme;
	void ShowScheme(const FParametersObject& Parameters);

public:

	bool GetMainCourseMapLoaded()const { return bMainCourseMapLoaded; };

	virtual ESimulatorMode GetSimulatorMode_Implementation() const override;

protected:
	UPROPERTY()
	TObjectPtr<ACS_PlayerController> CurrentPlayerController;

	UPROPERTY()
	ESimulatorMode SimulatorMode;

	UPROPERTY()
	TObjectPtr<UCS_JsonParser> JsonParser;

	UFUNCTION()
	void InitializeJsonParser();

	UFUNCTION()
	void DisableInteractActors();

	UFUNCTION()
	virtual void TryActivateSpecialPoints(const FString& ApiID, const FString& IconPath, const bool bUpdate);

	UFUNCTION()
	virtual void RecursionActivatePoints(const TArray<FPlacePointData>& ArrayPlacePoints, bool bUpdate);

	UFUNCTION()
	bool IsGameStateImplementedCSInterface() const;

	// Makes entry in event log.
	UFUNCTION()
	virtual void AddMessageToLog(FString& Descriptions, ELogType LogType);

	UFUNCTION()
	virtual void DeactivateSpecialPoints();

	UFUNCTION()
	virtual bool CheckBlinkingActor(const FString& IconPath);

	UFUNCTION()
	virtual void CheckBlinkingDialogItem(TArray<FDialogButtonParameters>& DialogItems);

	UPROPERTY()
	TMap<FString, ETypeDialog> MapDialogSubKeys;
};
