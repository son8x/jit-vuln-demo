/*
 * =============================================================================
 * Tên tệp       : cred_vuln.c
 * Kịch bản       : Hardcoded Credentials (CWE-798)
 * Mô tả         : Phiên bản CÓ LỖ HỔNG - mật khẩu bị hardcode trong code
 *
 * So với bản gốc, thay đổi:
 *   - Thêm hàm kiem_tra_mat_khau_admin() so sánh với mật khẩu hardcode
 *   - Mật khẩu "SuperSecret123!" được viết thẳng trong source code
 *
 * Lỗ hổng:
 *   - CWE-798: Use of Hard-coded Credentials
 *   - CWE-259: Use of Hard-coded Password
 *   - CWE-321: Use of Hard-coded Cryptographic Key
 *   - Rủi ro: ai đọc được source (GitHub, decompile) sẽ biết mật khẩu admin
 *
 * Dùng trong demo: SCGL phải phát hiện commit này là BUGGY.
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>

#define SO_NGUOI_DUNG_TOI_DA  100
#define DO_DAI_TEN_TOI_DA     64
#define DO_DAI_MAT_KHAU       128

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

/* HÀM NÀY CHỨA LỖ HỔNG HARDCODE CREDENTIALS */
int kiem_tra_mat_khau_admin(const char *mat_khau_nhap) {
    /* CẢNH BÁO: Mật khẩu bị hardcode trong source code! */
    const char *MAT_KHAU_ADMIN_HARDCODED = "SuperSecret123!";

    if (strcmp(mat_khau_nhap, MAT_KHAU_ADMIN_HARDCODED) == 0) {
        printf("[AUTH] Mat khau admin dung!\n");
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    khoi_tao_nguoi_dung_mau();

    if (argc < 3) {
        printf("Cách dùng: %s <ten_dang_nhap> <mat_khau>\n", argv[0]);
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
