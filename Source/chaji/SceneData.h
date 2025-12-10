// SceneData.h
#pragma once

#include "CoreMinimal.h"
#include "SceneData.generated.h"

// 场景数据结构
USTRUCT(BlueprintType)
struct FSceneData
{
    GENERATED_BODY()
    
    // 场景唯一ID
    UPROPERTY(BlueprintReadWrite)
    FString SceneId;
    
    // 场景名称
    UPROPERTY(BlueprintReadWrite)
    FString SceneName;
    
    // 场景描述
    UPROPERTY(BlueprintReadWrite)
    FString Description;
    
    // 缩略图URL或本地路径
    UPROPERTY(BlueprintReadWrite)
    FString ThumbnailPath;
    
    // UE关卡路径 (例如: /Game/Levels/LivingRoom)
    UPROPERTY(BlueprintReadWrite)
    FString LevelPath;
    
    // 场景分类标签
    UPROPERTY(BlueprintReadWrite)
    TArray<FString> Tags;
    
    // 产品类型 (衣柜, 室内门, 橱柜 等)
    UPROPERTY(BlueprintReadWrite)
    FString ProductType;
    
    // 风格 (现代, 意式, 极简 等)
    UPROPERTY(BlueprintReadWrite)
    FString Style;
    
    // 空间类型 (卧室, 厨房, 客厅 等)
    UPROPERTY(BlueprintReadWrite)
    FString SpaceType;
    
    // 是否已下载
    UPROPERTY(BlueprintReadWrite)
    bool bIsDownloaded = false;
    
    // 创建时间
    UPROPERTY(BlueprintReadWrite)
    FDateTime CreateTime;
    
    FSceneData()
    {
        CreateTime = FDateTime::Now();
    }
};

// 场景筛选条件
USTRUCT(BlueprintType)
struct FSceneFilter
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite)
    FString ProductType;
    
    UPROPERTY(BlueprintReadWrite)
    FString Style;
    
    UPROPERTY(BlueprintReadWrite)
    FString SpaceType;
    
    UPROPERTY(BlueprintReadWrite)
    FString SearchKeyword;
    
    UPROPERTY(BlueprintReadWrite)
    bool bShowDownloadedOnly = false;
};
