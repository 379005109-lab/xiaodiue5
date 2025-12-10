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
    // ============ 全屏主布局 ============
    // 视口已经通过 LocalPlayer 设置为中间区域
    // 这里只需要在左侧、右侧、底部放置不透明的深色面板
    // 
    // 布局 (1920x1080 为例):
    // 左侧面板: 180px (约9.4%)
    // 右侧面板: 300px (约15.6%)
    // 底部面板: 150px (约13.9%)
    // 中间3D画面: 剩余区域 (视口自动渲染到这里)
    
    const float LeftPanelWidth = 180.0f;
    const float RightPanelWidth = 300.0f;
    const float BottomPanelHeight = 150.0f;
    FLinearColor PanelBgColor = FLinearColor(0.06f, 0.06f, 0.08f, 1.0f); // 深色不透明背景
    FLinearColor HeaderBgColor = FLinearColor(0.1f, 0.1f, 0.12f, 1.0f);
    FLinearColor TextColor = FLinearColor(0.75f, 0.75f, 0.75f);
    
    return SNew(SHorizontalBox)
        // ===== 左侧面板 (完全不透明) =====
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
                    // 标题栏
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SBorder)
                        .BorderBackgroundColor(HeaderBgColor)
                        .Padding(FMargin(12.0f, 10.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("类别")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                            .ColorAndOpacity(TextColor)
                        ]
                    ]
                    // 类别列表区域
                    + SVerticalBox::Slot()
                    .FillHeight(1.0f)
                    [
                        SAssignNew(LeftPanelContainer, SBox)
                    ]
                ]
            ]
        ]
        // ===== 中间区域 (3D画面 + 底部控制) =====
        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        [
            SNew(SVerticalBox)
            // 3D画面区域 - 完全透明，视口会渲染到这里
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SAssignNew(ViewportContainer, SBox)
                // 不添加任何内容，保持透明
            ]
            // 底部面板 - 时间轴 + 多镜头 (完全不透明)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBox)
                .HeightOverride(BottomPanelHeight)
                [
                    SNew(SBorder)
                    .BorderBackgroundColor(PanelBgColor)
                    .Padding(FMargin(8.0f, 5.0f))
                    [
                        SAssignNew(BottomPanelContainer, SBox)
                    ]
                ]
            ]
        ]
        // ===== 右侧面板 (完全不透明) =====
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
                    // 标题栏
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SBorder)
                        .BorderBackgroundColor(HeaderBgColor)
                        .Padding(FMargin(12.0f, 10.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("参数")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                            .ColorAndOpacity(TextColor)
                        ]
                    ]
                    // 参数面板区域
                    + SVerticalBox::Slot()
                    .FillHeight(1.0f)
                    [
                        SAssignNew(RightPanelContainer, SBox)
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
