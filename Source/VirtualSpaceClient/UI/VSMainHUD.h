#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HttpClient/VSApiTypes.h"
#include "VSMainHUD.generated.h"

class UUserWidget;
class UVSApiClient;
class UVSLoadingWidget;
class UVSLoginWidget;
class UVSLevelSelectWidget;

/**
 * 主 HUD - 控制 UI 显示逻辑
 */
UCLASS()
class VIRTUALSPACECLIENT_API AVSMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	AVSMainHUD();

	virtual void BeginPlay() override;

	// 显示加载界面
	UFUNCTION(BlueprintCallable, Category = "VirtualSpace|UI")
	void ShowLoading();

	// 显示登录界面
	UFUNCTION(BlueprintCallable, Category = "VirtualSpace|UI")
	void ShowLogin();

	// 显示关卡选择界面
	UFUNCTION(BlueprintCallable, Category = "VirtualSpace|UI")
	void ShowLevelSelect();

	// 隐藏所有 UI
	UFUNCTION(BlueprintCallable, Category = "VirtualSpace|UI")
	void HideAllUI();

private:
	UFUNCTION()
	void OnConfigReceived(bool bSuccess, const FVSClientConfig& Config);

	UFUNCTION()
	void OnLoginRequested(const FString& Username, const FString& Password);

	UFUNCTION()
	void OnLoginComplete(bool bSuccess, const FString& Message);

	UFUNCTION()
	void OnLevelSelected(const FVSLevel& Level);

	void SetupInputMode();

	UPROPERTY()
	UVSLoadingWidget* LoadingWidget;

	UPROPERTY()
	UVSLoginWidget* LoginWidget;

	UPROPERTY()
	UVSLevelSelectWidget* LevelSelectWidget;

	UPROPERTY()
	UVSApiClient* ApiClient;

	FVSClientConfig CachedConfig;
};
