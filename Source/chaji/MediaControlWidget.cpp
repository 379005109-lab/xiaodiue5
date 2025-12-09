// MediaControlWidget.cpp
#include "MediaControlWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Images/SImage.h"
#include "Styling/CoreStyle.h"

TSharedRef<SWidget> UMediaControlWidget::RebuildWidget()
{
    return SNew(SVerticalBox)
        // 顶部模式选择图标
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SBorder)
            .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.95f))
            .Padding(FMargin(8.0f, 6.0f))
            [
                SNew(SHorizontalBox)
                // 相机图标
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity_Lambda([this]() { 
                        return CurrentMode == EMediaMode::Photo ? FLinearColor(0.2f, 0.5f, 0.8f, 1.0f) : FLinearColor(0.25f, 0.25f, 0.25f, 1.0f); 
                    })
                    .OnClicked_Lambda([this]() { return OnPhotoModeClicked(); })
                    .ContentPadding(FMargin(12.0f, 8.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("📷")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                ]
                // 视频图标
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(6.0f, 0.0f, 0.0f, 0.0f))
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity_Lambda([this]() { 
                        return CurrentMode == EMediaMode::Video ? FLinearColor(0.8f, 0.3f, 0.2f, 1.0f) : FLinearColor(0.25f, 0.25f, 0.25f, 1.0f); 
                    })
                    .OnClicked_Lambda([this]() { return OnVideoModeClicked(); })
                    .ContentPadding(FMargin(12.0f, 8.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("🎬")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                ]
                // 状态文本
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .Padding(FMargin(12.0f, 0.0f, 0.0f, 0.0f))
                .VAlign(VAlign_Center)
                [
                    SAssignNew(StatusText, STextBlock)
                    .Text(FText::FromString(TEXT("选择模式")))
                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                    .ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))
                ]
            ]
        ]
        // 相机面板
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(PhotoPanel, SBox)
            .Visibility_Lambda([this]() { return CurrentMode == EMediaMode::Photo ? EVisibility::Visible : EVisibility::Collapsed; })
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor(0.08f, 0.08f, 0.1f, 0.98f))
                .Padding(FMargin(10.0f))
                [
                    SNew(SVerticalBox)
                    // 快门按钮行
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.2f, 0.5f, 0.8f, 1.0f))
                            .OnClicked_Lambda([this]() { return OnShutterClicked(); })
                            .ContentPadding(FMargin(20.0f, 8.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("📷 快门")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                    ]
                    // 分辨率选择
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 8.0f, 0.0f, 0.0f))
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("分辨率: ")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                            .ColorAndOpacity(FLinearColor::White)
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return ResolutionIndex == 0 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { ResolutionIndex = 0; return FReply::Handled(); })
                            .ContentPadding(FMargin(8.0f, 4.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("1K")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return ResolutionIndex == 1 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { ResolutionIndex = 1; return FReply::Handled(); })
                            .ContentPadding(FMargin(8.0f, 4.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("2K")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return ResolutionIndex == 2 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { ResolutionIndex = 2; return FReply::Handled(); })
                            .ContentPadding(FMargin(8.0f, 4.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("4K")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                    ]
                ]
            ]
        ]
        // 视频面板
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(VideoPanel, SBox)
            .Visibility_Lambda([this]() { return CurrentMode == EMediaMode::Video ? EVisibility::Visible : EVisibility::Collapsed; })
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor(0.08f, 0.08f, 0.1f, 0.98f))
                .Padding(FMargin(10.0f))
                [
                    SNew(SVerticalBox)
                    // 控制按钮行
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SHorizontalBox)
                        // + 添加
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.2f, 0.5f, 0.3f, 1.0f))
                            .OnClicked_Lambda([this]() { return OnAddClip(); })
                            .ContentPadding(FMargin(10.0f, 5.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("+ 添加")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        // 首帧
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(6.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return bIsSettingStart ? FLinearColor(0.3f, 0.6f, 0.3f, 1.0f) : FLinearColor(0.2f, 0.4f, 0.5f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnSetStartClicked(); })
                            .ContentPadding(FMargin(10.0f, 5.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("首帧")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        // 尾帧
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return !bIsSettingStart ? FLinearColor(0.6f, 0.3f, 0.3f, 1.0f) : FLinearColor(0.5f, 0.3f, 0.2f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnSetEndClicked(); })
                            .ContentPadding(FMargin(10.0f, 5.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("尾帧")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        // 播放全部
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(6.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.3f, 0.5f, 0.3f, 1.0f))
                            .OnClicked_Lambda([this]() { return OnPlayAllClicked(); })
                            .ContentPadding(FMargin(10.0f, 5.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("▶ 播放")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        // 导出
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.5f, 0.3f, 0.5f, 1.0f))
                            .OnClicked_Lambda([this]() { return OnExportClicked(); })
                            .ContentPadding(FMargin(10.0f, 5.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("导出")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        // 总时长
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(10.0f, 0.0f, 0.0f, 0.0f))
                        .VAlign(VAlign_Center)
                        [
                            SAssignNew(TotalTimeText, STextBlock)
                            .Text(FText::FromString(TEXT("00:00")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
                            .ColorAndOpacity(FLinearColor(0.3f, 0.8f, 1.0f))
                        ]
                    ]
                    // 时间刻度线
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 8.0f, 0.0f, 0.0f))
                    [
                        SNew(SBox)
                        .WidthOverride(600.0f)
                        .HeightOverride(25.0f)
                        [
                            SNew(SBorder)
                            .BorderBackgroundColor(FLinearColor(0.18f, 0.18f, 0.2f, 1.0f))
                            .Padding(FMargin(5.0f, 3.0f))
                            [
                                SNew(SOverlay)
                                // 刻度标记
                                + SOverlay::Slot()
                                [
                                    SNew(SHorizontalBox)
                                    + SHorizontalBox::Slot()
                                    .FillWidth(1.0f)
                                    [
                                        SNew(STextBlock)
                                        .Text(FText::FromString(TEXT("0s     5s     10s    15s    20s    25s    30s    35s    40s    45s    50s    55s    60s")))
                                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                                        .ColorAndOpacity(FLinearColor(0.6f, 0.6f, 0.6f))
                                    ]
                                ]
                            ]
                        ]
                    ]
                    // 时间滑块/光标
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 2.0f, 0.0f, 0.0f))
                    [
                        SNew(SBox)
                        .WidthOverride(600.0f)
                        .HeightOverride(20.0f)
                        [
                            SAssignNew(TimelineSlider, SSlider)
                            .Value_Lambda([this]() { return TotalDuration > 0 ? TimelinePosition / TotalDuration : 0.0f; })
                            .OnValueChanged_Lambda([this](float NewValue) { OnTimelineValueChanged(NewValue); })
                            .SliderBarColor(FLinearColor(0.3f, 0.3f, 0.35f))
                            .SliderHandleColor(FLinearColor(1.0f, 0.4f, 0.2f))
                        ]
                    ]
                    // 视频轨道
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 5.0f, 0.0f, 0.0f))
                    [
                        SNew(SBox)
                        .WidthOverride(600.0f)
                        .HeightOverride(80.0f)
                        [
                            SNew(SBorder)
                            .BorderBackgroundColor(FLinearColor(0.12f, 0.12f, 0.15f, 1.0f))
                            .Padding(FMargin(5.0f))
                            [
                                SNew(SScrollBox)
                                .Orientation(Orient_Horizontal)
                                + SScrollBox::Slot()
                                [
                                    SAssignNew(ClipTrackContainer, SHorizontalBox)
                                ]
                            ]
                        ]
                    ]
                ]
            ]
        ];
}

void UMediaControlWidget::InitWidget()
{
    UpdateTotalTime();
}

FReply UMediaControlWidget::OnPhotoModeClicked()
{
    if (CurrentMode == EMediaMode::Photo)
    {
        CurrentMode = EMediaMode::None;
    }
    else
    {
        CurrentMode = EMediaMode::Photo;
    }
    OnModeChanged.Broadcast(CurrentMode);
    
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(CurrentMode == EMediaMode::Photo ? TEXT("📷 相机模式") : TEXT("选择模式")));
    }
    return FReply::Handled();
}

FReply UMediaControlWidget::OnVideoModeClicked()
{
    if (CurrentMode == EMediaMode::Video)
    {
        CurrentMode = EMediaMode::None;
    }
    else
    {
        CurrentMode = EMediaMode::Video;
    }
    OnModeChanged.Broadcast(CurrentMode);
    
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(CurrentMode == EMediaMode::Video ? TEXT("🎬 视频模式") : TEXT("选择模式")));
    }
    return FReply::Handled();
}

FReply UMediaControlWidget::OnShutterClicked()
{
    OnPhotoShutter.Broadcast();
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(TEXT("📷 拍照中...")));
    }
    return FReply::Handled();
}

FReply UMediaControlWidget::OnAddClip()
{
    FVideoClipData NewClip;
    NewClip.Duration = 5.0f;
    NewClip.bStartSet = false;
    NewClip.bEndSet = false;
    VideoClips.Add(NewClip);
    CurrentClipIndex = VideoClips.Num() - 1;
    
    RebuildVideoClips();
    UpdateTotalTime();
    
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(FString::Printf(TEXT("添加片段 #%d"), CurrentClipIndex + 1)));
    }
    return FReply::Handled();
}

FReply UMediaControlWidget::OnSetStartClicked()
{
    if (VideoClips.Num() == 0)
    {
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(TEXT("请先添加视频片段")));
        }
        return FReply::Handled();
    }
    
    bIsSettingStart = true;
    OnSetStartFrame.Broadcast();
    return FReply::Handled();
}

FReply UMediaControlWidget::OnSetEndClicked()
{
    if (VideoClips.Num() == 0)
    {
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(TEXT("请先添加视频片段")));
        }
        return FReply::Handled();
    }
    
    bIsSettingStart = false;
    OnSetEndFrame.Broadcast();
    return FReply::Handled();
}

FReply UMediaControlWidget::OnPlayClip(int32 Index)
{
    if (VideoClips.IsValidIndex(Index))
    {
        CurrentClipIndex = Index;
        RebuildVideoClips();
        OnVideoClipPlay.Broadcast(Index);
        
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(FString::Printf(TEXT("▶ 播放片段 #%d"), Index + 1)));
        }
    }
    return FReply::Handled();
}

FReply UMediaControlWidget::OnPlayAllClicked()
{
    OnVideoPlayAll.Broadcast();
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(TEXT("▶ 播放全部...")));
    }
    return FReply::Handled();
}

FReply UMediaControlWidget::OnExportClicked()
{
    OnVideoExport.Broadcast();
    if (StatusText.IsValid())
    {
        FString ExportPath = FPaths::ProjectSavedDir() / TEXT("VideoExport");
        StatusText->SetText(FText::FromString(FString::Printf(TEXT("导出到: %s"), *ExportPath)));
    }
    return FReply::Handled();
}

FReply UMediaControlWidget::OnDeleteClip(int32 Index)
{
    if (VideoClips.IsValidIndex(Index))
    {
        VideoClips.RemoveAt(Index);
        if (CurrentClipIndex >= VideoClips.Num())
        {
            CurrentClipIndex = FMath::Max(0, VideoClips.Num() - 1);
        }
        RebuildVideoClips();
        UpdateTotalTime();
        
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(TEXT("片段已删除")));
        }
    }
    return FReply::Handled();
}

void UMediaControlWidget::OnTimelineValueChanged(float NewValue)
{
    if (TotalDuration > 0)
    {
        TimelinePosition = NewValue * TotalDuration;
        OnTimelineScrub.Broadcast(TimelinePosition);
    }
}

void UMediaControlWidget::SetStartFrame(const FVideoFrameData& Frame)
{
    if (VideoClips.IsValidIndex(CurrentClipIndex))
    {
        VideoClips[CurrentClipIndex].StartFrame = Frame;
        VideoClips[CurrentClipIndex].bStartSet = true;
        RebuildVideoClips();
        
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(FString::Printf(TEXT("片段 #%d 首帧已设置"), CurrentClipIndex + 1)));
        }
    }
}

void UMediaControlWidget::SetEndFrame(const FVideoFrameData& Frame)
{
    if (VideoClips.IsValidIndex(CurrentClipIndex))
    {
        VideoClips[CurrentClipIndex].EndFrame = Frame;
        VideoClips[CurrentClipIndex].bEndSet = true;
        RebuildVideoClips();
        
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(FString::Printf(TEXT("片段 #%d 尾帧已设置"), CurrentClipIndex + 1)));
        }
    }
}

void UMediaControlWidget::SetTimelinePosition(float Position)
{
    TimelinePosition = Position;
}

FVideoClipData UMediaControlWidget::GetClipData(int32 Index) const
{
    if (VideoClips.IsValidIndex(Index))
    {
        return VideoClips[Index];
    }
    return FVideoClipData();
}

void UMediaControlWidget::RebuildVideoClips()
{
    if (!ClipTrackContainer.IsValid()) return;
    
    ClipTrackContainer->ClearChildren();
    
    for (int32 i = 0; i < VideoClips.Num(); i++)
    {
        const FVideoClipData& Clip = VideoClips[i];
        bool bIsSelected = (i == CurrentClipIndex);
        
        // 选中高亮颜色
        FLinearColor BorderColor = bIsSelected ? FLinearColor(0.0f, 0.7f, 1.0f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);
        FLinearColor BgColor = bIsSelected ? FLinearColor(0.15f, 0.2f, 0.25f, 1.0f) : FLinearColor(0.1f, 0.1f, 0.12f, 1.0f);
        
        ClipTrackContainer->AddSlot()
        .AutoWidth()
        .Padding(FMargin(0.0f, 0.0f, 4.0f, 0.0f))
        [
            SNew(SBox)
            .WidthOverride(130.0f)
            [
                SNew(SBorder)
                .BorderBackgroundColor(BorderColor)
                .Padding(FMargin(2.0f))
                [
                    SNew(SBorder)
                    .BorderBackgroundColor(BgColor)
                    .Padding(FMargin(4.0f))
                    [
                        SNew(SOverlay)
                        // 主内容
                        + SOverlay::Slot()
                        [
                            SNew(SVerticalBox)
                            // 首帧 + 时长 + 尾帧
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            [
                                SNew(SHorizontalBox)
                                // 首帧
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew(SButton)
                                    .ButtonColorAndOpacity(FLinearColor::Transparent)
                                    .OnClicked_Lambda([this, i]() {
                                        CurrentClipIndex = i;
                                        bIsSettingStart = true;
                                        RebuildVideoClips();
                                        return FReply::Handled();
                                    })
                                    .ContentPadding(FMargin(0.0f))
                                    [
                                        SNew(SBox)
                                        .WidthOverride(40.0f)
                                        .HeightOverride(30.0f)
                                        [
                                            SNew(SBorder)
                                            .BorderBackgroundColor(Clip.bStartSet ? FLinearColor(0.1f, 0.3f, 0.1f, 1.0f) : FLinearColor(0.15f, 0.15f, 0.15f, 1.0f))
                                            .HAlign(HAlign_Center)
                                            .VAlign(VAlign_Center)
                                            [
                                                SNew(STextBlock)
                                                .Text(FText::FromString(Clip.bStartSet ? TEXT("首") : TEXT("?")))
                                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
                                                .ColorAndOpacity(Clip.bStartSet ? FLinearColor(0.4f, 1.0f, 0.4f) : FLinearColor(0.4f, 0.4f, 0.4f))
                                            ]
                                        ]
                                    ]
                                ]
                                // 时长
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock)
                                    .Text(FText::FromString(FString::Printf(TEXT("%.0fs"), Clip.Duration)))
                                    .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                                    .ColorAndOpacity(FLinearColor(0.3f, 0.8f, 1.0f))
                                ]
                                // 尾帧
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew(SButton)
                                    .ButtonColorAndOpacity(FLinearColor::Transparent)
                                    .OnClicked_Lambda([this, i]() {
                                        CurrentClipIndex = i;
                                        bIsSettingStart = false;
                                        RebuildVideoClips();
                                        return FReply::Handled();
                                    })
                                    .ContentPadding(FMargin(0.0f))
                                    [
                                        SNew(SBox)
                                        .WidthOverride(40.0f)
                                        .HeightOverride(30.0f)
                                        [
                                            SNew(SBorder)
                                            .BorderBackgroundColor(Clip.bEndSet ? FLinearColor(0.3f, 0.1f, 0.1f, 1.0f) : FLinearColor(0.15f, 0.15f, 0.15f, 1.0f))
                                            .HAlign(HAlign_Center)
                                            .VAlign(VAlign_Center)
                                            [
                                                SNew(STextBlock)
                                                .Text(FText::FromString(Clip.bEndSet ? TEXT("尾") : TEXT("?")))
                                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
                                                .ColorAndOpacity(Clip.bEndSet ? FLinearColor(1.0f, 0.4f, 0.4f) : FLinearColor(0.4f, 0.4f, 0.4f))
                                            ]
                                        ]
                                    ]
                                ]
                            ]
                            // 底部控制
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(FMargin(0.0f, 3.0f, 0.0f, 0.0f))
                            [
                                SNew(SHorizontalBox)
                                // 播放
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew(SButton)
                                    .ButtonColorAndOpacity(FLinearColor(0.2f, 0.4f, 0.2f, 1.0f))
                                    .OnClicked_Lambda([this, i]() { return OnPlayClip(i); })
                                    .ContentPadding(FMargin(6.0f, 2.0f))
                                    [
                                        SNew(STextBlock)
                                        .Text(FText::FromString(TEXT("▶")))
                                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
                                        .ColorAndOpacity(FLinearColor::White)
                                    ]
                                ]
                                // 时长 -
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(FMargin(3.0f, 0.0f, 0.0f, 0.0f))
                                [
                                    SNew(SButton)
                                    .ButtonColorAndOpacity(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f))
                                    .OnClicked_Lambda([this, i]() {
                                        if (VideoClips.IsValidIndex(i) && VideoClips[i].Duration > 1.0f)
                                        {
                                            VideoClips[i].Duration -= 1.0f;
                                            RebuildVideoClips();
                                            UpdateTotalTime();
                                        }
                                        return FReply::Handled();
                                    })
                                    .ContentPadding(FMargin(4.0f, 1.0f))
                                    [
                                        SNew(STextBlock)
                                        .Text(FText::FromString(TEXT("-")))
                                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
                                        .ColorAndOpacity(FLinearColor::White)
                                    ]
                                ]
                                // 时长 +
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(FMargin(2.0f, 0.0f, 0.0f, 0.0f))
                                [
                                    SNew(SButton)
                                    .ButtonColorAndOpacity(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f))
                                    .OnClicked_Lambda([this, i]() {
                                        if (VideoClips.IsValidIndex(i) && VideoClips[i].Duration < 60.0f)
                                        {
                                            VideoClips[i].Duration += 1.0f;
                                            RebuildVideoClips();
                                            UpdateTotalTime();
                                        }
                                        return FReply::Handled();
                                    })
                                    .ContentPadding(FMargin(4.0f, 1.0f))
                                    [
                                        SNew(STextBlock)
                                        .Text(FText::FromString(TEXT("+")))
                                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
                                        .ColorAndOpacity(FLinearColor::White)
                                    ]
                                ]
                                // 片段编号
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                .HAlign(HAlign_Right)
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock)
                                    .Text(FText::FromString(FString::Printf(TEXT("#%d"), i + 1)))
                                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
                                    .ColorAndOpacity(bIsSelected ? FLinearColor(0.3f, 0.8f, 1.0f) : FLinearColor(0.5f, 0.5f, 0.5f))
                                ]
                            ]
                        ]
                        // 右上角删除按钮
                        + SOverlay::Slot()
                        .HAlign(HAlign_Right)
                        .VAlign(VAlign_Top)
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.6f, 0.15f, 0.15f, 0.9f))
                            .OnClicked_Lambda([this, i]() { return OnDeleteClip(i); })
                            .ContentPadding(FMargin(3.0f, 1.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("×")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                    ]
                ]
            ]
        ];
    }
    
    UpdateTimelineFromClips();
}

void UMediaControlWidget::UpdateTotalTime()
{
    TotalDuration = 0.0f;
    for (const FVideoClipData& Clip : VideoClips)
    {
        TotalDuration += Clip.Duration;
    }
    
    if (TotalTimeText.IsValid())
    {
        int32 Minutes = (int32)(TotalDuration / 60.0f);
        int32 Seconds = (int32)TotalDuration % 60;
        TotalTimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
    }
}

void UMediaControlWidget::UpdateTimelineFromClips()
{
    UpdateTotalTime();
}
