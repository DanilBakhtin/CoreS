#pragma once

#include "CoreMinimal.h"
#include "CS_Interact_Component_Interface.h"
#include "Components/DecalComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CS_Interaction_CellComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCellDelegate, bool, bIsSelected, class UCS_Interaction_CellComp*, CellRef);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCellPointAction, const FString&, apiId, const EPointAction&, PointAction);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CORE_SIMULATOR_API UCS_Interaction_CellComp : public UDecalComponent, public ICS_Interact_Component_Interface
{
    GENERATED_BODY()

public:
    UCS_Interaction_CellComp(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, Category = "CS_Interaction_CellComp")
    void SetDecalSize(FVector NewSize);

    UFUNCTION(BlueprintCallable, Category = "CS_Interaction_CellComp")
    void ResetCell();

    virtual void MakeDescription_Implementation(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description) override;
    virtual void DisableComponent_Implementation() override;
    virtual FString GetInteractID_Implementation() const override;

    UFUNCTION(BlueprintCallable, Category = "CS_Interaction_CellComp")
    virtual void RightClickInteract_Implementation() override;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Params")
    FString InteractID = "";
    /*UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Params")
    FString EndTwinID = "";*/


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CS_Interaction_CellComp")
    FName HighlightParamName = "EnableHighlight";

    UPROPERTY(BlueprintAssignable, Category = "Cell Events")
    FOnCellDelegate OnCellSelected;

    UPROPERTY(BlueprintAssignable, Category = "Cell Events")
    FCellPointAction OnCellPointAction;

    UFUNCTION()
    UBoxComponent* GetBoxCollision() const { return BoxCollision; };

protected:

    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    UPROPERTY()
    TObjectPtr<ACS_Interactable_Actor> Owner;

    UPROPERTY()
    FString Description;

    UPROPERTY()
    bool bInitDescription;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> DynamicDecalMaterial;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* BoxCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* AdditionMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Settings")
    TObjectPtr<UStaticMesh> AdditionStaticMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Settings")
    class UMaterialInterface* AdditionMeshMaterial;

    UPROPERTY()
    bool bIsSelected = false;

    UPROPERTY(BlueprintReadWrite, Category = "CS_Interaction_CellComp")
    TArray<FString> States;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecalPost")
    float AdditionHeight = 500.0f;

    UPROPERTY()
    bool bIsComponentEnable = false;

    UFUNCTION()
    void BeginMouseOverLap(UPrimitiveComponent* TouchedComponent);

    UFUNCTION()
    void EndMouseOverLap(UPrimitiveComponent* TouchedComponent);

    UFUNCTION()
    void OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

    UFUNCTION()
    void AddBoxCollision();

    UFUNCTION()
    void AddAdditionMesh();

    UFUNCTION()
    void CreateDynamicMaterial();

    UFUNCTION()
    void SetMaterialParameter(float NewValue);

    UFUNCTION()
    void PostModificationHeight();

    UFUNCTION(BlueprintCallable, Category = "CS_Interaction_CellComp")
    void ChoiceCell(bool bChoice);

    UFUNCTION(BlueprintCallable, Category = "CS_Interaction_CellComp")
    bool HasState(const FString& StateToCheck) const;

    UFUNCTION(BlueprintCallable, Category = "CS_Interaction_CellComp")
    void SetStateAtIndex(int32 Index, const FString& NewState);

    UFUNCTION(BlueprintCallable, Category = "CS_Interaction_CellComp")
    FString GetMatchState(const FString& PartState);

    UFUNCTION(BlueprintCallable, BlueprintPure,Category = "CS_Interaction_CellComp")
    FString GetStateAtIndex(int32 Index) const;
};
