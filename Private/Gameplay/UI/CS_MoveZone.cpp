// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/UI/CS_MoveZone.h"

#include "Components/SizeBox.h"

void UCS_MoveZone::NativeConstruct()
{
	Super::NativeConstruct();

	check(Left_Icon_SizeBox);
	check(Top_Icon_SizeBox);
	check(Right_Icon_SizeBox);
	check(Down_Icon_SizeBox);
}


void UCS_MoveZone::SetShowArrows(const bool& bUseUpDownMoveZone, const bool& bUseLeftRightMoveZone, bool bPlayAnim)
{
	if(bUseUpDownMoveZone)
	{
		Top_Icon_SizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		Down_Icon_SizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Top_Icon_SizeBox->SetVisibility(ESlateVisibility::Collapsed);
		Down_Icon_SizeBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	if(bUseLeftRightMoveZone)
	{
		Left_Icon_SizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		Right_Icon_SizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Left_Icon_SizeBox->SetVisibility(ESlateVisibility::Collapsed);
		Right_Icon_SizeBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	if((bUseUpDownMoveZone || bUseLeftRightMoveZone) && bPlayAnim)
		PlayArrowAnimation();
}

void UCS_MoveZone::SetShowArrows(const bool& bUseUpMoveZone, const bool& bUseDownMoveZone, const bool& bUseLeftMoveZone, const bool& bUseRightMoveZone)
{
	Top_Icon_SizeBox->SetVisibility(bUseUpMoveZone ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

	Down_Icon_SizeBox->SetVisibility(bUseDownMoveZone ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

	Left_Icon_SizeBox->SetVisibility(bUseLeftMoveZone ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

	Right_Icon_SizeBox->SetVisibility(bUseRightMoveZone ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UCS_MoveZone::SetLockCameraMove(bool Lock) const
{
	if(LockCameraMoveFun)
		LockCameraMoveFun(Lock);
}

void UCS_MoveZone::SetLockCameraMoveFun(TFunction<void(bool Lock)> Fun)
{
	LockCameraMoveFun = Fun;
}

