// ViewpointListWidget.cpp
#include "ViewpointListWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"

void UViewpointListWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

TSharedRef<SWidget> UViewpointListWidget::RebuildWidget()
{
    ViewpointContainer = SNew(SVerticalBox);
    
    return SNew(SBorder)
        .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.9f))
        .Padding(FMargin(10.0f))
        .Visibility(bIsVisible ? EVisibility::Visible : EVisibility::Collapsed)
        [
            SNew(SScrollBox)
            .Orientation(Orient_Vertical)
            + SScrollBox::Slot()
            [
                ViewpointContainer.ToSharedRef()
            ]
        ];
}

void UViewpointListWidget::SetViewpoints(int32 Count)
{
    ViewpointCount = Count;
    SelectedIndex = 0;
    CreateViewpointButtons();
}

void UViewpointListWidget::SetSelectedIndex(int32 Index)
{
    SelectedIndex = Index;
    CreateViewpointButtons();
}

void UViewpointListWidget::Show()
{
    bIsVisible = true;
    SetVisibility(ESlateVisibility::Visible);
}

void UViewpointListWidget::Hide()
{
    bIsVisible = false;
    SetVisibility(ESlateVisibility::Collapsed);
}

void UViewpointListWidget::CreateViewpointButtons()
{
    if (!ViewpointContainer.IsValid()) return;
    
    ViewpointContainer->ClearChildren();
    
    for (int32 i = 0; i < ViewpointCount; i++)
    {
        bool bIsSelected = (i == SelectedIndex);
        FLinearColor ButtonColor = bIsSelected 
            ? FLinearColor(0.2f, 0.5f, 0.8f, 1.0f) 
            : FLinearColor(0.4f, 0.4f, 0.4f, 1.0f);
        
        FString ButtonText = FString::Printf(TEXT("镜头 %d"), i + 1);
        
        ViewpointContainer->AddSlot()
            .Padding(FMargin(0.0f, 5.0f))
            .AutoHeight()
            [
                SNew(SButton)
                .ButtonColorAndOpacity(ButtonColor)
                .OnClicked_Lambda([this, i]() { return OnViewpointClicked(i); })
                .ContentPadding(FMargin(15.0f, 8.0f))
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(ButtonText))
                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
                    .ColorAndOpacity(FLinearColor::White)
                    .Justification(ETextJustify::Center)
                ]
            ];
    }
}

FReply UViewpointListWidget::OnViewpointClicked(int32 Index)
{
    SelectedIndex = Index;
    CreateViewpointButtons();
    OnViewpointSelected.Broadcast(Index);
    return FReply::Handled();
}
