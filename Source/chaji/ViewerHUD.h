// ViewerHUD.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CategoryTabWidget.h"
#include "ViewpointControlWidget.h"
#include "PhotoCaptureWidget.h"
#include "CameraViewController.h"
#include "ViewpointPreviewManager.h"
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
    ACameraViewController* CameraController;
    
    UPROPERTY()
    AViewpointPreviewManager* PreviewManager;

    void SetupUI();
    void HandleGlobalInput();
    void SetupPreviewsForCategory(int32 CategoryIndex);
    
    // Shortcut key hold tracking
    float KeyHoldTime = 0.0f;
    float KeyHoldAcceleration = 1.0f;
    bool bWasHoldingKey = false;

    UFUNCTION()
    void OnCategorySelected(int32 Index);
    
    UFUNCTION()
    void OnViewpointChanged(int32 Index);
};
