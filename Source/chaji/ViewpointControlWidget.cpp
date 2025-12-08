// ViewpointControlWidget.cpp
#include "ViewpointControlWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Styling/CoreStyle.h"

TSharedRef<SWidget> UViewpointControlWidget::RebuildWidget()
{
    return SNew(SBorder)
        .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.9f))
        .Padding(FMargin(15.0f))
        [
            SNew(SHorizontalBox)
            // - Button
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(FMargin(0.0f, 0.0f, 10.0f, 0.0f))
            [
                SNew(SButton)
                .ButtonColorAndOpacity(FLinearColor(0.3f, 0.3f, 0.3f, 1.0f))
                .OnClicked_Lambda([this]() { return OnPrevClicked(); })
                .ContentPadding(FMargin(15.0f, 8.0f))
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("-")))
                    .Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
                    .ColorAndOpacity(FLinearColor::White)
                ]
            ]
            // Current Viewpoint Text
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(FMargin(10.0f, 0.0f))
            [
                SAssignNew(ViewpointText, STextBlock)
                .Text(FText::FromString(TEXT("镜头 1/1")))
                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
                .ColorAndOpacity(FLinearColor::White)
            ]
            // + Button
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(FMargin(10.0f, 0.0f, 0.0f, 0.0f))
            [
                SNew(SButton)
                .ButtonColorAndOpacity(FLinearColor(0.3f, 0.3f, 0.3f, 1.0f))
                .OnClicked_Lambda([this]() { return OnNextClicked(); })
                .ContentPadding(FMargin(15.0f, 8.0f))
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("+")))
                    .Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
                    .ColorAndOpacity(FLinearColor::White)
                ]
            ]
        ];
}

void UViewpointControlWidget::SetViewpointCount(int32 Count)
{
    ViewpointCount = Count;
    CurrentViewpoint = 0;
    UpdateDisplay();
}

void UViewpointControlWidget::SetCurrentViewpoint(int32 Index)
{
    CurrentViewpoint = Index;
    UpdateDisplay();
}

void UViewpointControlWidget::UpdateDisplay()
{
    if (ViewpointText.IsValid())
    {
        FString Text = FString::Printf(TEXT("镜头 %d/%d"), CurrentViewpoint + 1, ViewpointCount);
        ViewpointText->SetText(FText::FromString(Text));
    }
}

FReply UViewpointControlWidget::OnPrevClicked()
{
    if (ViewpointCount > 0)
    {
        CurrentViewpoint = (CurrentViewpoint - 1 + ViewpointCount) % ViewpointCount;
        UpdateDisplay();
        OnViewpointChanged.Broadcast(CurrentViewpoint);
    }
    return FReply::Handled();
}

FReply UViewpointControlWidget::OnNextClicked()
{
    if (ViewpointCount > 0)
    {
        CurrentViewpoint = (CurrentViewpoint + 1) % ViewpointCount;
        UpdateDisplay();
        OnViewpointChanged.Broadcast(CurrentViewpoint);
    }
    return FReply::Handled();
}
