// ViewpointControlWidget.cpp
#include "ViewpointControlWidget.h"
#include "ViewpointPreviewManager.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"
#include "Styling/CoreStyle.h"
#include "Engine/TextureRenderTarget2D.h"

TSharedRef<SWidget> UViewpointControlWidget::RebuildWidget()
{
    return SNew(SHorizontalBox)
        // Toggle Button
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Bottom)
        [
            SNew(SButton)
            .ButtonColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.9f))
            .OnClicked_Lambda([this]() { return OnToggleClicked(); })
            .ContentPadding(FMargin(10.0f, 8.0f))
            [
                SNew(STextBlock)
                .Text_Lambda([this]() { return FText::FromString(bIsExpanded ? TEXT("▼") : TEXT("▲")); })
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                .ColorAndOpacity(FLinearColor::White)
            ]
        ]
        // Content
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SAssignNew(ContentBox, SBox)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor(0.05f, 0.05f, 0.05f, 0.85f))
                .Padding(FMargin(10.0f))
                [
                    SAssignNew(ThumbnailContainer, SHorizontalBox)
                ]
            ]
        ];
}

void UViewpointControlWidget::SetViewpointCount(int32 Count)
{
    ViewpointCount = Count;
    CurrentViewpoint = 0;
    RebuildThumbnails();
}

void UViewpointControlWidget::SetCurrentViewpoint(int32 Index)
{
    CurrentViewpoint = Index;
    RebuildThumbnails();
}

void UViewpointControlWidget::SetPreviewManager(AViewpointPreviewManager* Manager)
{
    PreviewManager = Manager;
    RebuildThumbnails();
}

void UViewpointControlWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
}

void UViewpointControlWidget::RebuildThumbnails()
{
    if (!ThumbnailContainer.IsValid()) return;
    
    ThumbnailContainer->ClearChildren();
    
    for (int32 i = 0; i < ViewpointCount; i++)
    {
        bool bIsSelected = (i == CurrentViewpoint);
        FLinearColor BorderColor = bIsSelected ? FLinearColor(0.0f, 0.7f, 1.0f, 1.0f) : FLinearColor(0.4f, 0.4f, 0.4f, 1.0f);
        FLinearColor BgColor = bIsSelected ? FLinearColor(0.1f, 0.15f, 0.2f, 1.0f) : FLinearColor(0.08f, 0.08f, 0.08f, 1.0f);
        float BorderSize = bIsSelected ? 3.0f : 2.0f;
        
        ThumbnailContainer->AddSlot()
            .AutoWidth()
            .Padding(FMargin(4.0f))
            [
                SNew(SButton)
                .ButtonColorAndOpacity(FLinearColor::Transparent)
                .OnClicked_Lambda([this, i]() { return OnThumbnailClicked(i); })
                .ContentPadding(FMargin(0.0f))
                [
                    SNew(SBorder)
                    .BorderImage(FCoreStyle::Get().GetBrush("Border"))
                    .BorderBackgroundColor(BorderColor)
                    .Padding(FMargin(BorderSize))
                    [
                        SNew(SBox)
                        .WidthOverride(140.0f)
                        .HeightOverride(80.0f)
                        [
                            SNew(SOverlay)
                            // Background
                            + SOverlay::Slot()
                            [
                                SNew(SBorder)
                                .BorderBackgroundColor(BgColor)
                            ]
                            // Viewpoint number (large, centered)
                            + SOverlay::Slot()
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Center)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(FString::Printf(TEXT("%d"), i + 1)))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 28))
                                .ColorAndOpacity(bIsSelected ? FLinearColor(0.0f, 0.8f, 1.0f) : FLinearColor(0.6f, 0.6f, 0.6f))
                            ]
                            // Label at bottom
                            + SOverlay::Slot()
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Bottom)
                            .Padding(FMargin(0.0f, 0.0f, 0.0f, 6.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("镜头")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                                .ColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f))
                            ]
                        ]
                    ]
                ]
            ];
    }
}

FReply UViewpointControlWidget::OnThumbnailClicked(int32 Index)
{
    if (Index >= 0 && Index < ViewpointCount)
    {
        CurrentViewpoint = Index;
        RebuildThumbnails();
        OnViewpointChanged.Broadcast(Index);
    }
    return FReply::Handled();
}

FReply UViewpointControlWidget::OnToggleClicked()
{
    bIsExpanded = !bIsExpanded;
    UpdateVisibility();
    return FReply::Handled();
}

void UViewpointControlWidget::UpdateVisibility()
{
    if (ContentBox.IsValid())
    {
        ContentBox->SetVisibility(bIsExpanded ? EVisibility::Visible : EVisibility::Collapsed);
    }
}
