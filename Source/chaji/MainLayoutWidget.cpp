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
#include "Widgets/Images/SImage.h"
#include "Styling/CoreStyle.h"
#include "Brushes/SlateColorBrush.h"

TSharedRef<SWidget> UMainLayoutWidget::RebuildWidget()
{
    // ============ 全屏布局 - 使用 SOverlay 确保面板完全遮挡3D画面 ============
    // 层级结构:
    // - 底层: 3D画面 (透过中间透明区域显示)
    // - 顶层: 左侧面板 + 右侧面板 + 底部面板 (完全不透明，遮挡3D)
    
    const float LeftPanelWidth = 180.0f;
    const float RightPanelWidth = 300.0f;
    const float BottomPanelHeight = 150.0f;
    
    // 使用纯色画刷确保完全不透明
    static FSlateColorBrush SolidBrush(FLinearColor(0.05f, 0.05f, 0.07f, 1.0f));
    FLinearColor PanelBgColor = FLinearColor(0.05f, 0.05f, 0.07f, 1.0f);
    FLinearColor HeaderBgColor = FLinearColor(0.1f, 0.1f, 0.12f, 1.0f);
    FLinearColor TextColor = FLinearColor(0.75f, 0.75f, 0.75f);
    
    // 使用 SOverlay 让面板覆盖在3D画面上
    return SNew(SOverlay)
        // 第一层：主布局框架
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
                    SNew(SImage)
                    .Image(&SolidBrush)
                ]
            ]
            // ===== 中间区域 =====
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(SVerticalBox)
                // 3D画面区域 - 透明
                + SVerticalBox::Slot()
                .FillHeight(1.0f)
                [
                    SAssignNew(ViewportContainer, SBox)
                ]
                // 底部面板
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SBox)
                    .HeightOverride(BottomPanelHeight)
                    [
                        SNew(SImage)
                        .Image(&SolidBrush)
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
                    SNew(SImage)
                    .Image(&SolidBrush)
                ]
            ]
        ]
        // 第二层：面板内容
        + SOverlay::Slot()
        [
            SNew(SHorizontalBox)
            // 左侧面板内容
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(LeftPanelWidth)
                [
                    SNew(SVerticalBox)
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
                    + SVerticalBox::Slot()
                    .FillHeight(1.0f)
                    [
                        SAssignNew(LeftPanelContainer, SBox)
                    ]
                ]
            ]
            // 中间区域 + 底部
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .FillHeight(1.0f)
                [
                    SNullWidget::NullWidget
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SBox)
                    .HeightOverride(BottomPanelHeight)
                    .Padding(FMargin(8.0f, 5.0f))
                    [
                        SAssignNew(BottomPanelContainer, SBox)
                    ]
                ]
            ]
            // 右侧面板内容
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(RightPanelWidth)
                [
                    SNew(SVerticalBox)
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
