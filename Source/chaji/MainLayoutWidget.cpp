// MainLayoutWidget.cpp
#include "MainLayoutWidget.h"
#include "CategoryTabWidget.h"
#include "ParameterDisplayWidget.h"
#include "MediaControlWidget.h"
#include "ViewpointControlWidget.h"
#include "Engine/TextureRenderTarget2D.h"
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
    // ============ 完全分离的布局 ============
    // 3D画面通过 SceneCapture 渲染到纹理，显示在中间区域
    // 左侧、右侧、底部是不透明的深色面板
    
    const float LeftPanelWidth = 180.0f;
    const float RightPanelWidth = 300.0f;
    const float BottomPanelHeight = 150.0f;
    
    // 深色背景画刷
    static FSlateColorBrush SolidBrush(FLinearColor(0.05f, 0.05f, 0.07f, 1.0f));
    FLinearColor HeaderBgColor = FLinearColor(0.1f, 0.1f, 0.12f, 1.0f);
    FLinearColor TextColor = FLinearColor(0.75f, 0.75f, 0.75f);
    
    // 中间区域保持透明，让3D画面显示
    // 使用不透明的边框面板来"遮挡"边缘的3D画面，实现视觉分离
    
    // 主布局
    return SNew(SBox)
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
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
                    .BorderBackgroundColor(FLinearColor(0.05f, 0.05f, 0.07f, 1.0f))
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
            ]
            // ===== 中间区域 (3D画面 + 底部控制) =====
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(SVerticalBox)
                // 3D画面区域 - 保持透明，让底层3D画面显示
                + SVerticalBox::Slot()
                .FillHeight(1.0f)
                [
                    SAssignNew(ViewportContainer, SBox)
                    // 透明区域，3D画面从这里显示
                ]
                // 底部面板
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SBox)
                    .HeightOverride(BottomPanelHeight)
                    [
                        SNew(SBorder)
                        .BorderBackgroundColor(FLinearColor(0.05f, 0.05f, 0.07f, 1.0f))
                        .Padding(FMargin(8.0f, 5.0f))
                        [
                            SAssignNew(BottomPanelContainer, SBox)
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
                    .BorderBackgroundColor(FLinearColor(0.05f, 0.05f, 0.07f, 1.0f))
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
            ]
        ];
}

void UMainLayoutWidget::SetRenderTarget(UTextureRenderTarget2D* InRenderTarget)
{
    // SceneCapture 功能已禁用，使用 UI 覆盖方案
    // 此方法保留以保持接口兼容性
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
