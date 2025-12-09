// VideoControlWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VideoClipData.h"
#include "VideoControlWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVideoClipSelected, int32, ClipIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayAllClips);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmStartFrame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmEndFrame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaySingleClip, int32, ClipIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExportVideo);

UCLASS()
class UVideoControlWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void InitWidget();
    
    // 设置首帧/尾帧
    void SetStartFrame(const FVideoFrameData& Frame);
    void SetEndFrame(const FVideoFrameData& Frame);
    
    // 获取片段数据
    FVideoClipData GetClipData(int32 Index) const;
    int32 GetClipCount() const { return VideoClips.Num(); }
    
    // 当前编辑的片段索引
    int32 GetCurrentClipIndex() const { return CurrentClipIndex; }
    
    // 事件
    UPROPERTY(BlueprintAssignable)
    FOnVideoClipSelected OnClipSelected;
    
    UPROPERTY(BlueprintAssignable)
    FOnPlayAllClips OnPlayAllClips;
    
    UPROPERTY(BlueprintAssignable)
    FOnConfirmStartFrame OnConfirmStartFrame;
    
    UPROPERTY(BlueprintAssignable)
    FOnConfirmEndFrame OnConfirmEndFrame;
    
    UPROPERTY(BlueprintAssignable)
    FOnPlaySingleClip OnPlaySingleClip;
    
    UPROPERTY(BlueprintAssignable)
    FOnExportVideo OnExportVideo;
    
    // Delete specific clip
    void DeleteClip(int32 Index);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    TSharedPtr<class SHorizontalBox> ClipContainer;
    TSharedPtr<class SBox> ContentBox;
    TSharedPtr<class STextBlock> StatusText;
    TSharedPtr<class STextBlock> TotalTimeText;
    
    // 视频片段数组
    TArray<FVideoClipData> VideoClips;
    int32 CurrentClipIndex = 0;
    
    bool bIsExpanded = true;
    bool bIsSettingStart = true; // true=设置首帧, false=设置尾帧
    
    void RebuildClips();
    FReply OnToggleClicked();
    FReply OnAddClip();
    FReply OnRemoveClip();
    FReply OnClipClicked(int32 Index);
    FReply OnSetStartFrame();
    FReply OnSetEndFrame();
    FReply OnPlayClip(int32 Index);
    FReply OnPlayAll();
    void OnDurationChanged(int32 Index, float NewDuration);
    void UpdateTotalTime();
};
