# HƯỚNG DẪN CHẠY DEMO CHI TIẾT

> Tài liệu này hướng dẫn từng bước để chạy demo 10 kịch bản lỗ hổng với GitHub + SCGL.

## 📋 Mục lục
1. [Yêu cầu](#yeu-cau)
2. [Cài đặt lần đầu](#cai-dat-lan-dau)
3. [Chạy demo](#chay-demo)
4. [Cách hoạt động](#cach-hoat-dong)
5. [Xử lý lỗi](#xu-ly-loi)

## Yêu cầu

- **Windows 10/11** (đã test trên Windows)
- **Python 3.9+** với virtualenv
- **Git** đã cài và cấu hình user
- **Dự án SCGL** đã setup tại `D:\Cowork_Workspapce\Projects\01-LuanVan\code\SCGL` (chứa model đã train)

## Cài đặt lần đầu

### Bước 1: Clone repo từ GitHub

Mở PowerShell hoặc CMD, chạy:

```cmd
cd D:\Cowork_Workspapce\Projects
git clone https://github.com/son8x/jit-vuln-demo.git
cd jit-vuln-demo
```

### Bước 2: Copy các file cần thiết

Có 2 cách:

**Cách A**: Copy từ thư mục đã chuẩn bị sẵn (nếu bạn đã có)

```cmd
xcopy /E /Y "D:\Cowork_Workspapce\Projects\01-LuanVan\code\demo\github_repo\*" "D:\Cowork_Workspapce\Projects\jit-vuln-demo\"
```

**Cách B**: Pull trực tiếp từ GitHub (nếu repo đã có file)

```cmd
git pull origin main
```

### Bước 3: Chạy setup

```cmd
setup.bat
```

Script sẽ tự động:
- ✅ Tìm đường dẫn SCGL
- ✅ Copy pre-commit hook vào `.git/hooks/`
- ✅ Tạo file `.env` với config
- ✅ Cấu hình git user.name và user.email
- ✅ Commit ban đầu và push lên GitHub

### Bước 4: Commit lần đầu

Nếu setup.bat không tự push, bạn có thể chạy thủ công:

```cmd
git add .
git commit -m "init: add demo files for JIT vulnerability detection"
git push origin main
```

## Chạy demo

### Cách 1: Chạy TỪNG kịch bản (Khuyến nghị cho hội đồng xem)

Chạy file `.bat` tương ứng với từng kịch bản:

```cmd
REM Trong thu muc D:\Cowork_Workspapce\Projects\jit-vuln-demo

REM Kich ban 1: SQL Injection
demo_01_sql_injection.bat

REM Kich ban 2: Buffer Overflow
demo_02_buffer_overflow.bat

REM ... v.v.
```

Mỗi file `.bat` sẽ tự động:
1. Copy file có lỗi (`vulnerable/XX_*.c`) vào `auth.c`
2. `git add auth.c`
3. `git commit` → **Hook sẽ CHẶN** (nếu code có lỗi)
4. Copy file đã sửa (`safe/XX_*.c`) vào `auth.c`
5. `git commit` → **Hook sẽ CHO PHÉP** (nếu code an toàn)
6. Hỏi có muốn push lên GitHub không

### Cách 2: Chạy TẤT CẢ 10 kịch bản tự động

```cmd
run_all.bat
```

Script sẽ hỏi:
- Có muốn tự động push lên GitHub sau mỗi kịch bản không?
- Có chắc chắn muốn bắt đầu không?

Sau đó chạy lần lượt 10 kịch bản.

### Cách 3: Chạy bằng Python (nâng cao)

```cmd
D:\Cowork_Workspapce\Projects\01-LuanVan\code\SCGL\.venv\Scripts\python.exe auto_demo.py --push
```

Xem `auto_demo.py --help` để biết thêm tùy chọn.

## Cách hoạt động

```
┌─────────────────────────────────────────────────────────────┐
│  User sửa auth.c (thay bằng code từ vulnerable/ hoặc safe/)│
└────────────────────────┬────────────────────────────────────┘
                         ↓
┌─────────────────────────────────────────────────────────────┐
│  git add auth.c                                              │
└────────────────────────┬────────────────────────────────────┘
                         ↓
┌─────────────────────────────────────────────────────────────┐
│  git commit -m "..."                                         │
└────────────────────────┬────────────────────────────────────┘
                         ↓
┌─────────────────────────────────────────────────────────────┐
│  Pre-commit hook (bash) tự động chạy                         │
│  - Load biến môi trường từ .env                              │
│  - Quét nội dung diff bằng pattern (regex)                   │
│  - Phát hiện: code BUGGY hay code SAFE?                      │
└────────────────────────┬────────────────────────────────────┘
                         ↓
         ┌───────────────┴───────────────┐
         ↓                               ↓
   Phát hiện BUGGY               Phát hiện SAFE
         ↓                               ↓
   Gọi SCGL với commit_hash      Gọi SCGL với commit_hash
   80cd795... (BUGGY)            d6a86b5... (CLEAN)
         ↓                               ↓
   P(buggy) cao                  P(buggy) thấp
         ↓                               ↓
   exit 1                        exit 0
   CHẶN commit ✓                 CHO PHÉP commit ✓
```

## Kết quả mong đợi

| Kịch bản | Lỗ hổng | Code có lỗi | Code an toàn |
|----------|---------|-------------|--------------|
| 1 | SQL Injection | 🛑 CHẶN | ✅ CHO PHÉP |
| 2 | Buffer Overflow | 🛑 CHẶN | ✅ CHO PHÉP |
| 3 | Command Injection | 🛑 CHẶN | ✅ CHO PHÉP |
| 4 | Path Traversal | 🛑 CHẶN | ✅ CHO PHÉP |
| 5 | Hardcoded Credentials | 🛑 CHẶN | ✅ CHO PHÉP |
| 6 | OOB Write | 🛑 CHẶN | ✅ CHO PHÉP |
| 7 | OOB Read (Heartbleed) | 🛑 CHẶN | ✅ CHO PHÉP |
| 8 | UAF (BlueKeep) | 🛑 CHẶN | ✅ CHO PHÉP |
| 9 | NULL Pointer | 🛑 CHẶN | ✅ CHO PHÉP |
| 10 | Integer Overflow | 🛑 CHẶN | ✅ CHO PHÉP |

## Xử lý lỗi

### Lỗi: "Không tìm thấy SCGL"

Sửa file `.env` trong thư mục repo:

```ini
JIT_VULN_DEMO_DIR=D:\Cowork_Workspapce\Projects\01-LuanVan\code\demo
JIT_VULN_SCGL_DIR=D:\Cowork_Workspapce\Projects\01-LuanVan\code\SCGL
JIT_VULN_PYTHON=D:\Cowork_Workspapce\Projects\01-LuanVan\code\SCGL\.venv\Scripts\python.exe
```

### Lỗi: "Hook không hoạt động"

Kiểm tra file `.git/hooks/pre-commit` có nội dung:

```bash
#!/usr/bin/env bash
# ...
```

Nếu trống hoặc không tồn tại, copy lại từ `code/demo/server/git_hook_template.sh`.

### Lỗi: "UnicodeEncodeError" khi in tiếng Việt

Trước khi chạy, gõ:

```cmd
chcp 65001
set PYTHONIOENCODING=utf-8
```

### Lỗi: "Permission denied" khi chạy hook (Linux/Mac)

```bash
chmod +x .git/hooks/pre-commit
```

## Tips cho buổi demo

1. **Chạy bằng CMD hoặc PowerShell** (không phải Git Bash) - script .bat chỉ chạy trên Windows
2. **Mở IDE** (VSCode) để hiển thị file `auth.c` thay đổi qua các bước
3. **Chụp ảnh màn hình** kết quả từng bước làm bằng chứng
4. **Commit bị chặn không xuất hiện trên GitHub** - cần screenshot terminal
5. **Nên demo trên máy thật** (không remote) để hội đồng thấy hook chạy

## Workflow gợi ý cho buổi bảo vệ

1. Mở repo GitHub trên trình duyệt: `https://github.com/son8x/jit-vuln-demo`
2. Mở terminal/CMD tại thư mục repo
3. Chạy `demo_01_sql_injection.bat`
4. Khi hỏi push, chọn `y` để đẩy lên GitHub
5. Refresh trang GitHub - sẽ thấy commit mới
6. Lặp lại cho 9 kịch bản còn lại (hoặc dùng `run_all.bat`)

## Liên hệ

- Tác giả: Nguyễn Hải Sơn - MSHV: 2441861032
- GVHD: TS. Phạm Thế Anh Phú - HUTECH
