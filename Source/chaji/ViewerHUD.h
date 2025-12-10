// ViewerHUD.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CategoryTabWidget.h"
#include "ViewpointControlWidget.h"
#include "PhotoCaptureWidget.h"
#include "CameraViewController.h"
#include "ViewpointPreviewManager.h"
#include "ViewerPlayerController.h"
#include "ViewerPawn.h"
#include "ParameterDisplayWidget.h"
#include "MediaControlWidget.h"
#include "VideoClipData.h"
#include "MainLayoutWidget.h"
#include "ViewerHUD.generated.h"

UCLASS()
class AViewerHUD : public AHUD
{
    GENERATED_BODY()

public:
    AViewerHUD();
    
    // Public access to PhotoCapture for global shortcuts
    UFUNCTION(BlueprintCallable)
    UPhotoCaptureWidget* GetPhotoCaptureWidget() const { return PhotoCapture; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY()
    UCategoryTabWidget* TabWidget;
    
    UPROPERTY()
    UViewpointControlWidget* ViewpointControl;
    
    UPROPERTY()
    UPhotoCaptureWidget* PhotoCapture;
    
    UPROPERTY()
    UParameterDisplayWidget* ParameterDisplay;
    
    UPROPERTY()
    UMediaControlWidget* MediaControl;
    
    UPROPERTY()
    UMainLayoutWidget* MainLayout;
    
    UPROPERTY()
    ACameraViewController* CameraController;
    
    UPROPERTY()
    AViewpointPreviewManager* PreviewManager;

    void SetupUI();
    void HandleGlobalInput();
    void SetupPreviewsForCategory(int32 CategoryIndex);

    UFUNCTION()
    void OnCategorySelected(int32 Index);
    
    UFUNCTION()
    void OnViewpointChanged(int32 Index);
    
    UFUNCTION()
    void OnViewpointSaved(int32 Index);
    
    UFUNCTION()
    void OnMouseWheel(float Delta);
    
    UFUNCTION()
    void OnBatchCaptureStart(const TArray<int32>& Indices);
    
    UFUNCTION()
    void OnResetViewpoints();
    
    void PerformBatchCapture(const TArray<int32>& Indices, int32 CurrentIndex);
    
    // Batch capture state
    TArray<int32> BatchCaptureIndices;
    int32 BatchCaptureCurrentIndex = 0;
    FTimerHandle BatchCaptureTimerHandle;
    
    // Media control
    UFUNCTION()
    void OnMediaModeChanged(EMediaMode NewMode);
    
    UFUNCTION()
    void OnMediaPhotoShutter();
    
    UFUNCTION()
    void OnVideoClipPlay(int32 ClipIndex);
    
    UFUNCTION()
    void OnVideoPlayAll();
    
    UFUNCTION()
    void OnVideoExport();
    
    UFUNCTION()
    void OnSetStartFrame();
    
    UFUNCTION()
    void OnSetEndFrame();
    
    UFUNCTION()
    void OnTimelineScrub(float TimePosition);
    
    UFUNCTION()
    void OnOpenFolder();
    
    UFUNCTION()
    void OnResetCamera();
    
    void PlayVideoClip(int32 ClipIndex);
    void UpdateVideoPlayback(float DeltaTime);
    void ExportVideoSequence();
    
    // Video animation state
    bool bIsPlayingVideo = false;
    bool bIsExporting = false;
    int32 CurrentPlayingClipIndex = 0;
    float VideoPlaybackTime = 0.0f;
    float VideoPlaybackDuration = 0.0f;
    FVideoFrameData VideoStartFrame;
    FVideoFrameData VideoEndFrame;
    TArray<int32> ClipsToPlay;
    int32 CurrentClipInSequence = 0;
    int32 ExportFrameIndex = 0;
    FString ExportPath;
};
