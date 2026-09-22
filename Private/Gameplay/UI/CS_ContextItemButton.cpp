
#include "Gameplay/UI/CS_ContextItemButton.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Framework/CS_CoreTypes.h"
#include "Framework/CS_DeveloperSettings.h"

void UCS_ContextItemButton::NativeConstruct()
{
	Super::NativeConstruct();

	check(Context_Image);
	check(Context_TextBlock);
	check(Context_Button);
		Context_Button->OnClicked.AddDynamic(this, &UCS_ContextItemButton::OnClickItem);
}

void UCS_ContextItemButton::InitContextMenuItem(const FContextMenuItem& ContextMenuItem)
{
	if (!ContextMenuItem.Name.IsEmpty())
	{
		Context_TextBlock->SetText(FText::FromString(ContextMenuItem.Name));
	}
	if (!ContextMenuItem.ApiID.IsEmpty())
	{
		Id = ContextMenuItem.ApiID;
	}

	bool bFounded = false;

	for (const FContextMenuIconItem& IconType : GetDefault<UCS_DeveloperSettings>()->ContextMenuIconItems)
	{
		if (ContextMenuItem.Tool.Equals(IconType.Type))
		{
			Context_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Context_Image->SetBrushFromSoftTexture(IconType.SoftTexture);

			bFounded = true;

			break;
		}
		else if (ContextMenuItem.Tool.Contains(IconType.Type))
		{
			Context_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Context_Image->SetBrushFromSoftTexture(IconType.SoftTexture);

			bFounded = true;
		}
	}

	if (!bFounded)
		Context_Image->SetVisibility(ESlateVisibility::Collapsed);

	Type = ContextMenuItem.Tool;
}

void UCS_ContextItemButton::SetCloseContextMenuFun(TFunction<void()> _CloseContextMenuFun)
{
	CloseContextMenuFun = _CloseContextMenuFun;
}

void UCS_ContextItemButton::SetClickOnObject(TFunction<void(const FString& Id, const FString& Type)> _ClickOnItemFun)
{
	ClickOnItemFun = _ClickOnItemFun;
}

void UCS_ContextItemButton::OnClickItem()
{
	if(ClickOnItemFun)
	{
		ClickOnItemFun(Id, Type);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ContextItemButton: Trying call ClickOnItemFun is nullptr!"));
	}
	if(CloseContextMenuFun)
		CloseContextMenuFun();
}
