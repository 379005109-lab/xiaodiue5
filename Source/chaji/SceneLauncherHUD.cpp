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
    // 添加主场景 - 指向实际的 LEVEL 关卡
    FSceneData MainScene;
    MainScene.SceneId = TEXT("scene_main");
    MainScene.SceneName = TEXT("主场景");
    MainScene.Description = TEXT("诧寂主场景");
    MainScene.LevelPath = TEXT("/Game/LEVEL");
    MainScene.ProductType = TEXT("样板间");
    MainScene.Style = TEXT("诧寂");
    MainScene.SpaceType = TEXT("展示");
    MainScene.bIsDownloaded = true;
    SceneList.Add(MainScene);
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
