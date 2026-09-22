// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_PanelStickers.h"
#include "Gameplay/UI/CS_LearningSticker.h"
#include "Gameplay/UI/CS_ShowReaction.h"
#include "Components/OverlaySlot.h"
#include "Components/WrapBox.h"
#include "Components/SizeBox.h"
#include "Math/UnrealMathUtility.h"
#include "Framework/CS_UtilityFunctionLibrary.h"

void UCS_PanelStickers::NativeConstruct()
{
    Super::NativeConstruct();

    check(ReactionsBox);
    check(MainContainer);
    check(ReactionContainer);

    check(LearningStickerBox);
    LearningStickerBox->SetVisibility( ESlateVisibility::Collapsed);

    check(LearningSticker);

    check(OpenLearningSticker);
    {   
        FWidgetAnimationDynamicEvent OpenFinishedEvent;
        OpenFinishedEvent.BindDynamic(this, &UCS_PanelStickers::OnOpenLearningAnimationFinished);
        BindToAnimationFinished(OpenLearningSticker, OpenFinishedEvent);
    }

    check(CloseLearningSticker);
    {
        FWidgetAnimationDynamicEvent CloseFinishedEvent;
        CloseFinishedEvent.BindDynamic(this, &UCS_PanelStickers::OnCloseLearningAnimationFinished);
        BindToAnimationFinished(CloseLearningSticker, CloseFinishedEvent);
    }
}

void UCS_PanelStickers::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (IsDesignTime()) {
        UpdateCondition();
    }
}

void UCS_PanelStickers::NativeDestruct()
{   
    TArray<UWidget*> Children = ReactionContainer->GetAllChildren();
    for (UWidget* Child : Children)
    {
        if (UCS_ShowReaction* ReactionWidget = Cast<UCS_ShowReaction>(Child))
        {
            ReactionWidget->OnReactionRemoving.RemoveDynamic(this, &UCS_PanelStickers::OnReactionRemoved);
        }
    }

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(LearningStickerTimer);
    }
    Super::NativeDestruct();
}

void UCS_PanelStickers::SetIsArm(bool bArmActive)
{
    bIsArm = bArmActive;
    UpdateCondition();
}

void UCS_PanelStickers::UpdateCondition()
{
    if (UPanelSlot* MainSlot = MainContainer->Slot) {
        if (UOverlaySlot* MainOverlaySlot = Cast<UOverlaySlot>(MainSlot)) {
            float TopPadding = bIsArm ? ArmTopPadding : BaseTopPadding;
            MainOverlaySlot->SetPadding(FMargin(MainOverlaySlot->GetPadding().Left, TopPadding, MainOverlaySlot->GetPadding().Right, MainOverlaySlot->GetPadding().Bottom));
        }
    }

    float NewHeightReactionBox = ReactionBoxBaseHeight;

    NewHeightReactionBox += bIsArm ? BaseTopPadding - ArmTopPadding : 0.0f;
    NewHeightReactionBox += LearningStickerBox->GetVisibility() == ESlateVisibility::Collapsed ? MainContainer->GetInnerSlotPadding().Y + LearningStickerBox->GetHeightOverride() : 0.0f;

    ReactionsBox->SetMaxDesiredHeight(NewHeightReactionBox);
}

void UCS_PanelStickers::ShowLearningSticker(const FStickerParams& StickerParams) {
    
    if (!LearningSticker) return; 

    if (LearningStickerBox->GetVisibility() == ESlateVisibility::Collapsed) {

        bManualHideSticker = false;

        LearningSticker->InitSticker(StickerParams);

        LearningStickerBox->SetVisibility(ESlateVisibility::HitTestInvisible);

        UpdateCondition();

        PlayStickerAnimation(OpenLearningSticker);
    }
    else {

        if (GetWorld())
            GetWorld()->GetTimerManager().ClearTimer(LearningStickerTimer);

        HideLearningSticker();

        bManualHideSticker = true;

        CachedStickerParams = StickerParams;
    }
}

void UCS_PanelStickers::SetCloseStickerFun(TFunction<void()> InputFun)
{
    CloseStickerFun = InputFun;
}

void UCS_PanelStickers::AddShowReaction(const FString& Description)
{   
    if (Description.IsEmpty()) return;

    if (ShowReactionClass)
    {
        if (UCS_ShowReaction* ShowReactionWidget = CreateWidget<UCS_ShowReaction>(GetWorld(), ShowReactionClass))
        {   
            float TimeLife = (static_cast<float>(UCS_UtilityFunctionLibrary::CountLettersOnly(Description)) / static_cast<float>(MinReactionText)) * MinTimeLifeReaction;

            TimeLife = FMath::Clamp(TimeLife, MinTimeLifeReaction, MaxTimeLifeReaction);

            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Some variable values: x: %f"), TimeLife));

            ShowReactionWidget->InitShowReaction(Description, TimeLife);
            ShowReactionWidget->SetVisibility(ESlateVisibility::Visible);

            ReactionContainer->AddChild(ShowReactionWidget);    
        }
    }
}

void UCS_PanelStickers::SetCloseReactionFun(TFunction<void()> InputFun)
{
    CloseReactionFun = InputFun;
}

void UCS_PanelStickers::StartTimerCloseSticker()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(LearningStickerTimer);

        GetWorld()->GetTimerManager().SetTimer(
            LearningStickerTimer,
            this,
            &UCS_PanelStickers::HideLearningSticker,
            TimeLifeLearningSticker,
            false
        );
    }
}

void UCS_PanelStickers::HideLearningSticker()
{
    PlayStickerAnimation(CloseLearningSticker);
}

void UCS_PanelStickers::CloseSticker()
{   
    LearningStickerBox->SetVisibility(ESlateVisibility::Collapsed);

    if (!bManualHideSticker) {

        if (CloseStickerFun)
            CloseStickerFun();
        UpdateCondition();

    }
    else {
        ShowLearningSticker(CachedStickerParams);
    }

}

void UCS_PanelStickers::PlayStickerAnimation(UWidgetAnimation* Animation)
{   
    if (Animation)
        PlayAnimation(Animation, 0.0f, 1, EUMGSequencePlayMode::Forward, PlayRateLearningAnimation);
}

void UCS_PanelStickers::OnOpenLearningAnimationFinished()
{
    StartTimerCloseSticker();
}

void UCS_PanelStickers::OnCloseLearningAnimationFinished()
{
    CloseSticker();
}

void UCS_PanelStickers::OnReactionRemoved(UCS_ShowReaction* ReactionWidget)
{   
    if (IsValid(ReactionWidget)) {

        ReactionWidget->OnReactionRemoving.RemoveDynamic(this, &UCS_PanelStickers::OnReactionRemoved);

        if (ReactionContainer && ReactionContainer->HasChild(ReactionWidget))
        {
            ReactionContainer->RemoveChild(ReactionWidget);
        }
    }

    if (ReactionContainer->GetAllChildren().Num() == 0)
        if (CloseReactionFun)
            CloseReactionFun();
}
