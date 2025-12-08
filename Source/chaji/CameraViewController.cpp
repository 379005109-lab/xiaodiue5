// CameraViewController.cpp
#include "CameraViewController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

ACameraViewController::ACameraViewController()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentCategoryIndex = 0;
    CurrentViewpointIndex = 0;
}

void ACameraViewController::BeginPlay()
{
    Super::BeginPlay();
    
    if (Categories.Num() > 0)
    {
        MoveToCurrentViewpoint();
    }
}

void ACameraViewController::SwitchCategory(int32 Index)
{
    if (Index >= 0 && Index < Categories.Num())
    {
        CurrentCategoryIndex = Index;
        CurrentViewpointIndex = 0;
        MoveToCurrentViewpoint();
    }
}

void ACameraViewController::NextViewpoint()
{
    if (Categories.Num() == 0) return;
    
    const FCategoryViewpoint& Category = Categories[CurrentCategoryIndex];
    if (Category.Viewpoints.Num() == 0) return;
    
    CurrentViewpointIndex = (CurrentViewpointIndex + 1) % Category.Viewpoints.Num();
    MoveToCurrentViewpoint();
}

TArray<FString> ACameraViewController::GetCategoryNames()
{
    TArray<FString> Names;
    for (const FCategoryViewpoint& Cat : Categories)
    {
        Names.Add(Cat.CategoryName);
    }
    return Names;
}

void ACameraViewController::MoveToCurrentViewpoint()
{
    if (Categories.Num() == 0) return;
    
    const FCategoryViewpoint& Category = Categories[CurrentCategoryIndex];
    if (Category.Viewpoints.Num() == 0) return;
    
    const FTransform& Target = Category.Viewpoints[CurrentViewpointIndex];
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && PC->GetPawn())
    {
        PC->GetPawn()->SetActorLocation(Target.GetLocation());
        PC->SetControlRotation(Target.GetRotation().Rotator());
    }
}
