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
#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"

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
                    // Capture Button
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(FMargin(0.0f, 15.0f, 0.0f, 0.0f))
                    .HAlign(HAlign_Center)
                    [
                        SNew(SButton)
                        .ButtonColorAndOpacity(FLinearColor(0.2f, 0.6f, 0.2f, 1.0f))
                        .OnClicked_Lambda([this]() { return OnCaptureClicked(); })
                        .ContentPadding(FMargin(30.0f, 10.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("📷 高清截图")))
                            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
                            .ColorAndOpacity(FLinearColor::White)
                        ]
                    ]
                ]
            ]
        ];
}

void UPhotoCaptureWidget::Initialize()
{
    UpdateParameterDisplay();
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
        FocusDistanceText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), FocusDistance)));
    }
}

void UPhotoCaptureWidget::ApplyToPlayerCamera()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && PC->PlayerCameraManager)
    {
        // Apply FOV based on focal length (approximate conversion)
        // FOV = 2 * atan(SensorWidth / (2 * FocalLength))
        // Using 36mm full frame sensor width
        float FOV = 2.0f * FMath::RadiansToDegrees(FMath::Atan(18.0f / FocalLength));
        PC->PlayerCameraManager->SetFOV(FOV);
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
    // Get screenshot directory
    FString ScreenshotDir = FPaths::ProjectSavedDir() / TEXT("Screenshots");
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    PlatformFile.CreateDirectoryTree(*ScreenshotDir);
    
    // Generate filename with timestamp
    FString Filename = FString::Printf(TEXT("Photo_%s.png"), *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
    FString FullPath = ScreenshotDir / Filename;
    
    // Take screenshot using console command for high resolution
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        FString Command = FString::Printf(TEXT("HighResShot %dx filename=\"%s\""), ScreenshotScale, *FullPath);
        PC->ConsoleCommand(Command);
        
        // Update status
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(FString::Printf(TEXT("已保存: %s"), *Filename)));
        }
        
        UE_LOG(LogTemp, Log, TEXT("Screenshot saved to: %s"), *FullPath);
    }
    
    return FReply::Handled();
}

void UPhotoCaptureWidget::OnFocalLengthChanged(float Value)
{
    FocalLength = 10.0f + Value * 190.0f; // 10mm to 200mm
    UpdateParameterDisplay();
    ApplyToPlayerCamera();
}

void UPhotoCaptureWidget::OnApertureChanged(float Value)
{
    Aperture = 1.2f + Value * 20.8f; // f/1.2 to f/22
    UpdateParameterDisplay();
    // Aperture affects DOF, which requires post-process settings
}

void UPhotoCaptureWidget::OnFocusDistanceChanged(float Value)
{
    FocusDistance = Value * 10000.0f; // 0 to 10000
    UpdateParameterDisplay();
    // Focus distance affects DOF focus plane
}
