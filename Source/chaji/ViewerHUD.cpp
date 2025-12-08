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
    else
    {
        // Auto-create CameraViewController if not found
        FActorSpawnParameters SpawnParams;
        CameraController = GetWorld()->SpawnActor<ACameraViewController>(ACameraViewController::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        
        if (CameraController)
        {
            // Auto-configure from CineCameraActors
            CameraController->AutoConfigureFromCineCameras();
        }
    }
    
    SetupUI();
}

void AViewerHUD::SetupUI()
{
    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;
    
    // Create the tab widget (top-left)
    TabWidget = CreateWidget<UCategoryTabWidget>(PC, UCategoryTabWidget::StaticClass());
    if (TabWidget)
    {
        TabWidget->AddToViewport(10);
        TabWidget->SetPositionInViewport(FVector2D(20.0f, 30.0f));
        
        // Bind category selection event
        TabWidget->OnCategorySelected.AddDynamic(this, &AViewerHUD::OnCategorySelected);
        
        // Set categories from controller
        if (CameraController)
        {
            TArray<FString> Names = CameraController->GetCategoryNames();
            TabWidget->SetCategories(Names);
        }
    }
    
    // Create the viewpoint control widget (bottom-left, at very bottom of screen)
    ViewpointControl = CreateWidget<UViewpointControlWidget>(PC, UViewpointControlWidget::StaticClass());
    if (ViewpointControl)
    {
        ViewpointControl->AddToViewport(9);
        // Position at bottom of screen
        ViewpointControl->SetAnchorsInViewport(FAnchors(0.0f, 1.0f, 0.0f, 1.0f));
        ViewpointControl->SetPositionInViewport(FVector2D(20.0f, -100.0f));
        
        // Bind viewpoint change event
        ViewpointControl->OnViewpointChanged.AddDynamic(this, &AViewerHUD::OnViewpointChanged);
        
        // Set initial viewpoint count
        if (CameraController && CameraController->Categories.Num() > 0)
        {
            ViewpointControl->SetViewpointCount(CameraController->Categories[0].Viewpoints.Num());
        }
    }
    
    // Create the photo capture widget (top-right)
    PhotoCapture = CreateWidget<UPhotoCaptureWidget>(PC, UPhotoCaptureWidget::StaticClass());
    if (PhotoCapture)
    {
        PhotoCapture->AddToViewport(8);
        // Anchor to top-right
        PhotoCapture->SetAnchorsInViewport(FAnchors(1.0f, 0.0f, 1.0f, 0.0f));
        PhotoCapture->SetPositionInViewport(FVector2D(-200.0f, 30.0f));
        PhotoCapture->Initialize();
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
        
        // Update viewpoint count for the new category
        if (ViewpointControl && Index >= 0 && Index < CameraController->Categories.Num())
        {
            ViewpointControl->SetViewpointCount(CameraController->Categories[Index].Viewpoints.Num());
        }
    }
    
    if (TabWidget)
    {
        TabWidget->SetSelectedIndex(Index);
    }
}

void AViewerHUD::OnViewpointChanged(int32 Index)
{
    if (CameraController)
    {
        CameraController->SetViewpoint(Index);
    }
}
