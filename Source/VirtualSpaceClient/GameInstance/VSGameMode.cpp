#include "VSGameMode.h"
#include "UI/VSMainHUD.h"

AVSGameMode::AVSGameMode()
{
	// 设置默认 HUD 类
	HUDClass = AVSMainHUD::StaticClass();
}
