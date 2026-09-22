#include "Gameplay/UI/CS_LearningSticker.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Framework/CS_UtilityFunctionLibrary.h"

void UCS_LearningSticker::NativeConstruct()
{
	Super::NativeConstruct();

	check(BackgroundImage);
	check(Sticker_MainTextBlock);
	check(Sticker_AdditionalTextBlock);
	check(ImageAnswer);
}

void UCS_LearningSticker::InitSticker(const FStickerParams& StickerParams)
{
	ApplyStickerParams(StickerParams);
}

void UCS_LearningSticker::ApplyStickerParams(const FStickerParams& StickerParams)
{
	CurrentStickerParams = StickerParams;

	Sticker_MainTextBlock->SetText(FText::FromString(StickerParams.Header));

	if (!StickerParams.Text.IsEmpty()) {
		Sticker_AdditionalTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Sticker_AdditionalTextBlock->SetText(FText::FromString(StickerParams.Text));
	}
	else
		Sticker_AdditionalTextBlock->SetVisibility(ESlateVisibility::Collapsed);

	switch (StickerParams.Penalty)
	{
	case 0:
		BackgroundImage->SetBrushFromMaterial(RightMaterial);
		ImageAnswer->SetBrushFromSoftTexture(CheckAnswerIcon);
		break;
	case 1:
		BackgroundImage->SetBrushFromMaterial(NotRightMaterial);
		ImageAnswer->SetBrushFromSoftTexture(UncheckAnswerIcon);
		break;
	default:
		break;
	}
}
