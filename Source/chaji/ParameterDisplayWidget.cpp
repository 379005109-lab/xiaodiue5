// ParameterDisplayWidget.cpp
#include "ParameterDisplayWidget.h"
#include "PhotoCaptureWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Styling/CoreStyle.h"

TSharedRef<SWidget> UParameterDisplayWidget::RebuildWidget()
{
    return SNew(SBorder)
        .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.6f))
        .Padding(FMargin(20.0f, 10.0f))
        [
            SAssignNew(DisplayText, STextBlock)
            .Text(FText::FromString(TEXT("焦距: 35mm | 光圈: f/2.8 | 对焦: 1000cm")))
            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
            .ColorAndOpacity(FLinearColor::White)
        ];
}

void UParameterDisplayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    if (PhotoCaptureRef && DisplayText.IsValid())
    {
        FString Text = FString::Printf(TEXT("焦距: %.0fmm | 光圈: f/%.1f | 对焦: %.0fcm"),
            PhotoCaptureRef->GetFocalLength(),
            PhotoCaptureRef->GetAperture(),
            PhotoCaptureRef->GetFocusDistance());
        DisplayText->SetText(FText::FromString(Text));
    }
}
