// ViewerPawn.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "ViewerPawn.generated.h"

UCLASS()
class AViewerPawn : public ADefaultPawn
{
    GENERATED_BODY()

public:
    AViewerPawn();

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
