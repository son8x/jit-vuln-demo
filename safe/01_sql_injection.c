/*
 * =============================================================================
 * Tên tệp       : auth_safe.c
 * Kịch bản       : SQL Injection (CWE-89) - PHIÊN BẢN ĐÃ SỬA
 * Mô tả         : Phiên bản an toàn - minh họa cách vá lỗi SQL Injection
 *
 * So với bản gốc, thay đổi:
 *   - Thêm hàm xac_thuc_nguoi_dung() sử dụng PARAMETERIZED QUERY
 *   - Input được validate (kiểm tra ký tự hợp lệ) trước khi dùng
 *   - Có giới hạn độ dài input để tránh tràn bộ đệm
 *
 * Đặc điểm:
 *   - CWE-89 fix: dùng parameterized query (mô phỏng bằng dấu ?)
 *   - Input validation: chỉ chấp nhận chữ cái, số, _, -
 *   - Length check: giới hạn tối đa DO_DAI_TEN_TOI_DA ký tự
 *
 * Dùng trong demo: SCGL phải cho phép commit (CLEAN).
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Hằng số - giữ đồng bộ với bản gốc */
#define SO_NGUOI_DUNG_TOI_DA  100
#define DO_DAI_TEN_TOI_DA     64
#define DO_DAI_TRUY_VAN       256

/* Cấu trúc người dùng - giống bản gốc */
typedef struct {
    int  ma_nguoi_dung;
    char ten_dang_nhap[DO_DAI_TEN_TOI_DA];
    int  trang_thai_hoat_dong;
} NguoiDung;

static NguoiDung bang_nguoi_dung[SO_NGUOI_DUNG_TOI_DA];
static int      so_luong_nguoi_dung = 0;

/* Khởi tạo dữ liệu mẫu */
void khoi_tao_nguoi_dung_mau(void) {
    bang_nguoi_dung[0] = (NguoiDung){1, "admin", 1};
    bang_nguoi_dung[1] = (NguoiDung){2, "alice", 1};
    bang_nguoi_dung[2] = (NguoiDung){3, "bob",   0};
    so_luong_nguoi_dung = 3;
}

/* Hàm tìm người dùng - giữ nguyên từ bản gốc */
int tim_nguoi_dung_theo_ten(const char *ten_dang_nhap) {
    for (int i = 0; i < so_luong_nguoi_dung; i++) {
        if (strcmp(bang_nguoi_dung[i].ten_dang_nhap, ten_dang_nhap) == 0) {
            return bang_nguoi_dung[i].ma_nguoi_dung;
        }
    }
    return -1;
}

/* Hàm validate input: chỉ cho phép chữ cái, số, dấu gạch dưới và gạch ngang */
int ten_dang_nhap_hop_le(const char *s) {
    if (s == NULL || s[0] == '\0') return 0;
    size_t do_dai = strnlen(s, DO_DAI_TEN_TOI_DA);
    if (do_dai >= DO_DAI_TEN_TOI_DA) return 0;
    for (size_t i = 0; i < do_dai; i++) {
        char c = s[i];
        if (!isalnum((unsigned char)c) && c != '_' && c != '-') {
            return 0;
        }
    }
    return 1;
}

/* HÀM AN TOÀN - sử dụng parameterized query (mô phỏng) */
int xac_thuc_nguoi_dung(const char *ten_dang_nhap, const char *mat_khau) {
    /* Bước 1: validate input trước khi sử dụng */
    if (!ten_dang_nhap_hop_le(ten_dang_nhap)) {
        printf("[TU CHOI] Ten dang nhap khong hop le\n");
        return -1;
    }

    /* Bước 2: parameterized query - input được bind, không ghép chuỗi */
    char cau_truy_van[DO_DAI_TRUY_VAN];
    snprintf(cau_truy_van, sizeof(cau_truy_van),
             "SELECT id FROM users WHERE name=? AND active=1");
    printf("[SQL] %s (tham so: %s)\n", cau_truy_van, ten_dang_nhap);

    /* Bước 3: tra cứu trực tiếp (an toàn, không qua SQL injection) */
    return tim_nguoi_dung_theo_ten(ten_dang_nhap);
}

int main(int argc, char **argv) {
    khoi_tao_nguoi_dung_mau();

    if (argc < 2) {
        printf("Cách dùng: %s <ten_dang_nhap>\n", argv[0]);
        return 1;
    }

    int ma = xac_thuc_nguoi_dung(argv[1], "");
    if (ma < 0) {
        printf("Xác thực thất bại: %s\n", argv[1]);
        return 2;
    }

    printf("Đã xác thực người dùng mã=%d\n", ma);
    return 0;
}
