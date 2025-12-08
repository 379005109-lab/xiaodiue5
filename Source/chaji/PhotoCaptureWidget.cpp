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

void UPhotoCaptureWidget::InitWidget()
{
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
    
    // Apply FOV based on focal length
    float FOV = 2.0f * FMath::RadiansToDegrees(FMath::Atan(18.0f / FocalLength));
    PC->PlayerCameraManager->SetFOV(FOV);
    
    // Apply DOF settings via post process
    FPostProcessSettings PPSettings;
    PPSettings.bOverride_DepthOfFieldFstop = true;
    PPSettings.DepthOfFieldFstop = Aperture;
    PPSettings.bOverride_DepthOfFieldFocalDistance = true;
    PPSettings.DepthOfFieldFocalDistance = FocusDistance;
    PPSettings.bOverride_DepthOfFieldSensorWidth = true;
    PPSettings.DepthOfFieldSensorWidth = 36.0f; // Full frame
    
    // Find or create camera with post process
    if (APawn* Pawn = PC->GetPawn())
    {
        UCameraComponent* CamComp = Pawn->FindComponentByClass<UCameraComponent>();
        if (CamComp)
        {
            CamComp->PostProcessSettings.bOverride_DepthOfFieldFstop = true;
            CamComp->PostProcessSettings.DepthOfFieldFstop = Aperture;
            CamComp->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
            CamComp->PostProcessSettings.DepthOfFieldFocalDistance = FocusDistance;
            CamComp->PostProcessSettings.bOverride_DepthOfFieldSensorWidth = true;
            CamComp->PostProcessSettings.DepthOfFieldSensorWidth = 36.0f;
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
        // Use HighResShot console command
        FString Command = FString::Printf(TEXT("HighResShot %d"), ScreenshotScale);
        PC->ConsoleCommand(Command);
        
        // Update status
        if (StatusText.IsValid())
        {
            StatusText->SetText(FText::FromString(TEXT("截图已保存到 Saved/Screenshots")));
        }
        
        UE_LOG(LogTemp, Log, TEXT("High resolution screenshot captured at %dx scale"), ScreenshotScale);
    }
    
    return FReply::Handled();
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

// Scroll wheel shortcuts
FReply UPhotoCaptureWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    float Delta = InMouseEvent.GetWheelDelta();
    
    if (FSlateApplication::Get().GetModifierKeys().IsControlDown())
    {
        // Ctrl + Scroll = Focal Length
        AdjustFocalLength(Delta * 5.0f);
        return FReply::Handled();
    }
    else if (FSlateApplication::Get().GetModifierKeys().IsShiftDown())
    {
        // Shift + Scroll = Aperture
        AdjustAperture(Delta * 0.5f);
        return FReply::Handled();
    }
    else if (FSlateApplication::Get().GetModifierKeys().IsAltDown())
    {
        // Alt + Scroll = Focus Distance
        AdjustFocusDistance(Delta * 100.0f);
        return FReply::Handled();
    }
    
    return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
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
