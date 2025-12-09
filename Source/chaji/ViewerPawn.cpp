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
    
    // Get movement component and set speed
    if (UFloatingPawnMovement* Movement = Cast<UFloatingPawnMovement>(GetMovementComponent()))
    {
        Movement->MaxSpeed = 1200.0f;
        Movement->Acceleration = 4000.0f;
        Movement->Deceleration = 8000.0f;
    }
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
        PhotoCaptureRef->AdjustFocusDistance(100.0f);
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
        PhotoCaptureRef->AdjustFocusDistance(-100.0f);
    }
}
