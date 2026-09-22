// Core Simulator


#include "Gameplay/UI/CS_ShowInfo.h"

#include "Components/TextBlock.h"
#include "Framework/CS_CoreTypes.h"
#include "Framework/CS_UtilityFunctionLibrary.h"

void UCS_ShowInfo::NativeConstruct()
{
	Super::NativeConstruct();

	check(Info_TextBlock)
}

void UCS_ShowInfo::Init(const FShowWelcomeParams& Params) const
{
	FString FaultInfoText = UCS_UtilityFunctionLibrary::ReplaceSubstring(Params.FaultInfoDescription, "<br>", "\n") + "\n\n"
		+ UCS_UtilityFunctionLibrary::ReplaceSubstring(Params.FaultInfoQuest, "<br>", "\n");
	Info_TextBlock->SetText(FText::FromString(FaultInfoText));
}
