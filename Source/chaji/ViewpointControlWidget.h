// ViewpointControlWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ViewpointControlWidget.generated.h"

class AViewpointPreviewManager;
class UTextureRenderTarget2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnViewpointChanged, int32, ViewpointIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnViewpointAdded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnViewpointRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBatchCapture, const TArray<int32>&, SelectedIndices);

UCLASS()
class UViewpointControlWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnViewpointChanged OnViewpointChanged;
    
    UPROPERTY(BlueprintAssignable)
    FOnViewpointAdded OnViewpointAdded;
    
    UPROPERTY(BlueprintAssignable)
    FOnViewpointRemoved OnViewpointRemoved;
    
    UPROPERTY(BlueprintAssignable)
    FOnBatchCapture OnBatchCapture;

    UFUNCTION(BlueprintCallable)
    void SetViewpointCount(int32 Count);

    UFUNCTION(BlueprintCallable)
    void SetCurrentViewpoint(int32 Index);
    
    UFUNCTION(BlueprintCallable)
    void SetPreviewManager(AViewpointPreviewManager* Manager);
    
    // Get selected viewpoints for batch capture
    TArray<int32> GetSelectedViewpoints() const;

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    TSharedPtr<class SHorizontalBox> ThumbnailContainer;
    TSharedPtr<class SBox> ContentBox;
    
    UPROPERTY()
    AViewpointPreviewManager* PreviewManager;
    
    int32 ViewpointCount = 0;
    int32 CurrentViewpoint = 0;
    bool bIsExpanded = true;
    
    // Checkbox selections
    TArray<bool> ViewpointSelected;

    void RebuildThumbnails();
    void UpdateVisibility();
    FReply OnThumbnailClicked(int32 Index);
    FReply OnToggleClicked();
    FReply OnAddViewpoint();
    FReply OnRemoveViewpoint();
    void OnCheckboxChanged(int32 Index, bool bChecked);
};
