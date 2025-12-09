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
    
    void OnScrollUp();
    void OnScrollDown();
};