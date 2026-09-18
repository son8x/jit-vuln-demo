@echo off
chcp 65001 >nul
REM =============================================================================
REM  scripts/demo_02_buffer_overflow.bat - Demo kich ban 2: Buffer Overflow
REM =============================================================================
REM  Kich ban nay minh hoa:
REM    - Loi Buffer Overflow (CWE-120)
REM    - Commit co loi se bi pre-commit hook CHAN
REM    - Commit da sua se duoc hook CHO PHEP
REM
REM  Cach dung: Double-click file nay, hoac chay tu cmd:
REM    cd github_demo
REM    scripts\demo_02_buffer_overflow.bat
REM =============================================================================

setlocal enabledelayedexpansion

REM ===== CAU HINH =====
set SO_KICH_BAN=2
set TEN_KICH_BAN=Buffer Overflow (CWE-120)
set CWE=CWE-120
set XEP_HANG=Tier 1 - kinh dien
set CVE_LIEN_QUAN=(kinh dien)

REM Duong dan - dung %~dp0 de lay thu muc scripts/
set SCRIPT_DIR=%~dp0
set REPO_ROOT=%SCRIPT_DIR%..

REM Xac dinh ten file (mot so kich ban co ten rieng)
set FILE_VULN=%REPO_ROOT%\scenarios\02\str_vuln.c
set FILE_SAFE=%REPO_ROOT%\scenarios\02\str_safe.c

REM ===== BAT DAU =====
echo.
echo +================================================================+
echo  KICH BAN %SO_KICH_BAN%: %TEN_KICH_BAN%
echo +================================================================+
echo  Ma CWE:        %CWE%
echo  Xep hang:      %XEP_HANG%
echo  CVE lien quan: %CVE_LIEN_QUAN%
echo +================================================================+
echo.

REM Kiem tra git repo
git rev-parse --show-toplevel >nul 2>&1
if errorlevel 1 (
    echo [LOI] Thu muc hien tai khong phai git repository!
    pause
    exit /b 1
)

REM Kiem tra file co loi va file an toan
if not exist "%FILE_VULN%" (
    echo [LOI] Khong tim thay file co loi: %FILE_VULN%
    pause
    exit /b 1
)
if not exist "%FILE_SAFE%" (
    echo [LOI] Khong tim thay file an toan: %FILE_SAFE%
    pause
    exit /b 1
)

REM ===== BUOC 1/3: Commit code co loi =====
echo === BUOC 1/3: COMMIT CODE CO LOI ===
echo.
echo [1.1] Copy file co loi: scenarios\02\str_vuln.c -^> auth.c
copy /Y "%FILE_VULN%" "%REPO_ROOT%\auth.c" >nul
if errorlevel 1 (
    echo [LOI] Khong the sao chep file.
    pause
    exit /b 1
)
echo       Da sao chep xong.
echo.
echo [1.2] Hien thi diff (toi da 12 dong dau):
echo       ----------------------------------------
git --no-pager diff -- "%REPO_ROOT%\auth.c" 2>nul > "%TEMP%\demo_diff.txt"
if exist "%TEMP%\demo_diff.txt" (
    set /a LINE_NUM=0
    for /f "usebackq delims=" %%d in ("%TEMP%\demo_diff.txt") do (
        set /a LINE_NUM+=1
        if !LINE_NUM! leq 12 echo       %%d
    )
    del "%TEMP%\demo_diff.txt" 2>nul
) else (
    echo       (khong co diff)
)
echo       ----------------------------------------
echo.
echo [1.3] git add auth.c
git add "%REPO_ROOT%\auth.c" 2>nul
echo.
echo [1.4] git commit -m "add buffer_overflow vulnerability"
echo       (Hook se tu dong chay SCGL de phan tich...)
git commit -m "add buffer_overflow vulnerability" 2>&1
set RC=%errorlevel%
echo.
if %RC% neq 0 (
    echo +================================================================+
    echo  [DUNG] Hook da CHAN commit co loi!
    echo +================================================================+
) else (
    echo +================================================================+
    echo  [SAI] Hook KHONG chan duoc commit co loi!
    echo +================================================================+
)
echo.

REM ===== BUOC 2/3: Commit code da sua =====
echo === BUOC 2/3: COMMIT CODE DA SUA (an toan) ===
echo.
echo [2.1] Copy file an toan: scenarios\02\str_safe.c -^> auth.c
copy /Y "%FILE_SAFE%" "%REPO_ROOT%\auth.c" >nul
echo       Da sao chep xong.
echo.
echo [2.2] git add auth.c
git add "%REPO_ROOT%\auth.c" 2>nul
echo.
echo [2.3] git commit -m "fix buffer_overflow - use safe pattern"
echo       (Hook se tu dong chay SCGL de phan tich...)
git commit -m "fix buffer_overflow - use safe pattern" 2>&1
set RC=%errorlevel%
echo.
if %RC% equ 0 (
    echo +================================================================+
    echo  [DUNG] Hook CHO PHEP commit an toan!
    echo +================================================================+
    echo.
    echo  Commit hien tai:
    git rev-parse --short HEAD
) else (
    echo +================================================================+
    echo  [SAI] Hook chan nham commit an toan!
    echo +================================================================+
)
echo.

REM ===== BUOC 3/3: Push len GitHub =====
echo === BUOC 3/3: PUSH LEN GITHUB (tuy chon) ===
echo.
set /p PUSH="  Ban co muon push commit len GitHub? (y/N): "
if /i "%PUSH%"=="y" (
    git push origin main
    echo [OK] Da push len GitHub
) else (
    echo [BO QUA] Ban co the push sau bang lenh: git push origin main
)
echo.

REM ===== KET THUC =====
echo +================================================================+
echo  HOAN THANH KICH BAN %SO_KICH_BAN%: %TEN_KICH_BAN%
echo +================================================================+
echo.
pause
