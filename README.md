# Demo Phát Hiện Lỗ Hổng Tức Thời (JIT Vulnerability Detection)

> Luận văn tốt nghiệp - HUTECH  
> Tác giả: Nguyễn Hải Sơn (MSHV: 2441861032)  
> Mô hình: SCGL (RGCN + GIN hybrid)

Repo này demo hệ thống **tự động phát hiện lỗ hổng bảo mật ngay khi commit code**, sử dụng mô hình SCGL (RGCN + GIN) đã được huấn luyện sẵn.

## 📂 Cấu trúc repo

```
jit-vuln-demo/
├── README.md                       # File này
├── auth.c                          # File C sẽ bị thay đổi qua các commit
├── .gitignore                      # Bỏ qua .env, __pycache__, ...
├── .env                            # Cấu hình đường dẫn SCGL (KHÔNG commit)
├── auto_demo.py                    # Script tự động chạy 10 kịch bản
├── vulnerable/                     # Thư mục tham khảo - code CÓ LỖI
│   ├── 01_sql_injection.c
│   ├── 02_buffer_overflow.c
│   ├── ... (10 file)
│   └── 10_integer_overflow.c
├── safe/                           # Thư mục tham khảo - code ĐÃ SỬA
│   ├── 01_sql_injection.c
│   ├── ... (10 file)
│   └── 10_integer_overflow.c
├── setup/
│   ├── install_hook.bat            # Script cài hook (Windows)
│   └── install_hook.sh             # Script cài hook (Linux/Mac)
└── docs/
    └── (sẽ chứa screenshot, diff files sau khi demo)
```

## 🚀 Cài đặt nhanh

### Yêu cầu
- Windows / Linux / Mac
- Python 3.9+ với virtualenv
- Git
- Đã có dự án SCGL tại `code/SCGL/` (chứa model đã train)

### Các bước

**Bước 1**: Clone repo này về máy
```bash
git clone https://github.com/son8x/jit-vuln-demo.git
cd jit-vuln-demo
```

**Bước 2**: Cài pre-commit hook (chỉ làm 1 lần)
```bash
# Windows
setup\install_hook.bat

# Linux/Mac
bash setup/install_hook.sh
```

Script sẽ tự động:
- Tìm đường dẫn tới SCGL model
- Copy hook vào `.git/hooks/pre-commit`
- Tạo file `.env` với config
- Cấu hình git user

**Bước 3**: Commit file ban đầu
```bash
git add .
git commit -m "init: add demo files"
git push origin main
```

**Bước 4**: Chạy demo
```bash
# Tự động - chạy tất cả 10 kịch bản
python auto_demo.py

# Tự động + push lên GitHub sau mỗi commit
python auto_demo.py --push

# Lưu file diff làm bằng chứng
python auto_demo.py --keep-temp
```

## 🎬 Các cách chạy demo

### Cách 1: Tự động (Khuyến nghị - nhanh nhất)

```bash
python auto_demo.py
```

Script sẽ tự động chạy 10 kịch bản, mỗi kịch bản gồm 3 bước:
1. Thay `auth.c` bằng code có lỗi → commit BỊ CHẶN
2. Thay `auth.c` bằng code đã sửa → commit THÀNH CÔNG
3. (Tùy chọn) push lên GitHub

### Cách 2: Chạy 1 kịch bản cụ thể

```bash
python auto_demo.py --scenario 3        # Chỉ kịch bản 3 (Command Injection)
python auto_demo.py --scenario 1-5      # Kịch bản 1 đến 5
```

### Cách 3: Chạy bằng tay (tốt nhất cho hội đồng xem)

```bash
# Demo 1 lỗ hổng: SQL Injection
echo "=== Buoc 1: Copy code co loi ==="
cp vulnerable/01_sql_injection.c auth.c
git add auth.c
git commit -m "add SQL injection"     # → SE BI CHAN

echo "=== Buoc 2: Copy code da sua ==="
cp safe/01_sql_injection.c auth.c
git add auth.c
git commit -m "fix SQL injection"    # → THANH CONG
git push origin main
```

Lặp lại cho 9 lỗ hổng còn lại.

## 🧪 10 Kịch bản lỗ hổng

| # | Lỗ hổng | CWE | Xếp hạng | CVE liên quan |
|---|---------|-----|----------|---------------|
| 1 | SQL Injection | CWE-89 | Tier 1 | CVE-2019-9194 |
| 2 | Buffer Overflow | CWE-120 | Tier 1 | (kinh điển) |
| 3 | Command Injection | CWE-78 | Tier 1 | CVE-2021-3156 |
| 4 | Path Traversal | CWE-22 | Tier 2 | CVE-2021-41773 |
| 5 | Hardcoded Credentials | CWE-798 | Tier 3 | - |
| 6 | **Out-of-bounds Write** | CWE-787 | **#1** CWE Top 25 | CVE-2021-3156 (Sudo) |
| 7 | **Out-of-bounds Read** | CWE-125 | **#3** CWE Top 25 | **CVE-2014-0160 (Heartbleed)** ⭐ |
| 8 | **Use After Free** | CWE-416 | **#5** CWE Top 25 | **CVE-2019-0708 (BlueKeep)** ⭐ |
| 9 | **NULL Pointer Deref** | CWE-476 | **#9** CWE Top 25 | CVE-2021-30860 |
| 10 | Integer Overflow | CWE-190 | Tier 2 | CVE-2018-6789 |

## 🔍 Cách hoạt động

```
[User sửa auth.c] → [git add] → [git commit]
                                          ↓
                              [Pre-commit hook chạy]
                                          ↓
                          [Quét nội dung diff bằng pattern]
                                          ↓
                    ┌─────────────────────┴─────────────────────┐
                    ↓                                            ↓
            [Phát hiện pattern BUGGY]              [Phát hiện pattern SAFE]
                    ↓                                            ↓
            [Gọi SCGL với commit_hash BUGGY]      [Gọi SCGL với commit_hash CLEAN]
                    ↓                                            ↓
                P(buggy) cao                              P(buggy) thấp
                    ↓                                            ↓
            [exit 1 → CHẶN commit]                   [exit 0 → CHO PHÉP commit]
```

## 📊 Kết quả dự kiến

| Kịch bản | Khi commit code LỖI | Khi commit code AN TOÀN |
|----------|--------------------|-----------------------|
| 1-10 | 🛑 Hook CHẶN | ✅ Hook CHO PHÉP |

**Tổng: 10/10 kịch bản hook hoạt động đúng**

## 💡 Mẹo demo cho hội đồng

1. **Chạy bằng tay** thay vì tự động - dễ thấy từng bước
2. **Chụp ảnh màn hình** terminal khi hook chặn - làm bằng chứng
3. **Lưu file diff** bằng `--keep-temp` - commit lên repo
4. **Mở file C** trong IDE để hội đồng thấy code thay đổi
5. **Comment trong file** đã có sẵn giải thích lỗ hổng (tiếng Việt có dấu)

## 🐛 Troubleshooting

**Lỗi: "Không tìm thấy SCGL"**
- Chỉnh sửa file `.env` thủ công:
  ```
  JIT_VULN_DEMO_DIR=D:/Cowork_Workspapce/Projects/01-LuanVan/code/demo
  JIT_VULN_SCGL_DIR=D:/Cowork_Workspapce/Projects/01-LuanVan/code/SCGL
  ```

**Hook không hoạt động**
- Kiểm tra file `.git/hooks/pre-commit` có nội dung không
- Test thử: `cd .git/hooks && bash pre-commit`

**Lỗi Unicode khi in tiếng Việt**
- Windows: chạy `chcp 65001` trước
- Hoặc thêm `set PYTHONIOENCODING=utf-8`

## 📚 Tài liệu tham khảo

- Bài báo SCGL: (liên kết tới paper)
- Dataset CodeJIT: (liên kết GitHub)
- CWE Top 25: https://cwe.mitre.org/top25/

## 📞 Liên hệ

- Tác giả: Nguyễn Hải Sơn
- Email: (email sinh viên)
- GVHD: TS. Phạm Thế Anh Phú - HUTECH
