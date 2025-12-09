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
        
        // Load initial camera settings and category name
        if (PhotoCapture)
        {
            FCameraSettings Settings = CameraController->GetCurrentCameraSettings();
            PhotoCapture->LoadCameraSettings(Settings.FocalLength, Settings.Aperture, Settings.FocusDistance);
            PhotoCapture->SetCurrentCategoryName(CameraController->Categories[0].CategoryName);
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
    
    // Create the viewpoint control widget (bottom-left, photos/viewpoints area)
    ViewpointControl = CreateWidget<UViewpointControlWidget>(PC, UViewpointControlWidget::StaticClass());
    if (ViewpointControl)
    {
        ViewpointControl->AddToViewport(9);
        float PosX = ViewportSize.X * 0.02f;
        float PosY = ViewportSize.Y * 0.92f; // Very bottom
        ViewpointControl->SetPositionInViewport(FVector2D(PosX, PosY));
        
        // Bind viewpoint change event
        ViewpointControl->OnViewpointChanged.AddDynamic(this, &AViewerHUD::OnViewpointChanged);
        ViewpointControl->OnViewpointSaved.AddDynamic(this, &AViewerHUD::OnViewpointSaved);
        
        // Set initial viewpoint count
        if (CameraController && CameraController->Categories.Num() > 0)
        {
            ViewpointControl->SetViewpointCount(CameraController->Categories[0].Viewpoints.Num());
            ViewpointControl->SaveInitialState();
        }
    }
    
    // Create the photo capture widget (hidden - now integrated into MediaControl)
    PhotoCapture = CreateWidget<UPhotoCaptureWidget>(PC, UPhotoCaptureWidget::StaticClass());
    if (PhotoCapture)
    {
        // Don't add to viewport - now handled by MediaControl
        PhotoCapture->InitWidget();
        
        // Set reference to viewpoint control for batch capture
        if (ViewpointControl)
        {
            PhotoCapture->SetViewpointControlRef(ViewpointControl);
        }
        
        // Bind batch capture event
        PhotoCapture->OnBatchCaptureStart.AddDynamic(this, &AViewerHUD::OnBatchCaptureStart);
        
        // Bind reset viewpoints event
        PhotoCapture->OnResetViewpoints.AddDynamic(this, &AViewerHUD::OnResetViewpoints);
        
        // Set reference to pawn for shortcuts
        AViewerPawn* ViewerPawn = Cast<AViewerPawn>(PC->GetPawn());
        if (ViewerPawn)
        {
            ViewerPawn->PhotoCaptureRef = PhotoCapture;
        }
    }
    
    // Create parameter display widget (top center)
    ParameterDisplay = CreateWidget<UParameterDisplayWidget>(PC, UParameterDisplayWidget::StaticClass());
    if (ParameterDisplay)
    {
        ParameterDisplay->AddToViewport(10);
        float PosX = ViewportSize.X * 0.35f;
        float PosY = ViewportSize.Y * 0.02f;
        ParameterDisplay->SetPositionInViewport(FVector2D(PosX, PosY));
        ParameterDisplay->SetPhotoCaptureRef(PhotoCapture);
    }
    
    // Create media control widget (bottom-left)
    MediaControl = CreateWidget<UMediaControlWidget>(PC, UMediaControlWidget::StaticClass());
    if (MediaControl)
    {
        MediaControl->AddToViewport(10);
        float PosX = ViewportSize.X * 0.02f;
        float PosY = ViewportSize.Y * 0.70f;
        MediaControl->SetPositionInViewport(FVector2D(PosX, PosY));
        MediaControl->InitWidget();
        
        // Bind events
        MediaControl->OnModeChanged.AddDynamic(this, &AViewerHUD::OnMediaModeChanged);
        MediaControl->OnPhotoShutter.AddDynamic(this, &AViewerHUD::OnMediaPhotoShutter);
        MediaControl->OnVideoClipPlay.AddDynamic(this, &AViewerHUD::OnVideoClipPlay);
        MediaControl->OnVideoPlayAll.AddDynamic(this, &AViewerHUD::OnVideoPlayAll);
        MediaControl->OnVideoExport.AddDynamic(this, &AViewerHUD::OnVideoExport);
        MediaControl->OnSetStartFrame.AddDynamic(this, &AViewerHUD::OnSetStartFrame);
        MediaControl->OnSetEndFrame.AddDynamic(this, &AViewerHUD::OnSetEndFrame);
        MediaControl->OnTimelineScrub.AddDynamic(this, &AViewerHUD::OnTimelineScrub);
        MediaControl->OnOpenFolder.AddDynamic(this, &AViewerHUD::OnOpenFolder);
        MediaControl->OnResetCamera.AddDynamic(this, &AViewerHUD::OnResetCamera);
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
            
            // Update category name for screenshot naming
            if (PhotoCapture)
            {
                FString CategoryName = CameraController->Categories[Index].CategoryName;
                PhotoCapture->SetCurrentCategoryName(CategoryName);
            }
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
    if (!ViewpointControl) return;
    
    // Check if this viewpoint has saved data
    if (ViewpointControl->HasViewpointData(Index))
    {
        // Load saved viewpoint data
        FViewpointData Data = ViewpointControl->GetViewpointData(Index);
        
        // Apply camera position and settings
        APlayerController* PC = GetOwningPlayerController();
        if (PC && PC->GetPawn())
        {
            PC->GetPawn()->SetActorLocation(Data.Location);
            PC->SetControlRotation(Data.Rotation);
        }
        
        if (PhotoCapture)
        {
            PhotoCapture->LoadCameraSettings(Data.FocalLength, Data.Aperture, Data.FocusDistance);
        }
        
        UE_LOG(LogTemp, Log, TEXT("Loaded viewpoint %d data"), Index);
    }
    else if (CameraController)
    {
        // Use default viewpoint from controller
        CameraController->SetViewpoint(Index);
        
        if (PhotoCapture)
        {
            FCameraSettings Settings = CameraController->GetCurrentCameraSettings();
            PhotoCapture->LoadCameraSettings(Settings.FocalLength, Settings.Aperture, Settings.FocusDistance);
        }
    }
}

void AViewerHUD::OnViewpointSaved(int32 Index)
{
    if (!ViewpointControl || !PhotoCapture) return;
    
    APlayerController* PC = GetOwningPlayerController();
    if (!PC || !PC->GetPawn()) return;
    
    // Capture current camera state
    FViewpointData Data;
    Data.Location = PC->GetPawn()->GetActorLocation();
    Data.Rotation = PC->GetControlRotation();
    Data.FocalLength = PhotoCapture->GetFocalLength();
    Data.Aperture = PhotoCapture->GetAperture();
    Data.FocusDistance = PhotoCapture->GetFocusDistance();
    Data.bHasData = true;
    
    // Save to viewpoint control
    ViewpointControl->SaveViewpointData(Index, Data);
    
    UE_LOG(LogTemp, Log, TEXT("Saved viewpoint %d: Loc=%s"), Index, *Data.Location.ToString());
}

void AViewerHUD::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    HandleGlobalInput();
    UpdateVideoPlayback(DeltaTime);
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

void AViewerHUD::OnBatchCaptureStart(const TArray<int32>& Indices)
{
    if (Indices.Num() == 0) return;
    
    // Store indices and start batch capture
    BatchCaptureIndices = Indices;
    BatchCaptureCurrentIndex = 0;
    
    // Start the sequence
    PerformBatchCapture(BatchCaptureIndices, 0);
}

void AViewerHUD::OnResetViewpoints()
{
    if (ViewpointControl)
    {
        ViewpointControl->ResetToInitialState();
        
        // Also reset camera position to initial
        APlayerController* PC = GetOwningPlayerController();
        if (PC)
        {
            AViewerPawn* ViewerPawn = Cast<AViewerPawn>(PC->GetPawn());
            if (ViewerPawn && ViewerPawn->bInitialStateSaved)
            {
                ViewerPawn->SetActorLocation(ViewerPawn->InitialLocation);
                PC->SetControlRotation(ViewerPawn->InitialRotation);
            }
        }
        
        UE_LOG(LogTemp, Log, TEXT("Reset viewpoints to initial state"));
    }
}

void AViewerHUD::PerformBatchCapture(const TArray<int32>& Indices, int32 CurrentIndex)
{
    if (CurrentIndex >= Indices.Num())
    {
        // Batch capture complete
        if (PhotoCapture)
        {
            // Update status to show completion
            UE_LOG(LogTemp, Log, TEXT("Batch capture complete: %d images"), Indices.Num());
        }
        return;
    }
    
    int32 ViewpointIndex = Indices[CurrentIndex];
    
    // Switch to this viewpoint
    if (ViewpointControl)
    {
        ViewpointControl->SetCurrentViewpoint(ViewpointIndex);
        OnViewpointChanged(ViewpointIndex);
    }
    
    // Delay capture to allow camera to settle, then take screenshot
    FTimerDelegate CaptureDelegate;
    CaptureDelegate.BindLambda([this, Indices, CurrentIndex]()
    {
        // Take the screenshot
        if (PhotoCapture)
        {
            PhotoCapture->CaptureSingle();
            UE_LOG(LogTemp, Log, TEXT("Captured viewpoint %d (%d/%d)"), Indices[CurrentIndex], CurrentIndex + 1, Indices.Num());
        }
        
        // Schedule next capture after a delay
        FTimerDelegate NextDelegate;
        NextDelegate.BindLambda([this, Indices, CurrentIndex]()
        {
            PerformBatchCapture(Indices, CurrentIndex + 1);
        });
        
        FTimerHandle NextTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(NextTimerHandle, NextDelegate, 0.5f, false);
    });
    
    // Wait for camera to move before capturing
    GetWorld()->GetTimerManager().SetTimer(BatchCaptureTimerHandle, CaptureDelegate, 0.3f, false);
}

void AViewerHUD::OnMediaModeChanged(EMediaMode NewMode)
{
    UE_LOG(LogTemp, Log, TEXT("Media mode changed to: %d"), (int32)NewMode);
}

void AViewerHUD::OnMediaPhotoShutter()
{
    if (PhotoCapture)
    {
        PhotoCapture->CaptureSingle();
    }
}

void AViewerHUD::OnVideoClipPlay(int32 ClipIndex)
{
    ClipsToPlay.Empty();
    PlayVideoClip(ClipIndex);
}

void AViewerHUD::OnVideoPlayAll()
{
    if (!MediaControl) return;
    
    ClipsToPlay.Empty();
    for (int32 i = 0; i < MediaControl->GetClipCount(); i++)
    {
        FVideoClipData Clip = MediaControl->GetClipData(i);
        if (Clip.bStartSet && Clip.bEndSet)
        {
            ClipsToPlay.Add(i);
        }
    }
    
    if (ClipsToPlay.Num() > 0)
    {
        CurrentClipInSequence = 0;
        PlayVideoClip(ClipsToPlay[0]);
    }
}

void AViewerHUD::OnSetStartFrame()
{
    if (!MediaControl) return;
    
    APlayerController* PC = GetOwningPlayerController();
    if (!PC || !PC->GetPawn()) return;
    
    FVideoFrameData Frame;
    Frame.Location = PC->GetPawn()->GetActorLocation();
    Frame.Rotation = PC->GetControlRotation();
    
    if (PhotoCapture)
    {
        Frame.FocalLength = PhotoCapture->GetFocalLength();
        Frame.Aperture = PhotoCapture->GetAperture();
        Frame.FocusDistance = PhotoCapture->GetFocusDistance();
    }
    Frame.bHasData = true;
    
    MediaControl->SetStartFrame(Frame);
}

void AViewerHUD::OnSetEndFrame()
{
    if (!MediaControl) return;
    
    APlayerController* PC = GetOwningPlayerController();
    if (!PC || !PC->GetPawn()) return;
    
    FVideoFrameData Frame;
    Frame.Location = PC->GetPawn()->GetActorLocation();
    Frame.Rotation = PC->GetControlRotation();
    
    if (PhotoCapture)
    {
        Frame.FocalLength = PhotoCapture->GetFocalLength();
        Frame.Aperture = PhotoCapture->GetAperture();
        Frame.FocusDistance = PhotoCapture->GetFocusDistance();
    }
    Frame.bHasData = true;
    
    MediaControl->SetEndFrame(Frame);
}

void AViewerHUD::OnTimelineScrub(float TimePosition)
{
    if (!MediaControl) return;
    
    // Find which clip this time position falls into
    float AccumulatedTime = 0.0f;
    for (int32 i = 0; i < MediaControl->GetClipCount(); i++)
    {
        FVideoClipData Clip = MediaControl->GetClipData(i);
        if (TimePosition < AccumulatedTime + Clip.Duration)
        {
            // Found the clip
            float LocalTime = TimePosition - AccumulatedTime;
            float Alpha = LocalTime / Clip.Duration;
            
            if (Clip.bStartSet && Clip.bEndSet)
            {
                // Interpolate to preview position
                FVector PreviewLoc = FMath::Lerp(Clip.StartFrame.Location, Clip.EndFrame.Location, Alpha);
                FRotator PreviewRot = FMath::Lerp(Clip.StartFrame.Rotation, Clip.EndFrame.Rotation, Alpha);
                
                APlayerController* PC = GetOwningPlayerController();
                if (PC && PC->GetPawn())
                {
                    PC->GetPawn()->SetActorLocation(PreviewLoc);
                    PC->SetControlRotation(PreviewRot);
                }
            }
            break;
        }
        AccumulatedTime += Clip.Duration;
    }
}

void AViewerHUD::OnVideoExport()
{
    ExportVideoSequence();
}

void AViewerHUD::OnOpenFolder()
{
    // Open screenshots folder
    FString ScreenshotsPath = FPaths::ProjectSavedDir() / TEXT("Screenshots");
    IFileManager::Get().MakeDirectory(*ScreenshotsPath, true);
    
    FPlatformProcess::ExploreFolder(*ScreenshotsPath);
    UE_LOG(LogTemp, Log, TEXT("Opening folder: %s"), *ScreenshotsPath);
}

void AViewerHUD::OnResetCamera()
{
    // Reset camera to initial state
    APlayerController* PC = GetOwningPlayerController();
    if (PC)
    {
        AViewerPawn* ViewerPawn = Cast<AViewerPawn>(PC->GetPawn());
        if (ViewerPawn)
        {
            ViewerPawn->SetActorLocation(ViewerPawn->InitialLocation);
            PC->SetControlRotation(ViewerPawn->InitialRotation);
            ViewerPawn->CurrentFocalLength = ViewerPawn->InitialFocalLength;
            ViewerPawn->CurrentAperture = ViewerPawn->InitialAperture;
            ViewerPawn->CurrentFocusDistance = ViewerPawn->InitialFocusDistance;
            ViewerPawn->ApplyCameraSettings();
            
            UE_LOG(LogTemp, Log, TEXT("Camera reset to initial state"));
        }
    }
}

void AViewerHUD::ExportVideoSequence()
{
    if (!MediaControl) return;
    
    // Get all valid clips
    TArray<int32> ValidClips;
    for (int32 i = 0; i < MediaControl->GetClipCount(); i++)
    {
        FVideoClipData Clip = MediaControl->GetClipData(i);
        if (Clip.bStartSet && Clip.bEndSet)
        {
            ValidClips.Add(i);
        }
    }
    
    if (ValidClips.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid clips to export"));
        return;
    }
    
    // Calculate total duration
    float TotalDuration = 0.0f;
    for (int32 ClipIdx : ValidClips)
    {
        TotalDuration += MediaControl->GetClipData(ClipIdx).Duration;
    }
    
    // Create export directory
    ExportPath = FPaths::ProjectSavedDir() / TEXT("VideoExport");
    IFileManager::Get().MakeDirectory(*ExportPath, true);
    
    // Start export mode
    bIsExporting = true;
    ExportFrameIndex = 0;
    
    // Use console command to start movie capture
    APlayerController* PC = GetOwningPlayerController();
    if (PC)
    {
        // Start screenshot sequence
        FString StartCommand = FString::Printf(TEXT("HighResShot 1920x1080"));
        
        UE_LOG(LogTemp, Log, TEXT("========================================"));
        UE_LOG(LogTemp, Log, TEXT("VIDEO EXPORT STARTED"));
        UE_LOG(LogTemp, Log, TEXT("Clips: %d, Duration: %.1f seconds"), ValidClips.Num(), TotalDuration);
        UE_LOG(LogTemp, Log, TEXT("Export Path: %s"), *ExportPath);
        UE_LOG(LogTemp, Log, TEXT("========================================"));
        
        // Play clips for recording
        ClipsToPlay = ValidClips;
        CurrentClipInSequence = 0;
        PlayVideoClip(ClipsToPlay[0]);
    }
}

void AViewerHUD::PlayVideoClip(int32 ClipIndex)
{
    if (!MediaControl) return;
    
    FVideoClipData ClipData = MediaControl->GetClipData(ClipIndex);
    if (!ClipData.bStartSet || !ClipData.bEndSet)
    {
        UE_LOG(LogTemp, Warning, TEXT("Clip %d is not fully set"), ClipIndex);
        return;
    }
    
    // Start playback
    bIsPlayingVideo = true;
    CurrentPlayingClipIndex = ClipIndex;
    VideoPlaybackTime = 0.0f;
    VideoPlaybackDuration = ClipData.Duration;
    VideoStartFrame = ClipData.StartFrame;
    VideoEndFrame = ClipData.EndFrame;
    
    // Move to start position
    APlayerController* PC = GetOwningPlayerController();
    if (PC && PC->GetPawn())
    {
        PC->GetPawn()->SetActorLocation(VideoStartFrame.Location);
        PC->SetControlRotation(VideoStartFrame.Rotation);
        
        if (PhotoCapture)
        {
            PhotoCapture->LoadCameraSettings(
                VideoStartFrame.FocalLength,
                VideoStartFrame.Aperture,
                VideoStartFrame.FocusDistance
            );
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Playing clip %d for %.1f seconds"), ClipIndex, VideoPlaybackDuration);
}

void AViewerHUD::UpdateVideoPlayback(float DeltaTime)
{
    if (!bIsPlayingVideo) return;
    
    VideoPlaybackTime += DeltaTime;
    float Alpha = FMath::Clamp(VideoPlaybackTime / VideoPlaybackDuration, 0.0f, 1.0f);
    
    // Smooth interpolation using ease in/out
    float SmoothAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, 2.0f);
    
    // Interpolate position
    FVector NewLocation = FMath::Lerp(VideoStartFrame.Location, VideoEndFrame.Location, SmoothAlpha);
    
    // Interpolate rotation
    FRotator NewRotation = FMath::Lerp(VideoStartFrame.Rotation, VideoEndFrame.Rotation, SmoothAlpha);
    
    // Interpolate camera parameters
    float NewFocal = FMath::Lerp(VideoStartFrame.FocalLength, VideoEndFrame.FocalLength, SmoothAlpha);
    float NewAperture = FMath::Lerp(VideoStartFrame.Aperture, VideoEndFrame.Aperture, SmoothAlpha);
    float NewFocus = FMath::Lerp(VideoStartFrame.FocusDistance, VideoEndFrame.FocusDistance, SmoothAlpha);
    
    // Apply
    APlayerController* PC = GetOwningPlayerController();
    if (PC && PC->GetPawn())
    {
        PC->GetPawn()->SetActorLocation(NewLocation);
        PC->SetControlRotation(NewRotation);
        
        if (PhotoCapture)
        {
            PhotoCapture->LoadCameraSettings(NewFocal, NewAperture, NewFocus);
        }
    }
    
    // Check if clip finished
    if (Alpha >= 1.0f)
    {
        bIsPlayingVideo = false;
        UE_LOG(LogTemp, Log, TEXT("Clip %d playback finished"), CurrentPlayingClipIndex);
        
        // Play next clip in sequence if available
        if (ClipsToPlay.Num() > 0)
        {
            CurrentClipInSequence++;
            if (CurrentClipInSequence < ClipsToPlay.Num())
            {
                // Small delay before next clip
                FTimerDelegate NextClipDelegate;
                NextClipDelegate.BindLambda([this]()
                {
                    PlayVideoClip(ClipsToPlay[CurrentClipInSequence]);
                });
                FTimerHandle NextClipTimerHandle;
                GetWorld()->GetTimerManager().SetTimer(NextClipTimerHandle, NextClipDelegate, 0.2f, false);
            }
            else
            {
                ClipsToPlay.Empty();
                UE_LOG(LogTemp, Log, TEXT("All clips playback finished"));
            }
        }
    }
}
