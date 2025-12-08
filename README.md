# VirtualSpace Client - UE 5.4 客户端

## 项目说明

这是 VirtualSpace 虚拟空间的 UE5 客户端项目，支持：
- 从后台 API 获取配置
- 根据配置决定是否跳过登录（skipLogin）
- 关卡选择界面
- 关卡加载

## 快速开始

### 1. 打开项目
双击 `VirtualSpaceClient.uproject` 用 UE 5.4 打开项目，等待编译完成。

### 2. 创建主菜单地图
1. File → New Level → Empty Level
2. 保存为 `Content/Maps/MainMenu`

### 3. 配置 API 地址
打开 `Source/VirtualSpaceClient/GameInstance/VSGameInstance.h`，修改：
```cpp
FString ApiBaseUrl = TEXT("http://你的服务器地址:4000");
```

### 4. 运行
点击 Play 即可运行，UI 会自动显示。

## 核心类说明

| 类名 | 说明 |
|------|------|
| `UVSApiClient` | HTTP API 客户端，处理网络请求 |
| `UVSGameInstance` | 游戏实例，管理全局状态 |
| `AVSMainHUD` | 主 HUD，控制 UI 切换逻辑 |
| `FVSLevel` | 关卡数据结构 |
| `FVSClientConfig` | 客户端配置数据 |

## 工作流程

```
启动 → 显示Loading → 请求 /api/client/config
                          ↓
                    skipLogin = true? 
                    ↓           ↓
                   是           否
                    ↓           ↓
              关卡选择界面    登录界面
                    ↓           ↓
              选择关卡     登录成功
                    ↓           ↓
              加载场景 ←←←←←←←←↓
```

## API 端点

| 端点 | 方法 | 说明 |
|------|------|------|
| `/api/client/config` | GET | 获取客户端配置（不需要登录）|
| `/api/auth/login` | POST | 用户登录 |
| `/api/sync/config` | GET | 同步完整配置（需要登录）|

## 编译

1. 在 UE 编辑器中打开项目
2. File → Package Project → Windows (64-bit)
