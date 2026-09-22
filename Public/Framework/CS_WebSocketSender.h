#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CS_WebSocketSender.generated.h"


class AWebSocketServer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLogMessage, const FString&, Message);

UCLASS()
class CORE_SIMULATOR_API UCS_WebSocketSender : public UObject
{
	GENERATED_BODY()
	
public:
    UFUNCTION()
    void Initialize(AWebSocketServer* InServer);

    UFUNCTION()
    FString SendEvent(const FString& ObjectId, const FString& EventType,const FString& Action, const FString& Value);
    UFUNCTION()
    void SendNavAction(const FString& Action);
    UFUNCTION()
    void SendEndTween(const FString& Point);
    UFUNCTION()
    void SendEndTweens(const TArray<FString>& _Points);
    UFUNCTION()
    void SendClickObject(const FString& ObjectId, const FString& String);
    UFUNCTION()
    void SendPointAction(const FString& ObjectId, const FString& ActionTypeString);
    UFUNCTION()
    void SendActionEvent(const FString& EventName, const FString& Attribute);
    UFUNCTION()
    void SendCustomEvent(const FString& ObjectId, const FString& CustomString);

    UPROPERTY(BlueprintAssignable, Category = "CS_WebSocketSender")
    FOnLogMessage OnLogMessage;

private:
    UPROPERTY()
    TWeakObjectPtr<AWebSocketServer> WebSocketServer;

    UFUNCTION()
    void LogMessage(const FString& Message);
};
