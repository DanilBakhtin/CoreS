// Core Simulator


#include "Gameplay/UI/CS_CheckWidget.h"

#include "GameplayTagContainer.h"
#include "Components/CheckBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"

void UCS_CheckWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateConstruct();

	if (IsValid(CheckBox)) {
		CheckBox->OnCheckStateChanged.Clear();
		CheckBox->OnCheckStateChanged.AddDynamic(this, &UCS_CheckWidget::CheckStateChanged);
	}
}

void UCS_CheckWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	UpdateConstruct();
}

void UCS_CheckWidget::UpdateConstruct() const
{	

	if (IsValid(Button_Image))
	{

		if (IconMaterialClass) {

			DynamicIconMaterial = UMaterialInstanceDynamic::Create(IconMaterialClass, const_cast<UCS_CheckWidget*>(this));

			if (DynamicIconMaterial && Button_Image)
			{
				Button_Image->SetBrushFromMaterial(DynamicIconMaterial);

				if (IconImage)
				{	
					UTexture* Texture = Cast<UTexture>(IconImage);
					if (Texture)
					{	
						DynamicIconMaterial->SetTextureParameterValue(FName("IconTexture"), Texture);
					}
				}
			}
		}
		else {
			FSlateBrush SlateBrush = FSlateBrush();
			SlateBrush.SetResourceObject(IconImage);
			Button_Image->SetBrush(SlateBrush);
		}

	}

	if(IsValid(Title_TextBlock))
	{
		Title_TextBlock->SetText(TitleText);
	}	

	if (IsValid(SizeBoxImage))
	{
		SizeBoxImage->SetWidthOverride(SizeImage);
		SizeBoxImage->SetHeightOverride(SizeImage);
	}
}

void UCS_CheckWidget::SetIsEnabledCheckBoxBox(bool bDisabled) const
{
	if(IsValid(CheckBox))
		CheckBox->SetIsEnabled(bDisabled);
}

bool UCS_CheckWidget::GetIsEnabledCheckBoxBox(bool bDisabled) const
{
	return IsValid(CheckBox) && CheckBox->GetIsEnabled();
}

void UCS_CheckWidget::UpdateButtonCondition(const FGameplayTagContainer& TagContainer) const
{
	if(!CheckBox)
		return;

	bool bIsUsed = false;
	CheckBox->SetIsEnabled(TagContainer.HasTag(ActivatedTag));
	for (FGameplayTag UsedTag : UsedArray)
	{
		if (TagContainer.HasTag(UsedTag))
		{
			bIsUsed = true;
			break;
		}
	}
	CheckBox->SetIsChecked(bIsUsed);
	UpdateTextAndIcon(bIsUsed);
}

void UCS_CheckWidget::Init(UObject* Icon, const FString& InputTitleText, const FHotkeySlot& HotKeysStruct)
{	
	IconImage = Icon;
	TitleText = FText::FromString(InputTitleText);
	ActivatedTag = HotKeysStruct.Activated;
	UsedArray = HotKeysStruct.Used;

	UpdateConstruct();
}

void UCS_CheckWidget::EnableBlinking(const bool bEnable) const
{
	if (DynamicIconMaterial)
		DynamicIconMaterial->SetScalarParameterValue(FName("IsBlinking"), bEnable ? 1.0f : 0.0f);
}

void UCS_CheckWidget::UpdateTextAndIcon(bool bIsChecked) const
{
	Title_TextBlock->SetFontMaterial(bIsChecked ? CheckedTextMaterial : UncheckedTextMaterial);
	
	if (DynamicIconMaterial)
		DynamicIconMaterial->SetScalarParameterValue(FName("IsChecked"), bIsChecked);
}

void UCS_CheckWidget::CheckStateChanged(bool bIsChecked)
{
	/*if(bIsChecked)
		AddTagFun(UsedArray);
	else
		RemoveTagFun(UsedArray);

	UpdateButtonCondition(GetTagContainerFun());*/

	OnCheckedChangeWithActiveTag.Broadcast(ActivatedTag, bIsChecked);
}