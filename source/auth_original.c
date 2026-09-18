/*
 * =============================================================================
 * Tên tệp       : auth.c
 * Mô tả         : Phiên bản gốc (an toàn) - file nền tảng cho tất cả kịch bản
 *                 demo phát hiện lỗ hổng bảo mật.
 * Tác giả       : Nguyễn Hải Sơn (Luận văn tốt nghiệp - HUTECH)
 * Ngày tạo      : 18/09/2026
 *
 * Tệp này KHÔNG chứa lỗ hổng, được dùng làm BASE để so sánh với các phiên bản
 * có lỗi (vuln) và đã sửa (safe) trong từng thư mục kịch bản:
 *   - sql_injection/         : CWE-89  - SQL Injection
 *   - buffer_overflow/       : CWE-120 - Buffer Overflow
 *   - command_injection/     : CWE-78  - OS Command Injection
 *   - hardcoded_credentials/ : CWE-798 - Hardcoded Credentials
 *   - path_traversal/        : CWE-22  - Path Traversal
 *
 * Lưu ý: Comment tiếng Việt có dấu được viết theo chuẩn UTF-8.
 *        Trình biên dịch C (gcc, clang, MSVC) chấp nhận vì C99+ xử lý comment
 *        như khoảng trắng, không giới hạn ký tự.
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>

/* Hằng số cấu hình - giới hạn kích thước mảng */
#define SO_NGUOI_DUNG_TOI_DA  100
#define DO_DAI_TEN_TOI_DA     64

/* Cấu trúc lưu thông tin người dùng trong bộ nhớ (mô phỏng) */
typedef struct {
    int  ma_nguoi_dung;                          /* Mã số người dùng */
    char ten_dang_nhap[DO_DAI_TEN_TOI_DA];       /* Tên đăng nhập */
    int  trang_thai_hoat_dong;                   /* 1 = đang hoạt động, 0 = bị khóa */
} NguoiDung;

/* Cơ sở dữ liệu người dùng toàn cục - mô phỏng bảng trong DB */
static NguoiDung bang_nguoi_dung[SO_NGUOI_DUNG_TOI_DA];
static int      so_luong_nguoi_dung = 0;

/* Hàm khởi tạo dữ liệu mẫu để chương trình có thể chạy thử */
void khoi_tao_nguoi_dung_mau(void) {
    bang_nguoi_dung[0] = (NguoiDung){1, "admin", 1};
    bang_nguoi_dung[1] = (NguoiDung){2, "alice", 1};
    bang_nguoi_dung[2] = (NguoiDung){3, "bob",   0};
    so_luong_nguoi_dung = 3;
}

/* Hàm tìm người dùng theo tên - trả về mã hoặc -1 nếu không thấy */
int tim_nguoi_dung_theo_ten(const char *ten_dang_nhap) {
    for (int i = 0; i < so_luong_nguoi_dung; i++) {
        if (strcmp(bang_nguoi_dung[i].ten_dang_nhap, ten_dang_nhap) == 0) {
            return bang_nguoi_dung[i].ma_nguoi_dung;
        }
    }
    return -1;
}

/* Hàm main - chương trình đơn giản chỉ tìm người dùng theo tên */
int main(int argc, char **argv) {
    khoi_tao_nguoi_dung_mau();

    if (argc < 2) {
        printf("Cách dùng: %s <ten_dang_nhap>\n", argv[0]);
        return 1;
    }

    int ma = tim_nguoi_dung_theo_ten(argv[1]);
    if (ma < 0) {
        printf("Không tìm thấy người dùng: %s\n", argv[1]);
        return 2;
    }

    printf("Tìm thấy người dùng có mã=%d\n", ma);
    return 0;
}
