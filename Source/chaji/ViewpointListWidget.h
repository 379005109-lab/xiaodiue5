// ViewpointListWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
    void SelectViewpoint(int32 Index);
    
    UFUNCTION(BlueprintCallable)
    void Show();
    
    UFUNCTION(BlueprintCallable)
    void Hide();
    
    UFUNCTION(BlueprintCallable)
    int32 GetViewpointCount() const { return ViewpointCount; }

protected:
    virtual void NativeConstruct() override;
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    TSharedPtr<class SVerticalBox> ViewpointContainer;
    int32 ViewpointCount = 0;
    int32 SelectedIndex = 0;

    void CreateViewpointButtons();
};
