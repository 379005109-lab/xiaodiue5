#include "VSMainHUD.h"
#include "VirtualSpaceClient.h"
#include "GameInstance/VSGameInstance.h"
#include "VSLoadingWidget.h"
#include "VSLoginWidget.h"
#include "VSLevelSelectWidget.h"
#include "Kismet/GameplayStatics.h"

AVSMainHUD::AVSMainHUD()
{
}

void AVSMainHUD::BeginPlay()
{
	Super::BeginPlay();

	// 获取 API 客户端
	UVSGameInstance* GI = Cast<UVSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI)
	{
		ApiClient = GI->GetApiClient();
		if (ApiClient)
		{
			// 绑定事件
			ApiClient->OnConfigReceived.AddDynamic(this, &AVSMainHUD::OnConfigReceived);
			ApiClient->OnLoginComplete.AddDynamic(this, &AVSMainHUD::OnLoginComplete);

			// 显示加载界面
			ShowLoading();

			// 获取配置
			ApiClient->FetchClientConfig();
		}
	}
}

void AVSMainHUD::OnConfigReceived(bool bSuccess, const FVSClientConfig& Config)
{
	CachedConfig = Config;

	if (bSuccess)
	{
		UE_LOG(LogVirtualSpace, Log, TEXT("Config received. SkipLogin: %d, Levels: %d"), Config.bSkipLogin, Config.Levels.Num());

		if (Config.bSkipLogin)
		{
			// 跳过登录，直接显示关卡选择
			ShowLevelSelect();
		}
		else
		{
			// 显示登录界面
			ShowLogin();
		}
	}
	else
	{
		// 获取配置失败，显示登录界面
		UE_LOG(LogVirtualSpace, Warning, TEXT("Failed to get config, showing login"));
		ShowLogin();
	}
}

void AVSMainHUD::OnLoginRequested(const FString& Username, const FString& Password)
{
	if (LoginWidget)
	{
		LoginWidget->SetLoading(true);
	}
	
	if (ApiClient)
	{
		ApiClient->Login(Username, Password);
	}
}

void AVSMainHUD::OnLoginComplete(bool bSuccess, const FString& Message)
{
	if (LoginWidget)
	{
		LoginWidget->SetLoading(false);
	}

	if (bSuccess)
	{
		// 登录成功，获取配置后显示关卡
		ShowLevelSelect();
	}
	else
	{
		// 登录失败，显示错误
		if (LoginWidget)
		{
			LoginWidget->SetErrorMessage(Message);
		}
	}
}

void AVSMainHUD::OnLevelSelected(const FVSLevel& Level)
{
	UE_LOG(LogVirtualSpace, Log, TEXT("Loading level: %s"), *Level.Name);

	// 保存当前关卡到 GameInstance
	UVSGameInstance* GI = Cast<UVSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI)
	{
		GI->SetCurrentLevel(Level);
	}

	// TODO: 加载实际的关卡地图
	// UGameplayStatics::OpenLevel(GetWorld(), FName(*Level.MapName));
	
	// 暂时显示提示
	UE_LOG(LogVirtualSpace, Log, TEXT("Level '%s' selected. Implement map loading here."), *Level.Name);
}

void AVSMainHUD::ShowLoading()
{
	HideAllUI();

	LoadingWidget = CreateWidget<UVSLoadingWidget>(GetWorld(), UVSLoadingWidget::StaticClass());
	if (LoadingWidget)
	{
		LoadingWidget->AddToViewport();
	}

	SetupInputMode();
}

void AVSMainHUD::ShowLogin()
{
	HideAllUI();

	LoginWidget = CreateWidget<UVSLoginWidget>(GetWorld(), UVSLoginWidget::StaticClass());
	if (LoginWidget)
	{
		LoginWidget->OnLoginRequested.AddDynamic(this, &AVSMainHUD::OnLoginRequested);
		LoginWidget->AddToViewport();
	}

	SetupInputMode();
}

void AVSMainHUD::ShowLevelSelect()
{
	HideAllUI();

	LevelSelectWidget = CreateWidget<UVSLevelSelectWidget>(GetWorld(), UVSLevelSelectWidget::StaticClass());
	if (LevelSelectWidget)
	{
		LevelSelectWidget->OnLevelSelected.AddDynamic(this, &AVSMainHUD::OnLevelSelected);
		LevelSelectWidget->SetLevels(CachedConfig.Levels);
		LevelSelectWidget->AddToViewport();
	}

	SetupInputMode();
}

void AVSMainHUD::HideAllUI()
{
	if (LoadingWidget)
	{
		LoadingWidget->RemoveFromParent();
		LoadingWidget = nullptr;
	}
	if (LoginWidget)
	{
		LoginWidget->RemoveFromParent();
		LoginWidget = nullptr;
	}
	if (LevelSelectWidget)
	{
		LevelSelectWidget->RemoveFromParent();
		LevelSelectWidget = nullptr;
	}
}

void AVSMainHUD::SetupInputMode()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}
}
