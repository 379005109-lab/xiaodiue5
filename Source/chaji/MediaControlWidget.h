// MediaControlWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VideoClipData.h"
#include "MediaControlWidget.generated.h"

UENUM(BlueprintType)
enum class EMediaMode : uint8
{
    None,
    Photo,
    Video
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMediaModeChanged, EMediaMode, NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotoShutter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVideoClipPlay, int32, ClipIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVideoPlayAll);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVideoExport);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSetStartFrame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSetEndFrame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimelineScrub, float, TimePosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOpenFolder);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResetCamera);

UCLASS()
class UMediaControlWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void InitWidget();
    
    // Mode
    EMediaMode GetCurrentMode() const { return CurrentMode; }
    
    // Video clips
    void SetStartFrame(const FVideoFrameData& Frame);
    void SetEndFrame(const FVideoFrameData& Frame);
    FVideoClipData GetClipData(int32 Index) const;
    int32 GetClipCount() const { return VideoClips.Num(); }
    int32 GetCurrentClipIndex() const { return CurrentClipIndex; }
    
    // Timeline
    float GetTimelinePosition() const { return TimelinePosition; }
    void SetTimelinePosition(float Position);
    
    // Events
    UPROPERTY(BlueprintAssignable)
    FOnMediaModeChanged OnModeChanged;
    
    UPROPERTY(BlueprintAssignable)
    FOnPhotoShutter OnPhotoShutter;
    
    UPROPERTY(BlueprintAssignable)
    FOnVideoClipPlay OnVideoClipPlay;
    
    UPROPERTY(BlueprintAssignable)
    FOnVideoPlayAll OnVideoPlayAll;
    
    UPROPERTY(BlueprintAssignable)
    FOnVideoExport OnVideoExport;
    
    UPROPERTY(BlueprintAssignable)
    FOnSetStartFrame OnSetStartFrame;
    
    UPROPERTY(BlueprintAssignable)
    FOnSetEndFrame OnSetEndFrame;
    
    UPROPERTY(BlueprintAssignable)
    FOnTimelineScrub OnTimelineScrub;
    
    UPROPERTY(BlueprintAssignable)
    FOnOpenFolder OnOpenFolder;
    
    UPROPERTY(BlueprintAssignable)
    FOnResetCamera OnResetCamera;

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    // UI Components
    TSharedPtr<class SVerticalBox> MainContainer;
    TSharedPtr<class SHorizontalBox> ClipTrackContainer;
    TSharedPtr<class STextBlock> StatusText;
    TSharedPtr<class STextBlock> TotalTimeText;
    TSharedPtr<class SBox> PhotoPanel;
    TSharedPtr<class SBox> VideoPanel;
    TSharedPtr<class SSlider> TimelineSlider;
    
    // State
    EMediaMode CurrentMode = EMediaMode::None;
    TArray<FVideoClipData> VideoClips;
    int32 CurrentClipIndex = 0;
    float TimelinePosition = 0.0f;
    float TotalDuration = 0.0f;
    bool bIsSettingStart = true;
    
    // Photo settings
    int32 ResolutionIndex = 1; // 0=1K, 1=2K, 2=4K
    int32 AspectRatioIndex = 0; // 0=16:9, 1=9:16, 2=1:1
    
    // UI functions
    void RebuildVideoClips();
    void UpdateTotalTime();
    void UpdateTimelineFromClips();
    
    // Button callbacks
    FReply OnPhotoModeClicked();
    FReply OnVideoModeClicked();
    FReply OnShutterClicked();
    FReply OnAddClip();
    FReply OnSetStartClicked();
    FReply OnSetEndClicked();
    FReply OnPlayClip(int32 Index);
    FReply OnPlayAllClicked();
    FReply OnExportClicked();
    FReply OnDeleteClip(int32 Index);
    void OnTimelineValueChanged(float NewValue);
};
