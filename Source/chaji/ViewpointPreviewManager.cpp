// ViewpointPreviewManager.cpp
#include "ViewpointPreviewManager.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

AViewpointPreviewManager::AViewpointPreviewManager()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.1f; // Tick less frequently
}

void AViewpointPreviewManager::BeginPlay()
{
    Super::BeginPlay();
}

void AViewpointPreviewManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Stagger updates to reduce performance impact
    TimeSinceLastUpdate += DeltaTime;
    if (TimeSinceLastUpdate >= UpdateInterval && CaptureComponents.Num() > 0)
    {
        TimeSinceLastUpdate = 0.0f;
        
        // Update one capture per tick cycle
        if (CurrentUpdateIndex < CaptureComponents.Num())
        {
            if (CaptureComponents[CurrentUpdateIndex])
            {
                CaptureComponents[CurrentUpdateIndex]->CaptureScene();
            }
            CurrentUpdateIndex++;
        }
        else
        {
            CurrentUpdateIndex = 0;
        }
    }
}

void AViewpointPreviewManager::SetupPreviews(const TArray<FTransform>& Transforms)
{
    ClearPreviews();
    ViewpointTransforms = Transforms;
    
    for (int32 i = 0; i < Transforms.Num(); i++)
    {
        CreateCaptureComponent(i, Transforms[i]);
    }
    
    // Initial capture of all viewpoints
    UpdatePreviews();
}

void AViewpointPreviewManager::CreateCaptureComponent(int32 Index, const FTransform& Transform)
{
    // Create render target
    UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(this);
    RenderTarget->InitCustomFormat(160, 90, PF_B8G8R8A8, false); // 16:9 thumbnail
    RenderTarget->UpdateResourceImmediate();
    RenderTargets.Add(RenderTarget);
    
    // Create scene capture component
    USceneCaptureComponent2D* CaptureComp = NewObject<USceneCaptureComponent2D>(this);
    CaptureComp->RegisterComponent();
    CaptureComp->SetWorldTransform(Transform);
    CaptureComp->TextureTarget = RenderTarget;
    CaptureComp->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    CaptureComp->bCaptureEveryFrame = false;
    CaptureComp->bCaptureOnMovement = false;
    CaptureComp->FOVAngle = 90.0f;
    
    // Optimize capture settings for thumbnails
    CaptureComp->ShowFlags.SetAntiAliasing(false);
    CaptureComp->ShowFlags.SetMotionBlur(false);
    CaptureComp->ShowFlags.SetBloom(true);
    CaptureComp->ShowFlags.SetAmbientOcclusion(false);
    
    CaptureComponents.Add(CaptureComp);
}

UTextureRenderTarget2D* AViewpointPreviewManager::GetPreviewTexture(int32 Index) const
{
    if (RenderTargets.IsValidIndex(Index))
    {
        return RenderTargets[Index];
    }
    return nullptr;
}

void AViewpointPreviewManager::UpdatePreviews()
{
    for (USceneCaptureComponent2D* Comp : CaptureComponents)
    {
        if (Comp)
        {
            Comp->CaptureScene();
        }
    }
}

void AViewpointPreviewManager::ClearPreviews()
{
    for (USceneCaptureComponent2D* Comp : CaptureComponents)
    {
        if (Comp)
        {
            Comp->DestroyComponent();
        }
    }
    CaptureComponents.Empty();
    RenderTargets.Empty();
    ViewpointTransforms.Empty();
    CurrentUpdateIndex = 0;
}
