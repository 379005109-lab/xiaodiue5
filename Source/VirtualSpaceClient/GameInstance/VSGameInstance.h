#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HttpClient/VSApiClient.h"
#include "VSGameInstance.generated.h"

/**
 * 游戏实例 - 管理全局状态和 API 客户端
 */
UCLASS()
class VIRTUALSPACECLIENT_API UVSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UVSGameInstance();

	virtual void Init() override;

	// 获取 API 客户端
	UFUNCTION(BlueprintPure, Category = "VirtualSpace")
	UVSApiClient* GetApiClient() const { return ApiClient; }

	// 获取当前选中的关卡
	UFUNCTION(BlueprintPure, Category = "VirtualSpace")
	const FVSLevel& GetCurrentLevel() const { return CurrentLevel; }

	// 设置当前关卡
	UFUNCTION(BlueprintCallable, Category = "VirtualSpace")
	void SetCurrentLevel(const FVSLevel& Level);

	// API 基础地址（可在项目设置中配置）
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VirtualSpace|Config")
	FString ApiBaseUrl = TEXT("http://localhost:4000");

private:
	UPROPERTY()
	UVSApiClient* ApiClient;

	FVSLevel CurrentLevel;
};
