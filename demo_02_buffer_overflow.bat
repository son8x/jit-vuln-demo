@echo off
REM =============================================================================
REM  demo_02_buffer_overflow.bat - Kich ban 2: Buffer Overflow
REM =============================================================================

setlocal enabledelayedexpansion
chcp 65001 >nul

echo ============================================================
echo  [2/10] KICH BAN: Buffer Overflow (CWE-120)
echo ============================================================
echo.

set FILE_AUTH=auth.c
set FILE_VULN=vulnerable\02_buffer_overflow.c
set FILE_SAFE=safe\02_buffer_overflow.c
set MSG_VULN=add buffer overflow vulnerability (strcpy without bounds check)
set MSG_SAFE=fix buffer overflow - use strncpy and snprintf

echo [BUOC 1/3] Commit code co loi Buffer Overflow
echo ------------------------------------------------
echo   Copy: %FILE_VULN% -^> %FILE_AUTH%
copy /Y "%FILE_VULN%" "%FILE_AUTH%" >nul
git add "%FILE_AUTH%"
echo.
echo   Hien thi diff (10 dong dau):
git --no-pager diff --cached -- "%FILE_AUTH%" | findstr /n "^" | findstr /b "1: 2: 3: 4: 5: 6: 7: 8: 9: 10:"
echo.
echo   Dang commit...
git commit -m "%MSG_VULN%"
set RC_VULN=%errorlevel%
echo.
if %RC_VULN% neq 0 (
    echo   [OK] HOOK DA CHAN commit co loi!
) else (
    echo   [LOI] Hook KHONG chan commit co loi.
)
echo.

echo [BUOC 2/3] Commit code da sua
echo ------------------------------------------------
echo   Copy: %FILE_SAFE% -^> %FILE_AUTH%
copy /Y "%FILE_SAFE%" "%FILE_AUTH%" >nul
git add "%FILE_AUTH%"
echo.
echo   Dang commit...
git commit -m "%MSG_SAFE%"
set RC_SAFE=%errorlevel%
echo.
if %RC_SAFE% equ 0 (
    echo   [OK] HOOK CHO PHEP commit an toan!
    git rev-parse --short HEAD
) else (
    echo   [LOI] Hook chan nham commit an toan.
)
echo.

set /p PUSH="Push len GitHub? (y/N): "
if /i "%PUSH%"=="y" git push origin main

echo.
echo ============================================================
echo  HOAN THANH KICH BAN 2: Buffer Overflow
echo ============================================================
pause
