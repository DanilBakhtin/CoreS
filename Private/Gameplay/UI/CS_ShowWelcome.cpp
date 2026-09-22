#include "Gameplay/UI/CS_ShowWelcome.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Border.h"
#include "Gameplay/UI/CS_BaseButton.h"
#include "Gameplay/UI/CS_ShowScheme.h"
#include <ImageUtils.h>

void UCS_ShowWelcome::NativePreConstruct()
{
	InitBackground();
}

void UCS_ShowWelcome::NativeConstruct()
{
	Super::NativeConstruct();

	check(Next_Button)
		Next_Button->OnReleased.AddDynamic(this, &UCS_ShowWelcome::NextIndex);
	check(Start_Button)
		Start_Button->OnReleased.AddDynamic(this, &UCS_ShowWelcome::StartCourse);
	check(ExitOne_Button)
		ExitOne_Button->OnReleased.AddDynamic(this, &UCS_ShowWelcome::Exit);
	check(ExitTwo_Button)
		ExitTwo_Button->OnReleased.AddDynamic(this, &UCS_ShowWelcome::Exit);
	check(Guide_Button)
		Guide_Button->OnReleased.AddDynamic(this, &UCS_ShowWelcome::Guide);
	check(ImageViewer)
		ImageViewer->OnCloseScheme.AddDynamic(this, &UCS_ShowWelcome::CloseScheme);

	check(ShowWelcome_WidgetSwitcher);

	InitBackground();
}

void UCS_ShowWelcome::InitParams(const FShowWelcomeParams& Params) const
{
	ShowWelcomeTitle_TextBlock->SetText(FText::FromString(Params.ShowWelcomeTitle));
	ShowWelcomeDescription_TextBlock->SetText(FText::FromString(Params.ShowWelcomeDescription));
	FaultInfoTitle_TextBlock->SetText(FText::FromString(Params.FaultInfoTitle));
	FaultInfoDescription_TextBlock->SetText(FText::FromString(Params.FaultInfoDescription));
	FaultInfoQuest_TextBlock->SetText(FText::FromString(Params.FaultInfoQuest));
}

void UCS_ShowWelcome::SetStartCourseFunc(TFunction<void()> Func)
{
	StartCourseFunc = Func;
}

void UCS_ShowWelcome::SetExitCourseFunc(TFunction<void()> Func)
{
	ExitCourseFunc = Func;
}

void UCS_ShowWelcome::InitImages(TArray<TSoftObjectPtr<UTexture2D>> Textures) const
{
	ImageViewer->InitSchemeImage(Textures);
}

void UCS_ShowWelcome::NextIndex()
{
	ShowWelcome_WidgetSwitcher->SetActiveWidgetIndex(1);
}

void UCS_ShowWelcome::Exit()
{
	if(ExitCourseFunc)
		ExitCourseFunc();
}

void UCS_ShowWelcome::Guide()
{
	ShowWelcome_WidgetSwitcher->SetActiveWidgetIndex(2);
}

void UCS_ShowWelcome::CloseScheme()
{
	ShowWelcome_WidgetSwitcher->SetActiveWidgetIndex(0);
}

void UCS_ShowWelcome::InitBackground()
{
	if (!IsValid(MainTexture))
	{
		FString TexturePath = FPaths::ProjectContentDir() + PathToMainImage;
		UTexture2D* LoadedTexture = FImageUtils::ImportFileAsTexture2D(TexturePath);

		if (LoadedTexture && MainBorder) {
			MainBorder->SetBrushFromTexture(LoadedTexture);
			BlurBG->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else {
			BlurBG->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		if (MainBorder) {
			MainBorder->SetBrushFromTexture(MainTexture);
			BlurBG->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else {
			BlurBG->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UCS_ShowWelcome::StartCourse()
{
	if(StartCourseFunc)
		StartCourseFunc();
}
