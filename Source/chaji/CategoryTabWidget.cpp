// CategoryTabWidget.cpp
#include "CategoryTabWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Styling/CoreStyle.h"

void UCategoryTabWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

TSharedRef<SWidget> UCategoryTabWidget::RebuildWidget()
{
    // 垂直布局 - 左侧面板
    TabContainer = SNew(SVerticalBox);
    
    return SNew(SBorder)
        .BorderBackgroundColor(FLinearColor(0.08f, 0.08f, 0.1f, 0.95f))
        .Padding(FMargin(0.0f))
        [
            SNew(SVerticalBox)
            // 标题栏
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor(0.12f, 0.12f, 0.15f, 1.0f))
                .Padding(FMargin(10.0f, 8.0f))
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("类别")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                        .ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f))
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew(SButton)
                        .ButtonColorAndOpacity(FLinearColor::Transparent)
                        .OnClicked_Lambda([this]() { return OnToggleClicked(); })
                        .ContentPadding(FMargin(4.0f, 0.0f))
                        [
                            SNew(STextBlock)
                            .Text_Lambda([this]() { return FText::FromString(bIsExpanded ? TEXT("▼") : TEXT("▶")); })
                            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                            .ColorAndOpacity(FLinearColor(0.6f, 0.6f, 0.6f))
                        ]
                    ]
                ]
            ]
            // 类别列表
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SAssignNew(ContentBox, SBox)
                [
                    SNew(SScrollBox)
                    + SScrollBox::Slot()
                    [
                        TabContainer.ToSharedRef()
                    ]
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
        FLinearColor ButtonColor = bIsSelected ? FLinearColor(0.15f, 0.4f, 0.6f, 1.0f) : FLinearColor(0.12f, 0.12f, 0.15f, 1.0f);
        FLinearColor TextColor = bIsSelected ? FLinearColor::White : FLinearColor(0.7f, 0.7f, 0.7f);
        
        TabContainer->AddSlot()
            .Padding(FMargin(0.0f, 2.0f))
            .AutoHeight()
            [
                SNew(SButton)
                .ButtonColorAndOpacity(ButtonColor)
                .OnClicked_Lambda([this, i]() { return OnTabClicked(i); })
                .ContentPadding(FMargin(12.0f, 8.0f))
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(Categories[i]))
                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                    .ColorAndOpacity(TextColor)
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
