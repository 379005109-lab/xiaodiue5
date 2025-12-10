// SceneLauncherHUD.cpp
#include "SceneLauncherHUD.h"
#include "SceneManagerWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"

ASceneLauncherHUD::ASceneLauncherHUD()
{
}

void ASceneLauncherHUD::BeginPlay()
{
    Super::BeginPlay();
    
    SetupUI();
    LoadSceneList();
}

void ASceneLauncherHUD::SetupUI()
{
    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;
    
    // 创建场景管理器Widget
    SceneManager = CreateWidget<USceneManagerWidget>(PC, USceneManagerWidget::StaticClass());
    if (SceneManager)
    {
        SceneManager->AddToViewport(0);
        SceneManager->InitWidget();
        
        // 绑定场景选择事件
        SceneManager->OnSceneSelected.AddDynamic(this, &ASceneLauncherHUD::OnSceneSelected);
    }
    
    // 设置输入模式
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);
    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;
}

void ASceneLauncherHUD::LoadSceneList()
{
    // 首先尝试从本地加载
    LoadLocalScenes();
    
    // 如果没有场景，添加示例场景
    if (SceneList.Num() == 0)
    {
        AddDemoScenes();
        SaveScenesToLocal();
    }
    
    // 设置到Widget
    if (SceneManager)
    {
        SceneManager->SetScenes(SceneList);
    }
    
    // TODO: 从服务器获取更新的场景列表
    // 可以使用 FHttpModule 发送HTTP请求
}

void ASceneLauncherHUD::LoadLocalScenes()
{
    FString ConfigPath = FPaths::ProjectDir() / LocalSceneConfigPath;
    FString JsonString;
    
    if (FFileHelper::LoadFileToString(JsonString, *ConfigPath))
    {
        SceneList = ParseScenesFromJson(JsonString);
        UE_LOG(LogTemp, Log, TEXT("Loaded %d scenes from local config"), SceneList.Num());
    }
}

TArray<FSceneData> ASceneLauncherHUD::ParseScenesFromJson(const FString& JsonString)
{
    TArray<FSceneData> Result;
    
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    TSharedPtr<FJsonValue> RootValue;
    
    if (FJsonSerializer::Deserialize(Reader, RootValue) && RootValue.IsValid())
    {
        const TArray<TSharedPtr<FJsonValue>>* ScenesArray;
        if (RootValue->TryGetArray(ScenesArray))
        {
            for (const TSharedPtr<FJsonValue>& SceneValue : *ScenesArray)
            {
                const TSharedPtr<FJsonObject>* SceneObj;
                if (SceneValue->TryGetObject(SceneObj))
                {
                    FSceneData Scene;
                    (*SceneObj)->TryGetStringField(TEXT("id"), Scene.SceneId);
                    (*SceneObj)->TryGetStringField(TEXT("name"), Scene.SceneName);
                    (*SceneObj)->TryGetStringField(TEXT("description"), Scene.Description);
                    (*SceneObj)->TryGetStringField(TEXT("thumbnail"), Scene.ThumbnailPath);
                    (*SceneObj)->TryGetStringField(TEXT("levelPath"), Scene.LevelPath);
                    (*SceneObj)->TryGetStringField(TEXT("productType"), Scene.ProductType);
                    (*SceneObj)->TryGetStringField(TEXT("style"), Scene.Style);
                    (*SceneObj)->TryGetStringField(TEXT("spaceType"), Scene.SpaceType);
                    (*SceneObj)->TryGetBoolField(TEXT("isDownloaded"), Scene.bIsDownloaded);
                    
                    const TArray<TSharedPtr<FJsonValue>>* TagsArray;
                    if ((*SceneObj)->TryGetArrayField(TEXT("tags"), TagsArray))
                    {
                        for (const TSharedPtr<FJsonValue>& Tag : *TagsArray)
                        {
                            Scene.Tags.Add(Tag->AsString());
                        }
                    }
                    
                    Result.Add(Scene);
                }
            }
        }
    }
    
    return Result;
}

void ASceneLauncherHUD::SaveScenesToLocal()
{
    TArray<TSharedPtr<FJsonValue>> ScenesArray;
    
    for (const FSceneData& Scene : SceneList)
    {
        TSharedPtr<FJsonObject> SceneObj = MakeShareable(new FJsonObject);
        SceneObj->SetStringField(TEXT("id"), Scene.SceneId);
        SceneObj->SetStringField(TEXT("name"), Scene.SceneName);
        SceneObj->SetStringField(TEXT("description"), Scene.Description);
        SceneObj->SetStringField(TEXT("thumbnail"), Scene.ThumbnailPath);
        SceneObj->SetStringField(TEXT("levelPath"), Scene.LevelPath);
        SceneObj->SetStringField(TEXT("productType"), Scene.ProductType);
        SceneObj->SetStringField(TEXT("style"), Scene.Style);
        SceneObj->SetStringField(TEXT("spaceType"), Scene.SpaceType);
        SceneObj->SetBoolField(TEXT("isDownloaded"), Scene.bIsDownloaded);
        
        TArray<TSharedPtr<FJsonValue>> TagsArray;
        for (const FString& Tag : Scene.Tags)
        {
            TagsArray.Add(MakeShareable(new FJsonValueString(Tag)));
        }
        SceneObj->SetArrayField(TEXT("tags"), TagsArray);
        
        ScenesArray.Add(MakeShareable(new FJsonValueObject(SceneObj)));
    }
    
    TSharedRef<FJsonValueArray> RootValue = MakeShareable(new FJsonValueArray(ScenesArray));
    
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(ScenesArray, Writer);
    
    FString ConfigPath = FPaths::ProjectDir() / LocalSceneConfigPath;
    FFileHelper::SaveStringToFile(OutputString, *ConfigPath);
    
    UE_LOG(LogTemp, Log, TEXT("Saved %d scenes to local config"), SceneList.Num());
}

void ASceneLauncherHUD::AddDemoScenes()
{
    // 添加示例场景用于测试
    TArray<FString> DemoNames = {
        TEXT("现代卧室"), TEXT("板材拍摄"), TEXT("空白场景"), TEXT("意式客餐厅"),
        TEXT("摄影棚"), TEXT("中古风客餐厅-028"), TEXT("现代客餐厅"), TEXT("奶油风卧室")
    };
    
    TArray<FString> DemoStyles = {
        TEXT("现代"), TEXT("现代"), TEXT("现代"), TEXT("意式"),
        TEXT("现代"), TEXT("中古风"), TEXT("现代"), TEXT("奶油风")
    };
    
    TArray<FString> DemoSpaces = {
        TEXT("卧室"), TEXT("展厅"), TEXT("展厅"), TEXT("客厅"),
        TEXT("展厅"), TEXT("客厅"), TEXT("客厅"), TEXT("卧室")
    };
    
    for (int32 i = 0; i < DemoNames.Num(); i++)
    {
        FSceneData Scene;
        Scene.SceneId = FString::Printf(TEXT("scene_%d"), i + 1);
        Scene.SceneName = DemoNames[i];
        Scene.Description = FString::Printf(TEXT("%s 场景"), *DemoNames[i]);
        Scene.LevelPath = FString::Printf(TEXT("/Game/Levels/%s"), *DemoNames[i].Replace(TEXT(" "), TEXT("_")));
        Scene.ProductType = TEXT("样板间");
        Scene.Style = DemoStyles[i];
        Scene.SpaceType = DemoSpaces[i];
        Scene.bIsDownloaded = (i < 4); // 前4个标记为已下载
        
        SceneList.Add(Scene);
    }
}

void ASceneLauncherHUD::OnSceneSelected(const FSceneData& Scene)
{
    UE_LOG(LogTemp, Log, TEXT("Scene selected: %s, LevelPath: %s"), *Scene.SceneName, *Scene.LevelPath);
    
    // 检查关卡是否存在
    if (Scene.LevelPath.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Scene level path is empty"));
        return;
    }
    
    // 加载关卡
    LoadScene(Scene);
}

void ASceneLauncherHUD::LoadScene(const FSceneData& Scene)
{
    UWorld* World = GetWorld();
    if (!World) return;
    
    // 使用 OpenLevel 加载关卡
    UGameplayStatics::OpenLevel(World, FName(*Scene.LevelPath));
}

void ASceneLauncherHUD::ReturnToLauncher()
{
    // 返回到启动器关卡
    UWorld* World = GEngine->GetWorldContexts()[0].World();
    if (World)
    {
        UGameplayStatics::OpenLevel(World, FName(TEXT("/Game/Levels/Launcher")));
    }
}
