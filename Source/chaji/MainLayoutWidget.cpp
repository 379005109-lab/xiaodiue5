// MainLayoutWidget.cpp
#include "MainLayoutWidget.h"
#include "CategoryTabWidget.h"
#include "ParameterDisplayWidget.h"
#include "MediaControlWidget.h"
#include "ViewpointControlWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"

TSharedRef<SWidget> UMainLayoutWidget::RebuildWidget()
{
    // 主布局：全屏，左中右三列 + 底部
    // 左侧：类别标签 (180px)
    // 中间：3D画面预览区 (透明，让游戏画面显示)
    // 右侧：参数面板 (300px)
    // 底部：时间轴 + 多镜头 (150px)
    
    const float LeftPanelWidth = 180.0f;
    const float RightPanelWidth = 300.0f;
    const float BottomPanelHeight = 150.0f;
    FLinearColor PanelBgColor = FLinearColor(0.08f, 0.08f, 0.1f, 1.0f); // 深色背景
    FLinearColor BorderColor = FLinearColor(0.15f, 0.15f, 0.18f, 1.0f);
    
    return SNew(SOverlay)
        // 全屏背景层
        + SOverlay::Slot()
        [
            SNew(SHorizontalBox)
            // ===== 左侧面板 =====
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(LeftPanelWidth)
                [
                    SNew(SBorder)
                    .BorderBackgroundColor(PanelBgColor)
                    .Padding(FMargin(0.0f))
                    [
                        SNew(SVerticalBox)
                        // 标题
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SBorder)
                            .BorderBackgroundColor(BorderColor)
                            .Padding(FMargin(12.0f, 10.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("类别")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                                .ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f))
                            ]
                        ]
                        // 类别列表容器
                        + SVerticalBox::Slot()
                        .FillHeight(1.0f)
                        [
                            SAssignNew(LeftPanelContainer, SBox)
                        ]
                    ]
                ]
            ]
            // ===== 中间区域 (画面 + 底部) =====
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(SVerticalBox)
                // 3D画面区域 - 透明
                + SVerticalBox::Slot()
                .FillHeight(1.0f)
                [
                    SAssignNew(ViewportContainer, SBox)
                    // 透明，让3D画面显示
                ]
                // 底部面板 - 时间轴 + 多镜头
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SBox)
                    .HeightOverride(BottomPanelHeight)
                    [
                        SNew(SBorder)
                        .BorderBackgroundColor(PanelBgColor)
                        .Padding(FMargin(0.0f))
                        [
                            SNew(SVerticalBox)
                            // 时间轴控制容器
                            + SVerticalBox::Slot()
                            .FillHeight(1.0f)
                            [
                                SAssignNew(BottomPanelContainer, SBox)
                            ]
                        ]
                    ]
                ]
            ]
            // ===== 右侧面板 =====
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(RightPanelWidth)
                [
                    SNew(SBorder)
                    .BorderBackgroundColor(PanelBgColor)
                    .Padding(FMargin(0.0f))
                    [
                        SNew(SVerticalBox)
                        // 标题
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SBorder)
                            .BorderBackgroundColor(BorderColor)
                            .Padding(FMargin(12.0f, 10.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("参数")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                                .ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f))
                            ]
                        ]
                        // 参数面板容器
                        + SVerticalBox::Slot()
                        .FillHeight(1.0f)
                        [
                            SAssignNew(RightPanelContainer, SBox)
                        ]
                    ]
                ]
            ]
        ];
}

void UMainLayoutWidget::InitSubWidgets(APlayerController* PC)
{
    if (!PC) return;
    
    // 创建类别标签
    CategoryTab = CreateWidget<UCategoryTabWidget>(PC, UCategoryTabWidget::StaticClass());
    if (CategoryTab && LeftPanelContainer.IsValid())
    {
        LeftPanelContainer->SetContent(CategoryTab->TakeWidget());
    }
    
    // 创建参数显示
    ParameterDisplay = CreateWidget<UParameterDisplayWidget>(PC, UParameterDisplayWidget::StaticClass());
    if (ParameterDisplay && RightPanelContainer.IsValid())
    {
        RightPanelContainer->SetContent(ParameterDisplay->TakeWidget());
    }
    
    // 创建媒体控制
    MediaControl = CreateWidget<UMediaControlWidget>(PC, UMediaControlWidget::StaticClass());
    if (MediaControl && BottomPanelContainer.IsValid())
    {
        MediaControl->InitWidget();
        BottomPanelContainer->SetContent(MediaControl->TakeWidget());
    }
}
