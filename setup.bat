@echo off
REM =============================================================================
REM  setup.bat - Cai dat moi thu cho repo GitHub demo
REM =============================================================================
REM  Script nay se:
REM    1. Kiem tra git repo
REM    2. Cai pre-commit hook (copy tu code/demo/server)
REM    3. Tao file .env voi duong dan SCGL
REM    4. Cau hinh git user (neu chua co)
REM    5. Commit va push lan dau tien
REM =============================================================================

setlocal enabledelayedexpansion
chcp 65001 >nul

echo ============================================================
echo  SETUP REPO DEMO JIT VULNERABILITY DETECTION
echo ============================================================
echo.

REM Kiem tra git repo
git rev-parse --show-toplevel >nul 2>&1
if errorlevel 1 (
    echo [LOI] Thu muc hien tai khong phai git repository!
    echo Hay clone repo truoc:
    echo   git clone https://github.com/son8x/jit-vuln-demo.git
    echo   cd jit-vuln-demo
    pause
    exit /b 1
)

REM Lay duong dan repo
for /f "delims=" %%i in ('git rev-parse --show-toplevel') do set REPO_ROOT=%%i
echo [INFO] Repo: %REPO_ROOT%
echo.

REM Tu dong phat hien SCGL va Demo
set SCGL_DIR=
set DEMO_DIR=

REM Tim SCGL di tu thu muc hien tai
if exist "..\01-LuanVan\code\SCGL\.venv\Scripts\python.exe" (
    set SCGL_DIR=..\01-LuanVan\code\SCGL
    set DEMO_DIR=..\01-LuanVan\code\demo
)
if exist "..\..\01-LuanVan\code\SCGL\.venv\Scripts\python.exe" (
    set SCGL_DIR=..\..\01-LuanVan\code\SCGL
    set DEMO_DIR=..\..\01-LuanVan\code\demo
)
if exist "..\..\..\01-LuanVan\code\SCGL\.venv\Scripts\python.exe" (
    set SCGL_DIR=..\..\..\01-LuanVan\code\SCGL
    set DEMO_DIR=..\..\..\01-LuanVan\code\demo
)

if "%SCGL_DIR%"=="" (
    echo [CANH BAO] Khong tu dong tim thay SCGL.
    echo Vi tri mac dinh se duoc thu:
    echo   D:\Cowork_Workspapce\Projects\01-LuanVan\code\SCGL
    echo.
    set /p SCGL_DIR="Nhap duong dan toi SCGL (Enter de dung mac dinh): "
    if "%SCGL_DIR%"=="" set "SCGL_DIR=D:\Cowork_Workspapce\Projects\01-LuanVan\code\SCGL"
    set "DEMO_DIR=%SCGL_DIR%\..\demo"
)

REM Chuyen thanh duong dan day du
for %%i in ("%SCGL_DIR%") do set SCGL_DIR_FULL=%%~fi
for %%i in ("%DEMO_DIR%") do set DEMO_DIR_FULL=%%~fi

echo [INFO] SCGL: %SCGL_DIR_FULL%
echo [INFO] Demo: %DEMO_DIR_FULL%
echo.

REM Kiem tra SCGL co ton tai khong
if not exist "%SCGL_DIR_FULL%\.venv\Scripts\python.exe" (
    echo [LOI] Khong tim thay Python trong SCGL\.venv
    echo Hay kiem tra duong dan: %SCGL_DIR_FULL%
    pause
    exit /b 1
)
echo [OK] Tim thay Python virtualenv

REM Copy hook
echo [1/5] Copy pre-commit hook...
set HOOK_SRC=%DEMO_DIR_FULL%\server\git_hook_template.sh
set HOOK_DST=%REPO_ROOT%\.git\hooks\pre-commit
if not exist "%HOOK_SRC%" (
    echo [LOI] Khong tim thay hook template: %HOOK_SRC%
    pause
    exit /b 1
)
copy /Y "%HOOK_SRC%" "%HOOK_DST%" >nul
echo       OK
echo.

REM Tao .env
echo [2/5] Tao file .env...
(
    echo # Cau hinh cho pre-commit hook
    echo # Tu dong tao boi setup.bat
    echo JIT_VULN_DEMO_DIR=%DEMO_DIR_FULL%
    echo JIT_VULN_SCGL_DIR=%SCGL_DIR_FULL%
    echo JIT_VULN_PYTHON=%SCGL_DIR_FULL%\.venv\Scripts\python.exe
) > "%REPO_ROOT%\.env"
echo       OK
echo.

REM Kiem tra .gitignore
echo [3/5] Cap nhat .gitignore...
findstr /C:".env" "%REPO_ROOT%\.gitignore" >nul 2>&1
if errorlevel 1 (
    echo. >> "%REPO_ROOT%\.gitignore"
    echo .env >> "%REPO_ROOT%\.gitignore"
    echo       Da them .env
) else (
    echo       .env da co
)
echo.

REM Cau hinh git user
echo [4/5] Cau hinh git user...
git config user.name >nul 2>&1
if errorlevel 1 (
    set /p GIT_NAME="Nhap ten hien thi (vd: Nguyen Hai Son): "
    git config user.name "!GIT_NAME!"
)
git config user.email >nul 2>&1
if errorlevel 1 (
    set /p GIT_EMAIL="Nhap email GitHub: "
    git config user.email "!GIT_EMAIL!"
)
echo       Git user: 
git config user.name
git config user.email
echo.

REM Commit va push
echo [5/5] Commit va push len GitHub...
git add .
git status --short
echo.
set /p CONFIRM="Commit va push? (y/N): "
if /i "%CONFIRM%"=="y" (
    git commit -m "init: add demo files for JIT vulnerability detection"
    git push origin main
    echo       OK
) else (
    echo       [SKIP] Ban co the chay 'git add . ^&^& git commit -m "init"' sau
)
echo.

echo ============================================================
echo  SETUP HOAN THANH!
echo ============================================================
echo.
echo Buoc tiep theo:
echo   1. Chay 1 kich ban cu the:
echo      demo_01_sql_injection.bat
echo.
echo   2. Chay TAT CA 10 kich ban tu dong:
echo      run_all.bat
echo.
echo   3. Hoac chay Python script:
echo      ..\01-LuanVan\code\SCGL\.venv\Scripts\python.exe auto_demo.py
echo.
pause
