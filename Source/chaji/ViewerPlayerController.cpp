// ViewerPlayerController.cpp
#include "ViewerPlayerController.h"
#include "Engine/LocalPlayer.h"

AViewerPlayerController::AViewerPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AViewerPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    // 设置视口区域：左侧180px，右侧300px，底部150px 留给UI面板
    // 假设1920x1080分辨率，计算比例
    // 左边距: 180/1920 ≈ 0.094
    // 右边距: 300/1920 ≈ 0.156
    // 底边距: 150/1080 ≈ 0.139
    // 视口区域: Origin(0.094, 0), Size(1 - 0.094 - 0.156, 1 - 0.139)
    
    SetViewportRegion(0.094f, 0.0f, 0.75f, 0.861f);
}

void AViewerPlayerController::SetViewportRegion(float OriginX, float OriginY, float SizeX, float SizeY)
{
    ULocalPlayer* LP = GetLocalPlayer();
    if (LP)
    {
        // 设置视口原点和大小 (0-1范围，相对于整个屏幕)
        LP->Origin = FVector2D(OriginX, OriginY);
        LP->Size = FVector2D(SizeX, SizeY);
    }
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