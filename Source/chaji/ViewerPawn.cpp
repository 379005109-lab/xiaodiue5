// ViewerPawn.cpp
#include "ViewerPawn.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerInput.h"

AViewerPawn::AViewerPawn()
{
    // Keep default movement bindings but we'll override crouch
    bAddDefaultMovementBindings = true;
}

void AViewerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Call parent to get all default bindings
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Remove MoveUp binding to prevent Ctrl from descending
    // The default pawn binds Ctrl to MoveUp with negative value
    PlayerInputComponent->RemoveAxisBinding("MoveUp");
    
    // Re-bind MoveUp only to Space (up) - no down binding
    PlayerInputComponent->BindAxis("MoveUp", this, &ADefaultPawn::MoveUp_World);
}
