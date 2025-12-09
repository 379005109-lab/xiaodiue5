// ViewerPawn.cpp
#include "ViewerPawn.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerInput.h"

AViewerPawn::AViewerPawn()
{
    // Disable default movement
    bAddDefaultMovementBindings = false;
}

void AViewerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Don't call Super - this removes all default bindings including Crouch
    // Only bind what we need
    
    check(PlayerInputComponent);
    
    // Bind basic movement (WASD)
    PlayerInputComponent->BindAxis("MoveForward", this, &ADefaultPawn::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ADefaultPawn::MoveRight);
    PlayerInputComponent->BindAxis("MoveUp", this, &ADefaultPawn::MoveUp_World);
    PlayerInputComponent->BindAxis("Turn", this, &ADefaultPawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &ADefaultPawn::AddControllerPitchInput);
    
    // No Crouch binding here - Ctrl will not cause descent
}
