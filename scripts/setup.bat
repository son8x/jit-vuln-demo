@echo off
chcp 65001 >nul
REM =============================================================================
REM  scripts/setup.bat - Cai dat ban dau cho repo GitHub demo
REM =============================================================================
REM  Script chay trong thu muc scripts/ cua repo github_demo
REM
REM  Se thuc hien:
REM    1. Kiem tra git repo
REM    2. Tao file .env voi duong dan SCGL
REM    3. Copy hook Python vao .git/hooks/pre-commit
REM    4. Cau hinh git user
REM    5. Huong dan commit + push
REM =============================================================================

setlocal enabledelayedexpansion

echo.
echo +================================================================+
echo  CAI DAT REPO DEMO JIT VULNERABILITY DETECTION
echo +================================================================+
echo.

REM Kiem tra git repo
git rev-parse --show-toplevel >nul 2>&1
if errorlevel 1 (
    echo [LOI] Thu muc hien tai khong phai git repository!
    echo Hay cd vao thu muc github_demo truoc.
    pause
    exit /b 1
)

REM Lay duong dan
for /f "delims=" %%i in ('git rev-parse --show-toplevel') do set REPO_ROOT=%%i
for %%i in ("%REPO_ROOT%") do set DEMO_PARENT=%%~dpi..

REM Cac duong dan co dinh
set SCGL_DIR=%DEMO_PARENT%\SCGL
set PYTHON_EXE=%SCGL_DIR%\.venv\Scripts\python.exe
set HOOK_SRC=%DEMO_PARENT%\server\pre-commit_hook.py
set HOOK_DST=%REPO_ROOT%\.git\hooks\pre-commit
set ENV_FILE=%REPO_ROOT%\.env

echo [INFO] Repo:        %REPO_ROOT%
echo [INFO] Demo dir:    %DEMO_PARENT%
echo [INFO] SCGL dir:    %SCGL_DIR%
echo.

REM Kiem tra SCGL ton tai
if not exist "%PYTHON_EXE%" (
    echo [LOI] Khong tim thay Python tai: %PYTHON_EXE%
    echo Hay kiem tra duong dan SCGL.
    pause
    exit /b 1
)
echo [OK] Tim thay Python virtualenv

REM Kiem tra hook source
if not exist "%HOOK_SRC%" (
    echo [LOI] Khong tim thay hook Python: %HOOK_SRC%
    echo Vi tri mong doi: code\demo\server\pre-commit_hook.py
    pause
    exit /b 1
)

REM Tao file .env
echo [1/4] Tao file .env...
(
    echo # Cau hinh cho pre-commit hook - JIT Vulnerability Detection
    echo # Tu dong tao boi setup.bat
    echo JIT_VULN_DEMO_DIR=%DEMO_PARENT%
    echo JIT_VULN_SCGL_DIR=%SCGL_DIR%
    echo JIT_VULN_PYTHON=%PYTHON_EXE%
) > "%ENV_FILE%"
echo       OK
echo.

REM Copy hook
echo [2/4] Copy pre-commit hook Python...
copy /Y "%HOOK_SRC%" "%HOOK_DST%" >nul
echo       OK
echo.

REM Kiem tra .gitignore
echo [3/4] Cap nhat .gitignore...
findstr /C:".env" "%REPO_ROOT%\.gitignore" >nul 2>&1
if errorlevel 1 (
    echo. >> "%REPO_ROOT%\.gitignore"
    echo .env >> "%REPO_ROOT%\.gitignore"
    echo       Da them .env
) else (
    echo       .env da co trong .gitignore
)
echo.

REM Cau hinh git user
echo [4/4] Cau hinh git user...
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

REM Hoan thanh
echo +================================================================+
echo  CAI DAT HOAN THANH!
echo +================================================================+
echo.
echo Cac buoc tiep theo (neu chua lam):
echo   1. Commit file dau tien:
echo      git add . ^&^& git commit -m "init: add original auth.c" ^&^& git push origin main
echo.
echo   2. Chay 1 kich ban cu the:
echo      scripts\demo_01_sql_injection.bat
echo.
echo   3. Chay tat ca 10 kich ban tu dong:
echo      scripts\run_all.bat
echo.
pause
