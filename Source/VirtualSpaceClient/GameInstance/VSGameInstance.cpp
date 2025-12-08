#include "VSGameInstance.h"
#include "VirtualSpaceClient.h"

UVSGameInstance::UVSGameInstance()
{
}

void UVSGameInstance::Init()
{
	Super::Init();

	// 创建 API 客户端
	ApiClient = NewObject<UVSApiClient>(this);
	ApiClient->SetBaseUrl(ApiBaseUrl);

	UE_LOG(LogVirtualSpace, Log, TEXT("VSGameInstance initialized with API URL: %s"), *ApiBaseUrl);
}

void UVSGameInstance::SetCurrentLevel(const FVSLevel& Level)
{
	CurrentLevel = Level;
	UE_LOG(LogVirtualSpace, Log, TEXT("Current level set to: %s (ID: %d)"), *Level.Name, Level.Id);
}
