// CameraViewController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraViewController.generated.h"

// Camera settings structure - stores all CineCamera parameters
USTRUCT(BlueprintType)
struct FCameraSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform Transform;

    // Lens settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FocalLength = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Aperture = 2.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FocusDistance = 1000.0f;

    // Film back settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SensorWidth = 36.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SensorHeight = 24.0f;

    // Additional settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinFstop = 1.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BladeCount = 7;
};

USTRUCT(BlueprintType)
struct FCategoryViewpoint
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CategoryName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FTransform> Viewpoints;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FCameraSettings> CameraSettings;
};

UCLASS()
class ACameraViewController : public AActor
{
    GENERATED_BODY()

public:
    ACameraViewController();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Viewpoints")
    TArray<FCategoryViewpoint> Categories;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentCategoryIndex;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentViewpointIndex;

    UFUNCTION(BlueprintCallable)
    void SwitchCategory(int32 Index);

    UFUNCTION(BlueprintCallable)
    void NextViewpoint();

    UFUNCTION(BlueprintCallable)
    void SetViewpoint(int32 Index);

    UFUNCTION(BlueprintCallable)
    TArray<FString> GetCategoryNames();

    UFUNCTION(BlueprintCallable)
    void AutoConfigureFromCineCameras();

    UFUNCTION(BlueprintCallable)
    FCameraSettings GetCurrentCameraSettings() const;

protected:
    virtual void BeginPlay() override;

private:
    void MoveToCurrentViewpoint();
};