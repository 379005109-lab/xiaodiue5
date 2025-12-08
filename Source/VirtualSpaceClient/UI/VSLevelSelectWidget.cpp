#include "VSLevelSelectWidget.h"
#include "VirtualSpaceClient.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"

void UVSLevelSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

TSharedRef<SWidget> UVSLevelSelectWidget::RebuildWidget()
{
	return SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.02f, 0.9f))
			.Padding(40)
			[
				SNew(SVerticalBox)
				// 标题
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 30)
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("选择场景")))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 28))
					.ColorAndOpacity(FLinearColor::White)
				]
				// 关卡列表
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					SNew(SBox)
					.MinDesiredWidth(500)
					.MinDesiredHeight(400)
					[
						SNew(SScrollBox)
						+ SScrollBox::Slot()
						[
							SAssignNew(LevelListBox, SVerticalBox)
						]
					]
				]
			]
		];
}

void UVSLevelSelectWidget::SetLevels(const TArray<FVSLevel>& InLevels)
{
	Levels = InLevels;
	RefreshLevelList();
}

void UVSLevelSelectWidget::RefreshLevelList()
{
	if (!LevelListBox.IsValid())
	{
		return;
	}

	LevelListBox->ClearChildren();

	for (int32 i = 0; i < Levels.Num(); i++)
	{
		const FVSLevel& Level = Levels[i];
		
		LevelListBox->AddSlot()
		.AutoHeight()
		.Padding(0, 5)
		[
			SNew(SButton)
			.OnClicked_Lambda([this, i]() -> FReply {
				OnLevelButtonClicked(i);
				return FReply::Handled();
			})
			.ButtonStyle(FCoreStyle::Get(), "Button")
			.ContentPadding(FMargin(20, 15))
			[
				SNew(SHorizontalBox)
				// 关卡名称
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(Level.Name))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0, 5, 0, 0)
					[
						SNew(STextBlock)
						.Text(FText::FromString(Level.Description.IsEmpty() ? TEXT("暂无描述") : Level.Description))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
						.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))
					]
				]
				// 箭头
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(20, 0, 0, 0)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT(">")))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 24))
					.ColorAndOpacity(FLinearColor::White)
				]
			]
		];
	}

	// 如果没有关卡
	if (Levels.Num() == 0)
	{
		LevelListBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(0, 50)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("暂无可用场景")))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
			.ColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f))
		];
	}
}

void UVSLevelSelectWidget::OnLevelButtonClicked(int32 LevelIndex)
{
	if (Levels.IsValidIndex(LevelIndex))
	{
		UE_LOG(LogVirtualSpace, Log, TEXT("Level selected: %s"), *Levels[LevelIndex].Name);
		OnLevelSelected.Broadcast(Levels[LevelIndex]);
	}
}
