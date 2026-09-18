/*
 * =============================================================================
 * Tên tệp       : cmd_safe.c
 * Kịch bản       : OS Command Injection (CWE-78) - PHIÊN BẢN ĐÃ SỬA
 * Mô tả         : Phiên bản an toàn - dùng whitelist validation
 *
 * So với bản gốc, thay đổi:
 *   - Thay system() bằng tra cứu trực tiếp trong bảng (không gọi shell)
 *   - Validate input: chỉ chấp nhận ký tự an toàn
 *   - Không thực thi lệnh hệ thống từ input người dùng
 *
 * Đặc điểm:
 *   - CWE-78 fix: KHÔNG gọi system()/exec*() với input người dùng
 *   - CWE-77 fix: dùng API an toàn thay vì shell
 *   - Input validation: whitelist ký tự
 *
 * Dùng trong demo: SCGL phải cho phép commit (CLEAN).
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define SO_NGUOI_DUNG_TOI_DA  100
#define DO_DAI_TEN_TOI_DA     64

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

/* Hàm validate whitelist: chỉ chấp nhận chữ cái, số, _, - */
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

/* HÀM AN TOÀN - tra cứu trực tiếp, KHÔNG gọi shell */
void xem_thong_tin_user(const char *ten_dang_nhap) {
    if (!ten_dang_nhap_hop_le(ten_dang_nhap)) {
        printf("[TU CHOI] Ten dang nhap chua ky tu khong hop le\n");
        return;
    }
    /* Tra cứu thông tin trong bảng - KHÔNG qua shell */
    int ma = tim_nguoi_dung_theo_ten(ten_dang_nhap);
    if (ma >= 0) {
        printf("[INFO] User '%s' co ma=%d (lay tu bang noi bo)\n",
               ten_dang_nhap, ma);
    } else {
        printf("[INFO] User '%s' khong ton tai trong he thong\n", ten_dang_nhap);
    }
}

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

    xem_thong_tin_user(argv[1]);
    printf("Đã xem thông tin người dùng mã=%d\n", ma);
    return 0;
}
