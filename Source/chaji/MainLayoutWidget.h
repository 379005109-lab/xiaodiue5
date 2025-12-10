// MainLayoutWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainLayoutWidget.generated.h"

class UCategoryTabWidget;
class UParameterDisplayWidget;
class UMediaControlWidget;
class UViewpointControlWidget;

UCLASS()
class UMainLayoutWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 获取各个子面板的引用
    UCategoryTabWidget* GetCategoryTab() const { return CategoryTab; }
    UParameterDisplayWidget* GetParameterDisplay() const { return ParameterDisplay; }
    UMediaControlWidget* GetMediaControl() const { return MediaControl; }
    UViewpointControlWidget* GetViewpointControl() const { return ViewpointControl; }

    void InitSubWidgets(APlayerController* PC);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    // 子控件
    UPROPERTY()
    UCategoryTabWidget* CategoryTab;
    
    UPROPERTY()
    UParameterDisplayWidget* ParameterDisplay;
    
    UPROPERTY()
    UMediaControlWidget* MediaControl;
    
    UPROPERTY()
    UViewpointControlWidget* ViewpointControl;

    // Slate容器
    TSharedPtr<SBox> LeftPanelContainer;
    TSharedPtr<SBox> RightPanelContainer;
    TSharedPtr<SBox> BottomPanelContainer;
    TSharedPtr<SBox> ViewportContainer;
};
