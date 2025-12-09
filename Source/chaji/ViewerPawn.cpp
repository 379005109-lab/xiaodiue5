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
    check(PlayerInputComponent);
    
    // DON'T call Super - we manually bind only what we need
    // This prevents default Ctrl binding for descent
    
    // Movement
    PlayerInputComponent->BindAxis("MoveForward", this, &ADefaultPawn::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ADefaultPawn::MoveRight);
    
    // Look
    PlayerInputComponent->BindAxis("Turn", this, &ADefaultPawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("TurnRate", this, &ADefaultPawn::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUp", this, &ADefaultPawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("LookUpRate", this, &ADefaultPawn::LookUpAtRate);
    
    // Up/Down only with Q/E (Space for up, no Ctrl for down)
    PlayerInputComponent->BindAxis("MoveUp", this, &ADefaultPawn::MoveUp_World);
}
