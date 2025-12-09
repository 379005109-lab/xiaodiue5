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

void AViewerHUD::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    HandleGlobalInput();
}

void AViewerHUD::HandleGlobalInput()
{
    if (!PhotoCapture) return;
    
    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;
    
    // Check for mouse wheel with modifiers
    static float LastScrollTime = 0.0f;
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    // Get scroll delta from input
    float ScrollDelta = PC->GetInputAxisValue(TEXT("MouseWheelAxis"));
    
    if (FMath::Abs(ScrollDelta) > 0.01f && (CurrentTime - LastScrollTime) > 0.05f)
    {
        LastScrollTime = CurrentTime;
        
        bool bCtrl = PC->IsInputKeyDown(EKeys::LeftControl) || PC->IsInputKeyDown(EKeys::RightControl);
        bool bShift = PC->IsInputKeyDown(EKeys::LeftShift) || PC->IsInputKeyDown(EKeys::RightShift);
        bool bAlt = PC->IsInputKeyDown(EKeys::LeftAlt) || PC->IsInputKeyDown(EKeys::RightAlt);
        
        if (bCtrl)
        {
            // Ctrl + Scroll = Focal Length
            PhotoCapture->AdjustFocalLength(ScrollDelta * 5.0f);
        }
        else if (bShift)
        {
            // Shift + Scroll = Aperture
            PhotoCapture->AdjustAperture(ScrollDelta * 0.5f);
        }
        else if (bAlt)
        {
            // Alt + Scroll = Focus Distance
            PhotoCapture->AdjustFocusDistance(ScrollDelta * 100.0f);
        }
    }
}
