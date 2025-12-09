// VideoClipData.h
#pragma once

#include "CoreMinimal.h"
#include "VideoClipData.generated.h"

// 视频片段帧数据
USTRUCT(BlueprintType)
struct FVideoFrameData
{
    GENERATED_BODY()
    
    UPROPERTY()
    FVector Location = FVector::ZeroVector;
    
    UPROPERTY()
    FRotator Rotation = FRotator::ZeroRotator;
    
    UPROPERTY()
    float FocalLength = 35.0f;
    
    UPROPERTY()
    float Aperture = 2.8f;
    
    UPROPERTY()
    float FocusDistance = 1000.0f;
    
    UPROPERTY()
    UTexture2D* Thumbnail = nullptr;
    
    bool bHasData = false;
};

// 视频片段数据
USTRUCT(BlueprintType)
struct FVideoClipData
{
    GENERATED_BODY()
    
    UPROPERTY()
    FVideoFrameData StartFrame;
    
    UPROPERTY()
    FVideoFrameData EndFrame;
    
    UPROPERTY()
    float Duration = 10.0f; // 秒数
    
    bool bStartSet = false;
    bool bEndSet = false;
};
