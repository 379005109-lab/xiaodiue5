# 一键同步和编译脚本
# 使用方法: 在 PowerShell 中运行此脚本

$ProjectPath = "E:\1chaji"
$RepoUrl = "https://github.com/379005109-lab/xiaodiue5/archive/refs/heads/main.zip"
$UBTPath = "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  自动同步和编译脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# 进入项目目录
Set-Location $ProjectPath
Write-Host "`n[1/6] 进入项目目录: $ProjectPath" -ForegroundColor Yellow

# 下载最新代码
Write-Host "`n[2/6] 下载最新代码..." -ForegroundColor Yellow
$TempZip = Join-Path $ProjectPath "temp_update.zip"
$TempDir = Join-Path $ProjectPath "xiaodiue5-main"

try {
    Invoke-WebRequest -Uri $RepoUrl -OutFile $TempZip -ErrorAction Stop
    Write-Host "  下载完成!" -ForegroundColor Green
} catch {
    Write-Host "  下载失败: $_" -ForegroundColor Red
    exit 1
}

# 解压
Write-Host "`n[3/6] 解压并更新文件..." -ForegroundColor Yellow
Expand-Archive -Path $TempZip -DestinationPath $ProjectPath -Force

# 更新 Source 文件夹
$SourceDir = Join-Path $TempDir "Source\chaji"
$TargetSourceDir = Join-Path $ProjectPath "Source\chaji"

if (Test-Path $SourceDir) {
    Copy-Item -Path "$SourceDir\*" -Destination $TargetSourceDir -Recurse -Force
    Write-Host "  Source 文件夹已更新!" -ForegroundColor Green
}

# 更新 Python 脚本
$PythonScript = Join-Path $TempDir "AutoConfigureCameras.py"
if (Test-Path $PythonScript) {
    Copy-Item -Path $PythonScript -Destination $ProjectPath -Force
    Write-Host "  Python 脚本已更新!" -ForegroundColor Green
}

# 清理临时文件
Remove-Item $TempZip -Force -ErrorAction SilentlyContinue
Remove-Item $TempDir -Recurse -Force -ErrorAction SilentlyContinue
Write-Host "  临时文件已清理!" -ForegroundColor Green

# 删除旧的编译文件和解决方案
Write-Host "`n[4/6] 清理旧的编译文件..." -ForegroundColor Yellow
Remove-Item "$ProjectPath\*.sln" -Force -ErrorAction SilentlyContinue
Remove-Item "$ProjectPath\Binaries" -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item "$ProjectPath\Intermediate" -Recurse -Force -ErrorAction SilentlyContinue
Write-Host "  清理完成!" -ForegroundColor Green

# 重新生成项目文件
Write-Host "`n[5/6] 重新生成项目文件..." -ForegroundColor Yellow
$UProjectFile = Join-Path $ProjectPath "chaji.uproject"

if (Test-Path $UBTPath) {
    & $UBTPath -projectfiles -project="$UProjectFile" -game -engine -2022
    Write-Host "  项目文件已生成!" -ForegroundColor Green
} else {
    Write-Host "  找不到 UnrealBuildTool，请手动生成项目文件" -ForegroundColor Red
}

# 编译项目
Write-Host "`n[6/6] 编译项目..." -ForegroundColor Yellow
$MSBuildPath = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
$SlnFile = Join-Path $ProjectPath "chaji.sln"

if (Test-Path $SlnFile) {
    if (Test-Path $MSBuildPath) {
        & $MSBuildPath $SlnFile /t:Build /p:Configuration="Development Editor" /p:Platform=Win64 /m
        Write-Host "`n  编译完成!" -ForegroundColor Green
    } else {
        Write-Host "  找不到 MSBuild，请手动在 Visual Studio 中编译" -ForegroundColor Yellow
        Write-Host "  打开 $SlnFile 并按 Ctrl+Shift+B" -ForegroundColor Yellow
    }
} else {
    Write-Host "  找不到解决方案文件，请检查项目文件生成是否成功" -ForegroundColor Red
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "  同步完成!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "`n下一步操作:" -ForegroundColor Yellow
Write-Host "1. 打开 UE5 编辑器" -ForegroundColor White
Write-Host "2. 在输出日志中运行: py E:/1chaji/AutoConfigureCameras.py" -ForegroundColor White
Write-Host "3. 设置 GameMode 为 ViewerGameMode" -ForegroundColor White
Write-Host "4. 按 Alt+P 测试" -ForegroundColor White
