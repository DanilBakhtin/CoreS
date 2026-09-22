
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/CS_CoreTypes.h" 
#include "CS_ContextMenu.generated.h"

class UCS_ContextItemButton;
class UWrapBox;
class USizeBox;
class UCanvasPanel;
class UButton;
class UImage;
class ACS_PlayerController; 

UCLASS()
class CORE_SIMULATOR_API UCS_ContextMenu : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ContextMenu")
    TSubclassOf<UCS_ContextItemButton> ContextItemButtonClass;

    UPROPERTY(BlueprintReadOnly, Category = "CS_ContextMenu", meta = (BindWidget))
    TObjectPtr<UCanvasPanel> Context_CanvasPanel;

    UPROPERTY(BlueprintReadOnly, Category = "CS_ContextMenu", meta = (BindWidget))
    TObjectPtr<UButton> Context_Button;

    UPROPERTY(BlueprintReadOnly, Category = "CS_ContextMenu", meta = (BindWidget))
    TObjectPtr<USizeBox> Context_SizeBox;

    UPROPERTY(BlueprintReadOnly, Category = "CS_ContextMenu", meta = (BindWidget))
    TObjectPtr<UWrapBox> Context_WrapBox;

    UPROPERTY(BlueprintReadOnly, Category = "CS_ContextMenu", meta = (BindWidget))
    TObjectPtr<UImage> OverlayBackground_Image;

    void SetPlayerController(ACS_PlayerController* PlayerController);

    void InitContextMenuItems(FString ApiID, const TArray<FContextMenuItem>& ContextMenuItems);

    void SetCloseContextMenuFun(TFunction<void()> CloseContextMenuFun);
    void SetClickOnObject(TFunction<void(const FString& Id, const FString& Type)> ClickOnItemFun);

    UFUNCTION()
    void FindWidgetsOverlapping();

    void AddWidgetForCollision(UUserWidget* NewWidget);

protected:
    UFUNCTION()
    void OnClickEmptyPlace();

    TArray<FContextMenuItem> PendingContextMenuItems;

    UFUNCTION()
    void PerformLayoutUpdate();

    UFUNCTION()
    void CheckCollisionWithOtherWidgets();

    UPROPERTY()
    TWeakObjectPtr<ACS_PlayerController> PlayerController;

    TFunction<void()> CloseContextMenuFun;
    TFunction<void(const FString& Id, const FString& Type)> ClickOnItemFun;

    UPROPERTY()
    float MouseX = 0.0f;
    float MouseY = 0.0f;

    UPROPERTY()
    TMap<FString, bool> prevIDs;

    UPROPERTY()
    bool bNeedUpdateMousePosition = true;

    UPROPERTY()
    TArray<UUserWidget*> WidgetsForCollision;
};
