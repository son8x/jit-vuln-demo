@echo off
REM =============================================================================
REM  run_all.bat - Chay tat ca 10 kich ban lien tiep
REM =============================================================================
REM  Luu y: Can phai chay setup.bat truoc khi chay file nay!
REM =============================================================================

setlocal enabledelayedexpansion
chcp 65001 >nul

echo ============================================================
echo  CHAY TAT CA 10 KICH BAN TU DONG
echo ============================================================
echo.

REM Kiem tra git repo
git rev-parse --show-toplevel >nul 2>&1
if errorlevel 1 (
    echo [LOI] Khong phai git repo!
    pause
    exit /b 1
)

REM Kiem tra auth.c da ton tai chua
if not exist "auth.c" (
    echo [INFO] Chua co auth.c - tao tu file goc
    git show HEAD:auth.c > auth.c 2>nul
    if not exist "auth.c" (
        echo [LOI] Khong tim thay auth.c trong commit truoc
        pause
        exit /b 1
    )
)

REM Kiem tra push
set /p AUTO_PUSH="Tu dong push len GitHub sau moi kich ban? (y/N): "

echo.
set /p START="Bat dau chay? (y/N): "
if /i not "%START%"=="y" exit /b 0

echo.
echo ============================================================
echo  BAT DAU CHAY 10 KICH BAN
echo ============================================================
echo.

REM Chay tung kich ban
set DEMO_DIR=%~dp0

call "%DEMO_DIR%demo_01_sql_injection.bat" <nul
if /i "%AUTO_PUSH%"=="y" git push origin main

call "%DEMO_DIR%demo_02_buffer_overflow.bat" <nul
if /i "%AUTO_PUSH%"=="y" git push origin main

call "%DEMO_DIR%demo_03_command_injection.bat" <nul
if /i "%AUTO_PUSH%"=="y" git push origin main

call "%DEMO_DIR%demo_04_path_traversal.bat" <nul
if /i "%AUTO_PUSH%"=="y" git push origin main

call "%DEMO_DIR%demo_05_hardcoded_credentials.bat" <nul
if /i "%AUTO_PUSH%"=="y" git push origin main

call "%DEMO_DIR%demo_06_out_of_bounds_write.bat" <nul
if /i "%AUTO_PUSH%"=="y" git push origin main

call "%DEMO_DIR%demo_07_out_of_bounds_read.bat" <nul
if /i "%AUTO_PUSH%"=="y" git push origin main

call "%DEMO_DIR%demo_08_use_after_free.bat" <nul
if /i "%AUTO_PUSH%"=="y" git push origin main

call "%DEMO_DIR%demo_09_null_pointer_deref.bat" <nul
if /i "%AUTO_PUSH%"=="y" git push origin main

call "%DEMO_DIR%demo_10_integer_overflow.bat" <nul
if /i "%AUTO_PUSH%"=="y" git push origin main

echo.
echo ============================================================
echo  HOAN THANH TAT CA 10 KICH BAN
echo ============================================================
echo.
echo Xem lich su commit:
git log --oneline -20
echo.
pause
