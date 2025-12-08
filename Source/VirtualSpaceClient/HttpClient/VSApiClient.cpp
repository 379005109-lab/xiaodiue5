#include "VSApiClient.h"
#include "VirtualSpaceClient.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UVSApiClient::UVSApiClient()
{
	// 默认 API 地址，可以通过配置文件或蓝图修改
	BaseUrl = TEXT("http://localhost:4000");
}

void UVSApiClient::SetBaseUrl(const FString& Url)
{
	BaseUrl = Url;
	UE_LOG(LogVirtualSpace, Log, TEXT("API Base URL set to: %s"), *BaseUrl);
}

void UVSApiClient::FetchClientConfig()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BaseUrl + TEXT("/api/client/config"));
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->OnProcessRequestComplete().BindUObject(this, &UVSApiClient::OnConfigResponseReceived);
	Request->ProcessRequest();

	UE_LOG(LogVirtualSpace, Log, TEXT("Fetching client config from: %s"), *Request->GetURL());
}

void UVSApiClient::OnConfigResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bool bSuccess = false;

	if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
	{
		FString ResponseStr = Response->GetContentAsString();
		
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			if (JsonObject->GetBoolField(TEXT("success")))
			{
				TSharedPtr<FJsonObject> DataObj = JsonObject->GetObjectField(TEXT("data"));
				if (DataObj.IsValid())
				{
					// 解析 skipLogin
					ClientConfig.bSkipLogin = DataObj->GetBoolField(TEXT("skipLogin"));

					// 解析关卡列表
					ClientConfig.Levels.Empty();
					const TArray<TSharedPtr<FJsonValue>>* LevelsArray;
					if (DataObj->TryGetArrayField(TEXT("levels"), LevelsArray))
					{
						for (const auto& LevelValue : *LevelsArray)
						{
							TSharedPtr<FJsonObject> LevelObj = LevelValue->AsObject();
							if (LevelObj.IsValid())
							{
								FVSLevel Level;
								Level.Id = LevelObj->GetIntegerField(TEXT("id"));
								Level.Name = LevelObj->GetStringField(TEXT("name"));
								Level.Description = LevelObj->GetStringField(TEXT("description"));
								Level.Thumbnail = LevelObj->GetStringField(TEXT("thumbnail"));
								Level.PakUrl = LevelObj->GetStringField(TEXT("pakUrl"));
								Level.PakVersion = LevelObj->GetStringField(TEXT("pakVersion"));

								// 解析分类
								const TArray<TSharedPtr<FJsonValue>>* CategoriesArray;
								if (LevelObj->TryGetArrayField(TEXT("categories"), CategoriesArray))
								{
									for (const auto& CatValue : *CategoriesArray)
									{
										TSharedPtr<FJsonObject> CatObj = CatValue->AsObject();
										if (CatObj.IsValid())
										{
											FVSCategory Category;
											Category.Id = CatObj->GetIntegerField(TEXT("id"));
											Category.Name = CatObj->GetStringField(TEXT("name"));
											Category.Icon = CatObj->GetStringField(TEXT("icon"));

											// 解析视角
											const TArray<TSharedPtr<FJsonValue>>* ViewpointsArray;
											if (CatObj->TryGetArrayField(TEXT("viewpoints"), ViewpointsArray))
											{
												for (const auto& VpValue : *ViewpointsArray)
												{
													TSharedPtr<FJsonObject> VpObj = VpValue->AsObject();
													if (VpObj.IsValid())
													{
														FVSViewpoint Viewpoint;
														Viewpoint.Name = VpObj->GetStringField(TEXT("name"));
														Viewpoint.Position = VpObj->GetStringField(TEXT("position"));
														Viewpoint.Rotation = VpObj->GetStringField(TEXT("rotation"));
														Viewpoint.bIsDefault = VpObj->GetBoolField(TEXT("isDefault"));
														Category.Viewpoints.Add(Viewpoint);
													}
												}
											}
											Level.Categories.Add(Category);
										}
									}
								}
								ClientConfig.Levels.Add(Level);
							}
						}
					}

					// 解析系统配置
					ClientConfig.SystemConfig.Empty();
					TSharedPtr<FJsonObject> SysConfigObj = DataObj->GetObjectField(TEXT("systemConfig"));
					if (SysConfigObj.IsValid())
					{
						for (const auto& Pair : SysConfigObj->Values)
						{
							ClientConfig.SystemConfig.Add(Pair.Key, Pair.Value->AsString());
						}
					}

					bSuccess = true;
					UE_LOG(LogVirtualSpace, Log, TEXT("Client config loaded: SkipLogin=%d, Levels=%d"), 
						ClientConfig.bSkipLogin, ClientConfig.Levels.Num());
				}
			}
		}
	}
	else
	{
		UE_LOG(LogVirtualSpace, Error, TEXT("Failed to fetch client config. Code: %d"), 
			Response.IsValid() ? Response->GetResponseCode() : -1);
	}

	OnConfigReceived.Broadcast(bSuccess, ClientConfig);
}

void UVSApiClient::Login(const FString& Username, const FString& Password)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BaseUrl + TEXT("/api/auth/login"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// 构建 JSON body
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("username"), Username);
	JsonObject->SetStringField(TEXT("password"), Password);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &UVSApiClient::OnLoginResponseReceived);
	Request->ProcessRequest();

	UE_LOG(LogVirtualSpace, Log, TEXT("Logging in as: %s"), *Username);
}

void UVSApiClient::OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bool bSuccess = false;
	FString Message;

	if (bWasSuccessful && Response.IsValid())
	{
		FString ResponseStr = Response->GetContentAsString();

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			if (JsonObject->GetBoolField(TEXT("success")))
			{
				TSharedPtr<FJsonObject> DataObj = JsonObject->GetObjectField(TEXT("data"));
				if (DataObj.IsValid())
				{
					AuthToken = DataObj->GetStringField(TEXT("token"));
					bSuccess = true;
					Message = TEXT("登录成功");
					UE_LOG(LogVirtualSpace, Log, TEXT("Login successful"));
				}
			}
			else
			{
				Message = JsonObject->GetStringField(TEXT("message"));
			}
		}
	}
	else
	{
		Message = TEXT("网络连接失败");
	}

	OnLoginComplete.Broadcast(bSuccess, Message);
}
