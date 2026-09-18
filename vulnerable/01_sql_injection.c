/*
 * =============================================================================
 * Tên tệp       : auth_vuln.c
 * Kịch bản       : SQL Injection (CWE-89)
 * Mô tả         : Phiên bản CÓ LỖ HỔNG - minh họa SQL Injection
 *
 * So với bản gốc (auth.c), thay đổi:
 *   - Thêm hàm xac_thuc_nguoi_dung() dùng sprintf() ghép chuỗi SQL
 *   - Input người dùng được chèn trực tiếp vào câu truy vấn
 *   - KHÔNG escape, KHÔNG parameterize
 *
 * Lỗ hổng:
 *   - CWE-89: SQL Injection
 *   - Nếu gọi: chuong_trinh "admin' OR '1'='1"
 *     => SQL: SELECT id FROM users WHERE name='admin' OR '1'='1' AND active=1
 *     => Có thể vượt qua kiểm tra trang_thai_hoat_dong
 *
 * Dùng trong demo: SCGL phải phát hiện commit này là BUGGY.
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>

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

/* HÀM NÀY CHỨA LỖ HỔNG SQL INJECTION */
int xac_thuc_nguoi_dung(const char *ten_dang_nhap, const char *mat_khau) {
    char cau_truy_van[DO_DAI_TRUY_VAN];

    /* Ghép chuỗi trực tiếp - attacker có thể chèn SQL */
    snprintf(cau_truy_van, sizeof(cau_truy_van),
             "SELECT id FROM users WHERE name='%s' AND active=1",
             ten_dang_nhap);

    /* Mô phỏng: in ra truy vấn thay vì thực thi */
    printf("[SQL] %s\n", cau_truy_van);

    /* Phát hiện chuỗi injection đơn giản (chỉ minh họa) */
    if (strstr(ten_dang_nhap, "' OR '1'='1") != NULL) {
        printf("[CANH BAO] Phat hien SQL Injection trong input!\n");
        return 1;
    }
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
