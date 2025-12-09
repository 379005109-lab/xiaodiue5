// VideoControlWidget.cpp
#include "VideoControlWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SCheckBox.h"
#include "Styling/CoreStyle.h"

TSharedRef<SWidget> UVideoControlWidget::RebuildWidget()
{
    return SNew(SVerticalBox)
        // 顶部控制栏
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SBorder)
            .BorderBackgroundColor(FLinearColor(0.12f, 0.12f, 0.12f, 0.98f))
            .Padding(FMargin(10.0f, 6.0f))
            [
                SNew(SHorizontalBox)
                // 视频图标和标题
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("🎬 视频")))
                    .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                    .ColorAndOpacity(FLinearColor::White)
                ]
                // + 添加按钮
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(10.0f, 0.0f, 0.0f, 0.0f))
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.2f, 0.5f, 0.3f, 1.0f))
                    .OnClicked_Lambda([this]() { return OnAddClip(); })
                    .ContentPadding(FMargin(8.0f, 3.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("+")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                ]
                // 设置首帧按钮
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.2f, 0.4f, 0.6f, 1.0f))
                    .OnClicked_Lambda([this]() { return OnSetStartFrame(); })
                    .ContentPadding(FMargin(8.0f, 3.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("首帧")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                ]
                // 设置尾帧按钮
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.6f, 0.3f, 0.2f, 1.0f))
                    .OnClicked_Lambda([this]() { return OnSetEndFrame(); })
                    .ContentPadding(FMargin(8.0f, 3.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("尾帧")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                ]
                // 播放全部按钮
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.3f, 0.5f, 0.3f, 1.0f))
                    .OnClicked_Lambda([this]() { return OnPlayAll(); })
                    .ContentPadding(FMargin(8.0f, 3.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("▶ 播放")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                ]
                // 导出按钮
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.4f, 0.3f, 0.5f, 1.0f))
                    .OnClicked_Lambda([this]() { 
                        OnExportVideo.Broadcast();
                        return FReply::Handled(); 
                    })
                    .ContentPadding(FMargin(8.0f, 3.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("导出")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                ]
                // 总时长显示
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(10.0f, 0.0f, 0.0f, 0.0f))
                .VAlign(VAlign_Center)
                [
                    SAssignNew(TotalTimeText, STextBlock)
                    .Text(FText::FromString(TEXT("总时长: 0s")))
                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                    .ColorAndOpacity(FLinearColor(0.4f, 0.8f, 1.0f))
                ]
                // 展开/收起
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f))
                    .OnClicked_Lambda([this]() { return OnToggleClicked(); })
                    .ContentPadding(FMargin(6.0f, 3.0f))
                    [
                        SNew(STextBlock)
                        .Text_Lambda([this]() { return FText::FromString(bIsExpanded ? TEXT("▲") : TEXT("▼")); })
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                ]
            ]
        ]
        // 可展开的内容区域
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(ContentBox, SBox)
            .Visibility(EVisibility::Visible)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor(0.08f, 0.08f, 0.08f, 0.98f))
                .Padding(FMargin(8.0f))
                [
                    SNew(SVerticalBox)
                    // 时间刻度线
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
                    [
                        SNew(SBox)
                        .HeightOverride(20.0f)
                        [
                            SNew(SBorder)
                            .BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 1.0f))
                            .Padding(FMargin(5.0f, 2.0f))
                            [
                                SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                [
                                    SNew(STextBlock)
                                    .Text(FText::FromString(TEXT("00:00  |  00:05  |  00:10  |  00:15  |  00:20  |  00:25  |  00:30  |  00:35  |  00:40  |  00:45  |  00:50  |  00:55  |  01:00")))
                                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
                                    .ColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f))
                                ]
                            ]
                        ]
                    ]
                    // 视频轨道
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SBox)
                        .HeightOverride(90.0f)
                        [
                            SNew(SBorder)
                            .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.12f, 1.0f))
                            .Padding(FMargin(5.0f))
                            [
                                SNew(SScrollBox)
                                .Orientation(Orient_Horizontal)
                                + SScrollBox::Slot()
                                [
                                    SAssignNew(ClipContainer, SHorizontalBox)
                                ]
                            ]
                        ]
                    ]
                    // 状态文本
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 5.0f, 0.0f, 0.0f))
                    [
                        SAssignNew(StatusText, STextBlock)
                        .Text(FText::FromString(TEXT("点击 + 添加视频片段")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                        .ColorAndOpacity(FLinearColor(0.6f, 0.8f, 0.6f))
                    ]
                ]
            ]
        ];
}

void UVideoControlWidget::InitWidget()
{
    UpdateTotalTime();
}

void UVideoControlWidget::RebuildClips()
{
    if (!ClipContainer.IsValid()) return;
    
    ClipContainer->ClearChildren();
    
    for (int32 i = 0; i < VideoClips.Num(); i++)
    {
        const FVideoClipData& Clip = VideoClips[i];
        bool bIsSelected = (i == CurrentClipIndex);
        
        // 选中框颜色
        FLinearColor BorderColor = bIsSelected ? FLinearColor(0.2f, 0.6f, 0.9f, 1.0f) : FLinearColor(0.25f, 0.25f, 0.25f, 1.0f);
        float BorderWidth = bIsSelected ? 2.0f : 1.0f;
        
        ClipContainer->AddSlot()
        .AutoWidth()
        .Padding(FMargin(0.0f, 0.0f, 5.0f, 0.0f))
        [
            SNew(SBox)
            .WidthOverride(140.0f)
            [
                SNew(SBorder)
                .BorderBackgroundColor(BorderColor)
                .Padding(FMargin(2.0f))
                [
                    SNew(SBorder)
                    .BorderBackgroundColor(FLinearColor(0.12f, 0.12f, 0.14f, 1.0f))
                    .Padding(FMargin(4.0f))
                    [
                        SNew(SOverlay)
                        // 主内容
                        + SOverlay::Slot()
                        [
                            SNew(SVerticalBox)
                            // 首帧和尾帧缩略图 + 时长
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
                                        RebuildClips();
                                        OnClipSelected.Broadcast(i);
                                        return FReply::Handled(); 
                                    })
                                    .ContentPadding(FMargin(0.0f))
                                    [
                                        SNew(SBox)
                                        .WidthOverride(50.0f)
                                        .HeightOverride(35.0f)
                                        [
                                            SNew(SBorder)
                                            .BorderBackgroundColor(Clip.bStartSet ? FLinearColor(0.15f, 0.3f, 0.15f, 1.0f) : FLinearColor(0.2f, 0.2f, 0.2f, 1.0f))
                                            .HAlign(HAlign_Center)
                                            .VAlign(VAlign_Center)
                                            [
                                                SNew(STextBlock)
                                                .Text(FText::FromString(Clip.bStartSet ? TEXT("首") : TEXT("?")))
                                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                                                .ColorAndOpacity(Clip.bStartSet ? FLinearColor(0.4f, 1.0f, 0.4f) : FLinearColor(0.5f, 0.5f, 0.5f))
                                            ]
                                        ]
                                    ]
                                ]
                                // 时长显示
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
                                        RebuildClips();
                                        OnClipSelected.Broadcast(i);
                                        return FReply::Handled(); 
                                    })
                                    .ContentPadding(FMargin(0.0f))
                                    [
                                        SNew(SBox)
                                        .WidthOverride(50.0f)
                                        .HeightOverride(35.0f)
                                        [
                                            SNew(SBorder)
                                            .BorderBackgroundColor(Clip.bEndSet ? FLinearColor(0.3f, 0.15f, 0.15f, 1.0f) : FLinearColor(0.2f, 0.2f, 0.2f, 1.0f))
                                            .HAlign(HAlign_Center)
                                            .VAlign(VAlign_Center)
                                            [
                                                SNew(STextBlock)
                                                .Text(FText::FromString(Clip.bEndSet ? TEXT("尾") : TEXT("?")))
                                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                                                .ColorAndOpacity(Clip.bEndSet ? FLinearColor(1.0f, 0.4f, 0.4f) : FLinearColor(0.5f, 0.5f, 0.5f))
                                            ]
                                        ]
                                    ]
                                ]
                            ]
                            // 底部控制: 播放 + 时长调整
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(FMargin(0.0f, 3.0f, 0.0f, 0.0f))
                            [
                                SNew(SHorizontalBox)
                                // 播放按钮
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
                                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
                                        .ColorAndOpacity(FLinearColor::White)
                                    ]
                                ]
                                // 时长 -
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
                                [
                                    SNew(SButton)
                                    .ButtonColorAndOpacity(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f))
                                    .OnClicked_Lambda([this, i]() {
                                        if (VideoClips.IsValidIndex(i) && VideoClips[i].Duration > 1.0f)
                                        {
                                            VideoClips[i].Duration -= 1.0f;
                                            RebuildClips();
                                            UpdateTotalTime();
                                        }
                                        return FReply::Handled();
                                    })
                                    .ContentPadding(FMargin(4.0f, 1.0f))
                                    [
                                        SNew(STextBlock)
                                        .Text(FText::FromString(TEXT("-")))
                                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
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
                                            RebuildClips();
                                            UpdateTotalTime();
                                        }
                                        return FReply::Handled();
                                    })
                                    .ContentPadding(FMargin(4.0f, 1.0f))
                                    [
                                        SNew(STextBlock)
                                        .Text(FText::FromString(TEXT("+")))
                                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
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
                                    .ColorAndOpacity(FLinearColor(0.6f, 0.6f, 0.6f))
                                ]
                            ]
                        ]
                        // 右上角删除按钮
                        + SOverlay::Slot()
                        .HAlign(HAlign_Right)
                        .VAlign(VAlign_Top)
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.6f, 0.2f, 0.2f, 0.9f))
                            .OnClicked_Lambda([this, i]() {
                                DeleteClip(i);
                                return FReply::Handled();
                            })
                            .ContentPadding(FMargin(3.0f, 1.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("🗑")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                    ]
                ]
            ]
        ];
    }
    
    // 如果没有片段，显示空状态
    if (VideoClips.Num() == 0)
    {
        ClipContainer->AddSlot()
        .AutoWidth()
        [
            SNew(SBox)
            .WidthOverride(200.0f)
            .HeightOverride(80.0f)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 0.5f))
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("点击 + 添加视频片段")))
                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                    .ColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f))
                ]
            ]
        ];
    }
}

FReply UVideoControlWidget::OnToggleClicked()
{
    bIsExpanded = !bIsExpanded;
    if (ContentBox.IsValid())
    {
        ContentBox->SetVisibility(bIsExpanded ? EVisibility::Visible : EVisibility::Collapsed);
    }
    return FReply::Handled();
}

FReply UVideoControlWidget::OnAddClip()
{
    FVideoClipData NewClip;
    NewClip.Duration = 10.0f;
    VideoClips.Add(NewClip);
    CurrentClipIndex = VideoClips.Num() - 1;
    
    RebuildClips();
    UpdateTotalTime();
    
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(FString::Printf(TEXT("已添加片段 %d，请设置首帧和尾帧"), CurrentClipIndex + 1)));
    }
    
    return FReply::Handled();
}

FReply UVideoControlWidget::OnRemoveClip()
{
    if (VideoClips.Num() > 0 && VideoClips.IsValidIndex(CurrentClipIndex))
    {
        VideoClips.RemoveAt(CurrentClipIndex);
        if (CurrentClipIndex >= VideoClips.Num())
        {
            CurrentClipIndex = FMath::Max(0, VideoClips.Num() - 1);
        }
        RebuildClips();
        UpdateTotalTime();
        
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(TEXT("已删除片段")));
        }
    }
    return FReply::Handled();
}

FReply UVideoControlWidget::OnClipClicked(int32 Index)
{
    if (VideoClips.IsValidIndex(Index))
    {
        CurrentClipIndex = Index;
        RebuildClips();
        OnClipSelected.Broadcast(Index);
    }
    return FReply::Handled();
}

FReply UVideoControlWidget::OnSetStartFrame()
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
    
    // Directly confirm current camera position as start frame
    OnConfirmStartFrame.Broadcast();
    
    return FReply::Handled();
}

FReply UVideoControlWidget::OnSetEndFrame()
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
    
    // Directly confirm current camera position as end frame
    OnConfirmEndFrame.Broadcast();
    
    return FReply::Handled();
}

void UVideoControlWidget::SetStartFrame(const FVideoFrameData& Frame)
{
    if (VideoClips.IsValidIndex(CurrentClipIndex))
    {
        VideoClips[CurrentClipIndex].StartFrame = Frame;
        VideoClips[CurrentClipIndex].bStartSet = true;
        RebuildClips();
        
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(FString::Printf(TEXT("片段 %d 首帧已设置"), CurrentClipIndex + 1)));
        }
    }
}

void UVideoControlWidget::SetEndFrame(const FVideoFrameData& Frame)
{
    if (VideoClips.IsValidIndex(CurrentClipIndex))
    {
        VideoClips[CurrentClipIndex].EndFrame = Frame;
        VideoClips[CurrentClipIndex].bEndSet = true;
        RebuildClips();
        
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(FString::Printf(TEXT("片段 %d 尾帧已设置"), CurrentClipIndex + 1)));
        }
    }
}

FReply UVideoControlWidget::OnPlayClip(int32 Index)
{
    if (VideoClips.IsValidIndex(Index))
    {
        CurrentClipIndex = Index;
        
        // Broadcast play event
        OnPlaySingleClip.Broadcast(Index);
        
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(FString::Printf(TEXT("播放片段 %d..."), Index + 1)));
        }
    }
    return FReply::Handled();
}

FReply UVideoControlWidget::OnPlayAll()
{
    OnPlayAllClips.Broadcast();
    
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(TEXT("播放全部片段...")));
    }
    return FReply::Handled();
}

FVideoClipData UVideoControlWidget::GetClipData(int32 Index) const
{
    if (VideoClips.IsValidIndex(Index))
    {
        return VideoClips[Index];
    }
    return FVideoClipData();
}

void UVideoControlWidget::OnDurationChanged(int32 Index, float NewDuration)
{
    if (VideoClips.IsValidIndex(Index))
    {
        VideoClips[Index].Duration = FMath::Clamp(NewDuration, 1.0f, 60.0f);
        UpdateTotalTime();
    }
}

void UVideoControlWidget::UpdateTotalTime()
{
    float TotalTime = 0.0f;
    for (const FVideoClipData& Clip : VideoClips)
    {
        TotalTime += Clip.Duration;
    }
    
    if (TotalTimeText.IsValid())
    {
        int32 Minutes = (int32)(TotalTime / 60.0f);
        int32 Seconds = (int32)TotalTime % 60;
        TotalTimeText->SetText(FText::FromString(FString::Printf(TEXT("总时长: %02d:%02d"), Minutes, Seconds)));
    }
}

void UVideoControlWidget::DeleteClip(int32 Index)
{
    if (VideoClips.IsValidIndex(Index))
    {
        VideoClips.RemoveAt(Index);
        if (CurrentClipIndex >= VideoClips.Num())
        {
            CurrentClipIndex = FMath::Max(0, VideoClips.Num() - 1);
        }
        RebuildClips();
        UpdateTotalTime();
        
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(TEXT("片段已删除")));
        }
    }
}
