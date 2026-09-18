"""
generate_bat_files.py - Tu dong tao 10 file .bat cho cac kich ban demo
=========================================================================
Chay: python generate_bat_files.py

Luu y: File .bat duoc luu UTF-8 (KHONG BOM). Khi chay tu CMD truc tiep,
       go "chcp 65001" truoc de hien thi tieng Viet co dau.
"""

import os

# Cau hinh cac kich ban
KICH_BAN = [
    (1,  "SQL Injection",            "CWE-89",  "Tier 1 - CWE Top 25",       "CVE-2019-9194 (PostgreSQL)",       "auth"),
    (2,  "Buffer Overflow",          "CWE-120", "Tier 1 - kinh dien",        "(kinh dien)",                       "str"),
    (3,  "Command Injection",        "CWE-78",  "Tier 1 (web app)",           "CVE-2021-3156 (Sudo)",             "cmd"),
    (4,  "Path Traversal",           "CWE-22",  "Tier 2 (web app)",           "CVE-2021-41773 (Apache)",          "path"),
    (5,  "Hardcoded Credentials",    "CWE-798", "Tier 3 (cau hinh)",          "lo secret tren GitHub",            "cred"),
    (6,  "Out-of-bounds Write",      "CWE-787", "#1 CWE Top 25",              "CVE-2019-0703 (WinRAR)",           "oob_write"),
    (7,  "Out-of-bounds Read",       "CWE-125", "#3 CWE Top 25 - Heartbleed", "CVE-2014-0160 (Heartbleed)",       "oob_read"),
    (8,  "Use After Free",           "CWE-416", "#5 CWE Top 25 - BlueKeep",   "CVE-2019-0708 (BlueKeep)",         "uaf"),
    (9,  "NULL Pointer Dereference", "CWE-476", "#9 CWE Top 25",              "CVE-2021-30860 (iOS)",             "null_deref"),
    (10, "Integer Overflow",         "CWE-190", "Tier 2 (dat trung C/C++)",   "CVE-2018-6789 (Exim)",             "int_overflow"),
]


def make_slug(ten):
    """Chuyển tên thành slug: 'SQL Injection' -> 'sql_injection'."""
    return ten.lower().replace(" ", "_").replace("-", "_")


# Template ASCII-only, tieng Viet co dau nhung khong co box drawing
TEMPLATE = '''@echo off
chcp 65001 >nul
REM =============================================================================
REM  scripts/{ten_file}.bat - Demo kich ban {so}: {ten}
REM =============================================================================
REM  Kich ban nay minh hoa:
REM    - Loi {ten} ({cwe})
REM    - Commit co loi se bi pre-commit hook CHAN
REM    - Commit da sua se duoc hook CHO PHEP
REM
REM  Cach dung: Double-click file nay, hoac chay tu cmd:
REM    cd github_demo
REM    scripts\\{ten_file}.bat
REM =============================================================================

setlocal enabledelayedexpansion

REM ===== CAU HINH =====
set SO_KICH_BAN={so}
set TEN_KICH_BAN={ten} ({cwe})
set CWE={cwe}
set XEP_HANG={xep_hang}
set CVE_LIEN_QUAN={cve}

REM Duong dan - dung %~dp0 de lay thu muc scripts/
set SCRIPT_DIR=%~dp0
set REPO_ROOT=%SCRIPT_DIR%..

REM Xac dinh ten file (mot so kich ban co ten rieng)
set FILE_VULN=%REPO_ROOT%\\scenarios\\{so:02d}\\{file_prefix}_vuln.c
set FILE_SAFE=%REPO_ROOT%\\scenarios\\{so:02d}\\{file_prefix}_safe.c

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
echo [1.1] Copy file co loi: scenarios\\{so:02d}\\{file_prefix}_vuln.c -^> auth.c
copy /Y "%FILE_VULN%" "%REPO_ROOT%\\auth.c" >nul
if errorlevel 1 (
    echo [LOI] Khong the sao chep file.
    pause
    exit /b 1
)
echo       Da sao chep xong.
echo.
echo [1.2] Hien thi diff (toi da 12 dong dau):
echo       ----------------------------------------
git --no-pager diff -- "%REPO_ROOT%\\auth.c" 2>nul > "%TEMP%\\demo_diff.txt"
if exist "%TEMP%\\demo_diff.txt" (
    set /a LINE_NUM=0
    for /f "usebackq delims=" %%d in ("%TEMP%\\demo_diff.txt") do (
        set /a LINE_NUM+=1
        if !LINE_NUM! leq 12 echo       %%d
    )
    del "%TEMP%\\demo_diff.txt" 2>nul
) else (
    echo       (khong co diff)
)
echo       ----------------------------------------
echo.
echo [1.3] git add auth.c
git add "%REPO_ROOT%\\auth.c" 2>nul
echo.
echo [1.4] git commit -m "add {ten_slug} vulnerability"
echo       (Hook se tu dong chay SCGL de phan tich...)
git commit -m "add {ten_slug} vulnerability" 2>&1
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
echo [2.1] Copy file an toan: scenarios\\{so:02d}\\{file_prefix}_safe.c -^> auth.c
copy /Y "%FILE_SAFE%" "%REPO_ROOT%\\auth.c" >nul
echo       Da sao chep xong.
echo.
echo [2.2] git add auth.c
git add "%REPO_ROOT%\\auth.c" 2>nul
echo.
echo [2.3] git commit -m "fix {ten_slug} - use safe pattern"
echo       (Hook se tu dong chay SCGL de phan tich...)
git commit -m "fix {ten_slug} - use safe pattern" 2>&1
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
'''


def main():
    """Tao 10 file .bat (overwrite neu ton tai)."""
    thu_muc = os.path.dirname(os.path.abspath(__file__))

    for so, ten, cwe, xep_hang, cve, file_prefix in KICH_BAN:
        ten_slug = make_slug(ten)
        ten_file = f"demo_{so:02d}_{ten_slug}"
        noi_dung = TEMPLATE.format(
            so=so,
            ten=ten,
            cwe=cwe,
            xep_hang=xep_hang,
            cve=cve,
            file_prefix=file_prefix,
            ten_slug=ten_slug,
            ten_file=ten_file,
        )
        duong_dan = os.path.join(thu_muc, f"{ten_file}.bat")
        with open(duong_dan, "w", encoding="utf-8") as f:
            f.write(noi_dung)

    print("[OK] Da tao", len(KICH_BAN), "file .bat trong scripts/")


if __name__ == "__main__":
    main()
