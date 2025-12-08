#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VSLoadingWidget.generated.h"

class UTextBlock;
class UThrobber;

/**
 * 加载界面 Widget
 */
UCLASS()
class VIRTUALSPACECLIENT_API UVSLoadingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "VirtualSpace|UI")
	void SetLoadingText(const FString& Text);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	TSharedPtr<STextBlock> LoadingTextBlock;
};
