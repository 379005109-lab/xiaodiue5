// CameraViewController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraViewController.generated.h"

USTRUCT(BlueprintType)
struct FCategoryViewpoint
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CategoryName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FTransform> Viewpoints;
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
    TArray<FString> GetCategoryNames();

protected:
    virtual void BeginPlay() override;

private:
    void MoveToCurrentViewpoint();
};