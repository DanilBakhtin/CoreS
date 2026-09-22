#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Framework/CS_CoreTypes.h"
#include "CS_ImageManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnImageLoaded, const FString&, ApiID, UTexture2D*, Texture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllImagesLoaded);
DECLARE_DELEGATE_OneParam(FOnImageLoadComplete, UTexture2D* /*Texture*/);

UCLASS()
class CORE_SIMULATOR_API UCS_ImageManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void SetImagesPointsInfo(const TMap<FString, FMainMenuPointInfo>& _MapImagesPointsInfo);

	const TArray<FLoadingImageInfo> GetImagesInfoByPointID(const FString& PointID);

	void TryLoadImages(const TArray<FLoadingImageInfo>& LoadingImagesInfo);

	void UpdatedInfoIconImageByPath(FString ApiID, FString Aos_Path);

	void LoadingAllIcons();

	const F_CS_ImageInfo* GetImageInfoByID(const FString& ApiID) const;

	const F_CS_ImageInfo* GetImageInfoByPath(const FString& ApiID) const;

	UTexture2D* GetImageByID(const FString& ApiID) const;

	UTexture2D* GetImageByPath(const FString& ApiID) const;

	bool HasImageInQueue(const FString& ApiID) const;

	UPROPERTY(BlueprintAssignable, Category = "CS_ImageManager")
	FOnImageLoaded OnImageLoaded;

	UPROPERTY(BlueprintAssignable, Category = "CS_ImageManager")
	FOnAllImagesLoaded OnAllImageLoaded;

private:

	UPROPERTY()
	TMap<FString, F_CS_ImageInfo> MapLoadedImageByID;

	UPROPERTY()
	TMap<FString, F_CS_ImageInfo> MapLoadedImageByPath;

	UPROPERTY()
	TMap<FString, int> MapImageQueueByID;

	UPROPERTY()
	TMap<FString, FMainMenuPointInfo> MapImagesPointsInfo;

	void LoadingImageAsync(const FLoadingImageInfo& Info);

	UFUNCTION()
	void RemoveFromQueue(const FString& ApiID);

	TMap<FString, FOnImageLoadComplete> PendingCallbacks;
};