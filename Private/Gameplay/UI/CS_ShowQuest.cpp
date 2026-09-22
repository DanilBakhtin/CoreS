// Core Simulator


#include "Gameplay/UI/CS_ShowQuest.h"
#include "Gameplay/UI/CS_ShowScheme.h"

void UCS_ShowQuest::NativeConstruct()
{
	check(ImageViewer);
		ImageViewer->SetClosSchemeFunc([&]() {CloseShowQuest(); });
}

void UCS_ShowQuest::InitHelpImages(TArray<TSoftObjectPtr<UTexture2D>> Textures) const
{
	ImageViewer->InitSchemeImage(Textures);
}

void UCS_ShowQuest::SetCloseQuestFunc(TFunction<void()> Func)
{
	CloseQuestFunc = Func;
}

void UCS_ShowQuest::CloseShowQuest()
{
	if (CloseQuestFunc)
		CloseQuestFunc();
}
