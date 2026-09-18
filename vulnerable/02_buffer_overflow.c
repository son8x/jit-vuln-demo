/*
 * =============================================================================
 * Tên tệp       : str_vuln.c
 * Kịch bản       : Buffer Overflow (CWE-120)
 * Mô tả         : Phiên bản CÓ LỖ HỔNG - tràn bộ đệm khi sao chép chuỗi
 *
 * So với bản gốc, thay đổi:
 *   - Thêm hàm sao_chep_ten() dùng strcpy() không kiểm tra độ dài
 *   - Nếu người dùng nhập tên dài hơn BUF_SIZE sẽ tràn bộ đệm
 *
 * Lỗ hổng:
 *   - CWE-120: Classic Buffer Overflow
 *   - CWE-119: Improper Restriction of Operations within Buffer Bounds
 *   - Ví dụ: nhập tên dài 200 ký tự vào bộ đệm 64 byte
 *     => Ghi đè các biến khác trên stack
 *     => Có thể thực thi mã tùy ý (RCE)
 *
 * Dùng trong demo: SCGL phải phát hiện commit này là BUGGY.
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>

/* Hằng số - giữ đồng bộ với bản gốc */
#define SO_NGUOI_DUNG_TOI_DA  100
#define DO_DAI_TEN_TOI_DA     64
#define BUF_SIZE              32    /* Bộ đệm đích - cố tình NHỎ để demo */

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

/* HÀM NÀY CHỨA LỖ HỔNG BUFFER OVERFLOW */
void sao_chep_ten(char *dich, const char *nguon) {
    /* strcpy KHÔNG kiểm tra độ dài - nguy hiểm! */
    char bo_dem[BUF_SIZE];
    strcpy(bo_dem, nguon);            /* Nếu nguon > 32 byte sẽ tràn */
    strcpy(dich, bo_dem);
}

int main(int argc, char **argv) {
    khoi_tao_nguoi_dung_mau();

    if (argc < 2) {
        printf("Cách dùng: %s <ten_dang_nhap>\n", argv[0]);
        return 1;
    }

    char ten_dang_nhap[DO_DAI_TEN_TOI_DA];
    sao_chep_ten(ten_dang_nhap, argv[1]);   /* Nguy hiểm nếu argv[1] dài */

    int ma = tim_nguoi_dung_theo_ten(ten_dang_nhap);
    if (ma < 0) {
        printf("Không tìm thấy người dùng: %s\n", ten_dang_nhap);
        return 2;
    }

    printf("Tìm thấy người dùng mã=%d\n", ma);
    return 0;
}
