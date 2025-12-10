// SceneManagerWidget.cpp
#include "SceneManagerWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Styling/CoreStyle.h"
#include "Brushes/SlateColorBrush.h"

void USceneManagerWidget::InitWidget()
{
    InitFilterOptions();
    RefreshDisplay();
}

void USceneManagerWidget::InitFilterOptions()
{
    // 产品类型
    ProductTypes = {TEXT("全部"), TEXT("衣柜"), TEXT("室内门"), TEXT("橱柜"), TEXT("浴室柜"), TEXT("软体活动家具"), TEXT("桌类"), TEXT("几类"), TEXT("样板间")};
    
    // 风格
    Styles = {TEXT("全部"), TEXT("现代"), TEXT("意式"), TEXT("极简"), TEXT("轻奢"), TEXT("侘寂"), TEXT("中古风"), TEXT("奶油风")};
    
    // 空间类型
    SpaceTypes = {TEXT("全部"), TEXT("卧室"), TEXT("厨房"), TEXT("衣帽间"), TEXT("客厅"), TEXT("卫浴"), TEXT("展厅"), TEXT("CMF数字展厅"), TEXT("虚拟拍摄空间"), TEXT("书房"), TEXT("餐厅")};
}

void USceneManagerWidget::SetScenes(const TArray<FSceneData>& InScenes)
{
    AllScenes = InScenes;
    ApplyFilter(CurrentFilter);
}

void USceneManagerWidget::AddScene(const FSceneData& Scene)
{
    AllScenes.Add(Scene);
    ApplyFilter(CurrentFilter);
}

void USceneManagerWidget::ApplyFilter(const FSceneFilter& Filter)
{
    CurrentFilter = Filter;
    FilteredScenes.Empty();
    
    for (const FSceneData& Scene : AllScenes)
    {
        bool bMatch = true;
        
        // 产品类型筛选
        if (!Filter.ProductType.IsEmpty() && Filter.ProductType != TEXT("全部"))
        {
            if (Scene.ProductType != Filter.ProductType)
                bMatch = false;
        }
        
        // 风格筛选
        if (!Filter.Style.IsEmpty() && Filter.Style != TEXT("全部"))
        {
            if (Scene.Style != Filter.Style)
                bMatch = false;
        }
        
        // 空间类型筛选
        if (!Filter.SpaceType.IsEmpty() && Filter.SpaceType != TEXT("全部"))
        {
            if (Scene.SpaceType != Filter.SpaceType)
                bMatch = false;
        }
        
        // 关键词搜索
        if (!Filter.SearchKeyword.IsEmpty())
        {
            if (!Scene.SceneName.Contains(Filter.SearchKeyword) && 
                !Scene.Description.Contains(Filter.SearchKeyword))
                bMatch = false;
        }
        
        // 已下载筛选
        if (Filter.bShowDownloadedOnly && !Scene.bIsDownloaded)
            bMatch = false;
        
        if (bMatch)
        {
            FilteredScenes.Add(Scene);
        }
    }
    
    CurrentPage = 0;
    RebuildSceneGrid();
}

void USceneManagerWidget::RefreshDisplay()
{
    ApplyFilter(CurrentFilter);
}

TSharedRef<SWidget> USceneManagerWidget::RebuildWidget()
{
    // 颜色定义
    FLinearColor BgColor(0.12f, 0.12f, 0.14f, 1.0f);
    FLinearColor HeaderBgColor(0.08f, 0.08f, 0.1f, 1.0f);
    FLinearColor FilterBgColor(0.95f, 0.95f, 0.97f, 1.0f);
    FLinearColor AccentColor(0.2f, 0.6f, 0.85f, 1.0f);
    FLinearColor TextDark(0.2f, 0.2f, 0.25f, 1.0f);
    
    return SNew(SBorder)
        .BorderBackgroundColor(FilterBgColor)
        .Padding(FMargin(0.0f))
        [
            SNew(SVerticalBox)
            // ===== 顶部导航栏 =====
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor::White)
                .Padding(FMargin(20.0f, 10.0f))
                [
                    SNew(SHorizontalBox)
                    // Logo
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("🏠 场景管理器")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
                        .ColorAndOpacity(AccentColor)
                    ]
                    // 导航按钮
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .HAlign(HAlign_Center)
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(20.0f, 0.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("首页")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                            .ColorAndOpacity(TextDark)
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(20.0f, 0.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("场景库")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                            .ColorAndOpacity(AccentColor)
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(20.0f, 0.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("用户中心")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                            .ColorAndOpacity(TextDark)
                        ]
                    ]
                    // 搜索框
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew(SBox)
                        .WidthOverride(200.0f)
                        [
                            SAssignNew(SearchBox, SEditableTextBox)
                            .HintText(FText::FromString(TEXT("请输入关键字")))
                            .OnTextChanged_Lambda([this](const FText& Text) { OnSearchTextChanged(Text); })
                        ]
                    ]
                ]
            ]
            // ===== 分类标签栏 =====
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor::White)
                .Padding(FMargin(20.0f, 8.0f))
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(FMargin(0.0f, 0.0f, 20.0f, 0.0f))
                    [
                        SNew(SButton)
                        .ButtonColorAndOpacity(AccentColor)
                        .ContentPadding(FMargin(15.0f, 5.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("公海库")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                            .ColorAndOpacity(FLinearColor::White)
                        ]
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(FMargin(0.0f, 0.0f, 20.0f, 0.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("企业库")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                        .ColorAndOpacity(TextDark)
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("方案库")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                        .ColorAndOpacity(TextDark)
                    ]
                ]
            ]
            // ===== 筛选区域 =====
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor::White)
                .Padding(FMargin(20.0f, 10.0f))
                [
                    SNew(SVerticalBox)
                    // 产品类型
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 3.0f))
                    [
                        CreateFilterRow(TEXT("产品:"), ProductTypes, SelectedProductIndex)
                    ]
                    // 风格
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 3.0f))
                    [
                        CreateFilterRow(TEXT("风格:"), Styles, SelectedStyleIndex)
                    ]
                    // 空间
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 3.0f))
                    [
                        CreateFilterRow(TEXT("空间:"), SpaceTypes, SelectedSpaceIndex)
                    ]
                ]
            ]
            // ===== 已下载筛选 =====
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBorder)
                .BorderBackgroundColor(FilterBgColor)
                .Padding(FMargin(20.0f, 8.0f))
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew(SButton)
                        .ButtonColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f, 1.0f))
                        .OnClicked_Lambda([this]() {
                            CurrentFilter.bShowDownloadedOnly = !CurrentFilter.bShowDownloadedOnly;
                            ApplyFilter(CurrentFilter);
                            return FReply::Handled();
                        })
                        .ContentPadding(FMargin(8.0f, 4.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("☑ 已下载")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                            .ColorAndOpacity(TextDark)
                        ]
                    ]
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .HAlign(HAlign_Right)
                    [
                        SAssignNew(StatusText, STextBlock)
                        .Text(FText::FromString(TEXT("")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                        .ColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f))
                    ]
                ]
            ]
            // ===== 场景网格 =====
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SAssignNew(SceneScrollBox, SScrollBox)
                + SScrollBox::Slot()
                [
                    SNew(SBox)
                    .Padding(FMargin(20.0f, 10.0f))
                    [
                        SAssignNew(SceneGrid, SWrapBox)
                        .UseAllottedSize(true)
                    ]
                ]
            ]
            // ===== 分页 =====
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBorder)
                .BorderBackgroundColor(FilterBgColor)
                .Padding(FMargin(20.0f, 10.0f))
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .HAlign(HAlign_Center)
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f, 1.0f))
                            .OnClicked_Lambda([this]() { OnPrevPageClicked(); return FReply::Handled(); })
                            .ContentPadding(FMargin(10.0f, 5.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("<")))
                                .ColorAndOpacity(TextDark)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(10.0f, 0.0f))
                        [
                            SNew(STextBlock)
                            .Text_Lambda([this]() { 
                                int32 TotalPages = FMath::Max(1, (FilteredScenes.Num() + ItemsPerPage - 1) / ItemsPerPage);
                                return FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentPage + 1, TotalPages));
                            })
                            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                            .ColorAndOpacity(AccentColor)
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f, 1.0f))
                            .OnClicked_Lambda([this]() { OnNextPageClicked(); return FReply::Handled(); })
                            .ContentPadding(FMargin(10.0f, 5.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT(">")))
                                .ColorAndOpacity(TextDark)
                            ]
                        ]
                    ]
                ]
            ]
        ];
}

TSharedRef<SWidget> USceneManagerWidget::CreateFilterRow(const FString& Label, const TArray<FString>& Options, int32& SelectedIndex)
{
    FLinearColor TextDark(0.2f, 0.2f, 0.25f, 1.0f);
    FLinearColor AccentColor(0.2f, 0.6f, 0.85f, 1.0f);
    
    TSharedRef<SHorizontalBox> Row = SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        .Padding(FMargin(0.0f, 0.0f, 10.0f, 0.0f))
        [
            SNew(STextBlock)
            .Text(FText::FromString(Label))
            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
            .ColorAndOpacity(TextDark)
        ];
    
    for (int32 i = 0; i < Options.Num(); i++)
    {
        Row->AddSlot()
        .AutoWidth()
        .Padding(FMargin(5.0f, 0.0f))
        [
            SNew(SButton)
            .ButtonColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f))
            .OnClicked_Lambda([this, i, &SelectedIndex, &Label]() {
                SelectedIndex = (SelectedIndex == i) ? -1 : i;
                
                // 更新筛选条件
                if (Label.Contains(TEXT("产品")))
                    OnProductFilterClicked(SelectedIndex);
                else if (Label.Contains(TEXT("风格")))
                    OnStyleFilterClicked(SelectedIndex);
                else if (Label.Contains(TEXT("空间")))
                    OnSpaceFilterClicked(SelectedIndex);
                    
                return FReply::Handled();
            })
            .ContentPadding(FMargin(8.0f, 4.0f))
            [
                SNew(STextBlock)
                .Text(FText::FromString(Options[i]))
                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                .ColorAndOpacity_Lambda([i, &SelectedIndex, AccentColor, TextDark]() {
                    return (SelectedIndex == i) ? AccentColor : TextDark;
                })
            ]
        ];
    }
    
    return Row;
}

TSharedRef<SWidget> USceneManagerWidget::CreateSceneCard(const FSceneData& Scene)
{
    FLinearColor CardBgColor(1.0f, 1.0f, 1.0f, 1.0f);
    FLinearColor TextDark(0.2f, 0.2f, 0.25f, 1.0f);
    FLinearColor OverlayColor(0.0f, 0.0f, 0.0f, 0.5f);
    
    return SNew(SBox)
        .WidthOverride(220.0f)
        .Padding(FMargin(5.0f))
        [
            SNew(SButton)
            .ButtonColorAndOpacity(CardBgColor)
            .OnClicked_Lambda([this, Scene]() {
                OnSceneCardClicked(Scene);
                return FReply::Handled();
            })
            .ContentPadding(FMargin(0.0f))
            [
                SNew(SVerticalBox)
                // 缩略图区域
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SBox)
                    .HeightOverride(140.0f)
                    [
                        SNew(SOverlay)
                        // 占位背景
                        + SOverlay::Slot()
                        [
                            SNew(SBorder)
                            .BorderBackgroundColor(FLinearColor(0.3f, 0.3f, 0.35f, 1.0f))
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Center)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("🖼")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 24))
                                .ColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f))
                            ]
                        ]
                        // 已下载标记
                        + SOverlay::Slot()
                        .HAlign(HAlign_Left)
                        .VAlign(VAlign_Top)
                        [
                            Scene.bIsDownloaded ?
                            SNew(SBorder)
                            .BorderBackgroundColor(FLinearColor(0.2f, 0.7f, 0.3f, 0.9f))
                            .Padding(FMargin(5.0f, 2.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("✓")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                            : SNullWidget::NullWidget
                        ]
                    ]
                ]
                // 场景名称
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(FMargin(8.0f, 8.0f))
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(Scene.SceneName))
                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                    .ColorAndOpacity(TextDark)
                ]
            ]
        ];
}

void USceneManagerWidget::RebuildSceneGrid()
{
    if (!SceneGrid.IsValid()) return;
    
    SceneGrid->ClearChildren();
    
    // 计算当前页的场景
    int32 StartIndex = CurrentPage * ItemsPerPage;
    int32 EndIndex = FMath::Min(StartIndex + ItemsPerPage, FilteredScenes.Num());
    
    for (int32 i = StartIndex; i < EndIndex; i++)
    {
        SceneGrid->AddSlot()
        [
            CreateSceneCard(FilteredScenes[i])
        ];
    }
    
    // 更新状态文本
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(FString::Printf(TEXT("共 %d 个场景"), FilteredScenes.Num())));
    }
}

void USceneManagerWidget::OnSearchTextChanged(const FText& Text)
{
    CurrentFilter.SearchKeyword = Text.ToString();
    ApplyFilter(CurrentFilter);
}

void USceneManagerWidget::OnProductFilterClicked(int32 Index)
{
    if (Index >= 0 && Index < ProductTypes.Num())
        CurrentFilter.ProductType = ProductTypes[Index];
    else
        CurrentFilter.ProductType = TEXT("");
    ApplyFilter(CurrentFilter);
}

void USceneManagerWidget::OnStyleFilterClicked(int32 Index)
{
    if (Index >= 0 && Index < Styles.Num())
        CurrentFilter.Style = Styles[Index];
    else
        CurrentFilter.Style = TEXT("");
    ApplyFilter(CurrentFilter);
}

void USceneManagerWidget::OnSpaceFilterClicked(int32 Index)
{
    if (Index >= 0 && Index < SpaceTypes.Num())
        CurrentFilter.SpaceType = SpaceTypes[Index];
    else
        CurrentFilter.SpaceType = TEXT("");
    ApplyFilter(CurrentFilter);
}

void USceneManagerWidget::OnSceneCardClicked(const FSceneData& Scene)
{
    OnSceneSelected.Broadcast(Scene);
}

void USceneManagerWidget::OnPrevPageClicked()
{
    if (CurrentPage > 0)
    {
        CurrentPage--;
        RebuildSceneGrid();
    }
}

void USceneManagerWidget::OnNextPageClicked()
{
    int32 TotalPages = (FilteredScenes.Num() + ItemsPerPage - 1) / ItemsPerPage;
    if (CurrentPage < TotalPages - 1)
    {
        CurrentPage++;
        RebuildSceneGrid();
    }
}
