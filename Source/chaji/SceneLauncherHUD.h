// SceneLauncherHUD.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SceneData.h"
#include "SceneLauncherHUD.generated.h"

class USceneManagerWidget;

UCLASS()
class ASceneLauncherHUD : public AHUD
{
    GENERATED_BODY()

public:
    ASceneLauncherHUD();
    
    virtual void BeginPlay() override;
    
    // 从服务器/本地加载场景列表
    UFUNCTION(BlueprintCallable)
    void LoadSceneList();
    
    // 加载指定场景
    UFUNCTION(BlueprintCallable)
    void LoadScene(const FSceneData& Scene);
    
    // 返回启动器
    UFUNCTION(BlueprintCallable)
    static void ReturnToLauncher();

protected:
    UPROPERTY()
    USceneManagerWidget* SceneManager;
    
    // 场景列表数据
    TArray<FSceneData> SceneList;
    
    // 场景API URL (用于从服务器获取场景列表)
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    FString SceneApiUrl = TEXT("http://localhost:8080/api/scenes");
    
    // 本地场景配置文件路径
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    FString LocalSceneConfigPath = TEXT("Config/Scenes.json");
    
private:
    void SetupUI();
    
    // 事件回调
    UFUNCTION()
    void OnSceneSelected(const FSceneData& Scene);
    
    // 加载本地场景配置
    void LoadLocalScenes();
    
    // 从JSON解析场景数据
    TArray<FSceneData> ParseScenesFromJson(const FString& JsonString);
    
    // 保存场景配置到本地
    void SaveScenesToLocal();
    
    // 添加示例场景（用于测试）
    void AddDemoScenes();
};
