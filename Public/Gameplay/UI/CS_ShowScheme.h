// Core Simulator

#pragma once

#include "CoreMinimal.h"
#include "CS_UserWidget.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_ShowScheme.generated.h"

class UButton;
class UTextBlock;
class UCS_SchemeViewer;
class UOverlay;
class UCS_BaseButton;
class UCS_LocalizableTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloseScheme);


UCLASS()
class CORE_SIMULATOR_API UCS_ShowScheme : public UCS_UserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ShowScheme")
	TSubclassOf<UCS_SchemeViewer> SchemeViewerClass;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowScheme", meta = (BindWidget))
	TObjectPtr<UOverlay> Scheme_Overlay;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowScheme", meta = (BindWidget))
	TObjectPtr<UCS_BaseButton> CloseScheme_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowScheme", meta = (BindWidget))
	TObjectPtr<UButton> PrevScheme_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowScheme", meta = (BindWidget))
	TObjectPtr<UButton> NextScheme_Button;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowScheme", meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentSchemePosition_TextBlock;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ShowScheme", meta = (BindWidget))
	TObjectPtr<UCS_LocalizableTextBlock> TextBlock_NameMenu;

	UPROPERTY(BlueprintAssignable, Category = "CS_ShowScheme")
	FOnCloseScheme OnCloseScheme;

	void InitSchemeImage(TArray<TSoftObjectPtr<UTexture2D>> Textures);

	bool InitSchemeImageFromInfo(TArray<FLoadingImageInfo> LoadingImagesInfo);

	void SetClosSchemeFunc(TFunction<void()> Func);

	void ResetImageZoom();

	UFUNCTION()
	bool IsEmptyImages();

	UFUNCTION()
	int GetCurrentZoomPercent();
	
protected:

	UFUNCTION()
	void NextScheme();
	UFUNCTION()
	void PrevScheme();
	UFUNCTION()
	virtual void CloseScheme();
	UFUNCTION()
	void OnImageZoomChanged(int PercentZoom);
	UFUNCTION()
	void ChangeTitleText(const FString& NewText);
	UFUNCTION()
	void TryUpdateNameByImageInfo();
	UFUNCTION()
	void OnImagesLoaded();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ShowScheme")
	FString PreviewTextNameMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ShowScheme")
	FString NameMenuKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_ShowScheme")
	bool bZoomEnable = true;

	int32 CurrentIndexImage = 0;
	int32 MaxGuides = 0;
	TMap<int32, UCS_SchemeViewer*> Guides;

	TFunction<void()> ClosSchemeFunc;

	UPROPERTY()
	TArray<FLoadingImageInfo> CurrentImagesInfo;
};
