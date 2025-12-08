// ViewpointListWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/Reply.h"
#include "ViewpointListWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnViewpointSelected, int32, ViewpointIndex);

UCLASS()
class UViewpointListWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnViewpointSelected OnViewpointSelected;

    UFUNCTION(BlueprintCallable)
    void SetViewpoints(int32 Count);

    UFUNCTION(BlueprintCallable)
    void SetSelectedIndex(int32 Index);
    
    UFUNCTION(BlueprintCallable)
    void Show();
    
    UFUNCTION(BlueprintCallable)
    void Hide();

protected:
    virtual void NativeConstruct() override;
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    TSharedPtr<class SVerticalBox> ViewpointContainer;
    TSharedPtr<class SBorder> RootBorder;
    int32 ViewpointCount = 0;
    int32 SelectedIndex = 0;
    bool bIsVisible = false;

    void CreateViewpointButtons();
    FReply HandleViewpointClick(int32 Index);
};
