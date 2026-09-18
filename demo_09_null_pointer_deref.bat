@echo off
REM =============================================================================
REM  demo_09_null_pointer_deref.bat - Kich ban 9: NULL Pointer Deref
REM =============================================================================

setlocal enabledelayedexpansion
chcp 65001 >nul

echo ============================================================
echo  [9/10] KICH BAN: NULL Pointer Dereference (CWE-476)
echo ============================================================
echo.

set FILE_AUTH=auth.c
set FILE_VULN=vulnerable\09_null_pointer_deref.c
set FILE_SAFE=safe\09_null_pointer_deref.c
set MSG_VULN=add null pointer deref - no NULL check before access
set MSG_SAFE=fix null pointer deref - add NULL check before access

echo [BUOC 1/3] Commit code co loi
echo ------------------------------------------------
copy /Y "%FILE_VULN%" "%FILE_AUTH%" >nul
git add "%FILE_AUTH%"
echo.
echo   Hien thi diff (10 dong dau):
git --no-pager diff --cached -- "%FILE_AUTH%" | findstr /n "^" | findstr /b "1: 2: 3: 4: 5: 6: 7: 8: 9: 10:"
echo.
git commit -m "%MSG_VULN%"
set RC_VULN=%errorlevel%
echo.
if %RC_VULN% neq 0 (echo   [OK] HOOK DA CHAN!) else (echo   [LOI] Hook KHONG chan.)
echo.

echo [BUOC 2/3] Commit code da sua
echo ------------------------------------------------
copy /Y "%FILE_SAFE%" "%FILE_AUTH%" >nul
git add "%FILE_AUTH%"
git commit -m "%MSG_SAFE%"
set RC_SAFE=%errorlevel%
echo.
if %RC_SAFE% equ 0 (echo   [OK] HOOK CHO PHEP!) else (echo   [LOI] Hook chan nham.)
git rev-parse --short HEAD
echo.

set /p PUSH="Push len GitHub? (y/N): "
if /i "%PUSH%"=="y" git push origin main

echo ============================================================
echo  HOAN THANH KICH BAN 9: NULL Pointer Dereference
echo ============================================================
pause
