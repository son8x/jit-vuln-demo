/*
 * =============================================================================
 * Tên tệp       : cmd_vuln.c
 * Kịch bản       : OS Command Injection (CWE-78)
 * Mô tả         : Phiên bản CÓ LỖ HỔNG - chèn lệnh hệ thống từ input người dùng
 *
 * So với bản gốc, thay đổi:
 *   - Thêm hàm xem_thong_tin_user() gọi system() với input người dùng
 *   - Input được ghép thẳng vào chuỗi lệnh shell
 *
 * Lỗ hổng:
 *   - CWE-78: OS Command Injection
 *   - CWE-77: Command Injection
 *   - Ví dụ: nhập "admin; rm -rf /"
 *     => Shell thực thi: id admin; rm -rf /
 *     => Có thể xóa dữ liệu, leo thang đặc quyền
 *
 * Dùng trong demo: SCGL phải phát hiện commit này là BUGGY.
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SO_NGUOI_DUNG_TOI_DA  100
#define DO_DAI_TEN_TOI_DA     64
#define DO_DAI_LENH           256

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

/* HÀM NÀY CHỨA LỖ HỔNG COMMAND INJECTION */
void xem_thong_tin_user(const char *ten_dang_nhap) {
    char lenh[DO_DAI_LENH];

    /* Ghép input vào lệnh shell - cực kỳ nguy hiểm! */
    snprintf(lenh, sizeof(lenh), "id %s", ten_dang_nhap);
    printf("[SHELL] Thuc thi: %s\n", lenh);

    /* Mô phỏng: in ra thay vì gọi system() thật */
    printf("[CANH BAO] Loi hong Command Injection neu goi system(%s)\n", lenh);
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
