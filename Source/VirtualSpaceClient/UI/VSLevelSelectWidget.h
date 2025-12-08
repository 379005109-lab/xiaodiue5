#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HttpClient/VSApiTypes.h"
#include "VSLevelSelectWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelSelected, const FVSLevel&, Level);

/**
 * 关卡选择界面 Widget
 */
UCLASS()
class VIRTUALSPACECLIENT_API UVSLevelSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 设置关卡列表
	UFUNCTION(BlueprintCallable, Category = "VirtualSpace|UI")
	void SetLevels(const TArray<FVSLevel>& InLevels);

	// 关卡选中事件
	UPROPERTY(BlueprintAssignable, Category = "VirtualSpace|UI")
	FOnLevelSelected OnLevelSelected;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	void OnLevelButtonClicked(int32 LevelIndex);
	void RefreshLevelList();

	TArray<FVSLevel> Levels;
	TSharedPtr<SVerticalBox> LevelListBox;
};
