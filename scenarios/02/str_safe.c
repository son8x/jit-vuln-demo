/*
 * =============================================================================
 * Tên tệp       : str_safe.c
 * Kịch bản       : Buffer Overflow (CWE-120) - PHIÊN BẢN ĐÃ SỬA
 * Mô tả         : Phiên bản an toàn - dùng strncpy/snprintf kiểm tra độ dài
 *
 * So với bản gốc, thay đổi:
 *   - Thay strcpy() bằng snprintf() có giới hạn kích thước
 *   - Luôn kiểm tra độ dài input trước khi sao chép
 *   - Đảm bảo chuỗi kết thúc bằng ký tự null
 *
 * Đặc điểm:
 *   - CWE-120 fix: sử dụng hàm an toàn (snprintf, strncpy có kiểm tra)
 *   - CWE-170 fix: đảm bảo null-terminator
 *
 * Dùng trong demo: SCGL phải cho phép commit (CLEAN).
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>

#define SO_NGUOI_DUNG_TOI_DA  100
#define DO_DAI_TEN_TOI_DA     64
#define BUF_SIZE              32

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

/* HÀM AN TOÀN - dùng snprintf giới hạn kích thước */
int sao_chep_ten(char *dich, size_t kich_thuoc_dich, const char *nguon) {
    if (nguon == NULL || dich == NULL || kich_thuoc_dich == 0) {
        return -1;
    }
    /* Kiểm tra độ dài nguồn có vượt bộ đệm đích không */
    size_t do_dai_nguon = strnlen(nguon, kich_thuoc_dich);
    if (do_dai_nguon >= kich_thuoc_dich) {
        printf("[CANH BAO] Input qua dai (>= %zu), bi tu choi\n", kich_thuoc_dich);
        return -1;
    }
    /* Sao chép an toàn với snprintf */
    snprintf(dich, kich_thuoc_dich, "%s", nguon);
    return 0;
}

int main(int argc, char **argv) {
    khoi_tao_nguoi_dung_mau();

    if (argc < 2) {
        printf("Cách dùng: %s <ten_dang_nhap>\n", argv[0]);
        return 1;
    }

    char ten_dang_nhap[DO_DAI_TEN_TOI_DA];
    if (sao_chep_ten(ten_dang_nhap, sizeof(ten_dang_nhap), argv[1]) != 0) {
        return 3;
    }

    int ma = tim_nguoi_dung_theo_ten(ten_dang_nhap);
    if (ma < 0) {
        printf("Không tìm thấy người dùng: %s\n", ten_dang_nhap);
        return 2;
    }

    printf("Tìm thấy người dùng mã=%d\n", ma);
    return 0;
}
