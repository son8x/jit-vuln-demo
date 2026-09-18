@echo off
REM =============================================================================
REM  demo_01_sql_injection.bat - Kich ban 1: SQL Injection
REM =============================================================================
REM  Mo ta: Demo commit code co loi SQL Injection bi hook chan,
REM          sau do commit code da sua duoc phep.
REM
REM  Cach su dung: Chi can chay file nay (double-click hoac tu cmd)
REM =============================================================================

setlocal enabledelayedexpansion
chcp 65001 >nul

echo ============================================================
echo  [1/10] KICH BAN: SQL Injection (CWE-89)
echo ============================================================
echo.

REM Load .env de co duong dan SCGL
if exist ".env" (
    for /f "usebackq tokens=1,2 delims==" %%a in (".env") do set %%a=%%b
)

REM Cau hinh
set FILE_AUTH=auth.c
set FILE_VULN=vulnerable\01_sql_injection.c
set FILE_SAFE=safe\01_sql_injection.c
set MSG_VULN=add SQL injection vulnerability
set MSG_SAFE=fix SQL injection - use parameterized query

echo [BUOC 1/3] Commit code co loi SQL Injection
echo ------------------------------------------------
echo   Copy: %FILE_VULN% -^> %FILE_AUTH%
copy /Y "%FILE_VULN%" "%FILE_AUTH%" >nul
git add "%FILE_AUTH%"
echo.
echo   Hien thi diff (10 dong dau):
git --no-pager diff --cached -- "%FILE_AUTH%" | findstr /n "^" | findstr /b "1: 2: 3: 4: 5: 6: 7: 8: 9: 10:"
echo.
echo   Dang commit... (HOOK se chan neu loi)
git commit -m "%MSG_VULN%"
set RC_VULN=%errorlevel%
echo.
if %RC_VULN% neq 0 (
    echo   [OK] HOOK DA CHAN commit co loi! (exit code: !RC_VULN!)
) else (
    echo   [LOI] Hook KHONG chan commit co loi. Kiem tra lai hook.
)
echo.

echo [BUOC 2/3] Commit code da sua
echo ------------------------------------------------
echo   Copy: %FILE_SAFE% -^> %FILE_AUTH%
copy /Y "%FILE_SAFE%" "%FILE_AUTH%" >nul
git add "%FILE_AUTH%"
echo.
echo   Dang commit... (HOOK se cho phep)
git commit -m "%MSG_SAFE%"
set RC_SAFE=%errorlevel%
echo.
if %RC_SAFE% equ 0 (
    echo   [OK] HOOK CHO PHEP commit an toan!
    echo   Commit hash: 
    git rev-parse --short HEAD
) else (
    echo   [LOI] Hook chan nham commit an toan. Kiem tra lai.
)
echo.

echo [BUOC 3/3] Push len GitHub (tuy chon)
echo ------------------------------------------------
set /p PUSH="Ban co muon push commit vua roi? (y/N): "
if /i "!PUSH!"=="y" (
    git push origin main
    echo   [OK] Da push len GitHub
) else (
    echo   [SKIP] Bo qua buoc push
)

echo.
echo ============================================================
echo  HOAN THANH KICH BAN 1: SQL Injection
echo ============================================================
pause
