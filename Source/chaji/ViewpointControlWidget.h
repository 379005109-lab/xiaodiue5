// ViewpointControlWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ViewpointControlWidget.generated.h"

class AViewpointPreviewManager;
class UTextureRenderTarget2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnViewpointChanged, int32, ViewpointIndex);

UCLASS()
class UViewpointControlWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnViewpointChanged OnViewpointChanged;

    UFUNCTION(BlueprintCallable)
    void SetViewpointCount(int32 Count);

    UFUNCTION(BlueprintCallable)
    void SetCurrentViewpoint(int32 Index);
    
    UFUNCTION(BlueprintCallable)
    void SetPreviewManager(AViewpointPreviewManager* Manager);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    TSharedPtr<class SHorizontalBox> ThumbnailContainer;
    TSharedPtr<class SBox> ContentBox;
    TArray<TSharedPtr<class SImage>> PreviewImages;
    TArray<FSlateBrush> PreviewBrushes;
    
    UPROPERTY()
    AViewpointPreviewManager* PreviewManager;
    
    int32 ViewpointCount = 0;
    int32 CurrentViewpoint = 0;
    bool bIsExpanded = true;

    void RebuildThumbnails();
    void UpdatePreviews();
    void UpdateVisibility();
    FReply OnThumbnailClicked(int32 Index);
    FReply OnToggleClicked();
};
