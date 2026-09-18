/*
 * =============================================================================
 * Tên tệp       : cred_safe.c
 * Kịch bản       : Hardcoded Credentials (CWE-798) - PHIÊN BẢN ĐÃ SỬA
 * Mô tả         : Phiên bản an toàn - đọc mật khẩu từ biến môi trường
 *
 * So với bản gốc, thay đổi:
 *   - Thay mật khẩu hardcode bằng getenv() đọc từ biến môi trường
 *   - Mật khẩu chỉ tồn tại trong runtime, không có trong source
 *   - So sánh dùng hàm an toàn (constant-time comparison)
 *
 * Đặc điểm:
 *   - CWE-798 fix: KHÔNG hardcode credentials
 *   - CWE-259 fix: đọc từ env / secret manager / file cấu hình
 *   - CWE-208 fix: so sánh constant-time chống timing attack
 *
 * Dùng trong demo: SCGL phải cho phép commit (CLEAN).
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SO_NGUOI_DUNG_TOI_DA  100
#define DO_DAI_TEN_TOI_DA     64
#define DO_DAI_MAT_KHAU       128
#define TEN_BIEN_MAT_KHAU      "ADMIN_PASSWORD"

typedef struct {
    int  ma_nguoi_dung;
    char ten_dang_nhap[DO_DAI_TEN_TOI_DA];
    int  trang_thai_hoat_dong;
} NguoiDung;

static NguoiDung bang_nguoi_dung[SO_NGUOI_DUNG_TOI_DA];
static int      so_luong_nguoi_dung = 0;

void khoi_tao_nguoi_dung_mau(void) {
    bang_nguoi_dung[0] = (NguoiDung){1, "admin", 1};
    bang_nguoi_dung[1] = (NguoiDung){2, "alice", 1};
    bang_nguoi_dung[2] = (NguoiDung){3, "bob",   0};
    so_luong_nguoi_dung = 3;
}

int tim_nguoi_dung_theo_ten(const char *ten_dang_nhap) {
    for (int i = 0; i < so_luong_nguoi_dung; i++) {
        if (strcmp(bang_nguoi_dung[i].ten_dang_nhap, ten_dang_nhap) == 0) {
            return bang_nguoi_dung[i].ma_nguoi_dung;
        }
    }
    return -1;
}

/* Hàm so sánh constant-time chống timing attack */
int so_sanh_mat_khau_an_toan(const char *a, const char *b) {
    if (a == NULL || b == NULL) return 0;
    size_t la = strlen(a);
    size_t lb = strlen(b);
    if (la != lb) return 0;
    unsigned char ket_qua = 0;
    for (size_t i = 0; i < la; i++) {
        ket_qua |= (unsigned char)a[i] ^ (unsigned char)b[i];
    }
    return ket_qua == 0;
}

/* HÀM AN TOÀN - đọc mật khẩu từ biến môi trường */
int kiem_tra_mat_khau_admin(const char *mat_khau_nhap) {
    /* Đọc mật khẩu từ biến môi trường - KHÔNG hardcode */
    const char *mat_khau_he_thong = getenv(TEN_BIEN_MAT_KHAU);
    if (mat_khau_he_thong == NULL) {
        printf("[LOI] Chua cau hinh bien moi truong %s\n", TEN_BIEN_MAT_KHAU);
        return 0;
    }
    /* So sánh constant-time */
    if (so_sanh_mat_khau_an_toan(mat_khau_nhap, mat_khau_he_thong)) {
        printf("[AUTH] Mat khau admin dung (xac thuc thanh cong)\n");
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    khoi_tao_nguoi_dung_mau();

    if (argc < 3) {
        printf("Cách dùng: %s <ten_dang_nhap> <mat_khau>\n", argv[0]);
        printf("Lưu ý: đặt biến môi trường %s trước khi chạy\n", TEN_BIEN_MAT_KHAU);
        return 1;
    }

    int ma = tim_nguoi_dung_theo_ten(argv[1]);
    if (ma < 0) {
        printf("Không tìm thấy người dùng: %s\n", argv[1]);
        return 2;
    }

    if (strcmp(argv[1], "admin") == 0) {
        if (kiem_tra_mat_khau_admin(argv[2])) {
            printf("Xác thực admin thành công\n");
        } else {
            printf("Sai mật khẩu admin\n");
        }
    }

    printf("Tìm thấy người dùng mã=%d\n", ma);
    return 0;
}
