@echo off
REM ============================================================
REM  install_hook.bat - Cai dat pre-commit hook vao repo GitHub
REM ============================================================
REM
REM  Su dung:
REM    1. Clone repo GitHub ve may (git clone <URL>)
REM    2. Copy file nay vao thu muc goc cua repo
REM    3. Chay: install_hook.bat
REM
REM  Script se:
REM    - Copy git_hook_template.sh vao .git/hooks/pre-commit
REM    - Tao file .env voi duong dan toi SCGL model
REM    - Cau hinh git user (neu chua co)
REM
REM ============================================================

setlocal enabledelayedexpansion

echo ============================================================
echo  CAI DAT PRE-COMMIT HOOK
echo ============================================================
echo.

REM Kiem tra co phai git repo khong
git rev-parse --show-toplevel >nul 2>&1
if errorlevel 1 (
    echo [LOI] Thu muc hien tai khong phai git repository!
    echo Hay cd vao thu muc repo truoc khi chay script nay.
    pause
    exit /b 1
)

REM Lay duong dan repo
for /f "delims=" %%i in ('git rev-parse --show-toplevel') do set REPO_ROOT=%%i
echo [INFO] Repo root: %REPO_ROOT%
echo.

REM Tim SCGL model (tim trong thu muc me)
set SCGL_DIR=
set DEMO_DIR=

REM Thu tim o mot so vi tri thuong gap
if exist "..\..\code\SCGL\.venv\Scripts\python.exe" (
    set SCGL_DIR=..\..\code\SCGL
    set DEMO_DIR=..\..\code\demo
    echo [INFO] Tim thay SCGL tai: !SCGL_DIR!
) else if exist "..\code\SCGL\.venv\Scripts\python.exe" (
    set SCGL_DIR=..\code\SCGL
    set DEMO_DIR=..\code\demo
    echo [INFO] Tim thay SCGL tai: !SCGL_DIR!
) else if exist "..\..\..\code\SCGL\.venv\Scripts\python.exe" (
    set SCGL_DIR=..\..\..\code\SCGL
    set DEMO_DIR=..\..\..\code\demo
    echo [INFO] Tim thay SCGL tai: !SCGL_DIR!
) else (
    echo [CANH BAO] Khong tu dong tim thay SCGL model.
    echo Vui long nhap duong dan toi thu muc SCGL:
    set /p SCGL_DIR="SCGL dir: "
    set DEMO_DIR=!SCGL_DIR!\..\demo
)

REM Chuyen thanh duong dan Windows day du
for %%i in ("%SCGL_DIR%") do set SCGL_DIR_FULL=%%~fi
for %%i in ("%DEMO_DIR%") do set DEMO_DIR_FULL=%%~fi

echo [INFO] SCGL dir (full): %SCGL_DIR_FULL%
echo [INFO] Demo dir (full): %DEMO_DIR_FULL%
echo.

REM Copy hook
set HOOK_SRC=%DEMO_DIR_FULL%\server\git_hook_template.sh
set HOOK_DST=%REPO_ROOT%\.git\hooks\pre-commit

if not exist "%HOOK_SRC%" (
    echo [LOI] Khong tim thay hook template: %HOOK_SRC%
    echo Hay kiem tra lai duong dan demo dir.
    pause
    exit /b 1
)

echo [1/4] Copy hook vao .git/hooks/pre-commit ...
copy /Y "%HOOK_SRC%" "%HOOK_DST%" >nul
REM Tren Windows, hook bash can co bash.exe de chay
REM Hook se duoc goi qua Git Bash, nen can chmod neu co the
git update-info-refs >nul 2>&1
echo       OK
echo.

REM Tao file .env voi cac bien moi truong
set ENV_FILE=%REPO_ROOT%\.env
echo [2/4] Tao file .env ...
(
    echo # Cau hinh cho pre-commit hook - JIT Vulnerability Detection
    echo # File nay duoc tu dong tao boi install_hook.bat
    echo.
    echo JIT_VULN_DEMO_DIR=%DEMO_DIR_FULL%
    echo JIT_VULN_SCGL_DIR=%SCGL_DIR_FULL%
    echo JIT_VULN_PYTHON=%SCGL_DIR_FULL%\.venv\Scripts\python.exe
) > "%ENV_FILE%"
echo       Da tao: %ENV_FILE%
echo.

REM Them .env vao .gitignore de khong commit file nay
set GITIGNORE=%REPO_ROOT%\.gitignore
echo [3/4] Cap nhat .gitignore ...
if not exist "%GITIGNORE%" (
    echo # Files khong can commit > "%GITIGNORE%"
)
findstr /C:".env" "%GITIGNORE%" >nul 2>&1
if errorlevel 1 (
    echo. >> "%GITIGNORE%"
    echo # Cau hinh local - khong commit >> "%GITIGNORE%"
    echo .env >> "%GITIGNORE%"
    echo       Da them .env vao .gitignore
) else (
    echo       .env da co trong .gitignore
)
echo.

REM Cau hinh git user
echo [4/4] Cau hinh git user ...
git config user.name >nul 2>&1
if errorlevel 1 (
    echo Chua co git user.name. Vui long nhap:
    set /p GIT_NAME="Ten hien thi (vd: Nguyen Hai Son): "
    git config user.name "!GIT_NAME!"
)
git config user.email >nul 2>&1
if errorlevel 1 (
    echo Chua co git user.email. Vui long nhap:
    set /p GIT_EMAIL="Email GitHub: "
    git config user.email "!GIT_EMAIL!"
)
echo       Git user: %GIT_NAME% ^<%GIT_EMAIL%^>
echo.

echo ============================================================
echo  HOAN THANH CAI DAT!
echo ============================================================
echo.
echo Buoc tiep theo:
echo   1. Them tat ca file vao git:
echo      git add .
echo      git commit -m "init: add demo files"
echo.
echo   2. Day len GitHub:
echo      git push origin main
echo.
echo   3. Chay demo tu dong:
echo      python auto_demo.py
echo.
echo   4. Hoac chay tung kich ban bang tay (xem README.md)
echo.
pause
