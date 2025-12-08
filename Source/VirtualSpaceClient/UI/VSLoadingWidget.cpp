#include "VSLoadingWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SThrobber.h"

void UVSLoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

TSharedRef<SWidget> UVSLoadingWidget::RebuildWidget()
{
	return SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			[
				SNew(SCircularThrobber)
				.Radius(32.0f)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 20, 0, 0)
			.HAlign(HAlign_Center)
			[
				SAssignNew(LoadingTextBlock, STextBlock)
				.Text(FText::FromString(TEXT("正在加载配置...")))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 18))
				.ColorAndOpacity(FLinearColor::White)
			]
		];
}

void UVSLoadingWidget::SetLoadingText(const FString& Text)
{
	if (LoadingTextBlock.IsValid())
	{
		LoadingTextBlock->SetText(FText::FromString(Text));
	}
}
