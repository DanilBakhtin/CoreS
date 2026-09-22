#include "Gameplay/UI/CS_ContextMenu.h"
#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/WrapBox.h"
#include "Components/Image.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Gameplay/UI/CS_ContextItemButton.h"
#include "Engine/Engine.h"
#include <Blueprint/WidgetLayoutLibrary.h>

void UCS_ContextMenu::NativeConstruct()
{
    Super::NativeConstruct();

    check(Context_CanvasPanel);
    check(Context_Button);
    Context_Button->OnClicked.AddDynamic(this, &UCS_ContextMenu::OnClickEmptyPlace);
    check(Context_SizeBox);
    check(Context_WrapBox);
    check(ContextItemButtonClass);

    check(OverlayBackground_Image);
    OverlayBackground_Image->SetVisibility(ESlateVisibility::Collapsed);

}
void UCS_ContextMenu::SetPlayerController(ACS_PlayerController* _PlayerController)
{
    PlayerController = _PlayerController;
}

void UCS_ContextMenu::InitContextMenuItems(FString ApiID, const TArray<FContextMenuItem>& ContextMenuItems)
{
    SetVisibility(ESlateVisibility::Hidden);

    if (!PlayerController.IsValid())
        return;

    PendingContextMenuItems = ContextMenuItems;

    if (Context_WrapBox)
    {
        Context_WrapBox->ClearChildren();
    }

    bNeedUpdateMousePosition = !prevIDs.Contains(ApiID);
    prevIDs.Empty();
    
    for (const FContextMenuItem& Item : PendingContextMenuItems) 
    {
        if (UCS_ContextItemButton* NewItem = CreateWidget<UCS_ContextItemButton>(GetWorld(), ContextItemButtonClass))
        {
            if (Context_WrapBox)
            {
                Context_WrapBox->AddChildToWrapBox(NewItem);
                NewItem->InitContextMenuItem(Item);
                NewItem->SetCloseContextMenuFun(CloseContextMenuFun);
                NewItem->SetClickOnObject(ClickOnItemFun);
                prevIDs.Add(Item.ApiID);
            }
        }
    }
    //PerformLayoutUpdate();
    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.ExecutionFunction = "PerformLayoutUpdate";
    LatentInfo.Linkage = 0;
    LatentInfo.UUID = FMath::Rand();

    UKismetSystemLibrary::Delay(GetWorld(), 0.1f, LatentInfo);
}

void UCS_ContextMenu::PerformLayoutUpdate()
{
    if (!PlayerController.IsValid() || !Context_SizeBox)
        return;

    UCanvasPanelSlot* SlotCanvas = Cast<UCanvasPanelSlot>(Context_SizeBox->Slot);
    if (!SlotCanvas)
        return;

    FVector2D MenuSize = Context_SizeBox->GetDesiredSize();
    if (MenuSize.IsNearlyZero())
        return;

    int32 RawViewportX, RawViewportY;
    PlayerController->GetViewportSize(RawViewportX, RawViewportY);

    const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(this);
    const float ViewportX = RawViewportX / DPIScale;
    const float ViewportY = RawViewportY / DPIScale;

    if (bNeedUpdateMousePosition)
        PlayerController->GetMousePosition(MouseX, MouseY);

    FVector2D MousePos(MouseX, MouseY);
    MousePos /= DPIScale;

    constexpr float CursorOffset = 6.f;

    FVector2D Position = MousePos + FVector2D(CursorOffset, CursorOffset);

    const float SpaceBelow = ViewportY - MousePos.Y;
    const float SpaceAbove = MousePos.Y;

    if (SpaceBelow < MenuSize.Y && SpaceAbove >= MenuSize.Y)
    {
        Position.Y = MousePos.Y - MenuSize.Y - CursorOffset;
    }
    Position.X = FMath::Clamp(Position.X, 0.f, ViewportX - MenuSize.X);
    Position.Y = FMath::Clamp(Position.Y, 0.f, ViewportY - MenuSize.Y);

    SlotCanvas->SetAnchors(FAnchors(0.f, 0.f));
    SlotCanvas->SetAlignment(FVector2D(0.f, 0.f));
    SlotCanvas->SetPosition(Position);

    SetVisibility(ESlateVisibility::SelfHitTestInvisible);

    FindWidgetsOverlapping();
}

void UCS_ContextMenu::CheckCollisionWithOtherWidgets()
{
    if (!WidgetsForCollision.IsEmpty())
    {
        FGeometry ThisGeometry = Context_SizeBox->GetCachedGeometry();
        FVector2D ThisPosition = ThisGeometry.GetAbsolutePosition();
        FVector2D ThisSize = ThisGeometry.GetAbsoluteSize();
        FBox2D ThisBox(ThisPosition, ThisPosition + ThisSize);

        for (UUserWidget* OtherWidget : WidgetsForCollision)
        {
            TArray<UWidget*> AllChildren;
            OtherWidget->WidgetTree->GetAllWidgets(AllChildren);

            for (UWidget* Child : AllChildren)
            {
                if (Child->GetName().StartsWith("Collision_"))
                {
                    FGeometry OtherGeometry = Child->GetCachedGeometry();
                    FVector2D OtherPosition = OtherGeometry.GetAbsolutePosition();
                    FVector2D OtherSize = OtherGeometry.GetAbsoluteSize();
                    FBox2D OtherBox(OtherPosition, OtherPosition + OtherSize);

                    if (ThisBox.Intersect(OtherBox) && OtherWidget->GetVisibility() != ESlateVisibility::Collapsed)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Widget intersects with: %s"), *Child->GetName());
                        OverlayBackground_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
                        return;
                    }
                }
            }
        }
    }

    OverlayBackground_Image->SetVisibility(ESlateVisibility::Collapsed);
}

void UCS_ContextMenu::FindWidgetsOverlapping()
{
    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.ExecutionFunction = "CheckCollisionWithOtherWidgets";
    LatentInfo.Linkage = 0;
    LatentInfo.UUID = FMath::Rand();

    UKismetSystemLibrary::Delay(GetWorld(), 0.1f, LatentInfo);
}

void UCS_ContextMenu::AddWidgetForCollision(UUserWidget* NewWidget)
{
    if (NewWidget)
        WidgetsForCollision.Add(NewWidget);
}

void UCS_ContextMenu::SetCloseContextMenuFun(TFunction<void()> _CloseContextMenuFun)
{
    CloseContextMenuFun = _CloseContextMenuFun;
}

void UCS_ContextMenu::SetClickOnObject(TFunction<void(const FString& Id, const FString& Type)> _ClickOnItemFun)
{
    ClickOnItemFun = _ClickOnItemFun;
}

void UCS_ContextMenu::OnClickEmptyPlace()
{
    if (CloseContextMenuFun)
        CloseContextMenuFun();
}