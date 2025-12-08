# 自动同步脚本 - 双击运行即可
# 配置：修改下面的路径为你的本地项目路径
$LocalProjectPath = "E:\诧寂(241105)"
$GitHubRepo = "https://github.com/379005109-lab/xiaodiue5.git"
$TempDir = "$env:TEMP\xiaodiue5_sync"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  UE5 项目自动同步工具" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 检查本地项目路径
if (-not (Test-Path $LocalProjectPath)) {
    Write-Host "错误: 本地项目路径不存在: $LocalProjectPath" -ForegroundColor Red
    Write-Host "请修改脚本中的 LocalProjectPath 变量" -ForegroundColor Yellow
    pause
    exit 1
}

# 清理临时目录
if (Test-Path $TempDir) {
    Remove-Item -Recurse -Force $TempDir
}

Write-Host "[1/4] 正在从 GitHub 下载最新代码..." -ForegroundColor Yellow
git clone --depth 1 $GitHubRepo $TempDir 2>$null

if (-not (Test-Path "$TempDir\Source")) {
    Write-Host "错误: 下载失败，请检查网络连接" -ForegroundColor Red
    pause
    exit 1
}

Write-Host "[2/4] 正在备份本地 Source 文件夹..." -ForegroundColor Yellow
$BackupDir = "$LocalProjectPath\Source_backup_$(Get-Date -Format 'yyyyMMdd_HHmmss')"
if (Test-Path "$LocalProjectPath\Source") {
    Copy-Item -Recurse "$LocalProjectPath\Source" $BackupDir
}

Write-Host "[3/4] 正在同步代码到本地项目..." -ForegroundColor Yellow
# 只复制 诧寂1 文件夹的内容
$SourceDir = Get-ChildItem "$TempDir\Source" -Directory | Where-Object { $_.Name -like "*诧寂*" -or $_.Name -like "*1*" } | Select-Object -First 1
if ($SourceDir) {
    $TargetDir = Get-ChildItem "$LocalProjectPath\Source" -Directory | Where-Object { $_.Name -like "*诧寂*" -or $_.Name -like "*1*" } | Select-Object -First 1
    if ($TargetDir) {
        Copy-Item -Recurse -Force "$($SourceDir.FullName)\*" $TargetDir.FullName
        Write-Host "  已同步: $($SourceDir.Name) -> $($TargetDir.Name)" -ForegroundColor Green
    }
}

# 复制 Target 文件
Copy-Item -Force "$TempDir\Source\*.Target.cs" "$LocalProjectPath\Source\" -ErrorAction SilentlyContinue

Write-Host "[4/4] 清理临时文件..." -ForegroundColor Yellow
Remove-Item -Recurse -Force $TempDir -ErrorAction SilentlyContinue

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  同步完成！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "下一步操作:" -ForegroundColor Cyan
Write-Host "1. 打开 UE5 编辑器" -ForegroundColor White
Write-Host "2. 点击 文件 -> 刷新 Visual Studio 项目" -ForegroundColor White
Write-Host "3. 编译并运行" -ForegroundColor White
Write-Host ""
Write-Host "备份已保存到: $BackupDir" -ForegroundColor Gray
Write-Host ""
pause
