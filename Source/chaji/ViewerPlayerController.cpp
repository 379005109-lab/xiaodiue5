// ViewerPlayerController.cpp
#include "ViewerPlayerController.h"

AViewerPlayerController::AViewerPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AViewerPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    // Bind mouse wheel
    InputComponent->BindAction("ScrollUp", IE_Pressed, this, &AViewerPlayerController::OnScrollUp);
    InputComponent->BindAction("ScrollDown", IE_Pressed, this, &AViewerPlayerController::OnScrollDown);
}

void AViewerPlayerController::OnScrollUp()
{
    LastScrollDelta = 1.0f;
    OnMouseWheelScroll.Broadcast(1.0f);
}

void AViewerPlayerController::OnScrollDown()
{
    LastScrollDelta = -1.0f;
    OnMouseWheelScroll.Broadcast(-1.0f);
}