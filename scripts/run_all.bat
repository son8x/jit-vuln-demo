@echo off
chcp 65001 >nul
REM =============================================================================
REM  scripts/run_all.bat - Chay tat ca 10 kich ban tu dong
REM =============================================================================
REM  Script se hoi:
REM    1. Co muon push len GitHub sau moi kich ban khong?
REM    2. Co bat dau chay khong?
REM  Sau do lan luot chay 10 file demo_XX_*.bat
REM =============================================================================

setlocal enabledelayedexpansion

REM Xac dinh thu muc
set SCRIPT_DIR=%~dp0
set REPO_ROOT=%SCRIPT_DIR%..

echo.
echo +================================================================+
echo  CHAY TAT CA 10 KICH BAN TU DONG
echo  He thong phat hien lo hong tuc thoi voi SCGL
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

REM Tao commit dau tien neu chua co
git log --oneline 2>nul | findstr "init: add" >nul
if errorlevel 1 (
    echo [INFO] Chua co commit "init" - tao commit dau tien voi file goc
    if not exist auth.c (
        copy /Y "source\\auth_original.c" "auth.c" >nul
    )
    git add auth.c source\\auth_original.c 2>nul
    git commit -q -m "init: add original auth.c (safe baseline)"
    echo       OK - da tao commit dau tien
    echo.
)

REM Hoi push
echo === CAU HINH ===
echo.
set /p AUTO_PUSH="  Tu dong push len GitHub sau moi kich ban? (y/N): "
set /p START_NOW="  Bat dau chay ngay? (y/N): "
if /i not "%START_NOW%"=="y" (
    echo.
    echo Da huy. Ban co the chay lai sau.
    pause
    exit /b 0
)

echo.
echo +================================================================+
echo  BAT DAU CHAY 10 KICH BAN
echo +================================================================+

REM Reset auth.c ve ban goc
copy /Y "source\\auth_original.c" "auth.c" >nul

REM Chay tung kich ban
for /f "delims=" %%f in ('dir /b "%SCRIPT_DIR%demo_*.bat" 2^>nul') do (
    echo.
    echo --- Dang chay: %%f ---
    call "%SCRIPT_DIR%%%f" <nul
    if /i "%AUTO_PUSH%"=="y" (
        git push origin main 2>&1 | findstr /v "^$"
    )
    echo.
)

REM Tong ket
echo.
echo +================================================================+
echo  HOAN THANH TAT CA 10 KICH BAN
echo +================================================================+
echo.
echo Lich su commit gan day:
git --no-pager log --oneline -20 2>nul
echo.
echo Ban co the truy cap GitHub repo de xem ket qua:
echo   https://github.com/son8x/jit-vuln-demo
echo.
pause
