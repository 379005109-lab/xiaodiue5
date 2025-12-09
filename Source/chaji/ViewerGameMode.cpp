// ViewerGameMode.cpp
#include "ViewerGameMode.h"
#include "ViewerHUD.h"

AViewerGameMode::AViewerGameMode()
{
    HUDClass = AViewerHUD::StaticClass();
    DefaultPawnClass = AViewerPawn::StaticClass();
}
