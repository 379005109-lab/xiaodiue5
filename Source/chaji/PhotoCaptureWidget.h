// PhotoCaptureWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PhotoCaptureWidget.generated.h"

UCLASS()
class UPhotoCaptureWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetCameraActor(class ACineCameraActor* InCamera);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    TSharedPtr<class SVerticalBox> SettingsContainer;
    TSharedPtr<class SBox> ContentBox;
    TSharedPtr<class STextBlock> FocalLengthText;
    TSharedPtr<class STextBlock> ApertureText;
    TSharedPtr<class STextBlock> FocusDistanceText;
    
    UPROPERTY()
    class ACineCameraActor* CameraActor;
    
    bool bIsExpanded = false;
    
    // Camera parameters
    float FocalLength = 35.0f;
    float Aperture = 2.8f;
    float FocusDistance = 1000.0f;
    int32 ScreenshotScale = 2;
    
    void RebuildSettings();
    void UpdateParameterDisplay();
    void ApplyCameraSettings();
    
    FReply OnToggleClicked();
    FReply OnCaptureClicked();
    
    void OnFocalLengthChanged(float Value);
    void OnApertureChanged(float Value);
    void OnFocusDistanceChanged(float Value);
};
