// ViewpointControlWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ViewpointControlWidget.generated.h"

class AViewpointPreviewManager;
class UTextureRenderTarget2D;

// Viewpoint camera data structure
USTRUCT(BlueprintType)
struct FViewpointData
{
    GENERATED_BODY()
    
    FVector Location = FVector::ZeroVector;
    FRotator Rotation = FRotator::ZeroRotator;
    float FocalLength = 35.0f;
    float Aperture = 2.8f;
    float FocusDistance = 1000.0f;
    bool bHasData = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnViewpointChanged, int32, ViewpointIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnViewpointAdded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnViewpointRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnViewpointSaved, int32, ViewpointIndex);

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
    FOnViewpointSaved OnViewpointSaved;

    UFUNCTION(BlueprintCallable)
    void SetViewpointCount(int32 Count);

    UFUNCTION(BlueprintCallable)
    void SetCurrentViewpoint(int32 Index);
    
    UFUNCTION(BlueprintCallable)
    void SetPreviewManager(AViewpointPreviewManager* Manager);
    
    // Get selected viewpoints for batch capture
    TArray<int32> GetSelectedViewpoints() const;
    
    // Viewpoint data management
    void SaveViewpointData(int32 Index, const FViewpointData& Data);
    FViewpointData GetViewpointData(int32 Index) const;
    bool HasViewpointData(int32 Index) const;

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
    
    // Viewpoint saved data
    TArray<FViewpointData> ViewpointDataArray;

    void RebuildThumbnails();
    void UpdateVisibility();
    FReply OnThumbnailClicked(int32 Index);
    FReply OnToggleClicked();
    FReply OnAddViewpoint();
    FReply OnRemoveViewpoint();
    FReply OnSaveViewpoint(int32 Index);
    FReply OnSelectAll();
    void OnCheckboxChanged(int32 Index, bool bChecked);
};
