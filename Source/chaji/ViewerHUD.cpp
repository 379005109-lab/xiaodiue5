// ViewerHUD.cpp
#include "ViewerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Framework/Application/SlateApplication.h"

AViewerHUD::AViewerHUD()
{
    PrimaryActorTick.bCanEverTick = true;
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
    }
    
    // Always auto-configure from CineCameras
    if (CameraController)
    {
        CameraController->AutoConfigureFromCineCameras();
        UE_LOG(LogTemp, Log, TEXT("CameraController configured with %d categories"), CameraController->Categories.Num());
    }
    
    // Create PreviewManager for viewpoint previews
    FActorSpawnParameters SpawnParams;
    PreviewManager = GetWorld()->SpawnActor<AViewpointPreviewManager>(AViewpointPreviewManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    
    SetupUI();
    
    // Bind mouse wheel from PlayerController
    AViewerPlayerController* VPC = Cast<AViewerPlayerController>(GetOwningPlayerController());
    if (VPC)
    {
        VPC->OnMouseWheelScroll.AddDynamic(this, &AViewerHUD::OnMouseWheel);
    }
    
    // Setup initial previews
    if (CameraController && CameraController->Categories.Num() > 0)
    {
        SetupPreviewsForCategory(0);
        
        // Load initial camera settings
        if (PhotoCapture)
        {
            FCameraSettings Settings = CameraController->GetCurrentCameraSettings();
            PhotoCapture->LoadCameraSettings(Settings.FocalLength, Settings.Aperture, Settings.FocusDistance);
        }
    }
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
    
    // Get viewport size for responsive positioning
    FVector2D ViewportSize;
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
    }
    else
    {
        ViewportSize = FVector2D(1920.0f, 1080.0f); // Default fallback
    }
    
    // Create the viewpoint control widget (bottom-left, 2% from left, 95% from top)
    ViewpointControl = CreateWidget<UViewpointControlWidget>(PC, UViewpointControlWidget::StaticClass());
    if (ViewpointControl)
    {
        ViewpointControl->AddToViewport(9);
        float PosX = ViewportSize.X * 0.02f;
        float PosY = ViewportSize.Y * 0.88f;
        ViewpointControl->SetPositionInViewport(FVector2D(PosX, PosY));
        
        // Bind viewpoint change event
        ViewpointControl->OnViewpointChanged.AddDynamic(this, &AViewerHUD::OnViewpointChanged);
        
        // Set initial viewpoint count
        if (CameraController && CameraController->Categories.Num() > 0)
        {
            ViewpointControl->SetViewpointCount(CameraController->Categories[0].Viewpoints.Num());
        }
    }
    
    // Create the photo capture widget (top-right, 85% from left, 3% from top)
    PhotoCapture = CreateWidget<UPhotoCaptureWidget>(PC, UPhotoCaptureWidget::StaticClass());
    if (PhotoCapture)
    {
        PhotoCapture->AddToViewport(8);
        float PosX = ViewportSize.X * 0.82f;
        float PosY = ViewportSize.Y * 0.03f;
        PhotoCapture->SetPositionInViewport(FVector2D(PosX, PosY));
        PhotoCapture->InitWidget();
        
        // Set reference to pawn for shortcuts
        AViewerPawn* ViewerPawn = Cast<AViewerPawn>(PC->GetPawn());
        if (ViewerPawn)
        {
            ViewerPawn->PhotoCaptureRef = PhotoCapture;
        }
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
            
            // Setup previews for the new category
            SetupPreviewsForCategory(Index);
        }
    }
    
    if (TabWidget)
    {
        TabWidget->SetSelectedIndex(Index);
    }
}

void AViewerHUD::SetupPreviewsForCategory(int32 CategoryIndex)
{
    if (!PreviewManager || !CameraController) return;
    if (!CameraController->Categories.IsValidIndex(CategoryIndex)) return;
    
    const FCategoryViewpoint& Category = CameraController->Categories[CategoryIndex];
    PreviewManager->SetupPreviews(Category.Viewpoints);
    
    // Connect preview manager to widget
    if (ViewpointControl)
    {
        ViewpointControl->SetPreviewManager(PreviewManager);
    }
}

void AViewerHUD::OnViewpointChanged(int32 Index)
{
    if (CameraController)
    {
        CameraController->SetViewpoint(Index);
        
        // Load camera settings for this viewpoint
        if (PhotoCapture)
        {
            FCameraSettings Settings = CameraController->GetCurrentCameraSettings();
            PhotoCapture->LoadCameraSettings(Settings.FocalLength, Settings.Aperture, Settings.FocusDistance);
        }
    }
}

void AViewerHUD::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    HandleGlobalInput();
}

void AViewerHUD::HandleGlobalInput()
{
    if (!PhotoCapture) return;
    
    AViewerPlayerController* VPC = Cast<AViewerPlayerController>(GetOwningPlayerController());
    if (!VPC) return;
    
    // Check if there was a scroll event
    float ScrollDelta = VPC->LastScrollDelta;
    if (FMath::Abs(ScrollDelta) > 0.01f)
    {
        // Reset scroll delta
        VPC->LastScrollDelta = 0.0f;
        
        // Check modifier keys
        bool bCtrl = VPC->IsInputKeyDown(EKeys::LeftControl) || VPC->IsInputKeyDown(EKeys::RightControl);
        bool bShift = VPC->IsInputKeyDown(EKeys::LeftShift) || VPC->IsInputKeyDown(EKeys::RightShift);
        bool bAlt = VPC->IsInputKeyDown(EKeys::LeftAlt) || VPC->IsInputKeyDown(EKeys::RightAlt);
        
        if (bCtrl)
        {
            PhotoCapture->AdjustFocalLength(ScrollDelta * 5.0f);
            UE_LOG(LogTemp, Log, TEXT("Ctrl+Scroll: Focal Length"));
        }
        else if (bShift)
        {
            PhotoCapture->AdjustAperture(ScrollDelta * 0.5f);
            UE_LOG(LogTemp, Log, TEXT("Shift+Scroll: Aperture"));
        }
        else if (bAlt)
        {
            PhotoCapture->AdjustFocusDistance(ScrollDelta * 100.0f);
            UE_LOG(LogTemp, Log, TEXT("Alt+Scroll: Focus Distance"));
        }
    }
}

void AViewerHUD::OnMouseWheel(float Delta)
{
    // This is called from PlayerController, handled in HandleGlobalInput
}
