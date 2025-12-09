// CameraViewController.cpp
#include "CameraViewController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"

ACameraViewController::ACameraViewController()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentCategoryIndex = 0;
    CurrentViewpointIndex = 0;
}

void ACameraViewController::BeginPlay()
{
    Super::BeginPlay();
    
    if (Categories.Num() > 0)
    {
        MoveToCurrentViewpoint();
    }
}

void ACameraViewController::SwitchCategory(int32 Index)
{
    if (Index >= 0 && Index < Categories.Num())
    {
        CurrentCategoryIndex = Index;
        CurrentViewpointIndex = 0;
        MoveToCurrentViewpoint();
    }
}

void ACameraViewController::NextViewpoint()
{
    if (Categories.Num() == 0) return;
    
    const FCategoryViewpoint& Category = Categories[CurrentCategoryIndex];
    if (Category.Viewpoints.Num() == 0) return;
    
    CurrentViewpointIndex = (CurrentViewpointIndex + 1) % Category.Viewpoints.Num();
    MoveToCurrentViewpoint();
}

void ACameraViewController::SetViewpoint(int32 Index)
{
    if (Categories.Num() == 0) return;
    
    const FCategoryViewpoint& Category = Categories[CurrentCategoryIndex];
    if (Index >= 0 && Index < Category.Viewpoints.Num())
    {
        CurrentViewpointIndex = Index;
        MoveToCurrentViewpoint();
    }
}

TArray<FString> ACameraViewController::GetCategoryNames()
{
    TArray<FString> Names;
    for (const FCategoryViewpoint& Cat : Categories)
    {
        Names.Add(Cat.CategoryName);
    }
    return Names;
}

FCameraSettings ACameraViewController::GetCurrentCameraSettings() const
{
    if (Categories.IsValidIndex(CurrentCategoryIndex))
    {
        const FCategoryViewpoint& Category = Categories[CurrentCategoryIndex];
        if (Category.CameraSettings.IsValidIndex(CurrentViewpointIndex))
        {
            return Category.CameraSettings[CurrentViewpointIndex];
        }
    }
    return FCameraSettings();
}

void ACameraViewController::MoveToCurrentViewpoint()
{
    if (Categories.Num() == 0) return;
    
    const FCategoryViewpoint& Category = Categories[CurrentCategoryIndex];
    if (Category.Viewpoints.Num() == 0) return;
    
    const FTransform& Target = Category.Viewpoints[CurrentViewpointIndex];
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && PC->GetPawn())
    {
        PC->GetPawn()->SetActorLocation(Target.GetLocation());
        PC->SetControlRotation(Target.GetRotation().Rotator());
    }
}

void ACameraViewController::AutoConfigureFromCineCameras()
{
    // 主分类映射
    TMap<FString, TArray<FString>> CategoryMapping;
    CategoryMapping.Add(TEXT("沙发"), {TEXT("沙发")});
    CategoryMapping.Add(TEXT("椅子"), {TEXT("休闲椅1"), TEXT("休闲椅2"), TEXT("休闲椅3")});
    CategoryMapping.Add(TEXT("床"), {TEXT("大床")});
    CategoryMapping.Add(TEXT("茶几"), {TEXT("茶几1")});
    CategoryMapping.Add(TEXT("边几"), {TEXT("边几1"), TEXT("边几2"), TEXT("边几3")});
    CategoryMapping.Add(TEXT("床头柜"), {TEXT("床头柜1"), TEXT("床头柜2")});
    CategoryMapping.Add(TEXT("玄关柜"), {TEXT("玄关柜")});
    CategoryMapping.Add(TEXT("地板"), {TEXT("地板")});
    
    // 获取所有包含"镜头"的 Actor
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
    
    TArray<AActor*> CameraActors;
    for (AActor* Actor : AllActors)
    {
        if (Actor->GetActorLabel().Contains(TEXT("镜头")))
        {
            CameraActors.Add(Actor);
        }
    }
    
    // 临时存储 - 包含相机设置
    TMap<FString, TArray<FCameraSettings>> TempGroups;
    
    for (AActor* Actor : CameraActors)
    {
        FString Name = Actor->GetActorLabel();
        
        // 提取原始分类名称（例如：休闲椅1镜头1 -> 休闲椅1）
        int32 Index = Name.Find(TEXT("镜头"));
        if (Index != INDEX_NONE)
        {
            FString OriginalCategory = Name.Left(Index);
            
            if (!TempGroups.Contains(OriginalCategory))
            {
                TempGroups.Add(OriginalCategory, TArray<FCameraSettings>());
            }
            
            // 创建相机设置
            FCameraSettings Settings;
            Settings.Transform = Actor->GetActorTransform();
            
            // 尝试获取 CineCamera 参数
            ACineCameraActor* CineCamera = Cast<ACineCameraActor>(Actor);
            if (CineCamera)
            {
                UCineCameraComponent* CineCamComp = CineCamera->GetCineCameraComponent();
                if (CineCamComp)
                {
                    Settings.FocalLength = CineCamComp->CurrentFocalLength;
                    Settings.Aperture = CineCamComp->CurrentAperture;
                    Settings.FocusDistance = CineCamComp->FocusSettings.ManualFocusDistance;
                    Settings.SensorWidth = CineCamComp->Filmback.SensorWidth;
                    Settings.SensorHeight = CineCamComp->Filmback.SensorHeight;
                    Settings.MinFstop = CineCamComp->LensSettings.MinFStop;
                    Settings.BladeCount = CineCamComp->LensSettings.DiaphragmBladeCount;
                }
            }
            
            TempGroups[OriginalCategory].Add(Settings);
        }
    }
    
    // 合并为主分类
    Categories.Empty();
    
    TArray<FString> CategoryOrder = {TEXT("沙发"), TEXT("椅子"), TEXT("床"), TEXT("茶几"), TEXT("边几"), TEXT("床头柜"), TEXT("玄关柜"), TEXT("地板")};
    
    for (const FString& MainCat : CategoryOrder)
    {
        if (CategoryMapping.Contains(MainCat))
        {
            FCategoryViewpoint NewCategory;
            NewCategory.CategoryName = MainCat;
            
            for (const FString& SubCat : CategoryMapping[MainCat])
            {
                if (TempGroups.Contains(SubCat))
                {
                    for (const FCameraSettings& Settings : TempGroups[SubCat])
                    {
                        NewCategory.Viewpoints.Add(Settings.Transform);
                        NewCategory.CameraSettings.Add(Settings);
                    }
                }
            }
            
            if (NewCategory.Viewpoints.Num() > 0)
            {
                Categories.Add(NewCategory);
            }
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("AutoConfigureFromCineCameras: Found %d categories"), Categories.Num());
}
