// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "CS_TextBlock.generated.h"

UCLASS()
class CORE_SIMULATOR_API UCS_TextBlock : public UTextBlock
{
	GENERATED_BODY()
	
public:

	UCS_TextBlock();

	virtual void SetText(FText InText) override;

	virtual void SetTextFromString(FString InString);

protected:

	virtual void Handle_HTML_tags(FString& SourceString);
};
