// ViewerPawn.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Camera/CameraComponent.h"
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
    
    virtual void Tick(float DeltaTime) override;
    
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
