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
};
