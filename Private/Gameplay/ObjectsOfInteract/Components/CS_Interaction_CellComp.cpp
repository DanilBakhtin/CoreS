#include "Gameplay/ObjectsOfInteract/Components/CS_Interaction_CellComp.h"
#include "Gameplay/Player/CS_PlayerController.h"
#include "Gameplay/ObjectsOfInteract/CS_Interactable_Actor.h"

UCS_Interaction_CellComp::UCS_Interaction_CellComp(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = false;

    DecalSize = FVector(200.0f, 300.0f, 200.0f);

    SetRelativeRotation(FRotator(-90.f, 90.f, 0.f));
}

void UCS_Interaction_CellComp::BeginPlay()
{
    Super::BeginPlay();

    AddBoxCollision();

    AddAdditionMesh();

    CreateDynamicMaterial();

    PostModificationHeight();
}

void UCS_Interaction_CellComp::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (BoxCollision)
    {
        BoxCollision->OnBeginCursorOver.RemoveDynamic(this, &UCS_Interaction_CellComp::BeginMouseOverLap);
        BoxCollision->OnEndCursorOver.RemoveDynamic(this, &UCS_Interaction_CellComp::EndMouseOverLap);
        BoxCollision->OnClicked.RemoveDynamic(this, &UCS_Interaction_CellComp::OnMouseClicked);
    }

    Super::EndPlay(EndPlayReason);
}

void UCS_Interaction_CellComp::BeginMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
    if (!bIsSelected)
        SetMaterialParameter(1.0f);

    OnCellPointAction.Broadcast(InteractID, EPointAction::OnMouseOver);

    if (Owner)
        if (Owner->GetCurrentPlayerController())
            Owner->GetCurrentPlayerController()->ShowMouseDescription(InteractID, Description);
}

void UCS_Interaction_CellComp::EndMouseOverLap(UPrimitiveComponent* TouchedComponent)
{
    if (!bIsSelected)
        SetMaterialParameter(0.0f);

    OnCellPointAction.Broadcast(InteractID, EPointAction::OnMouseOut);

    if(Owner)
        if (Owner->GetCurrentPlayerController())
            Owner->GetCurrentPlayerController()->ShowMouseDescription(InteractID, FString());
}

void UCS_Interaction_CellComp::OnMouseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
    if (ButtonPressed == EKeys::LeftMouseButton)
    {   
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Cell %s LMB"), *InteractID));

        UE_LOG(LogTemp, Log, TEXT("Left mouse button clicked on cell: %s"), *GetName());

        OnCellPointAction.Broadcast(InteractID, EPointAction::OnClick);

        SetMaterialParameter(!bIsSelected ? 1.0f : 0.0f);
    }
}

void UCS_Interaction_CellComp::AddBoxCollision()
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("UCS_Cell has no owner!"));
        return;
    }

    if (BoxCollision)
    {
        UE_LOG(LogTemp, Warning, TEXT("BoxCollision already exists!"));
        return;
    }

    FName BoxCollisionName = FName(FString::Printf(TEXT("BoxCollissionCell_%s"), *GetName()));
    BoxCollision = NewObject<UBoxComponent>(OwnerActor, BoxCollisionName);
    if (BoxCollision)
    {
        BoxCollision->SetupAttachment(OwnerActor->GetRootComponent());
        BoxCollision->RegisterComponent();

        FVector CollisionScale = FVector(DecalSize.X * 5.0f, DecalSize.Y, DecalSize.Z);
        BoxCollision->SetBoxExtent(CollisionScale);

        BoxCollision->SetWorldTransform(GetComponentTransform());
        //BoxCollision->SetRelativeTransform(GetRelativeTransform());

        BoxCollision->SetCollisionProfileName(FName("InteractObject"), true);
        BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

        OwnerActor->AddInstanceComponent(BoxCollision);

        BoxCollision->OnBeginCursorOver.AddDynamic(this, &UCS_Interaction_CellComp::BeginMouseOverLap);
        BoxCollision->OnEndCursorOver.AddDynamic(this, &UCS_Interaction_CellComp::EndMouseOverLap);
        BoxCollision->OnClicked.AddDynamic(this, &UCS_Interaction_CellComp::OnMouseClicked);

        UE_LOG(LogTemp, Log, TEXT("Box collision added successfully to owner: %s"), *OwnerActor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create BoxComponent!"));
    }
}

void UCS_Interaction_CellComp::AddAdditionMesh()
{

    if (!AdditionStaticMesh)
        return;

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("UCS_Cell has no owner!"));
        return;
    }

    if (AdditionMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("AdditionMesh already exists!"));
        return;
    }

    FVector MeshBounds = AdditionStaticMesh->GetBoundingBox().GetSize();
    if (MeshBounds.IsNearlyZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("Mesh bounds are zero! Using default size."));
        MeshBounds = FVector(100.0f, 100.0f, 100.0f);
    }


    FName MeshName = FName(FString::Printf(TEXT("MeshCell_%s"), *GetName()));

    AdditionMesh = NewObject<UStaticMeshComponent>(OwnerActor, MeshName);

    if (AdditionMesh)
    {
        AdditionMesh->SetupAttachment(OwnerActor->GetRootComponent());
        AdditionMesh->RegisterComponent();
        
        AdditionMesh->SetWorldTransform(GetComponentTransform());

        AdditionMesh->SetWorldLocation(AdditionMesh->GetComponentLocation() + FVector(0.0f,0.0f,1.0f));
        
        AdditionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        /*AdditionMesh->SetCollisionProfileName(FName("InteractObject"), true);
        AdditionMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);*/

        AdditionMesh->SetStaticMesh(AdditionStaticMesh);

        FVector MeshScale = FVector(
            DecalSize.X / (MeshBounds.X > 0 ? MeshBounds.X : 100.0f),
            DecalSize.Y / (MeshBounds.Y > 0 ? MeshBounds.Y : 100.0f),
            DecalSize.Z / (MeshBounds.Z > 0 ? MeshBounds.Z : 100.0f)
        );

        AdditionMesh->SetWorldScale3D(MeshScale);


        if (AdditionMeshMaterial)
        {
            int32 NumMaterials = AdditionStaticMesh->GetStaticMaterials().Num();
            for (int32 i = 0; i < NumMaterials; i++)
            {
                    AdditionMesh->SetMaterial(i, AdditionMeshMaterial);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No material specified for AdditionMesh, using default."));
        }

        //AdditionMesh->OnBeginCursorOver.AddDynamic(this, &UCS_Interaction_CellComp::BeginMouseOverLap);
        //AdditionMesh->OnEndCursorOver.AddDynamic(this, &UCS_Interaction_CellComp::EndMouseOverLap);
        //AdditionMesh->OnClicked.AddDynamic(this, &UCS_Interaction_CellComp::OnMouseClicked);

        AdditionMesh->bReceivesDecals = false;
        AdditionMesh->SetVisibility(false);

        OwnerActor->AddInstanceComponent(AdditionMesh);

        UE_LOG(LogTemp, Log, TEXT("AdditionMesh added successfully to owner: %s"), *OwnerActor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create AdditionMesh!"));
    }
}

void UCS_Interaction_CellComp::SetDecalSize(FVector NewSize)
{
    this->DecalSize = NewSize;
}

void UCS_Interaction_CellComp::CreateDynamicMaterial() 
{
    if (GetDecalMaterial())
    {
        DynamicDecalMaterial = UMaterialInstanceDynamic::Create(GetDecalMaterial(), this);
        if (DynamicDecalMaterial)
        {
            SetDecalMaterial(DynamicDecalMaterial);
            SetMaterialParameter(0.0f);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create dynamic material!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No decal material to make dynamic!"));
    }
}

void UCS_Interaction_CellComp::SetMaterialParameter(float NewValue) 
{
    if (AdditionMesh){

        AdditionMesh->SetVisibility(NewValue > 0.0f);
    }
    if (DynamicDecalMaterial)
    {
        DynamicDecalMaterial->SetScalarParameterValue(HighlightParamName, NewValue);
    }
}

void UCS_Interaction_CellComp::PostModificationHeight()
{
    TArray<USceneComponent*> childComponents;
    TArray<FTransform> childTransforms;

    GetChildrenComponents(true, childComponents);

    for (USceneComponent* Child : childComponents)
        childTransforms.Add(Child->GetComponentTransform());

    SetDecalSize(FVector(DecalSize.X + AdditionHeight, DecalSize.Y, DecalSize.Z));

    FVector NewLocation = GetComponentTransform().GetLocation();
    NewLocation.Z += AdditionHeight * GetComponentTransform().GetScale3D().Z;

    SetWorldLocation(NewLocation);

    for (int i = 0; i < childComponents.Num(); i++)
        childComponents[i]->SetWorldTransform(childTransforms[i]);
}

void UCS_Interaction_CellComp::ChoiceCell(bool bChoice)
{
    bIsSelected = bChoice;
    OnCellSelected.Broadcast(bIsSelected, this);
    SetMaterialParameter(bIsSelected ? 1.0f : 0.0f);
}

bool UCS_Interaction_CellComp::HasState(const FString& StateToCheck) const
{
    return States.Contains(StateToCheck);
}

void UCS_Interaction_CellComp::SetStateAtIndex(int32 Index, const FString& NewState)
{
    if (Index < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid index for SetStateAtIndex: %d"), Index);
        return;
    }

    // ≈сли индекс больше или равен текущему размеру массива
    if (Index >= States.Num())
    {
        // ƒобавл€ем пустые строки до нужного индекса
        int32 ElementsToAdd = Index - States.Num() + 1;
        for (int32 i = 0; i < ElementsToAdd; i++)
        {
            States.Add(FString());
        }
    }

    // ”станавливаем значение по индексу
    States[Index] = NewState;
}

FString UCS_Interaction_CellComp::GetMatchState(const FString& PartState)
{
    for (int i = 0; i < States.Num(); i++) {
        if (States[i].Contains(PartState))
            return States[i];
    }

    return FString();
}

FString UCS_Interaction_CellComp::GetStateAtIndex(int32 Index) const
{
    if (States.IsValidIndex(Index))
    {
        return States[Index];
    }

    // ¬озвращаем пустую строку если индекс невалидный
    return FString();
}

void UCS_Interaction_CellComp::ResetCell()
{   
    if (!bIsSelected) return;

    UE_LOG(LogTemp, Log, TEXT("Left mouse button clicked on cell RESET: %s"), *GetName());

    bIsSelected = !bIsSelected;

    OnCellSelected.Broadcast(bIsSelected, this);

    OnCellPointAction.Broadcast(InteractID, EPointAction::OnClick);

    SetMaterialParameter(bIsSelected ? 1.0f : 0.0f);
}

void UCS_Interaction_CellComp::MakeDescription_Implementation(ACS_Interactable_Actor* Interactable_Actor, const FString& _Description)
{   
    if (bIsComponentEnable)
        return;

    if (!bInitDescription) {
        bInitDescription = true;
        Description = _Description;
        Owner = Interactable_Actor;
    }

    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    bIsComponentEnable = true;
    SetVisibility(bIsComponentEnable, true);

    if (bIsSelected)
        AdditionMesh->SetVisibility(true);

}

void UCS_Interaction_CellComp::DisableComponent_Implementation()
{
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    bIsComponentEnable = false;
    SetVisibility(bIsComponentEnable, true);

    if (AdditionMesh)
        AdditionMesh->SetVisibility(false);
}

FString UCS_Interaction_CellComp::GetInteractID_Implementation() const
{
    return InteractID;
}

void UCS_Interaction_CellComp::RightClickInteract_Implementation()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Cell %s RMB"), *InteractID));

    OnCellPointAction.Broadcast(InteractID, EPointAction::OnRClick);
}
