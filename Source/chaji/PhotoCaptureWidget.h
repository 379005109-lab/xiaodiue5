// PhotoCaptureWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PhotoCaptureWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBatchCaptureStart, const TArray<int32>&, Indices);

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
    
    // Getters for camera parameters
    float GetFocalLength() const { return FocalLength; }
    float GetAperture() const { return Aperture; }
    float GetFocusDistance() const { return FocusDistance; }
    
    // Get selected viewpoints from ViewpointControl
    void SetViewpointControlRef(class UViewpointControlWidget* Ref) { ViewpointControlRef = Ref; }
    
    // Set current category name for screenshot naming
    void SetCurrentCategoryName(const FString& Name) { CurrentCategoryName = Name; }
    
    // Batch capture support
    UPROPERTY(BlueprintAssignable)
    FOnBatchCaptureStart OnBatchCaptureStart;
    
    void CaptureMultiple(const TArray<int32>& Indices);
    void CaptureSingle(); // Take a single screenshot
    
    // Called from PlayerController for scroll wheel shortcuts
    void AdjustFocalLength(float Delta);
    void AdjustAperture(float Delta);
    void AdjustFocusDistance(float Delta);

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
    void ShowSaveNotification(const FString& Path);
    
    FReply OnToggleClicked();
    FReply OnCaptureClicked();
    FReply OnShutterClicked();
    FReply OnSaveViewpointClicked();
    FReply OnResetClicked();
    FReply OnResolution1K();
    FReply OnResolution2K();
    FReply OnResolution4K();
    FReply OnOpenFolderClicked();
    
    void OnFocalLengthChanged(float Value);
    void OnApertureChanged(float Value);
    void OnFocusDistanceChanged(float Value);
    
    FString LastSavePath;
    FString CurrentCategoryName = TEXT("镜头");
    int32 ScreenshotCounter = 0;
    
    UPROPERTY()
    class UViewpointControlWidget* ViewpointControlRef = nullptr;
};
