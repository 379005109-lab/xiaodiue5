// ViewerHUD.cpp
#include "ViewerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

AViewerHUD::AViewerHUD()
{
}

void AViewerHUD::BeginPlay()
{
    Super::BeginPlay();
    
    // Find CameraViewController in the level
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraViewController::StaticClass(), FoundActors);
    
    if (FoundActors.Num() > 0)
    {
        CameraController = Cast<ACameraViewController>(FoundActors[0]);
    }
    
    SetupUI();
}

void AViewerHUD::SetupUI()
{
    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;
    
    // Create the tab widget (top)
    TabWidget = CreateWidget<UCategoryTabWidget>(PC, UCategoryTabWidget::StaticClass());
    if (TabWidget)
    {
        TabWidget->AddToViewport(10);
        TabWidget->SetPositionInViewport(FVector2D(50.0f, 50.0f));
        
        // Bind category selection event
        TabWidget->OnCategorySelected.AddDynamic(this, &AViewerHUD::OnCategorySelected);
        
        // Set categories from controller
        if (CameraController)
        {
            TArray<FString> Names = CameraController->GetCategoryNames();
            TabWidget->SetCategories(Names);
        }
    }
    
    // Create the viewpoint list widget (bottom-left)
    ViewpointListWidget = CreateWidget<UViewpointListWidget>(PC, UViewpointListWidget::StaticClass());
    if (ViewpointListWidget)
    {
        ViewpointListWidget->AddToViewport(9);
        ViewpointListWidget->SetPositionInViewport(FVector2D(50.0f, 600.0f));
        ViewpointListWidget->Hide(); // Initially hidden
        
        // Bind viewpoint selection event
        ViewpointListWidget->OnViewpointSelected.AddDynamic(this, &AViewerHUD::OnViewpointSelected);
    }
    
    // Set input mode to allow UI interaction while keeping game input
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);
    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;
}

void AViewerHUD::OnCategorySelected(int32 Index)
{
    if (CameraController)
    {
        CameraController->SwitchCategory(Index);
        
        // Show viewpoint list for this category
        if (ViewpointListWidget && Index >= 0 && Index < CameraController->Categories.Num())
        {
            int32 ViewpointCount = CameraController->Categories[Index].Viewpoints.Num();
            ViewpointListWidget->SetViewpoints(ViewpointCount);
            ViewpointListWidget->Show();
        }
    }
    
    if (TabWidget)
    {
        TabWidget->SetSelectedIndex(Index);
    }
}

void AViewerHUD::OnViewpointSelected(int32 Index)
{
    if (CameraController)
    {
        CameraController->CurrentViewpointIndex = Index;
        CameraController->NextViewpoint(); // This will move to the selected viewpoint
    }
    
    if (ViewpointListWidget)
    {
        ViewpointListWidget->SetSelectedIndex(Index);
    }
}
