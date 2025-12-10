// ViewerPlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ViewerPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMouseWheelScroll, float, Delta);

UCLASS()
class AViewerPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AViewerPlayerController();
    
    UPROPERTY(BlueprintAssignable)
    FOnMouseWheelScroll OnMouseWheelScroll;
    
    float LastScrollDelta = 0.0f;

protected:
    virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;
    
    void OnScrollUp();
    void OnScrollDown();
    
public:
    // 设置视口区域 (0-1范围)
    UFUNCTION(BlueprintCallable)
    void SetViewportRegion(float OriginX, float OriginY, float SizeX, float SizeY);
    
    // 应用视口分离
    void ApplyViewportSeparation();
};