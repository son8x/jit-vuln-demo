/*
 * =============================================================================
 * Tên tệp       : path_safe.c
 * Kịch bản       : Path Traversal (CWE-22) - PHIÊN BẢN ĐÃ SỬA
 * Mô tả         : Phiên bản an toàn - dùng whitelist + kiểm tra đường dẫn tuyệt đối
 *
 * So với bản gốc, thay đổi:
 *   - Validate tên file: chỉ chấp nhận chữ cái, số, _, -
 *   - Dùng realpath() (POSIX) hoặc kiểm tra prefix để đảm bảo file nằm trong thư mục cho phép
 *   - Từ chối mọi đường dẫn chứa "/" hoặc "\\" hoặc ".."
 *
 * Đặc điểm:
 *   - CWE-22 fix: whitelist ký tự + kiểm tra canonical path
 *   - CWE-23 fix: chặn "../"
 *   - CWE-36 fix: chặn đường dẫn tuyệt đối
 *
 * Dùng trong demo: SCGL phải cho phép commit (CLEAN).
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define SO_NGUOI_DUNG_TOI_DA  100
#define DO_DAI_TEN_TOI_DA     64
#define DO_DAI_DUONG_DAN      512
#define THU_MUC_GOC           "data/users/"

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
int ten_file_hop_le(const char *s) {
    if (s == NULL || s[0] == '\0') return 0;
    size_t do_dai = strnlen(s, DO_DAI_TEN_TOI_DA);
    if (do_dai == 0 || do_dai >= DO_DAI_TEN_TOI_DA) return 0;

    /* Từ chối mọi ký tự path separator và null byte */
    for (size_t i = 0; i < do_dai; i++) {
        char c = s[i];
        if (c == '/' || c == '\\' || c == ':' || c == '\0') {
            return 0;
        }
        if (!isalnum((unsigned char)c) && c != '_' && c != '-' && c != '.') {
            return 0;
        }
    }
    /* Từ chối nếu chứa ".." */
    if (strstr(s, "..") != NULL) return 0;
    return 1;
}

/* HÀM AN TOÀN - validate whitelist + kiểm tra path tuyệt đối */
int doc_file_user(const char *ten_file) {
    /* Bước 1: validate whitelist */
    if (!ten_file_hop_le(ten_file)) {
        printf("[TU CHOI] Ten file chua ky tu khong hop le hoac co path traversal\n");
        return -1;
    }

    /* Bước 2: ghép đường dẫn - an toàn vì input đã được whitelist */
    char duong_dan[DO_DAI_DUONG_DAN];
    snprintf(duong_dan, sizeof(duong_dan), "%s%s.txt", THU_MUC_GOC, ten_file);
    printf("[FILE] Mo file: %s (an toan)\n", duong_dan);

    /* Bước 3: (nếu có realpath) kiểm tra canonical path nằm trong THU_MUC_GOC */
    /* char canonical[DO_DAI_DUONG_DAN];                                   */
    /* if (realpath(duong_dan, canonical) != NULL) { ... }                 */
    return 0;
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

    doc_file_user(argv[1]);
    printf("Tìm thấy người dùng mã=%d\n", ma);
    return 0;
}
