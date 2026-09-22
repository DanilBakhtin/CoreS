
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_MoveZone.generated.h"


class USizeBox;

UCLASS()
class CORE_SIMULATOR_API UCS_MoveZone : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "CS_ReceivedMessage", meta = (BindWidget))
	TObjectPtr<USizeBox> Left_Icon_SizeBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ReceivedMessage", meta = (BindWidget))
	TObjectPtr<USizeBox> Top_Icon_SizeBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ReceivedMessage", meta = (BindWidget))
	TObjectPtr<USizeBox> Right_Icon_SizeBox;
	UPROPERTY(BlueprintReadOnly, Category = "CS_ReceivedMessage", meta = (BindWidget))
	TObjectPtr<USizeBox> Down_Icon_SizeBox;
	
	void SetShowArrows(const bool& bUseUpDownMoveZone, const bool& bUseLeftRightMoveZone, bool bPlayAnim = true);

	void SetShowArrows(const bool& bUseUpMoveZone, const bool& bUseDownMoveZone, const bool& bUseLeftMoveZone, const bool& bUseRightMoveZone);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayArrowAnimation();
	
	void SetLockCameraMove(bool Lock) const;

	void SetLockCameraMoveFun(TFunction<void(bool Lock)> Fun);

protected:


	TFunction<void(bool Lock)> LockCameraMoveFun;
};
