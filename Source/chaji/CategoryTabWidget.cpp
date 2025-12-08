// CategoryTabWidget.cpp
#include "CategoryTabWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Styling/CoreStyle.h"

void UCategoryTabWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

TSharedRef<SWidget> UCategoryTabWidget::RebuildWidget()
{
    TabContainer = SNew(SHorizontalBox);
    
    return SNew(SHorizontalBox)
        // Toggle Button
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.9f))
            .OnClicked_Lambda([this]() { return OnToggleClicked(); })
            .ContentPadding(FMargin(8.0f))
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("☰")))
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
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
                .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f))
                .Padding(FMargin(10.0f))
                [
                    TabContainer.ToSharedRef()
                ]
            ]
        ];
}

void UCategoryTabWidget::SetCategories(const TArray<FString>& CategoryNames)
{
    Categories = CategoryNames;
    CreateTabs();
}

void UCategoryTabWidget::SetSelectedIndex(int32 Index)
{
    SelectedIndex = Index;
    CreateTabs();
}

void UCategoryTabWidget::CreateTabs()
{
    if (!TabContainer.IsValid()) return;
    
    TabContainer->ClearChildren();
    
    for (int32 i = 0; i < Categories.Num(); i++)
    {
        bool bIsSelected = (i == SelectedIndex);
        FLinearColor ButtonColor = bIsSelected ? FLinearColor(0.2f, 0.5f, 0.8f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);
        
        TabContainer->AddSlot()
            .Padding(FMargin(5.0f, 0.0f))
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonColorAndOpacity(ButtonColor)
                .OnClicked_Lambda([this, i]() { return OnTabClicked(i); })
                .ContentPadding(FMargin(20.0f, 10.0f))
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(Categories[i]))
                    .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                    .ColorAndOpacity(FLinearColor::White)
                ]
            ];
    }
}

FReply UCategoryTabWidget::OnTabClicked(int32 Index)
{
    SelectedIndex = Index;
    CreateTabs();
    OnCategorySelected.Broadcast(Index);
    return FReply::Handled();
}

FReply UCategoryTabWidget::OnToggleClicked()
{
    bIsExpanded = !bIsExpanded;
    UpdateVisibility();
    return FReply::Handled();
}

void UCategoryTabWidget::UpdateVisibility()
{
    if (ContentBox.IsValid())
    {
        ContentBox->SetVisibility(bIsExpanded ? EVisibility::Visible : EVisibility::Collapsed);
    }
}
