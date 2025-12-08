#pragma once

#include "CoreMinimal.h"
#include "VSApiTypes.generated.h"

// 视角数据
USTRUCT(BlueprintType)
struct FVSViewpoint
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString Position;

	UPROPERTY(BlueprintReadOnly)
	FString Rotation;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDefault = false;
};

// 分类数据
USTRUCT(BlueprintType)
struct FVSCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 Id = 0;

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString Icon;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVSViewpoint> Viewpoints;
};

// 关卡数据
USTRUCT(BlueprintType)
struct FVSLevel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 Id = 0;

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString Description;

	UPROPERTY(BlueprintReadOnly)
	FString Thumbnail;

	UPROPERTY(BlueprintReadOnly)
	FString PakUrl;

	UPROPERTY(BlueprintReadOnly)
	FString PakVersion;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVSCategory> Categories;
};

// 热更资源
USTRUCT(BlueprintType)
struct FVSAsset
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 Id = 0;

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString Type;

	UPROPERTY(BlueprintReadOnly)
	FString FileUrl;

	UPROPERTY(BlueprintReadOnly)
	FString Version;

	UPROPERTY(BlueprintReadOnly)
	FString Md5;
};

// 客户端配置响应
USTRUCT(BlueprintType)
struct FVSClientConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bSkipLogin = false;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVSLevel> Levels;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVSAsset> Assets;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FString> SystemConfig;
};
