// ViewpointControlWidget.cpp
#include "ViewpointControlWidget.h"
#include "ViewpointPreviewManager.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
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
                    SNew(SHorizontalBox)
                    // Thumbnails
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SAssignNew(ThumbnailContainer, SHorizontalBox)
                    ]
                    // Add/Remove/SelectAll buttons
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    .Padding(FMargin(10.0f, 0.0f, 0.0f, 0.0f))
                    [
                        SNew(SVerticalBox)
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(FMargin(0.0f, 2.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.2f, 0.5f, 0.2f, 1.0f))
                            .OnClicked_Lambda([this]() { return OnAddViewpoint(); })
                            .ContentPadding(FMargin(12.0f, 6.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("+")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(FMargin(0.0f, 2.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.5f, 0.2f, 0.2f, 1.0f))
                            .OnClicked_Lambda([this]() { return OnRemoveViewpoint(); })
                            .ContentPadding(FMargin(12.0f, 6.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("-")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(FMargin(0.0f, 4.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.3f, 0.4f, 0.6f, 1.0f))
                            .OnClicked_Lambda([this]() { return OnSelectAll(); })
                            .ContentPadding(FMargin(8.0f, 6.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("全选")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                    ]
                ]
            ]
        ];
}

void UViewpointControlWidget::SetViewpointCount(int32 Count)
{
    ViewpointCount = Count;
    CurrentViewpoint = 0;
    
    // Initialize selection array
    ViewpointSelected.SetNum(Count);
    for (int32 i = 0; i < Count; i++)
    {
        ViewpointSelected[i] = false;
    }
    
    // Initialize viewpoint data array
    ViewpointDataArray.SetNum(Count);
    
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
    
    // Ensure selection array is sized correctly
    if (ViewpointSelected.Num() != ViewpointCount)
    {
        ViewpointSelected.SetNum(ViewpointCount);
    }
    
    for (int32 i = 0; i < ViewpointCount; i++)
    {
        bool bIsSelected = (i == CurrentViewpoint);
        bool bIsChecked = ViewpointSelected.IsValidIndex(i) ? ViewpointSelected[i] : false;
        FLinearColor BorderColor = bIsSelected ? FLinearColor(0.0f, 0.7f, 1.0f, 1.0f) : FLinearColor(0.4f, 0.4f, 0.4f, 1.0f);
        FLinearColor BgColor = bIsSelected ? FLinearColor(0.1f, 0.15f, 0.2f, 1.0f) : FLinearColor(0.15f, 0.15f, 0.15f, 1.0f);
        float BorderSize = bIsSelected ? 3.0f : 2.0f;
        
        int32 Index = i; // Capture for lambda
        
        ThumbnailContainer->AddSlot()
            .AutoWidth()
            .Padding(FMargin(4.0f))
            [
                SNew(SBox)
                .WidthOverride(140.0f)
                .HeightOverride(90.0f)
                [
                    SNew(SOverlay)
                    // Main button area
                    + SOverlay::Slot()
                    [
                        SNew(SButton)
                        .ButtonColorAndOpacity(FLinearColor::Transparent)
                        .OnClicked_Lambda([this, Index]() { return OnThumbnailClicked(Index); })
                        .ContentPadding(FMargin(0.0f))
                        [
                            SNew(SBorder)
                            .BorderImage(FCoreStyle::Get().GetBrush("Border"))
                            .BorderBackgroundColor(BorderColor)
                            .Padding(FMargin(BorderSize))
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
                                    .Text(FText::FromString(FString::Printf(TEXT("%d"), Index + 1)))
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
                    // Checkbox at top-left
                    + SOverlay::Slot()
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Top)
                    .Padding(FMargin(6.0f))
                    [
                        SNew(SCheckBox)
                        .IsChecked_Lambda([this, Index]() { 
                            return ViewpointSelected.IsValidIndex(Index) && ViewpointSelected[Index] ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; 
                        })
                        .OnCheckStateChanged_Lambda([this, Index](ECheckBoxState NewState) {
                            OnCheckboxChanged(Index, NewState == ECheckBoxState::Checked);
                        })
                    ]
                    // Save button at top-right
                    + SOverlay::Slot()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Top)
                    .Padding(FMargin(6.0f))
                    [
                        SNew(SButton)
                        .ButtonColorAndOpacity(FLinearColor(0.2f, 0.5f, 0.7f, 1.0f))
                        .OnClicked_Lambda([this, Index]() { return OnSaveViewpoint(Index); })
                        .ContentPadding(FMargin(4.0f, 2.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("保存")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                            .ColorAndOpacity(FLinearColor::White)
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

FReply UViewpointControlWidget::OnAddViewpoint()
{
    ViewpointCount++;
    ViewpointSelected.Add(false);
    RebuildThumbnails();
    OnViewpointAdded.Broadcast();
    return FReply::Handled();
}

FReply UViewpointControlWidget::OnRemoveViewpoint()
{
    if (ViewpointCount > 1)
    {
        ViewpointCount--;
        if (ViewpointSelected.Num() > 0)
        {
            ViewpointSelected.Pop();
        }
        if (CurrentViewpoint >= ViewpointCount)
        {
            CurrentViewpoint = ViewpointCount - 1;
        }
        RebuildThumbnails();
        OnViewpointRemoved.Broadcast();
    }
    return FReply::Handled();
}

void UViewpointControlWidget::OnCheckboxChanged(int32 Index, bool bChecked)
{
    if (ViewpointSelected.IsValidIndex(Index))
    {
        ViewpointSelected[Index] = bChecked;
    }
}

FReply UViewpointControlWidget::OnSaveViewpoint(int32 Index)
{
    OnViewpointSaved.Broadcast(Index);
    UE_LOG(LogTemp, Log, TEXT("Save viewpoint %d requested"), Index);
    return FReply::Handled();
}

FReply UViewpointControlWidget::OnSelectAll()
{
    // Toggle all - if any unselected, select all; otherwise deselect all
    bool bAnyUnselected = false;
    for (int32 i = 0; i < ViewpointSelected.Num(); i++)
    {
        if (!ViewpointSelected[i])
        {
            bAnyUnselected = true;
            break;
        }
    }
    
    for (int32 i = 0; i < ViewpointSelected.Num(); i++)
    {
        ViewpointSelected[i] = bAnyUnselected;
    }
    
    RebuildThumbnails();
    return FReply::Handled();
}

TArray<int32> UViewpointControlWidget::GetSelectedViewpoints() const
{
    TArray<int32> Selected;
    for (int32 i = 0; i < ViewpointSelected.Num(); i++)
    {
        if (ViewpointSelected[i])
        {
            Selected.Add(i);
        }
    }
    return Selected;
}

void UViewpointControlWidget::SaveViewpointData(int32 Index, const FViewpointData& Data)
{
    if (ViewpointDataArray.IsValidIndex(Index))
    {
        ViewpointDataArray[Index] = Data;
        ViewpointDataArray[Index].bHasData = true;
        RebuildThumbnails();
        UE_LOG(LogTemp, Log, TEXT("Viewpoint %d saved: Loc=%s Rot=%s Focal=%.0f"), 
            Index, *Data.Location.ToString(), *Data.Rotation.ToString(), Data.FocalLength);
    }
}

FViewpointData UViewpointControlWidget::GetViewpointData(int32 Index) const
{
    if (ViewpointDataArray.IsValidIndex(Index))
    {
        return ViewpointDataArray[Index];
    }
    return FViewpointData();
}

bool UViewpointControlWidget::HasViewpointData(int32 Index) const
{
    return ViewpointDataArray.IsValidIndex(Index) && ViewpointDataArray[Index].bHasData;
}
