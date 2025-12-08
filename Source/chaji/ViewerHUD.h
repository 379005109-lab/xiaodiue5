// ViewerHUD.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CategoryTabWidget.h"
#include "CameraViewController.h"
#include "ViewerHUD.generated.h"

UCLASS()
class AViewerHUD : public AHUD
{
    GENERATED_BODY()

public:
    AViewerHUD();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UCategoryTabWidget* TabWidget;

    UPROPERTY()
    ACameraViewController* CameraController;

    void SetupUI();

    UFUNCTION()
    void OnCategorySelected(int32 Index);
};
