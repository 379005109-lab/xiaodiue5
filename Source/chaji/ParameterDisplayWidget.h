// ParameterDisplayWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ParameterDisplayWidget.generated.h"

class UPhotoCaptureWidget;

UCLASS()
class UParameterDisplayWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetPhotoCaptureRef(UPhotoCaptureWidget* InRef) { PhotoCaptureRef = InRef; }

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    TSharedPtr<class STextBlock> DisplayText;
    
    UPROPERTY()
    UPhotoCaptureWidget* PhotoCaptureRef;
};
