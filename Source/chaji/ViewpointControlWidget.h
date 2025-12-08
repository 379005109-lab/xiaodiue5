// ViewpointControlWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ViewpointControlWidget.generated.h"

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

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    TSharedPtr<class STextBlock> ViewpointText;
    int32 ViewpointCount = 0;
    int32 CurrentViewpoint = 0;

    void UpdateDisplay();
    FReply OnPrevClicked();
    FReply OnNextClicked();
};
