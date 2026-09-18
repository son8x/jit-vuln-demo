/*
 * =============================================================================
 * Tên tệp       : null_deref_safe.c
 * Kịch bản       : NULL Pointer Dereference (CWE-476) - PHIÊN BẢN ĐÃ SỬA
 * Mô tả         : Phiên bản an toàn - kiểm tra NULL trước mọi dereference
 *
 * So với bản gốc, thay đổi:
 *   - Tất cả hàm dereference con trỏ đều kiểm tra NULL trước
 *   - Trả về mã lỗi (-1) khi con trỏ NULL
 *   - In thông báo lỗi rõ ràng thay vì để chương trình crash
 *
 * Đặc điểm:
 *   - CWE-476 fix: NULL check trước mọi dereference
 *   - CWE-754 fix: xử lý đúng các trường hợp đặc biệt
 *   - CWE-252 fix: kiểm tra return value của hàm
 *
 * Dùng trong demo: SCGL phải cho phép commit (CLEAN).
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>

typedef struct {
    int  ma_nguoi_dung;
    char ten[64];
    int  tuoi;
} NguoiDung;

static NguoiDung danh_sach[5] = {
    {1, "Nguyen Van A", 25},
    {2, "Tran Thi B",   30},
    {3, "Le Van C",     22},
    {4, "Pham Thi D",   28},
    {5, "Hoang Van E",  35},
};

NguoiDung* tim_nguoi_dung(int ma) {
    for (int i = 0; i < 5; i++) {
        if (danh_sach[i].ma_nguoi_dung == ma) {
            return &danh_sach[i];
        }
    }
    return NULL;
}

/* HÀM AN TOÀN - kiểm tra NULL trước khi dereference */
int in_nguoi_dung(const NguoiDung *nd) {
    if (nd == NULL) {
        printf("[LOI] Con tro NULL, khong the in thong tin\n");
        return -1;
    }
    printf("[User] ma=%d, ten=%s, tuoi=%d\n",
           nd->ma_nguoi_dung, nd->ten, nd->tuoi);
    return 0;
}

int main(int argc, char **argv) {
    int ma_can_tim = 999;

    NguoiDung *nd = tim_nguoi_dung(ma_can_tim);
    if (nd == NULL) {
        printf("[Thong bao] Khong tim thay nguoi dung ma=%d\n", ma_can_tim);
        return 0;
    }
    in_nguoi_dung(nd);
    return 0;
}
