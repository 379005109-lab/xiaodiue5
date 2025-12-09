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
    void InitWidget();
    
    // Load settings from camera
    UFUNCTION(BlueprintCallable)
    void LoadCameraSettings(float InFocalLength, float InAperture, float InFocusDistance);
    
    // Called from PlayerController for scroll wheel shortcuts
    void AdjustFocalLength(float Delta);
    void AdjustAperture(float Delta);
    void AdjustFocusDistance(float Delta);
    
    // Get current values for display
    float GetFocalLength() const { return FocalLength; }
    float GetAperture() const { return Aperture; }
    float GetFocusDistance() const { return FocusDistance; }

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    TSharedPtr<class SBox> ContentBox;
    TSharedPtr<class STextBlock> FocalLengthText;
    TSharedPtr<class STextBlock> ApertureText;
    TSharedPtr<class STextBlock> FocusDistanceText;
    TSharedPtr<class STextBlock> StatusText;
    TSharedPtr<class STextBlock> ResolutionText;
    
    bool bIsExpanded = false;
    
    // Camera parameters
    float FocalLength = 35.0f;
    float Aperture = 2.8f;
    float FocusDistance = 1000.0f;
    
    // Default values for reset
    float DefaultFocalLength = 35.0f;
    float DefaultAperture = 2.8f;
    float DefaultFocusDistance = 1000.0f;
    
    // Screenshot resolution: 0=1K, 1=2K, 2=4K
    int32 ResolutionIndex = 0;
    
    void UpdateParameterDisplay();
    void ApplyAllCameraSettings();
    void UpdateResolutionText();
    
    FReply OnToggleClicked();
    FReply OnCaptureClicked();
    FReply OnResetClicked();
    FReply OnResolution1K();
    FReply OnResolution2K();
    FReply OnResolution4K();
    
    void OnFocalLengthChanged(float Value);
    void OnApertureChanged(float Value);
    void OnFocusDistanceChanged(float Value);
};
