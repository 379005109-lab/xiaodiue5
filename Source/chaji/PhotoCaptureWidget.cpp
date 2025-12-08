// PhotoCaptureWidget.cpp
#include "PhotoCaptureWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Styling/CoreStyle.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HighResScreenshot.h"
#include "Misc/FileHelper.h"

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

void UPhotoCaptureWidget::SetCameraActor(ACineCameraActor* InCamera)
{
    CameraActor = InCamera;
    if (CameraActor)
    {
        UCineCameraComponent* CineComp = CameraActor->GetCineCameraComponent();
        if (CineComp)
        {
            FocalLength = CineComp->CurrentFocalLength;
            Aperture = CineComp->CurrentAperture;
            FocusDistance = CineComp->FocusSettings.ManualFocusDistance;
            UpdateParameterDisplay();
        }
    }
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

void UPhotoCaptureWidget::ApplyCameraSettings()
{
    if (CameraActor)
    {
        UCineCameraComponent* CineComp = CameraActor->GetCineCameraComponent();
        if (CineComp)
        {
            CineComp->CurrentFocalLength = FocalLength;
            CineComp->CurrentAperture = Aperture;
            CineComp->FocusSettings.ManualFocusDistance = FocusDistance;
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
    // Take high resolution screenshot
    FString Filename = FString::Printf(TEXT("Screenshot_%s"), *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
    FScreenshotRequest::RequestScreenshot(Filename, true, false);
    
    // Set resolution multiplier for high-res capture
    GetHighResScreenshotConfig().ResolutionMultiplier = ScreenshotScale;
    GetHighResScreenshotConfig().bMaskEnabled = false;
    
    UE_LOG(LogTemp, Log, TEXT("Screenshot captured: %s"), *Filename);
    
    return FReply::Handled();
}

void UPhotoCaptureWidget::OnFocalLengthChanged(float Value)
{
    FocalLength = 10.0f + Value * 190.0f; // 10mm to 200mm
    UpdateParameterDisplay();
    ApplyCameraSettings();
}

void UPhotoCaptureWidget::OnApertureChanged(float Value)
{
    Aperture = 1.2f + Value * 20.8f; // f/1.2 to f/22
    UpdateParameterDisplay();
    ApplyCameraSettings();
}

void UPhotoCaptureWidget::OnFocusDistanceChanged(float Value)
{
    FocusDistance = Value * 10000.0f; // 0 to 10000
    UpdateParameterDisplay();
    ApplyCameraSettings();
}

void UPhotoCaptureWidget::RebuildSettings()
{
    // Placeholder for future settings rebuild
}
