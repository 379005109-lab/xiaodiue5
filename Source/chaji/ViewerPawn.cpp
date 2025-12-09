// ViewerPawn.cpp
#include "ViewerPawn.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerInput.h"

AViewerPawn::AViewerPawn()
{
    // Keep default movement bindings but we'll override crouch
    bAddDefaultMovementBindings = true;
    
    // Create camera component with DOF support
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewerCamera"));
    CameraComp->SetupAttachment(RootComponent);
    CameraComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    CameraComp->bUsePawnControlRotation = true;
    
    // Enable post process for DOF
    CameraComp->PostProcessBlendWeight = 1.0f;
    
    // Default DOF settings - will be overridden by PhotoCaptureWidget
    CameraComp->PostProcessSettings.bOverride_DepthOfFieldFstop = true;
    CameraComp->PostProcessSettings.DepthOfFieldFstop = 2.8f;
    CameraComp->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
    CameraComp->PostProcessSettings.DepthOfFieldFocalDistance = 1000.0f;
    CameraComp->PostProcessSettings.bOverride_DepthOfFieldSensorWidth = true;
    CameraComp->PostProcessSettings.DepthOfFieldSensorWidth = 36.0f;
    CameraComp->PostProcessSettings.bOverride_DepthOfFieldMinFstop = true;
    CameraComp->PostProcessSettings.DepthOfFieldMinFstop = 1.2f;
}

void AViewerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Call parent to get all default bindings
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Note: We keep all default bindings including MoveUp
    // The Ctrl key descent is handled by MoveUp axis, but we don't remove it
    // to keep Q/E working for up/down movement
}
