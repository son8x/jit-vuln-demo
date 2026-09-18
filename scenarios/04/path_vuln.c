/*
 * =============================================================================
 * Tên tệp       : path_vuln.c
 * Kịch bản       : Path Traversal (CWE-22)
 * Mô tả         : Phiên bản CÓ LỖ HỔNG - cho phép truy cập file ngoài thư mục
 *
 * So với bản gốc, thay đổi:
 *   - Thêm hàm doc_file_user() mở file theo tên người dùng nhập vào
 *   - Không kiểm tra đường dẫn, attacker có thể dùng "../" để đọc file hệ thống
 *
 * Lỗ hổng:
 *   - CWE-22: Path Traversal
 *   - CWE-23: Relative Path Traversal
 *   - CWE-36: Absolute Path Traversal
 *   - Ví dụ: nhập "../../../../etc/passwd"
 *     => Mở file /etc/passwd (Linux) hoặc C:\Windows\System32\config\SAM (Windows)
 *
 * Dùng trong demo: SCGL phải phát hiện commit này là BUGGY.
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>

#define SO_NGUOI_DUNG_TOI_DA  100
#define DO_DAI_TEN_TOI_DA     64
#define DO_DAI_DUONG_DAN      512

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

/* HÀM NÀY CHỨA LỖ HỔNG PATH TRAVERSAL */
int doc_file_user(const char *ten_file) {
    char duong_dan[DO_DAI_DUONG_DAN];

    /* Ghép đường dẫn trực tiếp - KHÔNG kiểm tra "../" */
    snprintf(duong_dan, sizeof(duong_dan), "data/users/%s.txt", ten_file);
    printf("[FILE] Mo file: %s\n", duong_dan);

    /* Mô phỏng lỗi: cho phép "../etc/passwd" */
    if (strstr(ten_file, "..") != NULL) {
        printf("[CANH BAO] Path traversal! Co the doc file ngoai thu muc\n");
        return 1;
    }
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

    /* Lấy tên file từ input - mô phỏng việc đọc profile */
    char ten_file[DO_DAI_TEN_TOI_DA];
    snprintf(ten_file, sizeof(ten_file), "%s", argv[1]);
    doc_file_user(ten_file);

    printf("Tìm thấy người dùng mã=%d\n", ma);
    return 0;
}
