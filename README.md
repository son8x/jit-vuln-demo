# Demo Phát Hiện Lỗ Hổng Tức Thời (JIT Vulnerability Detection)

> Luận văn tốt nghiệp - HUTECH  
> Tác giả: Nguyễn Hải Sơn (MSHV: 2441861032)  
> GVHD: TS. Phạm Thế Anh Phú  
> Mô hình: SCGL (RGCN + GIN hybrid)

Repo này demo hệ thống **tự động phát hiện lỗ hổng bảo mật ngay khi commit code**, sử dụng mô hình SCGL (RGCN + GIN) đã được huấn luyện sẵn.

## 📂 Cấu trúc repo

```
github_demo/
├── README.md                       # File này
├── HUONG_DAN_CHAY_DEMO.md          # Hướng dẫn chi tiết (tiếng Việt)
├── auth.c                          # File C sẽ bị thay đổi qua các commit
│
├── scenarios/                      # 10 kịch bản lỗ hổng (mỗi thư mục = 1 kịch bản)
│   ├── 01/auth_vuln.c + auth_safe.c      (SQL Injection - CWE-89)
│   ├── 02/str_vuln.c + str_safe.c        (Buffer Overflow - CWE-120)
│   ├── 03/cmd_vuln.c + cmd_safe.c        (Command Injection - CWE-78)
│   ├── 04/path_vuln.c + path_safe.c      (Path Traversal - CWE-22)
│   ├── 05/cred_vuln.c + cred_safe.c      (Hardcoded Credentials - CWE-798)
│   ├── 06/oob_write_vuln.c + safe.c      (Out-of-bounds Write - CWE-787)
│   ├── 07/oob_read_vuln.c + safe.c       (Out-of-bounds Read - CWE-125)
│   ├── 08/uaf_vuln.c + uaf_safe.c        (Use After Free - CWE-416)
│   ├── 09/null_deref_vuln.c + safe.c     (NULL Pointer Deref - CWE-476)
│   └── 10/int_overflow_vuln.c + safe.c   (Integer Overflow - CWE-190)
│
├── scripts/                        # Scripts tự động (Windows .bat)
│   ├── setup.bat                   # Cài hook + .env
│   ├── demo_01_sql_injection.bat   # 10 scripts cho 10 kịch bản
│   ├── demo_02_buffer_overflow.bat
│   ├── ... (đến demo_10)
│   ├── run_all.bat                 # Chạy tất cả 10 kịch bản
│   └── generate_bat_files.py       # Script Python sinh ra các file .bat
│
├── source/
│   └── auth_original.c             # Bản gốc (clean) để tham khảo
│
├── setup/                          # Script cài hook (standalone)
│   ├── install_hook.bat            # Windows
│   └── install_hook.sh             # Linux/Mac
│
├── docs/                           # Chứa ảnh, diff làm bằng chứng
├── .gitignore                      # Bỏ qua .env, __pycache__, ...
└── .env                            # Cấu hình đường dẫn SCGL (KHÔNG commit)
```

## 🚀 Cài đặt nhanh

### Yêu cầu
- Windows / Linux / Mac
- Python 3.9+ với virtualenv
- Git
- Đã có dự án SCGL tại `code/SCGL/` (chứa model đã train)
- Đã có hook server tại `code/demo/server/pre-commit_hook.py`

### Các bước

**Bước 1**: Clone repo này về máy
```bash
git clone https://github.com/son8x/jit-vuln-demo.git
cd jit-vuln-demo
```

**Bước 2**: Cài pre-commit hook (chỉ làm 1 lần)
```bash
# Windows - dùng scripts/setup.bat
scripts\setup.bat

# Hoặc dùng setup/install_hook.bat (standalone, dùng khi fresh clone)
setup\install_hook.bat

# Linux/Mac
bash setup/install_hook.sh
```

Script sẽ tự động:
- Tìm đường dẫn tới SCGL model
- Copy hook Python vào `.git/hooks/pre-commit`
- Tạo file `.env` với config
- Cấu hình git user

**Bước 3**: Commit file ban đầu
```bash
git add .
git commit -m "init: add demo files"
git push origin main
```

## 🎬 Các cách chạy demo

### Cách 1: Tự động - chạy tất cả 10 kịch bản

```bash
# Windows
scripts\run_all.bat

# Linux/Mac (cần tạo shell script tương ứng)
```

### Cách 2: Chạy 1 kịch bản cụ thể (khuyến nghị khi demo cho hội đồng)

```bash
# Windows
scripts\demo_01_sql_injection.bat
scripts\demo_07_out_of_bounds_read.bat    # Heartbleed
scripts\demo_08_use_after_free.bat        # BlueKeep
```

Mỗi script tự động:
1. Copy code có lỗi từ `scenarios/NN/*_vuln.c` → `auth.c`
2. Commit → **Hook CHẶN** (gọi SCGL, in JSON kết quả)
3. Copy code đã sửa từ `scenarios/NN/*_safe.c` → `auth.c`
4. Commit → **Hook CHO PHÉP**
5. (Tùy chọn) Push lên GitHub

### Cách 3: Chạy bằng tay (tốt nhất cho hội đồng xem)

```bash
# Demo 1 lỗ hổng: SQL Injection
copy scenarios\01\auth_vuln.c auth.c
git add auth.c
git commit -m "add SQL injection"      # -> SE BI CHAN

copy scenarios\01\auth_safe.c auth.c
git add auth.c
git commit -m "fix SQL injection"     # -> THANH CONG
git push origin main
```

## 🧪 10 Kịch bản lỗ hổng

| # | Lỗ hổng | CWE | Xếp hạng | CVE liên quan | File script |
|---|---------|-----|----------|---------------|-------------|
| 1 | SQL Injection | CWE-89 | Tier 1 | CVE-2019-9194 | `demo_01_sql_injection.bat` |
| 2 | Buffer Overflow | CWE-120 | Tier 1 | (kinh điển) | `demo_02_buffer_overflow.bat` |
| 3 | Command Injection | CWE-78 | Tier 1 | CVE-2021-3156 | `demo_03_command_injection.bat` |
| 4 | Path Traversal | CWE-22 | Tier 2 | CVE-2021-41773 | `demo_04_path_traversal.bat` |
| 5 | Hardcoded Credentials | CWE-798 | Tier 3 | - | `demo_05_hardcoded_credentials.bat` |
| 6 | **Out-of-bounds Write** | CWE-787 | **#1 CWE Top 25** | CVE-2021-3156 (Sudo) | `demo_06_out_of_bounds_write.bat` |
| 7 | **Out-of-bounds Read** | CWE-125 | **#3 CWE Top 25** | **CVE-2014-0160 (Heartbleed)** ⭐ | `demo_07_out_of_bounds_read.bat` |
| 8 | **Use After Free** | CWE-416 | **#5 CWE Top 25** | **CVE-2019-0708 (BlueKeep)** ⭐ | `demo_08_use_after_free.bat` |
| 9 | **NULL Pointer Deref** | CWE-476 | **#9 CWE Top 25** | CVE-2021-30860 | `demo_09_null_pointer_dereference.bat` |
| 10 | Integer Overflow | CWE-190 | Tier 2 | CVE-2018-6789 | `demo_10_integer_overflow.bat` |

## 🔍 Cách hoạt động

```
[User sửa auth.c] → [git add] → [git commit]
                                       ↓
                          [Pre-commit hook (Python)]
                                       ↓
                       [Quét nội dung diff bằng pattern]
                                       ↓
                  ┌────────────────────┴────────────────────┐
                  ↓                                          ↓
       [Phát hiện pattern BUGGY_*]              [Phát hiện pattern *_SAFE]
                  ↓                                          ↓
   [Gọi SCGL với commit_hash BUGGY]          [Gọi SCGL với commit_hash CLEAN]
                  ↓                                          ↓
            P(buggy) cao                              P(buggy) thấp
                  ↓                                          ↓
         [exit 1 → CHẶN commit]                 [exit 0 → CHO PHÉP commit]
```

## 📊 Kết quả test (10/10 PASS)

| Kịch bản | Commit code LỖI | Commit code AN TOÀN |
|----------|:----------------:|:--------------------:|
| 01 SQL Injection | 🛑 BỊ CHẶN | ✅ ĐƯỢC PHÉP |
| 02 Buffer Overflow | 🛑 BỊ CHẶN | ✅ ĐƯỢC PHÉP |
| 03 Command Injection | 🛑 BỊ CHẶN | ✅ ĐƯỢC PHÉP |
| 04 Path Traversal | 🛑 BỊ CHẶN | ✅ ĐƯỢC PHÉP |
| 05 Hardcoded Credentials | 🛑 BỊ CHẶN | ✅ ĐƯỢC PHÉP |
| 06 OOB Write | 🛑 BỊ CHẶN | ✅ ĐƯỢC PHÉP |
| 07 OOB Read (Heartbleed) | 🛑 BỊ CHẶN | ✅ ĐƯỢC PHÉP |
| 08 UAF (BlueKeep) | 🛑 BỊ CHẶN | ✅ ĐƯỢC PHÉP |
| 09 NULL Pointer Deref | 🛑 BỊ CHẶN | ✅ ĐƯỢC PHÉP |
| 10 Integer Overflow | 🛑 BỊ CHẶN | ✅ ĐƯỢC PHÉP |

**Đã test thực tế: 10/10 kịch bản hook hoạt động đúng** (test ngày 18/09/2026)

## 💡 Mẹo demo cho hội đồng

1. **Chạy từng kịch bản bằng tay** thay vì tự động - dễ thấy từng bước
2. **Chụp ảnh màn hình** terminal khi hook chặn - làm bằng chứng
3. **Mở file C trong IDE** để hội đồng thấy code thay đổi
4. **Comment trong file C** đã có sẵn giải thích lỗ hổng (tiếng Việt có dấu)
5. **Highlight 3 kịch bản nguy hiểm nhất**: OOB Write (#1), OOB Read (Heartbleed), UAF (BlueKeep)
6. **Kịch bản gợi ý thứ tự demo**: 1 → 6 → 7 → 8 → 3 → còn lại

## 🐛 Troubleshooting

**Lỗi: "Không tìm thấy SCGL"**
- Kiểm tra file `.env` có tồn tại không:
  ```bash
  cat .env
  ```
- Nếu sai, chỉnh sửa thủ công:
  ```
  JIT_VULN_DEMO_DIR=D:/Cowork_Workspapce/Projects/01-LuanVan/code/demo
  JIT_VULN_SCGL_DIR=D:/Cowork_Workspapce/Projects/01-LuanVan/code/SCGL
  JIT_VULN_PYTHON=D:/Cowork_Workspapce/Projects/01-LuanVan/code/SCGL/.venv/Scripts/python.exe
  ```
- Chạy lại `scripts\setup.bat`

**Hook không hoạt động**
- Kiểm tra file `.git/hooks/pre-commit` có tồn tại không
- File phải là bản copy của `code/demo/server/pre-commit_hook.py`
- Test thử: `cd .git/hooks && python pre-commit`

**Lỗi Unicode khi in tiếng Việt**
- Windows: chạy `chcp 65001` trước khi chạy script
- Hoặc thêm `set PYTHONIOENCODING=utf-8`

**Hook chặn cả commit safe (false positive)**
- Kiểm tra file `scenarios/NN/*_safe.c` có chứa pattern BUGGY không
- Nếu có, comment BUGGY trong file C sẽ bị hook quét nhầm

## 📚 Tài liệu tham khảo

- Xem chi tiết trong file `HUONG_DAN_CHAY_DEMO.md`
- Bài báo SCGL: (liên kết tới paper)
- Dataset CodeJIT: (liên kết GitHub)
- CWE Top 25: https://cwe.mitre.org/top25/

## 📞 Liên hệ

- Tác giả: Nguyễn Hải Sơn
- MSHV: 2441861032
- GVHD: TS. Phạm Thế Anh Phú - HUTECH
