#include "Gameplay/Utilities/CS_ImageManager.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "Async/Async.h"

void UCS_ImageManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCS_ImageManager::Deinitialize()
{
	for (auto& Pair : MapLoadedImageByID)
	{
		if (Pair.Value.Image)
		{
			Pair.Value.Image->RemoveFromRoot();
		}
	}
	MapLoadedImageByPath.Empty();

	for (auto& Pair : MapLoadedImageByPath)
	{
		if (Pair.Value.Image)
		{
			Pair.Value.Image->RemoveFromRoot();
		}
	}
	MapLoadedImageByPath.Empty();

	Super::Deinitialize();
}

void UCS_ImageManager::SetImagesPointsInfo(const TMap<FString, FMainMenuPointInfo>& _MapImagesPointsInfo)
{
	MapImagesPointsInfo = _MapImagesPointsInfo;
}

const TArray<FLoadingImageInfo> UCS_ImageManager::GetImagesInfoByPointID(const FString& PointID)
{
	if (MapImagesPointsInfo.Contains(PointID)) {
		FMainMenuPointInfo PointInfo = *MapImagesPointsInfo.Find(PointID);

		return PointInfo.LoadingImagesInfo;
	}

	return TArray<FLoadingImageInfo>();
}

void UCS_ImageManager::TryLoadImages(const TArray<FLoadingImageInfo>& LoadingImagesInfo)
{
	for (const auto& Info : LoadingImagesInfo)
	{
		if (!Info.Path.IsEmpty() && !MapLoadedImageByID.Contains(Info.ApiID))
		{
			FLoadingImageInfo ModifiedInfo = Info;

			FString LeftPart, RightPart;
			ModifiedInfo.Path.Split(TEXT("/"), &LeftPart, &RightPart, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

			ModifiedInfo.Path = FPaths::ProjectContentDir() + "Images/Schemes/" + RightPart;

			MapImageQueueByID.Add(ModifiedInfo.ApiID, 0);
			LoadingImageAsync(ModifiedInfo);

			UE_LOG(LogTemp, Warning, TEXT("Image file: %s"), *RightPart);
			UE_LOG(LogTemp, Warning, TEXT("Image file full path: %s"), *ModifiedInfo.Path);
		}
	}
}

void UCS_ImageManager::UpdatedInfoIconImageByPath(FString ApiID, FString Aos_Path)
{
    if (!ApiID.IsEmpty() && !Aos_Path.IsEmpty())
    {
        if (MapLoadedImageByID.Contains(ApiID)) return;

        FString LeftPart, RightPart;
        FString ModifiedPath = Aos_Path;
        ModifiedPath.Split(TEXT("/"), &LeftPart, &RightPart, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

        ModifiedPath = FPaths::ProjectContentDir() + "Images/Icons/" + RightPart;

        if (MapLoadedImageByPath.Contains(ModifiedPath))
        {
            MapLoadedImageByID.Add(ApiID, MapLoadedImageByPath[ModifiedPath]);
        }
    }
}

void UCS_ImageManager::LoadingAllIcons()
{
    FString DirectoryPath = FPaths::ProjectContentDir() + "Images/Icons/";

    if (!FPaths::DirectoryExists(DirectoryPath))
    {
        UE_LOG(LogTemp, Error, TEXT("[ImageManager] Directory does not exist: %s"), *DirectoryPath);
        return;
    }

    TArray<FString> FoundFiles;
    IFileManager& FileManager = IFileManager::Get();

    FString FileMask = DirectoryPath + "*.png";
    FileManager.FindFiles(FoundFiles, *FileMask, true, false);

    FileMask = DirectoryPath + "*.jpg";
    FileManager.FindFiles(FoundFiles, *FileMask, true, false);

    FileMask = DirectoryPath + "*.jpeg";
    FileManager.FindFiles(FoundFiles, *FileMask, true, false);

    FileMask = DirectoryPath + "*.bmp";
    FileManager.FindFiles(FoundFiles, *FileMask, true, false);

    if (FoundFiles.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ImageManager] No image files found in: %s"), *DirectoryPath);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[ImageManager] Found %d image files to load"), FoundFiles.Num());

    for (const FString& FileName : FoundFiles)
    {
        FString FullPath = DirectoryPath + FileName;

        FLoadingImageInfo ImageInfo;
        ImageInfo.Name = FPaths::GetBaseFilename(FileName);
        ImageInfo.Path = FullPath;
        ImageInfo.ApiID = FGuid::NewGuid().ToString();

        if (!MapLoadedImageByPath.Contains(FullPath))
        {
            UE_LOG(LogTemp, Log, TEXT("[ImageManager] Loading image: %s, ID: %s"), *ImageInfo.Name, *ImageInfo.ApiID);
            MapImageQueueByID.Add(ImageInfo.ApiID, 0);
            LoadingImageAsync(ImageInfo);
        }
        else
        {
            UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] Image already loaded, skipping: %s"), *FileName);
        }
    }
}

const F_CS_ImageInfo* UCS_ImageManager::GetImageInfoByID(const FString& ApiID) const
{
	return MapLoadedImageByID.Find(ApiID);
}

const F_CS_ImageInfo* UCS_ImageManager::GetImageInfoByPath(const FString& ApiID) const
{
	return MapLoadedImageByPath.Find(ApiID);
}

UTexture2D* UCS_ImageManager::GetImageByID(const FString& ApiID) const
{
	if (const F_CS_ImageInfo* ImageInfo = MapLoadedImageByID.Find(ApiID))
	{
		return ImageInfo->Image;
	}

	return nullptr;
}

UTexture2D* UCS_ImageManager::GetImageByPath(const FString& ApiID) const
{
	if (const F_CS_ImageInfo* ImageInfo = MapLoadedImageByPath.Find(ApiID))
	{
		return ImageInfo->Image;
	}

	return nullptr;
}

bool UCS_ImageManager::HasImageInQueue(const FString& ApiID) const
{
	return MapImageQueueByID.Contains(ApiID);
}

void UCS_ImageManager::LoadingImageAsync(const FLoadingImageInfo& Info)
{
    UE_LOG(LogTemp, Log, TEXT("[ImageManager] Starting async image load - name: %s, apiId: %s, path: %s"),
        *Info.Name, *Info.ApiID, *Info.Path);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
            FString::Printf(TEXT("[START] Loading image: %s, %s"), *Info.Name, *Info.Path));
    }

    TWeakObjectPtr<UCS_ImageManager> WeakThis(this);

    Async(EAsyncExecution::ThreadPool, [WeakThis, Info]()
        {
            UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] Thread pool execution started for: %s"), *Info.Name);

            if (!WeakThis.IsValid())
            {
                UE_LOG(LogTemp, Warning, TEXT("[ImageManager] WeakThis invalid at thread pool start for: %s"), *Info.Name);

                if (WeakThis.IsValid())
                {
                    WeakThis->RemoveFromQueue(Info.ApiID);
                }
                return;
            }

            UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] Loading file from path: %s"), *Info.Path);

            TArray<uint8> FileData;
            if (!FFileHelper::LoadFileToArray(FileData, *Info.Path))
            {
                UE_LOG(LogTemp, Error, TEXT("[ImageManager] Failed to load file from path: %s"), *Info.Path);

                if (WeakThis.IsValid())
                {
                    AsyncTask(ENamedThreads::GameThread, [WeakThis, ApiID = Info.ApiID]()
                        {
                            if (WeakThis.IsValid())
                            {
                                UCS_ImageManager* Manager = WeakThis.Get();
                                Manager->OnImageLoaded.Broadcast(ApiID, nullptr);
                                Manager->RemoveFromQueue(ApiID);
                            }
                        });
                }
                return;
            }

            UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] File loaded successfully - size: %d bytes, name: %s"),
                FileData.Num(), *Info.Name);

            IImageWrapperModule& ImageWrapperModule =
                FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");

            EImageFormat Format =
                ImageWrapperModule.DetectImageFormat(FileData.GetData(), FileData.Num());

            if (Format == EImageFormat::Invalid)
            {
                UE_LOG(LogTemp, Error, TEXT("[ImageManager] Invalid image format for file: %s, path: %s"),
                    *Info.Name, *Info.Path);

                if (WeakThis.IsValid())
                {
                    AsyncTask(ENamedThreads::GameThread, [WeakThis, ApiID = Info.ApiID]()
                        {
                            if (WeakThis.IsValid())
                            {
                                UCS_ImageManager* Manager = WeakThis.Get();
                                Manager->OnImageLoaded.Broadcast(ApiID, nullptr);
                                Manager->RemoveFromQueue(ApiID);
                            }
                        });
                }
                return;
            }

            UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] Detected image format: %d for: %s"),
                (int)Format, *Info.Name);

            TSharedPtr<IImageWrapper> ImageWrapper =
                ImageWrapperModule.CreateImageWrapper(Format);

            if (!ImageWrapper.IsValid())
            {
                UE_LOG(LogTemp, Error, TEXT("[ImageManager] Failed to create image wrapper for: %s"), *Info.Name);

                if (WeakThis.IsValid())
                {
                    AsyncTask(ENamedThreads::GameThread, [WeakThis, ApiID = Info.ApiID]()
                        {
                            if (WeakThis.IsValid())
                            {
                                UCS_ImageManager* Manager = WeakThis.Get();
                                Manager->OnImageLoaded.Broadcast(ApiID, nullptr);
                                Manager->RemoveFromQueue(ApiID);
                            }
                        });
                }
                return;
            }

            if (!ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num()))
            {
                UE_LOG(LogTemp, Error, TEXT("[ImageManager] Failed to set compressed data for: %s"), *Info.Name);

                if (WeakThis.IsValid())
                {
                    AsyncTask(ENamedThreads::GameThread, [WeakThis, ApiID = Info.ApiID]()
                        {
                            if (WeakThis.IsValid())
                            {
                                UCS_ImageManager* Manager = WeakThis.Get();
                                Manager->OnImageLoaded.Broadcast(ApiID, nullptr);
                                Manager->RemoveFromQueue(ApiID);
                            }
                        });
                }
                return;
            }

            UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] Compressed data set successfully for: %s"), *Info.Name);

            TArray<uint8> UncompressedRGBA;

            if (!ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
            {
                UE_LOG(LogTemp, Error, TEXT("[ImageManager] Failed to get raw RGBA data for: %s"), *Info.Name);

                if (WeakThis.IsValid())
                {
                    AsyncTask(ENamedThreads::GameThread, [WeakThis, ApiID = Info.ApiID]()
                        {
                            if (WeakThis.IsValid())
                            {
                                UCS_ImageManager* Manager = WeakThis.Get();
                                Manager->OnImageLoaded.Broadcast(ApiID, nullptr);
                                Manager->RemoveFromQueue(ApiID);
                            }
                        });
                }
                return;
            }

            int32 Width = ImageWrapper->GetWidth();
            int32 Height = ImageWrapper->GetHeight();

            UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] Image decoded - dimensions: %dx%d, size: %d bytes, name: %s"),
                Width, Height, UncompressedRGBA.Num(), *Info.Name);

            AsyncTask(ENamedThreads::GameThread,
                [WeakThis, Info, Width, Height, UncompressedRGBA = MoveTemp(UncompressedRGBA)]() mutable
                {
                    UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] Game thread execution started for: %s"), *Info.Name);

                    if (!WeakThis.IsValid())
                    {
                        UE_LOG(LogTemp, Warning, TEXT("[ImageManager] WeakThis invalid in game thread for: %s"), *Info.Name);
                        return;
                    }

                    UCS_ImageManager* Manager = WeakThis.Get();

                    UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] Checking if image already loaded by path: %s"), *Info.Path);

                    if (!Manager || Manager->MapLoadedImageByPath.Contains(Info.Path))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("[ImageManager] Image already loaded by path: %s, skipping"), *Info.Path);

                        if (Manager)
                        {
                            Manager->MapLoadedImageByID.Add(Info.ApiID, *Manager->GetImageInfoByPath(Info.Path));
                            UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] Added existing image to MapLoadedImageByID - apiId: %s"), *Info.ApiID);
                        }

                        Manager->RemoveFromQueue(Info.ApiID);
                        return;
                    }

                    if (!Manager || Manager->MapLoadedImageByID.Contains(Info.ApiID))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("[ImageManager] Image already loaded by ID: %s, skipping"), *Info.ApiID);

                        Manager->RemoveFromQueue(Info.ApiID);
                        return;
                    }

                    UE_LOG(LogTemp, Log, TEXT("[ImageManager] Creating texture - dimensions: %dx%d, name: %s"),
                        Width, Height, *Info.Name);

                    UTexture2D* Texture =
                        UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);

                    if (!Texture || !Texture->GetPlatformData() ||
                        Texture->GetPlatformData()->Mips.Num() == 0)
                    {
                        UE_LOG(LogTemp, Error, TEXT("[ImageManager] Failed to create texture for: %s"), *Info.Name);

                        if (GEngine)
                        {
                            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
                                FString::Printf(TEXT("[ERROR] Failed to create texture: %s"), *Info.Name));
                        }

                        Manager->OnImageLoaded.Broadcast(Info.ApiID, nullptr);
                        Manager->RemoveFromQueue(Info.ApiID);
                        return;
                    }

                    UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] Texture created successfully, copying pixel data..."));

                    void* TextureData =
                        Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

                    FMemory::Memcpy(TextureData,
                        UncompressedRGBA.GetData(),
                        UncompressedRGBA.Num());

                    Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
                    Texture->UpdateResource();

                    Texture->AddToRoot();

                    UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] Texture resource updated and protected from GC"));

                    F_CS_ImageInfo ImageInfo;
                    ImageInfo.Name = Info.Name;
                    ImageInfo.Image = Texture;

                    Manager->MapLoadedImageByID.Add(Info.ApiID, ImageInfo);
                    Manager->MapLoadedImageByPath.Add(Info.Path, ImageInfo);

                    UE_LOG(LogTemp, Log, TEXT("[ImageManager] Image added to maps - MapLoadedImageByID count: %d, MapLoadedImageByPath count: %d"),
                        Manager->MapLoadedImageByID.Num(), Manager->MapLoadedImageByPath.Num());

                    Manager->RemoveFromQueue(Info.ApiID);

                    Manager->OnImageLoaded.Broadcast(Info.ApiID, Texture);

                    UE_LOG(LogTemp, Log, TEXT("[ImageManager] SUCCESS - Image fully loaded and broadcasted - name: %s, apiId: %s, path: %s, dimensions: %dx%d"),
                        *Info.Name, *Info.ApiID, *Info.Path, Width, Height);
                });
        });
}

void UCS_ImageManager::RemoveFromQueue(const FString& ApiID)
{
	if (MapImageQueueByID.Num() == 0) 
	{
		OnAllImageLoaded.Broadcast();
		return;
	}

	if (MapImageQueueByID.Contains(ApiID))
	{
		MapImageQueueByID.Remove(ApiID);
		UE_LOG(LogTemp, Verbose, TEXT("[ImageManager] Removed %s from queue. Queue size: %d"),
			*ApiID, MapImageQueueByID.Num());
	}
}