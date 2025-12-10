// ViewerPawn.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ViewerPawn.generated.h"

class UPhotoCaptureWidget;

UCLASS()
class AViewerPawn : public ADefaultPawn
{
    GENERATED_BODY()

public:
    AViewerPawn();
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCameraComponent* CameraComp;
    
    // Scene capture for rendering to texture (用于将3D场景渲染到纹理)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneCaptureComponent2D* SceneCapture;
    
    // Render target texture
    UPROPERTY(BlueprintReadOnly)
    UTextureRenderTarget2D* RenderTarget;
    
    // Get the render target for UI display
    UFUNCTION(BlueprintCallable)
    UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }
    
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
    
    // Reference to photo capture widget for shortcuts
    UPROPERTY()
    UPhotoCaptureWidget* PhotoCaptureRef;
    
    bool bRightMouseDown = false;
    
    // Store initial position for reset (public for HUD access)
    FVector InitialLocation;
    FRotator InitialRotation;
    bool bInitialStateSaved = false;

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    
private:
    FVector MoveInput = FVector::ZeroVector;
    
    void StartMoveForward();
    void StopMoveForward();
    void StartMoveBackward();
    void StopMoveBackward();
    void StartMoveLeft();
    void StopMoveLeft();
    void StartMoveRight();
    void StopMoveRight();
    void StartMoveUp();
    void StopMoveUp();
    void StartMoveDown();
    void StopMoveDown();
    
    void OnRightMousePressed();
    void OnRightMouseReleased();
    void OnMouseScrollUp();
    void OnMouseScrollDown();
    void OnResetShortcut();
    void SaveInitialState();
};
