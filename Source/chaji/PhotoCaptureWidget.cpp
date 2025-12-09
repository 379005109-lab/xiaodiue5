// PhotoCaptureWidget.cpp
#include "PhotoCaptureWidget.h"
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
        // Toggle Button Row
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.9f))
                .OnClicked_Lambda([this]() { return OnToggleClicked(); })
                .ContentPadding(FMargin(12.0f, 8.0f))
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("📷 拍照")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text_Lambda([this]() { return FText::FromString(bIsExpanded ? TEXT("▲") : TEXT("▼")); })
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                ]
            ]
        ]
        // Settings Panel
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(ContentBox, SBox)
            .Visibility(EVisibility::Collapsed)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.9f))
                .Padding(FMargin(15.0f))
                [
                    SNew(SVerticalBox)
                    // Focal Length
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
                            .WidthOverride(80.0f)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("焦距")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .Padding(FMargin(10.0f, 0.0f))
                        [
                            SNew(SSlider)
                            .Value_Lambda([this]() { return (FocalLength - 10.0f) / 190.0f; })
                            .OnValueChanged_Lambda([this](float Value) { OnFocalLengthChanged(Value); })
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(SBox)
                            .WidthOverride(60.0f)
                            [
                                SAssignNew(FocalLengthText, STextBlock)
                                .Text(FText::FromString(TEXT("35mm")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                    ]
                    // Aperture
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
                            .WidthOverride(80.0f)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("光圈")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .Padding(FMargin(10.0f, 0.0f))
                        [
                            SNew(SSlider)
                            .Value_Lambda([this]() { return (Aperture - 1.2f) / 20.8f; })
                            .OnValueChanged_Lambda([this](float Value) { OnApertureChanged(Value); })
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(SBox)
                            .WidthOverride(60.0f)
                            [
                                SAssignNew(ApertureText, STextBlock)
                                .Text(FText::FromString(TEXT("f/2.8")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                    ]
                    // Focus Distance
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
                            .WidthOverride(80.0f)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("手动距离")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .Padding(FMargin(10.0f, 0.0f))
                        [
                            SNew(SSlider)
                            .Value_Lambda([this]() { return FocusDistance / 10000.0f; })
                            .OnValueChanged_Lambda([this](float Value) { OnFocusDistanceChanged(Value); })
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(SBox)
                            .WidthOverride(60.0f)
                            [
                                SAssignNew(FocusDistanceText, STextBlock)
                                .Text(FText::FromString(TEXT("1000")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                    ]
                    // Reset Button
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 10.0f, 0.0f, 0.0f))
                    .HAlign(HAlign_Center)
                    [
                        SNew(SButton)
                        .ButtonColorAndOpacity(FLinearColor(0.4f, 0.4f, 0.4f, 1.0f))
                        .OnClicked_Lambda([this]() { return OnResetClicked(); })
                        .ContentPadding(FMargin(20.0f, 8.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("恢复默认")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                            .ColorAndOpacity(FLinearColor::White)
                        ]
                    ]
                    // Resolution Selection
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 15.0f, 0.0f, 0.0f))
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("分辨率：")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                            .ColorAndOpacity(FLinearColor::White)
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(10.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return ResolutionIndex == 0 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnResolution1K(); })
                            .ContentPadding(FMargin(10.0f, 5.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("1K")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return ResolutionIndex == 1 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnResolution2K(); })
                            .ContentPadding(FMargin(10.0f, 5.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("2K")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity_Lambda([this]() { return ResolutionIndex == 2 ? FLinearColor(0.2f, 0.6f, 0.2f, 1.0f) : FLinearColor(0.3f, 0.3f, 0.3f, 1.0f); })
                            .OnClicked_Lambda([this]() { return OnResolution4K(); })
                            .ContentPadding(FMargin(10.0f, 5.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("4K")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                    ]
                    // Shutter and Save Viewpoint buttons row
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 15.0f, 0.0f, 0.0f))
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .Padding(FMargin(0.0f, 0.0f, 5.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.2f, 0.4f, 0.8f, 1.0f))
                            .OnClicked_Lambda([this]() { return OnShutterClicked(); })
                            .ContentPadding(FMargin(15.0f, 10.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("快门")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .Padding(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SButton)
                            .ButtonColorAndOpacity(FLinearColor(0.4f, 0.4f, 0.4f, 1.0f))
                            .OnClicked_Lambda([this]() { return OnSaveViewpointClicked(); })
                            .ContentPadding(FMargin(15.0f, 10.0f))
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("保存视角")))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                                .ColorAndOpacity(FLinearColor::White)
                            ]
                        ]
                    ]
                    // Batch Capture Button
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 10.0f, 0.0f, 0.0f))
                    .HAlign(HAlign_Center)
                    [
                        SNew(SButton)
                        .ButtonColorAndOpacity(FLinearColor(0.2f, 0.6f, 0.2f, 1.0f))
                        .OnClicked_Lambda([this]() { return OnBatchCaptureClicked(); })
                        .ContentPadding(FMargin(30.0f, 10.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("批量出图(勾选镜头)")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                            .ColorAndOpacity(FLinearColor::White)
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
                    // Open Folder Button (shown after save)
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 5.0f, 0.0f, 0.0f))
                    .HAlign(HAlign_Center)
                    [
                        SNew(SButton)
                        .ButtonColorAndOpacity(FLinearColor(0.3f, 0.3f, 0.5f, 1.0f))
                        .OnClicked_Lambda([this]() { return OnOpenFolderClicked(); })
                        .ContentPadding(FMargin(15.0f, 6.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("点击打开文件夹")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                            .ColorAndOpacity(FLinearColor(0.6f, 0.8f, 1.0f))
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
        // Resolution based on selection
        int32 Width = 1920;
        int32 Height = 1080;
        FString ResName = TEXT("1K");
        
        switch (ResolutionIndex)
        {
            case 0: Width = 1920; Height = 1080; ResName = TEXT("1K"); break;
            case 1: Width = 2560; Height = 1440; ResName = TEXT("2K"); break;
            case 2: Width = 3840; Height = 2160; ResName = TEXT("4K"); break;
        }
        
        // Use HighResShot console command with resolution
        FString Command = FString::Printf(TEXT("HighResShot %dx%d"), Width, Height);
        PC->ConsoleCommand(Command);
        
        // Get save path
        FString SavePath = FPaths::ProjectSavedDir() / TEXT("Screenshots");
        
        // Update status
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(FString::Printf(TEXT("%s 截图已保存: %s"), *ResName, *SavePath)));
        }
        
        UE_LOG(LogTemp, Log, TEXT("Screenshot %s (%dx%d) saved to: %s"), *ResName, Width, Height, *SavePath);
    }
    
    return FReply::Handled();
}

FReply UPhotoCaptureWidget::OnResetClicked()
{
    FocalLength = DefaultFocalLength;
    Aperture = DefaultAperture;
    FocusDistance = DefaultFocusDistance;
    UpdateParameterDisplay();
    ApplyAllCameraSettings();
    
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(TEXT("已恢复默认参数")));
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

FReply UPhotoCaptureWidget::OnShutterClicked()
{
    // Quick capture - same as capture but with notification
    return OnCaptureClicked();
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

FReply UPhotoCaptureWidget::OnBatchCaptureClicked()
{
    if (StatusText.IsValid())
    {
        StatusText->SetText(FText::FromString(TEXT("请先勾选镜头，然后点击此按钮批量出图")));
    }
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
    FocusDistance = 50.0f + Value * 9950.0f; // 50cm to 10000cm
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
    FocusDistance = FMath::Clamp(FocusDistance + Delta, 50.0f, 10000.0f);
    UpdateParameterDisplay();
    ApplyAllCameraSettings();
}
