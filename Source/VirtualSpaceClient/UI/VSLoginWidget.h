#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VSLoginWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginRequested, const FString&, Username, const FString&, Password);

/**
 * 登录界面 Widget
 */
UCLASS()
class VIRTUALSPACECLIENT_API UVSLoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 登录请求事件
	UPROPERTY(BlueprintAssignable, Category = "VirtualSpace|UI")
	FOnLoginRequested OnLoginRequested;

	// 设置错误信息
	UFUNCTION(BlueprintCallable, Category = "VirtualSpace|UI")
	void SetErrorMessage(const FString& Message);

	// 设置加载状态
	UFUNCTION(BlueprintCallable, Category = "VirtualSpace|UI")
	void SetLoading(bool bIsLoading);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	void OnLoginButtonClicked();

	TSharedPtr<SEditableTextBox> UsernameInput;
	TSharedPtr<SEditableTextBox> PasswordInput;
	TSharedPtr<STextBlock> ErrorTextBlock;
	TSharedPtr<SButton> LoginButton;
	TSharedPtr<STextBlock> LoginButtonText;
	bool bIsLoading = false;
};
