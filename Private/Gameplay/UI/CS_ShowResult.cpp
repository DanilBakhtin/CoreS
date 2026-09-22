#include "Gameplay/UI/CS_ShowResult.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Framework/CS_CoreTypes.h"
#include "Gameplay/UI/CS_ResultCategory.h"
#include "Framework/CS_GameInstance.h"
#include "Gameplay/Utilities/CS_LocalizationManager.h"
#include "Gameplay/UI/CS_BaseButton.h"

void UCS_ShowResult::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(Eval_TextBlock);
	check(EvalDescription_TextBlock);
	check(Category_WrapBox);
	check(ResultCategoryClass);
	check(Exit_Button)
		Exit_Button->OnReleased.AddDynamic(this, &UCS_ShowResult::Exit);
	check(Description_Border);
}

void UCS_ShowResult::Init(const FShowResultParams& ShowResultParams)
{	
	Category_WrapBox->ClearChildren();

	if (UCS_GameInstance* GameInstance = Cast<UCS_GameInstance>(GetGameInstance())) {
		FText LocalizedText = GameInstance->GetLocalizationManager()->GetText("SHOW_RESULT_TITLE");
		Eval_TextBlock->SetText(FText::FromString(LocalizedText.ToString() + ShowResultParams.Eval));
	}
	else {
		Eval_TextBlock->SetText(FText::FromString("Результат: " + ShowResultParams.Eval));
	}

	EvalDescription_TextBlock->SetText(FText::FromString(ShowResultParams.EvalDescription));

	for (const FResultParams& Result : ShowResultParams.Results)
	{
		if (UCS_ResultCategory* ResultCategory = CreateWidget<UCS_ResultCategory>(GetWorld(), ResultCategoryClass))
		{
			Category_WrapBox->AddChildToWrapBox(ResultCategory);
			CategoryButtons.Add(ResultCategory);
			ResultCategory->Init(Result);
			ResultCategory->SetShowDescriptionFun([&](UCS_ResultCategory* Button){ShowDescription(Button);});
		}
	}
}

void UCS_ShowResult::SetExitCourseFunc(TFunction<void()> Func)
{
	ExitCourseFunc = Func;
}

void UCS_ShowResult::ShowDescription(UCS_ResultCategory* ResultCategory)
{
	if(ResultCategory->GetResultParams().MSG != "")
	{
		Description_Border->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Description_TextBlock->SetText(FText::FromString(ResultCategory->GetResultParams().MSG ));
	}
	else
	{
		Description_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}
	for(UCS_ResultCategory*& Button : CategoryButtons)
	{
		if(Button == ResultCategory)
		{
			ResultCategory->Check();
		}
		else
		{
			Button->Uncheck();
		}
	}
}

void UCS_ShowResult::Exit()
{
	if(ExitCourseFunc)
		ExitCourseFunc();
}
