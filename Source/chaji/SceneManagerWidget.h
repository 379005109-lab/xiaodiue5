// SceneManagerWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SceneData.h"
#include "SceneManagerWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneSelected, const FSceneData&, Scene);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefreshScenes);

class SScrollBox;
class SWrapBox;
class SEditableTextBox;

UCLASS()
class USceneManagerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void InitWidget();
    
    // 设置场景列表
    UFUNCTION(BlueprintCallable)
    void SetScenes(const TArray<FSceneData>& InScenes);
    
    // 添加场景
    UFUNCTION(BlueprintCallable)
    void AddScene(const FSceneData& Scene);
    
    // 应用筛选
    UFUNCTION(BlueprintCallable)
    void ApplyFilter(const FSceneFilter& Filter);
    
    // 刷新显示
    UFUNCTION(BlueprintCallable)
    void RefreshDisplay();
    
    // 事件
    UPROPERTY(BlueprintAssignable)
    FOnSceneSelected OnSceneSelected;
    
    UPROPERTY(BlueprintAssignable)
    FOnRefreshScenes OnRefreshScenes;

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    // 所有场景
    TArray<FSceneData> AllScenes;
    
    // 筛选后的场景
    TArray<FSceneData> FilteredScenes;
    
    // 当前筛选条件
    FSceneFilter CurrentFilter;
    
    // UI组件
    TSharedPtr<SScrollBox> SceneScrollBox;
    TSharedPtr<SWrapBox> SceneGrid;
    TSharedPtr<SEditableTextBox> SearchBox;
    TSharedPtr<class STextBlock> StatusText;
    
    // 分类选项
    TArray<FString> ProductTypes;
    TArray<FString> Styles;
    TArray<FString> SpaceTypes;
    
    int32 SelectedProductIndex = -1;
    int32 SelectedStyleIndex = -1;
    int32 SelectedSpaceIndex = -1;
    
    // 分页
    int32 CurrentPage = 0;
    int32 ItemsPerPage = 12;
    
    // 创建场景卡片
    TSharedRef<SWidget> CreateSceneCard(const FSceneData& Scene);
    
    // 创建筛选按钮行
    TSharedRef<SWidget> CreateFilterRow(const FString& Label, const TArray<FString>& Options, int32& SelectedIndex);
    
    // 回调
    void OnSearchTextChanged(const FText& Text);
    void OnProductFilterClicked(int32 Index);
    void OnStyleFilterClicked(int32 Index);
    void OnSpaceFilterClicked(int32 Index);
    void OnSceneCardClicked(const FSceneData& Scene);
    void OnPrevPageClicked();
    void OnNextPageClicked();
    
    // 重建场景网格
    void RebuildSceneGrid();
    
    // 初始化分类选项
    void InitFilterOptions();
};
