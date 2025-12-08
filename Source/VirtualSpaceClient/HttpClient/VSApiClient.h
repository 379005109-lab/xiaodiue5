#pragma once

#include "CoreMinimal.h"
#include "VSApiTypes.h"
#include "VSApiClient.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConfigReceived, bool, bSuccess, const FVSClientConfig&, Config);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginComplete, bool, bSuccess, const FString&, Message);

/**
 * API 客户端 - 处理与后台的 HTTP 通信
 */
UCLASS(BlueprintType, Blueprintable)
class VIRTUALSPACECLIENT_API UVSApiClient : public UObject
{
	GENERATED_BODY()

public:
	UVSApiClient();

	// 设置 API 基础地址
	UFUNCTION(BlueprintCallable, Category = "VirtualSpace|API")
	void SetBaseUrl(const FString& Url);

	// 获取客户端配置（不需要登录）
	UFUNCTION(BlueprintCallable, Category = "VirtualSpace|API")
	void FetchClientConfig();

	// 登录
	UFUNCTION(BlueprintCallable, Category = "VirtualSpace|API")
	void Login(const FString& Username, const FString& Password);

	// 获取配置响应事件
	UPROPERTY(BlueprintAssignable, Category = "VirtualSpace|API")
	FOnConfigReceived OnConfigReceived;

	// 登录完成事件
	UPROPERTY(BlueprintAssignable, Category = "VirtualSpace|API")
	FOnLoginComplete OnLoginComplete;

	// 获取当前配置
	UFUNCTION(BlueprintPure, Category = "VirtualSpace|API")
	const FVSClientConfig& GetClientConfig() const { return ClientConfig; }

	// 是否已登录
	UFUNCTION(BlueprintPure, Category = "VirtualSpace|API")
	bool IsLoggedIn() const { return !AuthToken.IsEmpty(); }

private:
	void OnConfigResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FString BaseUrl;
	FString AuthToken;
	FVSClientConfig ClientConfig;
};
