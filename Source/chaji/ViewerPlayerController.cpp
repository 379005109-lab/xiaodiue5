// ViewerPlayerController.cpp
#include "ViewerPlayerController.h"
#include "Engine/LocalPlayer.h"
#include "TimerManager.h"
#include "Engine/GameViewportClient.h"

AViewerPlayerController::AViewerPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AViewerPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    // 多次尝试设置视口区域，确保生效
    for (float Delay : {0.1f, 0.5f, 1.0f, 2.0f})
    {
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
            ApplyViewportSeparation();
        }, Delay, false);
    }
}

void AViewerPlayerController::ApplyViewportSeparation()
{
    // 计算面板占用比例
    FVector2D ViewportSize;
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
    }
    else
    {
        ViewportSize = FVector2D(1920.0f, 1080.0f);
    }
    
    // 面板尺寸
    const float LeftPanelWidth = 180.0f;
    const float RightPanelWidth = 300.0f;
    const float BottomPanelHeight = 150.0f;
    
    // 计算视口区域 (0-1范围)
    float OriginX = LeftPanelWidth / ViewportSize.X;
    float OriginY = 0.0f;
    float SizeX = (ViewportSize.X - LeftPanelWidth - RightPanelWidth) / ViewportSize.X;
    float SizeY = (ViewportSize.Y - BottomPanelHeight) / ViewportSize.Y;
    
    ULocalPlayer* LP = GetLocalPlayer();
    if (LP)
    {
        LP->Origin = FVector2D(OriginX, OriginY);
        LP->Size = FVector2D(SizeX, SizeY);
        
        // 强制更新视口
        if (GEngine && GEngine->GameViewport)
        {
            GEngine->GameViewport->Invalidate();
        }
        
        UE_LOG(LogTemp, Warning, TEXT("Viewport Separation Applied: Origin(%.3f, %.3f) Size(%.3f, %.3f)"), 
            OriginX, OriginY, SizeX, SizeY);
    }
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