// Core Simulator


#include "Gameplay/UI/CS_ShowScheme.h"

#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "Gameplay/UI/CS_SchemeViewer.h"
#include "Gameplay/UI/CS_BaseButton.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Gameplay/UI/CS_LocalizableTextBlock.h"

#include "Gameplay/Utilities/CS_ImageManager.h"


void UCS_ShowScheme::NativeConstruct()
{
	Super::NativeConstruct();

	check(NextScheme_Button)
		NextScheme_Button->OnReleased.AddDynamic(this, &UCS_ShowScheme::NextScheme);
	check(PrevScheme_Button)
		PrevScheme_Button->OnReleased.AddDynamic(this, &UCS_ShowScheme::PrevScheme);
	check(CloseScheme_Button)
		CloseScheme_Button->OnReleased.AddDynamic(this, &UCS_ShowScheme::CloseScheme);
	check(Scheme_Overlay);
	check(TextBlock_NameMenu);

	TextBlock_NameMenu->SetText(FText::FromString(PreviewTextNameMenu));
}

void UCS_ShowScheme::NativePreConstruct()
{
	if (TextBlock_NameMenu) {
		TextBlock_NameMenu->LocalizationKey = NameMenuKey;
		TextBlock_NameMenu->SetPreviewText(FText::FromString(PreviewTextNameMenu));
	}
}

void UCS_ShowScheme::InitSchemeImage(TArray<TSoftObjectPtr<UTexture2D>> Textures)
{
	CurrentIndexImage = 0;
	MaxGuides = Textures.Num();
	Scheme_Overlay->ClearChildren();
	Guides.Empty();

	TryUpdateNameByImageInfo();

	for(int32 i = 0; i < Textures.Num(); ++i)
	{
		if(UCS_SchemeViewer* Widget = CreateWidget<UCS_SchemeViewer>(GetWorld(), SchemeViewerClass))
		{
			UOverlaySlot* ChildOverlaySlot = Scheme_Overlay->AddChildToOverlay(Widget);
			Widget->SetTextureByImage(Textures[i]);
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			Widget->OnZoomChanged.AddDynamic(this, &UCS_ShowScheme::OnImageZoomChanged);
			Widget->bZoomEnable = bZoomEnable;

			ChildOverlaySlot->SetHorizontalAlignment(HAlign_Fill);
			ChildOverlaySlot->SetVerticalAlignment(VAlign_Fill);
			
			Guides.Add(i, Widget);
		}
	}
	if(Guides.Num() > 0)
	{
		if(UCS_SchemeViewer* Widget = Guides[0])
		{
			Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			FString Position = FString::FromInt(CurrentIndexImage + 1)+"/"+FString::FromInt(MaxGuides);
			CurrentSchemePosition_TextBlock->SetText(FText::FromString(Position));
			PrevScheme_Button->SetIsEnabled(false);
			NextScheme_Button->SetIsEnabled(true);

			if (Guides.Num() == 1)
				NextScheme_Button->SetIsEnabled(false);
		}
	}
}

bool UCS_ShowScheme::InitSchemeImageFromInfo(TArray<FLoadingImageInfo> LoadingImagesInfo)
{
	TArray<TSoftObjectPtr<UTexture2D>> Textures;
	CurrentImagesInfo.Empty();
	bool bAllImageLoaded = true;


	if (UCS_ImageManager* ImageManager = GetGameInstance()->GetSubsystem<UCS_ImageManager>()) {

		for (int i = 0; i < LoadingImagesInfo.Num(); i++) {
			if (UTexture2D* Texture = ImageManager->GetImageByID(LoadingImagesInfo[i].ApiID)) {
				Textures.Add(Texture);
				CurrentImagesInfo.Add(LoadingImagesInfo[i]);
			}
			else if (ImageManager->HasImageInQueue(LoadingImagesInfo[i].ApiID))
			{
				CurrentImagesInfo.Add(LoadingImagesInfo[i]);
				bAllImageLoaded = false;
			}
		}
	}

	if (!bAllImageLoaded) {
		if (UCS_ImageManager* ImageManager = GetGameInstance()->GetSubsystem<UCS_ImageManager>()) {

			ImageManager->OnAllImageLoaded.AddDynamic(this, &UCS_ShowScheme::OnImagesLoaded);
		}
	}

	if (Textures.Num() > 0) {

		InitSchemeImage(Textures);
		return true;
	}

	CloseScheme();
	return false;
}

void UCS_ShowScheme::SetClosSchemeFunc(TFunction<void()> Func)
{
	ClosSchemeFunc = Func;
}

void UCS_ShowScheme::ResetImageZoom()
{
	if (!IsEmptyImages())
		if (Guides[CurrentIndexImage])
			Guides[CurrentIndexImage]->ReturnBaseSize();
}

bool UCS_ShowScheme::IsEmptyImages()
{
	return Guides.Num() == 0;
}

int UCS_ShowScheme::GetCurrentZoomPercent()
{
	if (Guides.Contains(CurrentIndexImage)) 
	{
		if (Guides[CurrentIndexImage])
			return Guides[CurrentIndexImage]->GetCurrentZoomValue();
	}

	return 100;
}

void UCS_ShowScheme::NextScheme()
{
	if(CurrentIndexImage + 1 < MaxGuides && IsValid(Guides[CurrentIndexImage]))
	{
		Guides[CurrentIndexImage]->SetVisibility(ESlateVisibility::Collapsed);
		Guides[CurrentIndexImage]->ReturnBaseSize();

		if(IsValid(Guides[CurrentIndexImage + 1]))
		{
			Guides[CurrentIndexImage + 1]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			++CurrentIndexImage;
			if(CurrentIndexImage + 1 == MaxGuides)
				NextScheme_Button->SetIsEnabled(false);

			PrevScheme_Button->SetIsEnabled(true);

			FString Position = FString::FromInt(CurrentIndexImage + 1)+"/"+FString::FromInt(MaxGuides);
			CurrentSchemePosition_TextBlock->SetText(FText::FromString(Position));
		}

		TryUpdateNameByImageInfo();
	}
}

void UCS_ShowScheme::PrevScheme()
{
	if(CurrentIndexImage > 0 && IsValid(Guides[CurrentIndexImage]))
	{
		Guides[CurrentIndexImage]->SetVisibility(ESlateVisibility::Collapsed);
		Guides[CurrentIndexImage]->ReturnBaseSize();

		if(IsValid(Guides[CurrentIndexImage - 1]))
		{
			Guides[CurrentIndexImage - 1]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			--CurrentIndexImage;
			if(CurrentIndexImage == 0)
				PrevScheme_Button->SetIsEnabled(false);			
			
			NextScheme_Button->SetIsEnabled(true);

			FString Position = FString::FromInt(CurrentIndexImage + 1)+"/"+FString::FromInt(MaxGuides);
			CurrentSchemePosition_TextBlock->SetText(FText::FromString(Position));
		}

		TryUpdateNameByImageInfo();
	}
}

void UCS_ShowScheme::CloseScheme()
{
	OnCloseScheme.Broadcast();

	if (Guides.Contains(CurrentIndexImage))
		if (Guides[CurrentIndexImage])
			Guides[CurrentIndexImage]->ReturnBaseSize();

	if (ClosSchemeFunc)
		ClosSchemeFunc();
}

void UCS_ShowScheme::OnImageZoomChanged(int PercentZoom)
{
	if (GetWorld()->GetFirstPlayerController())
	{
		if (ACS_PlayerController* PlayerController = Cast<ACS_PlayerController>(GetWorld()->GetFirstPlayerController()))
			PlayerController->ZoomChanged(PercentZoom);
	}
}

void UCS_ShowScheme::ChangeTitleText(const FString& NewText)
{
	TextBlock_NameMenu->LocalizationKey = "";
	TextBlock_NameMenu->SetPreviewText(FText::FromString(NewText));
}

void UCS_ShowScheme::TryUpdateNameByImageInfo()
{
	if (!CurrentImagesInfo.IsEmpty())
		if (CurrentImagesInfo.Num() - 1 >= CurrentIndexImage) {
			ChangeTitleText(CurrentImagesInfo[CurrentIndexImage].Name);
		}	
}

void UCS_ShowScheme::OnImagesLoaded()
{
	InitSchemeImageFromInfo(CurrentImagesInfo);

	if (UCS_ImageManager* ImageManager = GetGameInstance()->GetSubsystem<UCS_ImageManager>()) {

		ImageManager->OnAllImageLoaded.RemoveDynamic(this, &UCS_ShowScheme::OnImagesLoaded);
	}
}
