// ViewpointPreviewManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ViewpointPreviewManager.generated.h"

UCLASS()
class AViewpointPreviewManager : public AActor
{
    GENERATED_BODY()

public:
    AViewpointPreviewManager();

    // Initialize previews for given transforms
    UFUNCTION(BlueprintCallable)
    void SetupPreviews(const TArray<FTransform>& Transforms);

    // Get render target for a specific viewpoint
    UFUNCTION(BlueprintCallable)
    UTextureRenderTarget2D* GetPreviewTexture(int32 Index) const;

    // Update all preview captures
    UFUNCTION(BlueprintCallable)
    void UpdatePreviews();

    // Clear all previews
    UFUNCTION(BlueprintCallable)
    void ClearPreviews();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY()
    TArray<class USceneCaptureComponent2D*> CaptureComponents;

    UPROPERTY()
    TArray<UTextureRenderTarget2D*> RenderTargets;

    TArray<FTransform> ViewpointTransforms;
    
    float UpdateInterval = 0.5f; // Update every 0.5 seconds
    float TimeSinceLastUpdate = 0.0f;
    int32 CurrentUpdateIndex = 0;

    void CreateCaptureComponent(int32 Index, const FTransform& Transform);
};
