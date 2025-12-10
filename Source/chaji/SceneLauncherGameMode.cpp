// SceneLauncherGameMode.cpp
#include "SceneLauncherGameMode.h"
#include "SceneLauncherHUD.h"
#include "GameFramework/DefaultPawn.h"

ASceneLauncherGameMode::ASceneLauncherGameMode()
{
    // 使用默认Pawn（不需要复杂的相机控制）
    DefaultPawnClass = ADefaultPawn::StaticClass();
    
    // 使用场景启动器HUD
    HUDClass = ASceneLauncherHUD::StaticClass();
}
