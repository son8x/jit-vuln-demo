"""
auto_demo.py - Script tự động hóa demo 10 kịch bản lỗ hổng
============================================================
Script này tự động thực hiện toàn bộ workflow cho 10 lỗ hổng:
    1. Commit file gốc (auth.c) - LUÔN ĐƯỢC PHÉP
    2. Thay bằng code có lỗi → commit BỊ CHẶN bởi hook
    3. Thay bằng code đã sửa → commit THÀNH CÔNG
    4. Push lên GitHub

Cách sử dụng:
    python auto_demo.py                    # Chạy tất cả 10 kịch bản
    python auto_demo.py --scenario 1       # Chỉ chạy kịch bản số 1
    python auto_demo.py --scenario 1-5     # Chạy kịch bản 1 đến 5
    python auto_demo.py --push             # Tự động push sau mỗi commit thành công
    python auto_demo.py --keep-temp        # Giữ lại file diff làm bằng chứng

Yêu cầu:
    - Đã clone repo GitHub về máy
    - Đã chạy setup/install_hook.bat để cài pre-commit hook
    - Git đã cấu hình user.name, user.email
"""

import argparse
import os
import shutil
import subprocess
import sys
import time

# Ép UTF-8 cho output Windows
try:
    sys.stdout.reconfigure(encoding="utf-8")
    sys.stderr.reconfigure(encoding="utf-8")
except (AttributeError, OSError):
    pass


def in_banner(tieu_de: str, ky_tu: str = "=") -> None:
    """In banner ngăn cách các phần."""
    print(f"\n{ky_tu * 64}")
    print(f"  {tieu_de}")
    print(ky_tu * 64)


def chay_lenh(cmd, cwd=None, hien_thi=False):
    """Chạy shell command, trả về (returncode, stdout, stderr)."""
    if isinstance(cmd, str):
        cmd = cmd.split()
    ket_qua = subprocess.run(
        cmd, cwd=cwd, capture_output=True,
        text=True, encoding="utf-8", errors="replace"
    )
    if hien_thi:
        if ket_qua.stdout:
            print(ket_qua.stdout)
        if ket_qua.stderr:
            print(ket_qua.stderr, file=sys.stderr)
    return ket_qua


def lay_commit_hash(cwd: str) -> str:
    """Lấy commit hash ngắn (7 ký tự) của commit hiện tại."""
    r = chay_lenh(["git", "rev-parse", "--short", "HEAD"], cwd=cwd)
    return r.stdout.strip()


def kiem_tra_git_repo() -> str:
    """Kiểm tra thư mục hiện tại có phải git repo không, trả về repo root."""
    r = chay_lenh(["git", "rev-parse", "--show-toplevel"])
    if r.returncode != 0:
        print("[LỖI] Thư mục hiện tại không phải git repository!")
        print("Hãy chạy lệnh: cd <đường-dẫn-tới-repo-github>")
        sys.exit(1)
    return r.stdout.strip()


def kiem_tra_hook(duong_dan_repo: str) -> bool:
    """Kiểm tra pre-commit hook đã được cài chưa."""
    hook_path = os.path.join(duong_dan_repo, ".git", "hooks", "pre-commit")
    if not os.path.isfile(hook_path):
        print("[CẢNH BÁO] Chưa cài pre-commit hook!")
        print(f"Hook cần ở: {hook_path}")
        print("Chạy setup/install_hook.bat để cài.")
        return False
    return True


def commit_voi_message(duong_dan_repo: str, message: str, hien_hook: bool = True) -> int:
    """Thực hiện git commit, trả về exit code (0=OK, khác 0=lỗi)."""
    print(f"\n  → git commit -m \"{message}\"")
    r = subprocess.run(
        ["git", "commit", "-m", message],
        cwd=duong_dan_repo,
        capture_output=True,
        text=True, encoding="utf-8", errors="replace"
    )
    if hien_hook:
        if r.stdout:
            print(r.stdout)
        if r.stderr:
            print(r.stderr)
    return r.returncode


def copy_file(src: str, dst: str) -> None:
    """Copy file, báo lỗi nếu không tìm thấy."""
    if not os.path.isfile(src):
        print(f"[LỖI] Không tìm thấy file nguồn: {src}")
        sys.exit(1)
    shutil.copy(src, dst)


def thuc_hien_kich_ban(
    so_thu_tu: int,
    ten_kich_ban: str,
    file_vuln: str,
    file_safe: str,
    duong_dan_repo: str,
    push_sau_commit: bool = False,
    giu_file_diff: bool = False,
) -> dict:
    """
    Thực hiện đầy đủ 1 kịch bản demo (3 bước).

    Returns:
        dict: {
            "so_thu_tu": int,
            "ten": str,
            "vuln_blocked": bool,
            "safe_allowed": bool,
            "success": bool
        }
    """
    in_banner(f"[{so_thu_tu}/10] KỊCH BẢN: {ten_kich_ban}", "=")
    file_auth = os.path.join(duong_dan_repo, "auth.c")

    # ===== BƯỚC 1: Commit file gốc (chỉ lần đầu) =====
    if so_thu_tu == 1:
        in_banner("Bước 1/3: Commit file gốc (auth.c) - LÀM 1 LẦN DUY NHẤT", "-")
        if os.path.isfile(file_auth):
            print("  [1.1] auth.c đã tồn tại, bỏ qua bước này")
        else:
            print("  [1.1] Copy file gốc auth.c")
            shutil.copy(file_auth, file_auth)  # no-op
        # Stage và commit
        print("  [1.2] git add auth.c")
        chay_lenh(["git", "add", "auth.c"], cwd=duong_dan_repo, hien_thi=True)
        print("  [1.3] Commit file gốc")
        rc = commit_voi_message(duong_dan_repo, "init: add original auth.c (safe)")
        if rc == 0:
            print(f"  [OK] Commit gốc: {lay_commit_hash(duong_dan_repo)}")
            if push_sau_commit:
                chay_lenh(["git", "push", "origin", "main"], cwd=duong_dan_repo, hien_thi=True)
        else:
            print("  [BỎ QUA] Commit gốc đã tồn tại hoặc lỗi")

    # ===== BƯỚC 2: Thay bằng code có lỗi, commit BỊ CHẶN =====
    in_banner(f"Bước 2/3: Thay auth.c = code có lỗi", "-")
    print(f"  [2.1] Copy file có lỗi: vulnerable/{os.path.basename(file_vuln)}")
    copy_file(file_vuln, file_auth)
    chay_lenh(["git", "add", "auth.c"], cwd=duong_dan_repo)
    print("  [2.2] Hiển thị diff (15 dòng đầu):")
    print("  " + "-" * 60)
    r = chay_lenh(["git", "diff", "--cached"], cwd=duong_dan_repo)
    for dong in r.stdout.splitlines()[:15]:
        print(f"  {dong}")
    if len(r.stdout.splitlines()) > 15:
        print("  ...")
    print("  " + "-" * 60)

    if giu_file_diff:
        diff_path = os.path.join(duong_dan_repo, "docs", f"diff_{so_thu_tu:02d}_vuln.txt")
        with open(diff_path, "w", encoding="utf-8") as f:
            f.write(r.stdout)
        print(f"  [INFO] Diff lưu tại: docs/diff_{so_thu_tu:02d}_vuln.txt")

    print("\n  [2.3] Thực hiện git commit (HY VỌNG BỊ CHẶN):")
    message_vuln = f"add {ten_kich_ban} vulnerability"
    rc = commit_voi_message(duong_dan_repo, message_vuln)
    vuln_blocked = (rc != 0)

    if vuln_blocked:
        print(f"\n  [✓ ĐÚNG] Hook đã CHẶN commit có lỗi!")
    else:
        print(f"\n  [✗ SAI] Hook KHÔNG chặn được commit có lỗi!")

    # ===== BƯỚC 3: Thay bằng code an toàn, commit THÀNH CÔNG =====
    in_banner(f"Bước 3/3: Thay auth.c = code đã sửa (an toàn)", "-")
    print(f"  [3.1] Copy file an toàn: safe/{os.path.basename(file_safe)}")
    copy_file(file_safe, file_auth)
    chay_lenh(["git", "add", "auth.c"], cwd=duong_dan_repo)
    print("\n  [3.2] Thực hiện git commit (HY VỌNG THÀNH CÔNG):")
    message_safe = f"fix {ten_kich_ban} - use safe pattern"
    rc = commit_voi_message(duong_dan_repo, message_safe)
    safe_allowed = (rc == 0)

    if safe_allowed:
        commit_hash = lay_commit_hash(duong_dan_repo)
        print(f"\n  [✓ ĐÚNG] Hook cho phép commit an toàn! (hash: {commit_hash})")
        if push_sau_commit:
            print(f"  [3.3] Push lên GitHub:")
            chay_lenh(["git", "push", "origin", "main"], cwd=duong_dan_repo, hien_thi=True)
    else:
        print(f"\n  [✗ SAI] Hook đã chặn nhầm commit an toàn!")

    return {
        "so_thu_tu": so_thu_tu,
        "ten": ten_kich_ban,
        "vuln_blocked": vuln_blocked,
        "safe_allowed": safe_allowed,
        "success": vuln_blocked and safe_allowed,
    }


def main():
    """Điểm vào chính."""
    parser = argparse.ArgumentParser(
        description="Tự động demo 10 kịch bản lỗ hổng với GitHub + SCGL"
    )
    parser.add_argument(
        "--scenario",
        type=str,
        default="all",
        help="Số kịch bản (1-10), khoảng (1-5), hoặc 'all'"
    )
    parser.add_argument(
        "--push",
        action="store_true",
        help="Tự động push lên GitHub sau mỗi commit thành công"
    )
    parser.add_argument(
        "--keep-temp",
        action="store_true",
        help="Lưu file diff vào docs/ làm bằng chứng"
    )
    args = parser.parse_args()

    # Kiểm tra môi trường
    repo_root = kiem_tra_git_repo()
    print(f"  Git repo root: {repo_root}")

    if not kiem_tra_hook(repo_root):
        print("\n  Bạn có thể tiếp tục nhưng hook sẽ KHÔNG hoạt động.")
        tra_loi = input("  Tiếp tục? (y/N): ").strip().lower()
        if tra_loi != 'y':
            sys.exit(1)

    # Xác định danh sách kịch bản
    DS_KICH_BAN = [
        (1, "SQL Injection",            "vulnerable/01_sql_injection.c",        "safe/01_sql_injection.c"),
        (2, "Buffer Overflow",          "vulnerable/02_buffer_overflow.c",      "safe/02_buffer_overflow.c"),
        (3, "Command Injection",        "vulnerable/03_command_injection.c",    "safe/03_command_injection.c"),
        (4, "Path Traversal",           "vulnerable/04_path_traversal.c",       "safe/04_path_traversal.c"),
        (5, "Hardcoded Credentials",    "vulnerable/05_hardcoded_credentials.c","safe/05_hardcoded_credentials.c"),
        (6, "Out-of-bounds Write",      "vulnerable/06_out_of_bounds_write.c",  "safe/06_out_of_bounds_write.c"),
        (7, "Out-of-bounds Read",       "vulnerable/07_out_of_bounds_read.c",   "safe/07_out_of_bounds_read.c"),
        (8, "Use After Free",           "vulnerable/08_use_after_free.c",       "safe/08_use_after_free.c"),
        (9, "NULL Pointer Dereference", "vulnerable/09_null_pointer_deref.c",   "safe/09_null_pointer_deref.c"),
        (10, "Integer Overflow",        "vulnerable/10_integer_overflow.c",     "safe/10_integer_overflow.c"),
    ]

    # Parse --scenario
    if args.scenario == "all":
        ds_chay = DS_KICH_BAN
    elif "-" in args.scenario:
        start, end = map(int, args.scenario.split("-"))
        ds_chay = [kb for kb in DS_KICH_BAN if start <= kb[0] <= end]
    else:
        so = int(args.scenario)
        ds_chay = [kb for kb in DS_KICH_BAN if kb[0] == so]

    # Banner đầu
    in_banner("DEMO TỰ ĐỘNG - PHÁT HIỆN LỖ HỔNG TỨC THỜI VỚI SCGL + GITHUB")
    print(f"  Repository       : {repo_root}")
    print(f"  Số kịch bản      : {len(ds_chay)}")
    print(f"  Tự động push     : {'Có' if args.push else 'Không'}")
    print(f"  Lưu diff         : {'Có' if args.keep_temp else 'Không'}")

    # Chạy từng kịch bản
    ds_ket_qua = []
    for so, ten, vuln, safe in ds_chay:
        kq = thuc_hien_kich_ban(
            so, ten,
            os.path.join(repo_root, vuln),
            os.path.join(repo_root, safe),
            repo_root,
            push_sau_commit=args.push,
            giu_file_diff=args.keep_temp,
        )
        ds_ket_qua.append(kq)
        time.sleep(1)  # nghỉ 1s giữa các kịch bản

    # Tổng kết
    in_banner("TỔNG KẾT")
    for kq in ds_ket_qua:
        v = "[CHẶN]" if kq["vuln_blocked"] else "[CHO PHÉP]"
        s = "[CHO PHÉP]" if kq["safe_allowed"] else "[CHẶN]"
        ok = "✓" if kq["success"] else "✗"
        print(f"  {ok} [{kq['so_thu_tu']:2d}] {kq['ten']:30s}  VULN={v}  SAFE={s}")

    so_tc = sum(1 for kq in ds_ket_qua if kq["success"])
    print(f"\n  Kết quả: {so_tc}/{len(ds_ket_qua)} kịch bản thành công")

    if args.push and so_tc > 0:
        print("\n  💡 Nhớ chụp ảnh màn hình các output làm bằng chứng!")
        print("     Vì commit BỊ CHẶN không xuất hiện trên GitHub.")


if __name__ == "__main__":
    main()
