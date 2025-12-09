// PhotoCaptureWidget.cpp
#include "PhotoCaptureWidget.h"
#include "ViewpointControlWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Styling/CoreStyle.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameViewportClient.h"
#include "Engine/PostProcessVolume.h"
#include "Components/PostProcessComponent.h"
#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Framework/Application/SlateApplication.h"

TSharedRef<SWidget> UPhotoCaptureWidget::RebuildWidget()
{
    return SNew(SVerticalBox)
        // Top Row: Camera icon + Shutter + Expand arrow
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SBorder)
            .BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 0.95f))
            .Padding(FMargin(10.0f, 8.0f))
            [
                SNew(SHorizontalBox)
                // Camera icon
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("📷")))
                    .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                    .ColorAndOpacity(FLinearColor::White)
                ]
                // Shutter button
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(10.0f, 0.0f, 0.0f, 0.0f))
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.2f, 0.5f, 0.8f, 1.0f))
                    .OnClicked_Lambda([this]() { return OnShutterClicked(); })
                    .ContentPadding(FMargin(20.0f, 6.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("快门")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                ]
                // Expand arrow
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f))
                    .OnClicked_Lambda([this]() { return OnToggleClicked(); })
                    .ContentPadding(FMargin(8.0f, 6.0f))
                    [
                        SNew(STextBlock)
                        .Text_Lambda([this]() { return FText::FromString(bIsExpanded ? TEXT("▲") : TEXT("▼")); })
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                ]
            ]
        ]
        // Expandable Settings Panel
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(ContentBox, SBox)
            .Visibility(EVisibility::Collapsed)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.95f))
                .Padding(FMargin(15.0f))
                [
                    SNew(SVerticalBox)
                    // Resolution Selection
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 5.0f))
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(SBox)
                            .WidthOverride(70.0f)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("分辨率")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return ResolutionIndex == 0 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnResolution1K(); })
                            .ContentPadding(FMargin(8.0f, 4.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("1K")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return ResolutionIndex == 1 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnResolution2K(); })
                            .ContentPadding(FMargin(8.0f, 4.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("2K")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return ResolutionIndex == 2 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnResolution4K(); })
                            .ContentPadding(FMargin(8.0f, 4.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("4K")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                    ]
                    // Aspect Ratio Selection
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 8.0f, 0.0f, 0.0f))
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(SBox)
                            .WidthOverride(70.0f)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("比例")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return AspectRatioIndex == 0 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnAspectRatio16_9(); })
                            .ContentPadding(FMargin(6.0f, 4.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("16:9")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(3.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return AspectRatioIndex == 1 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnAspectRatio9_16(); })
                            .ContentPadding(FMargin(6.0f, 4.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("9:16")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(3.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return AspectRatioIndex == 2 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnAspectRatio1_1(); })
                            .ContentPadding(FMargin(6.0f, 4.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("1:1")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(3.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return AspectRatioIndex == 3 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnAspectRatio3_2(); })
                            .ContentPadding(FMargin(6.0f, 4.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("3:2")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(3.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return AspectRatioIndex == 4 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnAspectRatio2_3(); })
                            .ContentPadding(FMargin(6.0f, 4.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("2:3")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                    ]
                    // Status Text
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 10.0f, 0.0f, 0.0f))
                    .HAlign(HAlign_Center)
                    [
                        SAssignNew(StatusText, STextBlock)
                        .Text(FText::FromString(TEXT("")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                        .ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))
                    ]
                    // Open Folder Button
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 8.0f, 0.0f, 0.0f))
                    .HAlign(HAlign_Center)
                    [
                        SNew(SButton)
                        .ButtonColorAndOpacity(FLinearColor(0.3f, 0.3f, 0.5f, 1.0f))
                        .OnClicked_Lambda([this]() { return OnOpenFolderClicked(); })
                        .ContentPadding(FMargin(12.0f, 5.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("打开文件夹")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                            .ColorAndOpacity(FLinearColor(0.6f, 0.8f, 1.0f))
                        ]
                    ]
                    // Reset Button (at bottom)
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 12.0f, 0.0f, 0.0f))
                    .HAlign(HAlign_Center)
                    [
                        SNew(SButton)
                        .ButtonColorAndOpacity(FLinearColor(0.5f, 0.3f, 0.3f, 1.0f))
                        .OnClicked_Lambda([this]() { return OnResetClicked(); })
                        .ContentPadding(FMargin(15.0f, 6.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("恢复默认")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                            .ColorAndOpacity(FLinearColor::White)
                        ]
                    ]
                ]
            ]
        ];
}

void UPhotoCaptureWidget::InitWidget()
{
    UpdateParameterDisplay();
    ApplyAllCameraSettings();
}

void UPhotoCaptureWidget::LoadCameraSettings(float InFocalLength, float InAperture, float InFocusDistance)
{
    FocalLength = InFocalLength;
    Aperture = InAperture;
    FocusDistance = InFocusDistance;
    UpdateParameterDisplay();
    ApplyAllCameraSettings();
}

void UPhotoCaptureWidget::UpdateParameterDisplay()
{
    if (FocalLengthText.IsValid())
    {
        FocalLengthText->SetText(FText::FromString(FString::Printf(TEXT("%.0fmm"), FocalLength)));
    }
    if (ApertureText.IsValid())
    {
        ApertureText->SetText(FText::FromString(FString::Printf(TEXT("f/%.1f"), Aperture)));
    }
    if (FocusDistanceText.IsValid())
    {
        FocusDistanceText->SetText(FText::FromString(FString::Printf(TEXT("%.0fcm"), FocusDistance)));
    }
}

void UPhotoCaptureWidget::ApplyAllCameraSettings()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC || !PC->PlayerCameraManager) return;
    
    // Apply FOV based on focal length (full frame 36mm sensor)
    float FOV = 2.0f * FMath::RadiansToDegrees(FMath::Atan(18.0f / FocalLength));
    PC->PlayerCameraManager->SetFOV(FOV);
    
    // Enable high quality DOF
    PC->ConsoleCommand(TEXT("r.DepthOfFieldQuality 4"));
    PC->ConsoleCommand(TEXT("r.DOF.Gather.RingCount 5"));
    
    // Apply DOF settings to camera component on pawn
    if (APawn* Pawn = PC->GetPawn())
    {
        UCameraComponent* CamComp = Pawn->FindComponentByClass<UCameraComponent>();
        if (CamComp)
        {
            // Enable DOF
            CamComp->PostProcessSettings.bOverride_DepthOfFieldFstop = true;
            CamComp->PostProcessSettings.DepthOfFieldFstop = Aperture;
            
            CamComp->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
            CamComp->PostProcessSettings.DepthOfFieldFocalDistance = FocusDistance;
            
            CamComp->PostProcessSettings.bOverride_DepthOfFieldSensorWidth = true;
            CamComp->PostProcessSettings.DepthOfFieldSensorWidth = 36.0f;
            
            CamComp->PostProcessSettings.bOverride_DepthOfFieldMinFstop = true;
            CamComp->PostProcessSettings.DepthOfFieldMinFstop = 1.2f;
            
            CamComp->PostProcessSettings.bOverride_DepthOfFieldBladeCount = true;
            CamComp->PostProcessSettings.DepthOfFieldBladeCount = 7;
            
            // Enable camera post process
            CamComp->PostProcessBlendWeight = 1.0f;
            
            // Log for debugging
            UE_LOG(LogTemp, Log, TEXT("DOF Applied: Aperture=%.1f, FocusDistance=%.0f, FOV=%.1f"), 
                Aperture, FocusDistance, FOV);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No CameraComponent found on Pawn"));
        }
    }
}

FReply UPhotoCaptureWidget::OnToggleClicked()
{
    bIsExpanded = !bIsExpanded;
    if (ContentBox.IsValid())
    {
        ContentBox->SetVisibility(bIsExpanded ? EVisibility::Visible : EVisibility::Collapsed);
    }
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnCaptureClicked()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        // Base resolution
        int32 BaseSize = 1080;
        switch (ResolutionIndex)
        {
            case 0: BaseSize = 1080; break; // 1K
            case 1: BaseSize = 1440; break; // 2K
            case 2: BaseSize = 2160; break; // 4K
        }
        
        // Calculate dimensions based on aspect ratio
        int32 Width, Height;
        switch (AspectRatioIndex)
        {
            case 0: Width = BaseSize * 16 / 9; Height = BaseSize; break;  // 16:9
            case 1: Width = BaseSize * 9 / 16; Height = BaseSize; break;  // 9:16
            case 2: Width = BaseSize; Height = BaseSize; break;           // 1:1
            case 3: Width = BaseSize * 3 / 2; Height = BaseSize; break;   // 3:2
            case 4: Width = BaseSize * 2 / 3; Height = BaseSize; break;   // 2:3
            default: Width = BaseSize * 16 / 9; Height = BaseSize; break;
        }
        
        // Increment counter for naming
        ScreenshotCounter++;
        FString Filename = FString::Printf(TEXT("%s_%d"), *CurrentCategoryName, ScreenshotCounter);
        
        // Use HighResShot console command with resolution
        FString Command = FString::Printf(TEXT("HighResShot %dx%d filename=%s"), Width, Height, *Filename);
        PC->ConsoleCommand(Command);
        
        // Save path for opening folder
        LastSavePath = FPaths::ProjectSavedDir() / TEXT("Screenshots");
        
        // Update status with clear message
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(FString::Printf(TEXT("已保存: %s (%dx%d)"), *Filename, Width, Height)));
        }
        
        UE_LOG(LogTemp, Log, TEXT("Screenshot saved: %s (%dx%d)"), *Filename, Width, Height);
    }
    
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnResetClicked()
{
    // Reset camera parameters
    FocalLength = DefaultFocalLength;
    Aperture = DefaultAperture;
    FocusDistance = DefaultFocusDistance;
    UpdateParameterDisplay();
    ApplyAllCameraSettings();
    
    // Notify HUD to reset viewpoints to initial state
    OnResetViewpoints.Broadcast();
    
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(TEXT("已恢复镜头初始状态")));
    }
    
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnResolution1K()
{
    ResolutionIndex = 0;
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnResolution2K()
{
    ResolutionIndex = 1;
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnResolution4K()
{
    ResolutionIndex = 2;
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnAspectRatio16_9()
{
    AspectRatioIndex = 0;
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnAspectRatio9_16()
{
    AspectRatioIndex = 1;
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnAspectRatio1_1()
{
    AspectRatioIndex = 2;
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnAspectRatio3_2()
{
    AspectRatioIndex = 3;
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnAspectRatio2_3()
{
    AspectRatioIndex = 4;
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnShutterClicked()
{
    // Check if there are selected viewpoints for batch capture
    if (ViewpointControlRef)
    {
        TArray<int32> Selected = ViewpointControlRef->GetSelectedViewpoints();
        if (Selected.Num() > 0)
        {
            // Notify HUD to perform batch capture with viewpoint switching
            OnBatchCaptureStart.Broadcast(Selected);
            
            // Update status
            if (StatusText.IsValid())
            {
                StatusText->SetText(FText::FromString(FString::Printf(TEXT("正在批量截图 %d 张..."), Selected.Num())));
            }
            return FReply::Handled();
        }
    }
    
    // Single capture if no selection
    return OnCaptureClicked();
}

void UPhotoCaptureWidget::CaptureSingle()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;
    
    // Base resolution
    int32 BaseSize = 1080;
    switch (ResolutionIndex)
    {
        case 0: BaseSize = 1080; break; // 1K
        case 1: BaseSize = 1440; break; // 2K
        case 2: BaseSize = 2160; break; // 4K
    }
    
    // Calculate dimensions based on aspect ratio
    int32 Width, Height;
    switch (AspectRatioIndex)
    {
        case 0: Width = BaseSize * 16 / 9; Height = BaseSize; break;  // 16:9
        case 1: Width = BaseSize * 9 / 16; Height = BaseSize; break;  // 9:16
        case 2: Width = BaseSize; Height = BaseSize; break;           // 1:1
        case 3: Width = BaseSize * 3 / 2; Height = BaseSize; break;   // 3:2
        case 4: Width = BaseSize * 2 / 3; Height = BaseSize; break;   // 2:3
        default: Width = BaseSize * 16 / 9; Height = BaseSize; break;
    }
    
    // Increment counter for naming
    ScreenshotCounter++;
    
    // Create filename with category name
    FString Filename = FString::Printf(TEXT("%s_%d"), *CurrentCategoryName, ScreenshotCounter);
    
    // Use HighResShot with filename
    FString Command = FString::Printf(TEXT("HighResShot %dx%d filename=%s"), Width, Height, *Filename);
    PC->ConsoleCommand(Command);
    
    LastSavePath = FPaths::ProjectSavedDir() / TEXT("Screenshots");
}

void UPhotoCaptureWidget::CaptureMultiple(const TArray<int32>& Indices)
{
    // This is now handled by HUD via OnBatchCaptureStart delegate
    // Keep for backward compatibility
    OnBatchCaptureStart.Broadcast(Indices);
}

FReply UPhotoCaptureWidget::OnSaveViewpointClicked()
{
    // Save current camera parameters as viewpoint data
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(FString::Printf(
            TEXT("已保存视角: 焦距%.0fmm 光圈f/%.1f 对焦%.0fcm"), 
            FocalLength, Aperture, FocusDistance)));
    }
    UE_LOG(LogTemp, Log, TEXT("Saved viewpoint: Focal=%.0f, Aperture=%.1f, Focus=%.0f"), FocalLength, Aperture, FocusDistance);
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnOpenFolderClicked()
{
    if (!LastSavePath.IsEmpty())
    {
        FPlatformProcess::ExploreFolder(*LastSavePath);
    }
    else
    {
        FString DefaultPath = FPaths::ProjectSavedDir() / TEXT("Screenshots");
        FPlatformProcess::ExploreFolder(*DefaultPath);
    }
    return FReply::Handled();
}

void UPhotoCaptureWidget::ShowSaveNotification(const FString& Path)
{
    LastSavePath = Path;
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(FString::Printf(TEXT("照片已保存在文件夹中"))));
    }
}

void UPhotoCaptureWidget::OnFocalLengthChanged(float Value)
{
    FocalLength = 10.0f + Value * 190.0f; // 10mm to 200mm
    UpdateParameterDisplay();
    ApplyAllCameraSettings();
}

void UPhotoCaptureWidget::OnApertureChanged(float Value)
{
    Aperture = 1.2f + Value * 20.8f; // f/1.2 to f/22
    UpdateParameterDisplay();
    ApplyAllCameraSettings();
}

void UPhotoCaptureWidget::OnFocusDistanceChanged(float Value)
{
    FocusDistance = 10.0f + Value * 9990.0f; // 10cm to 10000cm (more precise start)
    UpdateParameterDisplay();
    ApplyAllCameraSettings();
}

void UPhotoCaptureWidget::AdjustFocalLength(float Delta)
{
    FocalLength = FMath::Clamp(FocalLength + Delta, 10.0f, 200.0f);
    UpdateParameterDisplay();
    ApplyAllCameraSettings();
}

void UPhotoCaptureWidget::AdjustAperture(float Delta)
{
    Aperture = FMath::Clamp(Aperture + Delta, 1.2f, 22.0f);
    UpdateParameterDisplay();
    ApplyAllCameraSettings();
}

void UPhotoCaptureWidget::AdjustFocusDistance(float Delta)
{
    FocusDistance = FMath::Clamp(FocusDistance + Delta, 10.0f, 10000.0f);
    UpdateParameterDisplay();
    ApplyAllCameraSettings();
}
