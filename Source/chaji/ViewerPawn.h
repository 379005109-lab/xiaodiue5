// ViewerPawn.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Camera/CameraComponent.h"
#include "ViewerPawn.generated.h"

UCLASS()
class AViewerPawn : public ADefaultPawn
{
    GENERATED_BODY()

public:
    AViewerPawn();
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCameraComponent* CameraComp;

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
