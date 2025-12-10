// ViewerPawn.cpp
#include "ViewerPawn.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "PhotoCaptureWidget.h"
#include "GameFramework/PlayerController.h"

AViewerPawn::AViewerPawn()
{
    // Enable tick for custom movement
    PrimaryActorTick.bCanEverTick = true;
    
    // Disable default movement bindings - we do our own
    bAddDefaultMovementBindings = false;
    
    // Create camera component with DOF support
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewerCamera"));
    CameraComp->SetupAttachment(RootComponent);
    CameraComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    CameraComp->bUsePawnControlRotation = true;
    
    // Enable post process for DOF
    CameraComp->PostProcessBlendWeight = 1.0f;
    
    // Default DOF settings
    CameraComp->PostProcessSettings.bOverride_DepthOfFieldFstop = true;
    CameraComp->PostProcessSettings.DepthOfFieldFstop = 2.8f;
    CameraComp->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
    CameraComp->PostProcessSettings.DepthOfFieldFocalDistance = 1000.0f;
    CameraComp->PostProcessSettings.bOverride_DepthOfFieldSensorWidth = true;
    CameraComp->PostProcessSettings.DepthOfFieldSensorWidth = 36.0f;
    CameraComp->PostProcessSettings.bOverride_DepthOfFieldMinFstop = true;
    CameraComp->PostProcessSettings.DepthOfFieldMinFstop = 1.2f;
    
    // ============ 创建 SceneCapture2D 用于将3D场景渲染到纹理 ============
    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
    SceneCapture->SetupAttachment(CameraComp);
    SceneCapture->SetRelativeLocation(FVector::ZeroVector);
    SceneCapture->SetRelativeRotation(FRotator::ZeroRotator);
    
    // 场景捕获设置
    SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalToneCurveHDR;
    SceneCapture->bCaptureEveryFrame = true;
    SceneCapture->bCaptureOnMovement = false;
    SceneCapture->bAlwaysPersistRenderingState = true;
    
    // 复制相机的后处理设置到场景捕获
    SceneCapture->PostProcessSettings = CameraComp->PostProcessSettings;
    SceneCapture->PostProcessBlendWeight = 1.0f;
    
    // 创建渲染目标纹理 (在BeginPlay中初始化大小)
    // RenderTarget 将在 BeginPlay 中创建
    
    // Get movement component and set speed
    if (UFloatingPawnMovement* Movement = Cast<UFloatingPawnMovement>(GetMovementComponent()))
    {
        Movement->MaxSpeed = 1200.0f;
        Movement->Acceleration = 4000.0f;
        Movement->Deceleration = 8000.0f;
    }
}

void AViewerPawn::BeginPlay()
{
    Super::BeginPlay();
    
    // 计算视口中间区域的大小
    // 左侧面板: 180px, 右侧面板: 300px, 底部面板: 150px
    // 假设屏幕分辨率 1920x1080
    FVector2D ViewportSize(1920.0f, 1080.0f);
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
    }
    
    const float LeftPanelWidth = 180.0f;
    const float RightPanelWidth = 300.0f;
    const float BottomPanelHeight = 150.0f;
    
    // 计算3D视图区域大小
    int32 ViewWidth = FMath::RoundToInt(ViewportSize.X - LeftPanelWidth - RightPanelWidth);
    int32 ViewHeight = FMath::RoundToInt(ViewportSize.Y - BottomPanelHeight);
    
    // 创建渲染目标纹理
    RenderTarget = NewObject<UTextureRenderTarget2D>(this);
    RenderTarget->InitAutoFormat(ViewWidth, ViewHeight);
    RenderTarget->UpdateResourceImmediate(true);
    
    // 设置场景捕获的渲染目标
    if (SceneCapture)
    {
        SceneCapture->TextureTarget = RenderTarget;
        
        // 设置场景捕获的FOV与主相机一致
        if (CameraComp)
        {
            SceneCapture->FOVAngle = CameraComp->FieldOfView;
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("RenderTarget created: %dx%d"), ViewWidth, ViewHeight);
}

void AViewerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);
    
    // Bind movement directly to keys
    PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &AViewerPawn::StartMoveForward);
    PlayerInputComponent->BindKey(EKeys::W, IE_Released, this, &AViewerPawn::StopMoveForward);
    PlayerInputComponent->BindKey(EKeys::S, IE_Pressed, this, &AViewerPawn::StartMoveBackward);
    PlayerInputComponent->BindKey(EKeys::S, IE_Released, this, &AViewerPawn::StopMoveBackward);
    PlayerInputComponent->BindKey(EKeys::A, IE_Pressed, this, &AViewerPawn::StartMoveLeft);
    PlayerInputComponent->BindKey(EKeys::A, IE_Released, this, &AViewerPawn::StopMoveLeft);
    PlayerInputComponent->BindKey(EKeys::D, IE_Pressed, this, &AViewerPawn::StartMoveRight);
    PlayerInputComponent->BindKey(EKeys::D, IE_Released, this, &AViewerPawn::StopMoveRight);
    PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &AViewerPawn::StartMoveUp);
    PlayerInputComponent->BindKey(EKeys::E, IE_Released, this, &AViewerPawn::StopMoveUp);
    PlayerInputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AViewerPawn::StartMoveDown);
    PlayerInputComponent->BindKey(EKeys::Q, IE_Released, this, &AViewerPawn::StopMoveDown);
    PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AViewerPawn::StartMoveUp);
    PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &AViewerPawn::StopMoveUp);
    
    // Right mouse for look
    PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AViewerPawn::OnRightMousePressed);
    PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &AViewerPawn::OnRightMouseReleased);
    
    // Mouse scroll for camera shortcuts
    PlayerInputComponent->BindKey(EKeys::MouseScrollUp, IE_Pressed, this, &AViewerPawn::OnMouseScrollUp);
    PlayerInputComponent->BindKey(EKeys::MouseScrollDown, IE_Pressed, this, &AViewerPawn::OnMouseScrollDown);
    
    // R key for reset (instead of Esc which exits simulation)
    PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &AViewerPawn::OnResetShortcut);
    
    // Save initial state for reset
    SaveInitialState();
}

void AViewerPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;
    
    // Mouse look only when right button held
    if (bRightMouseDown)
    {
        float MouseX, MouseY;
        PC->GetInputMouseDelta(MouseX, MouseY);
        AddControllerYawInput(MouseX * 0.5f);
        AddControllerPitchInput(MouseY * -0.5f);
    }
    
    // Apply movement input
    FVector MoveDirection = FVector(MoveInput.X, MoveInput.Y, MoveInput.Z);
    if (!MoveDirection.IsNearlyZero())
    {
        // Transform to world space based on controller rotation
        FRotator ControlRot = GetControlRotation();
        ControlRot.Pitch = 0.0f;
        ControlRot.Roll = 0.0f;
        
        FVector Forward = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::X);
        FVector Right = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::Y);
        FVector Up = FVector::UpVector;
        
        FVector WorldMove = Forward * MoveInput.X + Right * MoveInput.Y + Up * MoveInput.Z;
        AddMovementInput(WorldMove, 1.0f);
    }
}

void AViewerPawn::StartMoveForward() { MoveInput.X = 1.0f; }
void AViewerPawn::StopMoveForward() { if (MoveInput.X > 0) MoveInput.X = 0.0f; }
void AViewerPawn::StartMoveBackward() { MoveInput.X = -1.0f; }
void AViewerPawn::StopMoveBackward() { if (MoveInput.X < 0) MoveInput.X = 0.0f; }
void AViewerPawn::StartMoveLeft() { MoveInput.Y = -1.0f; }
void AViewerPawn::StopMoveLeft() { if (MoveInput.Y < 0) MoveInput.Y = 0.0f; }
void AViewerPawn::StartMoveRight() { MoveInput.Y = 1.0f; }
void AViewerPawn::StopMoveRight() { if (MoveInput.Y > 0) MoveInput.Y = 0.0f; }
void AViewerPawn::StartMoveUp() { MoveInput.Z = 1.0f; }
void AViewerPawn::StopMoveUp() { if (MoveInput.Z > 0) MoveInput.Z = 0.0f; }
void AViewerPawn::StartMoveDown() { MoveInput.Z = -1.0f; }
void AViewerPawn::StopMoveDown() { if (MoveInput.Z < 0) MoveInput.Z = 0.0f; }

void AViewerPawn::OnRightMousePressed()
{
    bRightMouseDown = true;
}

void AViewerPawn::OnRightMouseReleased()
{
    bRightMouseDown = false;
}

void AViewerPawn::OnMouseScrollUp()
{
    if (!PhotoCaptureRef) return;
    
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;
    
    bool bCtrl = PC->IsInputKeyDown(EKeys::LeftControl) || PC->IsInputKeyDown(EKeys::RightControl);
    bool bShift = PC->IsInputKeyDown(EKeys::LeftShift) || PC->IsInputKeyDown(EKeys::RightShift);
    bool bAlt = PC->IsInputKeyDown(EKeys::LeftAlt) || PC->IsInputKeyDown(EKeys::RightAlt);
    
    if (bCtrl)
    {
        PhotoCaptureRef->AdjustFocalLength(5.0f);
    }
    else if (bShift)
    {
        PhotoCaptureRef->AdjustAperture(0.5f);
    }
    else if (bAlt)
    {
        PhotoCaptureRef->AdjustFocusDistance(20.0f); // More precise adjustment
    }
}

void AViewerPawn::OnMouseScrollDown()
{
    if (!PhotoCaptureRef) return;
    
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;
    
    bool bCtrl = PC->IsInputKeyDown(EKeys::LeftControl) || PC->IsInputKeyDown(EKeys::RightControl);
    bool bShift = PC->IsInputKeyDown(EKeys::LeftShift) || PC->IsInputKeyDown(EKeys::RightShift);
    bool bAlt = PC->IsInputKeyDown(EKeys::LeftAlt) || PC->IsInputKeyDown(EKeys::RightAlt);
    
    if (bCtrl)
    {
        PhotoCaptureRef->AdjustFocalLength(-5.0f);
    }
    else if (bShift)
    {
        PhotoCaptureRef->AdjustAperture(-0.5f);
    }
    else if (bAlt)
    {
        PhotoCaptureRef->AdjustFocusDistance(-20.0f); // More precise adjustment
    }
}

void AViewerPawn::OnResetShortcut()
{
    // Reset to initial state (position + camera settings)
    APlayerController* PC = Cast<APlayerController>(GetController());
    
    if (bInitialStateSaved)
    {
        // Restore initial position
        SetActorLocation(InitialLocation);
        if (PC)
        {
            PC->SetControlRotation(InitialRotation);
        }
    }
    
    // Reset camera settings
    if (PhotoCaptureRef)
    {
        PhotoCaptureRef->LoadCameraSettings(35.0f, 2.8f, 1000.0f);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Reset to initial state"));
}

void AViewerPawn::SaveInitialState()
{
    InitialLocation = GetActorLocation();
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        InitialRotation = PC->GetControlRotation();
    }
    bInitialStateSaved = true;
    UE_LOG(LogTemp, Log, TEXT("Saved initial state: %s"), *InitialLocation.ToString());
}
