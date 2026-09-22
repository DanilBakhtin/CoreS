#include "Framework/CS_WebSocketSender.h"
#include "WebSocketServer/WebSocketServer.h"
#include "Framework/CS_UtilityFunctionLibrary.h"

void UCS_WebSocketSender::Initialize(AWebSocketServer* InServer)
{
    WebSocketServer = InServer;
}

FString UCS_WebSocketSender::SendEvent(const FString& ObjectId, const FString& EventType, const FString& Action, const FString& Value)
{
    if (!WebSocketServer.IsValid())
    {
        LogMessage("WebSocketServer is not valid!");
        return FString();
    }

    FBodyOfAnswer Body(ObjectId, EventType, Action, Value);
    FString JsonString = UCS_UtilityFunctionLibrary::FBodyOfAnswerToString(Body);

    WebSocketServer->Send(JsonString);

    return JsonString;
}

void UCS_WebSocketSender::SendNavAction(const FString& Action)
{
    FString JsonString = SendEvent("api", "Event", "navAction", Action);

    if (!JsonString.IsEmpty()) {
        LogMessage(FString::Printf(TEXT("navAction: %s"), *JsonString));
    }
}

void UCS_WebSocketSender::SendEndTween(const FString& Point)
{
    FString JsonString = SendEvent("api", "Event", "EndTween", Point);

    if (!JsonString.IsEmpty()) {
        LogMessage(FString::Printf(TEXT("EndTween: %s"), *JsonString));
    }
}

void UCS_WebSocketSender::SendEndTweens(const TArray<FString>& _Points)
{
    const int32 PointsLength = _Points.Num();
    if (PointsLength <= 0)
        return;
    FString Points = "[\"";

    int32 PointsIndex = 0;
    for (const FString& Point : _Points)
    {
        Points += Point;
        if (PointsIndex < PointsLength - 1)
            Points.Append("\",\"");
        else
            Points.Append("\"]");
        ++PointsIndex;
    }

    SendEndTween(Points);
}

void UCS_WebSocketSender::SendClickObject(const FString& ObjectId, const FString& String)
{
    FString JsonString = SendEvent(ObjectId, "Event", "OnClickObject", String);

    if (!JsonString.IsEmpty()) {
        LogMessage(FString::Printf(TEXT("OnClickObject: %s"), *JsonString));
    }
}

void UCS_WebSocketSender::SendPointAction(const FString& ObjectId, const FString& ActionTypeString)
{
    FString JsonString = SendEvent(ObjectId, "Event", "OnPointAction", ActionTypeString);

    if (!JsonString.IsEmpty()) {
        LogMessage(FString::Printf(TEXT("OnPointAction: %s"), *JsonString));
    }
}

void UCS_WebSocketSender::SendActionEvent(const FString& EventName, const FString& Attribute)
{
    FString JsonString = SendEvent("api", "Event", EventName, Attribute);

    if (!JsonString.IsEmpty()) {
        LogMessage(FString::Printf(TEXT("%s: %s"), *EventName, *JsonString));
    }
}

void UCS_WebSocketSender::SendCustomEvent(const FString& ObjectId, const FString& CustomString)
{
    FString JsonString = SendEvent(ObjectId, "CustomEvent", "OnCustomAction", CustomString);

    if (!JsonString.IsEmpty()) {
        LogMessage(FString::Printf(TEXT("CustomActionEvent: %s"), *JsonString));
    }
}

void UCS_WebSocketSender::LogMessage(const FString& Message)
{
    OnLogMessage.Broadcast(Message);
}
