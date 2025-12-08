#include "VSLoginWidget.h"
#include "VirtualSpaceClient.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"

void UVSLoginWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

TSharedRef<SWidget> UVSLoginWidget::RebuildWidget()
{
	return SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.02f, 0.9f))
			.Padding(50)
			[
				SNew(SVerticalBox)
				// 标题
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 30)
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("VirtualSpace")))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 32))
					.ColorAndOpacity(FLinearColor::White)
				]
				// 用户名
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 5)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("账号")))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
					.ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 20)
				[
					SNew(SBox)
					.MinDesiredWidth(300)
					.MinDesiredHeight(40)
					[
						SAssignNew(UsernameInput, SEditableTextBox)
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
						.HintText(FText::FromString(TEXT("请输入账号")))
					]
				]
				// 密码
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 5)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("密码")))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
					.ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 20)
				[
					SNew(SBox)
					.MinDesiredWidth(300)
					.MinDesiredHeight(40)
					[
						SAssignNew(PasswordInput, SEditableTextBox)
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
						.HintText(FText::FromString(TEXT("请输入密码")))
						.IsPassword(true)
					]
				]
				// 错误信息
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 15)
				.HAlign(HAlign_Center)
				[
					SAssignNew(ErrorTextBlock, STextBlock)
					.Text(FText::FromString(TEXT("")))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
					.ColorAndOpacity(FLinearColor(1.0f, 0.3f, 0.3f))
				]
				// 登录按钮
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(SBox)
					.MinDesiredWidth(300)
					.MinDesiredHeight(45)
					[
						SAssignNew(LoginButton, SButton)
						.OnClicked_Lambda([this]() -> FReply {
							OnLoginButtonClicked();
							return FReply::Handled();
						})
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SAssignNew(LoginButtonText, STextBlock)
							.Text(FText::FromString(TEXT("登 录")))
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
							.ColorAndOpacity(FLinearColor::White)
						]
					]
				]
			]
		];
}

void UVSLoginWidget::OnLoginButtonClicked()
{
	if (bIsLoading)
	{
		return;
	}

	FString Username = UsernameInput->GetText().ToString();
	FString Password = PasswordInput->GetText().ToString();

	if (Username.IsEmpty())
	{
		SetErrorMessage(TEXT("请输入账号"));
		return;
	}

	if (Password.IsEmpty())
	{
		SetErrorMessage(TEXT("请输入密码"));
		return;
	}

	SetErrorMessage(TEXT(""));
	UE_LOG(LogVirtualSpace, Log, TEXT("Login requested for user: %s"), *Username);
	OnLoginRequested.Broadcast(Username, Password);
}

void UVSLoginWidget::SetErrorMessage(const FString& Message)
{
	if (ErrorTextBlock.IsValid())
	{
		ErrorTextBlock->SetText(FText::FromString(Message));
	}
}

void UVSLoginWidget::SetLoading(bool bInIsLoading)
{
	bIsLoading = bInIsLoading;
	if (LoginButtonText.IsValid())
	{
		LoginButtonText->SetText(FText::FromString(bIsLoading ? TEXT("登录中...") : TEXT("登 录")));
	}
	if (LoginButton.IsValid())
	{
		LoginButton->SetEnabled(!bIsLoading);
	}
}
